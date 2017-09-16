/** \file
 * Generate a word cloud from source files and show the domain as
 * expressed by the code. */

#include <ctype.h>
#include <errno.h>
#include <error.h>
#include <getopt.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "domaincloud.h"

/** \struct cli_options
 *  \brief Flags and arguments to be set by \ref parse_cli_options.
 *
 *  \var const char *cli_options::output_file
 *      Where to put the final result.
 *  \var bool cli_options::substitute_only
 *      Strip unwanted clutter from source only.
 *  \var char **cli_options::arguments
 *      The part of \a argv where the arguments begin.
 *  \var int cli_options::num_arguments
 *      Number of arguments.
 */
struct cli_options
{
    char **arguments;
    const char *output_file;
    int num_arguments;
    bool substitute_only;
};

static void parse_cli_options (char *argv[], int argc, struct cli_options *options);
static void process_input_file (const char *input_file, FILE *ostr);
static void generate_word_cloud (const char *input_file, const char *output_file);

int
main (int argc, char *argv[])
{
    struct cli_options options = {
        .output_file = "-", .substitute_only = false};

    parse_cli_options (argv, argc, &options);

    const char *tmp_name = ".rename_me_42";
    FILE *output_stream;
    bool to_stdout = !strcmp (options.output_file, "-");

    if (options.substitute_only)
        output_stream = to_stdout ? stdout : fopen (options.output_file, "w");
    else
        output_stream = fopen (tmp_name, "w");

    if (!output_stream)
        error (
            EXIT_FAILURE, errno,
            "Can't open '%s' for writing!", options.output_file);

    for (int input_file = 0; input_file < options.num_arguments; ++input_file)
        process_input_file (options.arguments[input_file], output_stream);

    if (!to_stdout)
        fclose (output_stream);

    if (!options.substitute_only)
        generate_word_cloud (tmp_name, options.output_file);
}

/** Parse CLI options and put results into \a options.  Will exit on error. */
static void
parse_cli_options (char *argv[], int argc, struct cli_options *options)
{
    opterr = 1;

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

            case 'h':
                print_usage (stdout);
                exit (EXIT_SUCCESS);

            case 'o':
                options->output_file = optarg;
                break;

            case 'S':
                options->substitute_only = true;
                break;

            case '?':
                /* getopt_long will have already printed an error */
                print_usage (stderr);
                exit (EXIT_FAILURE);

            default:
                fprintf (
                    stderr, "?? getopt returned character code %#x ??\n", choice);
        }
    }

    if (optind < argc)
    {
        options->arguments = argv + optind;
        options->num_arguments = argc - optind;
    }
    else
    {
        fprintf (stderr, "No input files!\n");
        print_usage (stderr);
        exit (EXIT_FAILURE);
    }
}

/** Generate a word cloud from \a input_file and save the resulting PNG
 *  image to the file \a output_file.
 *  Using the
 *  <a href="https://github.com/amueller/word_cloud">wordcloud_cli.py</a>
 *  program.  Exit if this program encounters problems.
 */
static void
generate_word_cloud (const char *input_file, const char *output_file)
{
    char *cmd;
    int length = asprintf (
        &cmd,
        "wordcloud_cli.py --text '%s' --imagefile '%s' "
        "--width=1500 --height=1000",
        input_file, output_file);
    if (length < 0)
        error (EXIT_FAILURE, 0, "Memory allocation error");

    int res = system (cmd);
    remove (input_file);
    free (cmd);

    if (res)
        error (EXIT_FAILURE, 0, "wordcloud_cli.py error!");
}

/** Print version information to \a ostr.  */
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

/** Print usage information to \a ostr.  */
void
print_usage (FILE *ostr)
{
    fprintf (ostr, "Usage: %s %s\n", PROJECT_NAME, "[OPTION]... [FILE]...\n");
    fprintf (ostr,
"Generate a word cloud from source files and show the domain as expressed by\n"
"the code.\n\n");

    fprintf (ostr,
"  -h, --help          Display this help and exit.\n"
"  -V, --version       Output version information and exit.\n"
"  -o FILE, --output=FILE\n"
"                      Save output int file FILE.\n"
"  -S, --substitute-only\n"
"                      Remove comments and string literals only and don't\n"
"                      generate an image. If no -o Option is present print\n"
"                      to stdout.\n");
}

/** Try to open \a input_file and use this together with \a ostr as arguments
 *  to \ref remove_clutter.
 *
 *  If \a input_file is \c "-", will use \a stdin as input.
 *  Print an error message, if the file can't be opened or if \a remove_clutter
 *  failed.
 *
 *  \param input_file Name of an existing file or \c "-".
 *  \param ostr The file handle where non-skipped text will be appended.  Has
 *      to be opened for writing.
 */
static void
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

/** Advance \a istr past the next <tt>*</tt><tt>/</tt> or to \a EOF.
 *
 *  \param istr The file handle to the input source.  Has to be opened
 *      for reading.
 */
static void
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

/** Advance \a istr past the first not escaped delimiter char \a delim
 *  or to \a EOF.
 *
 *  \param delim The delimiter char.
 *  \param istr The file handle to the input source.  Has to be opened
 *      for reading.
 */
static void
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

/** Write a space to \a ostr and skip following white space. The first
 *  char that is not white space is pushed back to \a istr.
 *
 *  \param istr The file handle to the input source.  Has to be opened
 *      for reading.
 *  \param ostr The file handle where non-skipped text will be put.  Has
 *      to be opened for writing.
 *  \pre The last char read was a white space.
 *  \post The position of \a ostr is at the first char that is not white space
 *      or at \a EOF.
 */
static void
skip_white_space (FILE *istr, FILE *ostr)
{
    putc (' ', ostr);
    int cur;
    while ((cur = getc (istr)) != EOF)
    {
        if (!isspace (cur))
        {
            ungetc (cur, istr);
            break;
        }
    }
}

/** Skip block or line comments if the next read char is \c / or <tt>*</tt>,
 *  respectively.  Otherwise push back this char with \a ungetc and put \c /
 *  to \a ostr.
 *
 *  \param istr The file handle to the input source.  Has to be opened
 *      for reading.
 *  \param ostr The file handle where non-skipped text will be put.  Has
 *      to be opened for writing.
 *  \pre The last char read was a \c /.
 *  \post The position of \a ostr is
 *    \li after the first not escaped newline (line comment \c //),
 *    \li after the first <tt>*</tt><tt>/</tt> (block comment),
 *    \li at the position of the start of the function call (not a comment) or
 *    \li at \a EOF (comment "terminated" by \a EOF).
 */
static void
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

/** Copy content of \a istr to \a ostr while skipping comments,
 *  string literals and replacing successive white space by a single
 *  space.
 *
 *  \param istr The file handle to the input source.  Has to be opened
 *      for reading.
 *  \param ostr The file handle where non-skipped text will be put.  Has
 *      to be opened for writing.
 *      Will be flushed after processing.
 *  \returns \a errno if some I/O error occurred else 0.
 *  \post \c feof(istr) is true.
 */
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
        else if (isspace (cur))
            skip_white_space (istr, ostr);
        else
            putc (cur, ostr);
    }

    fflush (ostr);

    if (ferror (istr) || ferror (ostr))
        return errno;
    else
        return 0;
}

/* Copyright 2017 A. Johannes RICHTER <albrechtjohannes.richter@gmail.com>

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

