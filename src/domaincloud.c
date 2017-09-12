/*
   domaincloud.c -- Generate a word cloud from source files and
                    show the domain as expressed by the code.
   Copyright 2017 A. Johannes RICHTER <albrechtjohannes.richter@gmail.com>

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#include <errno.h>
#include <error.h>
#include <getopt.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "domaincloud.h"

void
process_input_file (const char *input_file, FILE *ostr)
{
    bool from_stdin = !strcmp (input_file, "-");
    FILE *istr = from_stdin ? stdin : fopen (input_file, "r");
    if (!istr)
    {
        error (0, errno, "Can't open '%s'!", input_file);
        return;
    }

    int res = remove_clutter (istr, ostr);
    if (res)
        error (0, res, "Error during processing of '%s'!", input_file);

    if (!from_stdin)
        fclose (istr);
}

int
main (int argc, char *argv[])
{
    const char *output_file = "-";
    bool subs_only = false;

    while (true)
    {
        int option_index = 0;

        static struct option long_options[] = {
            {"version", no_argument, 0, 'V'},
            {"help",    no_argument, 0, 'h'},
            {"substitute-only", no_argument, 0, 'S'},
            {"output",  required_argument, 0, 'o'},
            {0, 0, 0, 0}
        };

        int choice = getopt_long (
            argc, argv, "VhSo:", long_options, &option_index);

        if (choice == -1)
            break;

        switch (choice)
        {
            case 'V':
                print_version (stdout);
                exit (EXIT_SUCCESS);
                break;

            case 'h':
                print_usage (stdout);
                exit (EXIT_SUCCESS);
                break;

            case 'o':
                output_file = optarg;
                break;

            case 'S':
                subs_only = true;
                break;

            case '?':
                /* getopt_long will have already printed an error */
                print_usage (stderr);
                exit (EXIT_FAILURE);
                break;

            default:
                fprintf (
                    stderr, "?? getopt returned character code %#x ??\n", choice);
        }
    }

    if (optind < argc)
    {
        const char *tmp_name = ".rename_me_42";
        FILE *output_stream;
        bool to_stdout = !strcmp (output_file, "-");

        if (subs_only)
            output_stream = to_stdout ? stdout : fopen (output_file, "w");
        else
            output_stream = fopen (tmp_name, "w");

        if (!output_stream)
            error (EXIT_FAILURE, errno, "Can't open '%s' for writing!", output_file);

        for (; optind < argc; ++optind)
            process_input_file (argv[optind], output_stream);

        if (!to_stdout)
            fclose (output_stream);

        if (!subs_only)
        {
            char *cmd;
            asprintf (
                &cmd,
                "wordcloud_cli.py --text '%s' --imagefile '%s' "
                    "--width=1500 --height=1000",
                tmp_name, output_file);
            int res = system (cmd);
            remove (tmp_name);
            free (cmd);

            if (res)
                error (EXIT_FAILURE, 0, "wordcloud_cli.py error!");
        }
    }
    else
    {
        fprintf (stderr, "No input files!\n");
        print_usage (stderr);
        exit (EXIT_FAILURE);
    }
}

/* Print version information to `ostr`.  */
void
print_version (FILE *ostr)
{
    fprintf (ostr, "%s (%s)\n", PROJECT_NAME, PROJECT_VERSION);
    fprintf (ostr, "Copyright %s %s\n", PROJECT_COPY_YEARS, PROJECT_AUTHORS);
    fprintf (ostr,
"License GPLv3+: "
    "GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>\n"
"This is free software: you are free to change and redistribute it.\n"
"There is NO WARRANTY, to the extent permitted by law.\n");
}

/* Print usage to `ostr`.  */
void
print_usage (FILE *ostr)
{
    fprintf (ostr, "Usage: %s %s\n", PROJECT_NAME, "[OPTION]... [FILE]...");
    fprintf (ostr,
        "Generate a word cloud from source files"
        "and show the domain as expressed by the code.\n\n");

    fprintf (ostr,
"  -h, --help          Display this help and exit.\n"
"  -V, --version       Output version information and exit.\n"
"  -o FILE, --output=FILE\n"
"                      Save output int file FILE.\n"
"  -S, --substitute-only\n"
"                      Remove comments and string literals only and don't"
"                      generate an image. If no -o Option is present print"
"                      to stdout.\n");
}

void
skip_block_comments (FILE *istr)
{
    int cur = getc (istr);
    int next;
    while ((next = getc (istr)) != EOF)
    {
        if (cur == '*' && next == '/')
            break;
        cur = next;
    }
}

void
skip_delimiter_escape_aware (int delim, FILE *istr)
{
    int cur;
    bool ignore_next = false;
    while ((cur = getc (istr)) != EOF)
    {
        if (ignore_next)
            ignore_next = false;
        else if (cur == '\\')
            ignore_next = true;
        else if (cur == delim)
            break;
    }
}

void
try_skip_comments (FILE *istr, FILE *ostr)
{
    int next = getc (istr);

    if (next == '/')
        skip_delimiter_escape_aware ('\n', istr);
    else if (next == '*')
        skip_block_comments (istr);
    else
    {
        putc ('/', ostr);
        ungetc (next, istr);
    }
}

int
remove_clutter (FILE *istr, FILE *ostr)
{
    int cur;

    while ((cur = getc (istr)) != EOF && !ferror (ostr))
    {
        if (cur == '/')
            try_skip_comments (istr, ostr);
        else if (cur == '"' || cur == '\'')
            skip_delimiter_escape_aware (cur, istr);
        else
            putc (cur, ostr);
    }

    fflush (ostr);

    if (ferror (istr) || ferror (ostr))
        return errno;
    else
        return 0;
}

