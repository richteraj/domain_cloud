/** \file
 * Generate a word cloud from source files and show the domain as expressed by
 * the code.  */

#if defined (HAVE_CONFIG_H) && HAVE_CONFIG_H
    #include "config.h"
#endif

#include <errno.h>
#include <error.h>
#include <getopt.h>
#include <stdlib.h>
#include <string.h>

#include "domaincloud.h"
#include "extractwords.h"

/** Flags and arguments to be set by \ref parse_cli_options.  */
struct cli_options
{
    /** The part of \a argv where the arguments begin.  */
    char **arguments;
    /** Where to put the final result.  */
    const char *output_file;
    /** Number of arguments.  */
    int num_arguments;
    /** Strip unwanted clutter from source only.  */
    bool substitute_only;
    /** Substitute only and print results raw.  */
    bool raw_dump;
};

static void
parse_cli_options (char *argv[], int argc, struct cli_options *options);
static void process_input_file (
    const char *input_file, struct Word_frequency_s *result_words);
static void
generate_word_cloud (const char *input_file, const char *output_file);

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

    struct Word_frequency_s *result_words = NULL;
    if (wfreq_init (&result_words))
        error (EXIT_FAILURE, errno, "Memory error");

    for (int input_file = 0; input_file < options.num_arguments; ++input_file)
        process_input_file (options.arguments[input_file], result_words);

    if (options.substitute_only && !options.raw_dump)
        print_words_with_freq (output_stream, result_words);
    else
        print_words_raw (output_stream, result_words);

    if (!to_stdout)
        fclose (output_stream);

    if (!options.substitute_only)
        generate_word_cloud (tmp_name, options.output_file);

#ifndef NDEBUG
    wfreq_destroy (result_words);
#endif /* not NDEBUG */
}

/** Print version information to \a ostr.
 * \param ostr The file handle where to output to.  Has to be opened for
 * writing.  */
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

/** Print usage information to \a ostr.
 * \param ostr The file handle where to output to.  Has to be opened for
 * writing.  */
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
"  -r, --raw-dump      Similar to -S but print every word the number of times\n"
"                      it was counted.\n"
"  -S, --substitute-only\n"
"                      Remove comments and string literals only and don't\n"
"                      generate an image.  Every word will be printed on a\n"
"                      separate line following the word's count.\n"
"                      If no -o Option is present print to stdout.\n");
}

/** Parse CLI options and put results into \a options.  Will exit on error.
 * \param argv Like main argv.
 * \param argc Like main argc.
 * \param options Write parsed options into this.  Defaults should already
 * be present.  */
static void
parse_cli_options (char *argv[], int argc, struct cli_options *options)
{
    opterr = 1;

    while (true)
    {
        int option_index = 0;

        static struct option long_options[] = {
            {"version", no_argument, 0, 'V'},
            {"help", no_argument, 0, 'h'},
            {"raw-dump", no_argument, 0, 'r'},
            {"substitute-only", no_argument, 0, 'S'},
            {"output", required_argument, 0, 'o'},
            {0, 0, 0, 0}};

        int choice =
            getopt_long (argc, argv, "VhrSo:", long_options, &option_index);

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

        case 'r':
            options->raw_dump = true;
            /* [[falltrough]] */
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

/** Try to open \a input_file and use this together with \a result_words as
 * arguments to \ref count_words.
 *
 * If \a input_file is `-`, will use \a stdin as input.  Print an error
 * message, if the file can't be opened or if \a count_words failed.
 *
 * \param input_file Name of an existing file or `-`.
 * \param result_words An initialized \ref Word_frequency_s variable where the
 * results will be put.
 */
static void
process_input_file (
    const char *input_file, struct Word_frequency_s *result_words)
{
    bool from_stdin = !strcmp (input_file, "-");
    FILE *istr = from_stdin ? stdin : fopen (input_file, "r");
    if (!istr)
    {
        error (0, errno, "Can't open '%s'!", input_file);
        return;
    }

    int res = count_words (istr, result_words);
    if (res)
        error (0, res, "Error during processing of '%s'!", input_file);

    if (!from_stdin)
        fclose (istr);
}

/** Generate a word cloud from \a input_file and save the resulting PNG image to
 * the file \a output_file.  Using the <a
 * href="https://github.com/amueller/word_cloud">`wordcloud_cli.py`</a> program.
 * Exit if this program encounters problems.
 *
 * \param input_file Pass this file name to `wordcloud_cli.py` as `--text`.
 * \param output_file Pass this file name to `wordcloud_cli.py` as
 * `--imagefile`.  */
static void
generate_word_cloud (const char *input_file, const char *output_file)
{
    char *cmd;
    // Since wordcloud_cli.py also considers adjacent words as one word, we
    // shuffle before giving it to the program.
    int length = asprintf (
        &cmd, "shuf '%s' | wordcloud_cli.py --text - --imagefile '%s' "
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
