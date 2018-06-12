/********************************************************************************

    kdr_main.c    main module

    Other files:
            Makefile
            kdr.h
            kdr_build_dictionary.c
            kdr_process_input.c
            README
 
    Solution for the following Kata problem
        http://codingdojo.org/cgi-bin/index.pl?KataDictionaryReplacer

    Written in C language by Ismo Suihko, Espoo Finland, 2017-01-01
    Updates:  2017-01-23 (minor) Small adjustments to comments in the beginning
                          of kdr_main.c, so also README was updated.

    Compiled and tested in Fedora Linux using GNU C
         gcc (GCC) 6.3.1 20161221 (Red Hat 6.3.1-1)


!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
!  Some changes to Kata problem:
!!
!!        - dictionary is read from the text file (easy to modify) and in format
!!               "key","value"
!!          Also text file can have entries in format (to be compatible)
!!                dict ["temp", "temporary"]
!!
!!        - "dict empty" means no key,value pair in a file i.e. empty file
!!          or there are only comment lines starting with # or empty lines.
!!          Then no conversions are made to the input text.
!!
!!      Input string is read from stdin and has one line. Can be read also
!!      from a file using <inputfile as in Linux usually.
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!


    INPUT FILE: text file containing dictionary with a few "key","value" pairs
                e.g. dict ["temp", "temporary"]
                or just "temp","temporary"
                or "name", "John Doe"
        Format:
                Both key and value are inside double quotation characters.
                There is a comma just after the key, i.e. "keyname", "value"
                Key and value can't have character ", but both of them can have $. (*)
        
        If there are two keys with the same name, only the first defined is used.
        Duplicate checking prints an error message with line number of 2nd same key, 
        and then continues with OK status.

        File can have comment lines starting with #.
        Empty lines are allowed.

        Lines which don't have "key","value" in correct format cause a failure.

        *) About dollars in keys and values:
        - in values:
                dict ["price", "$20"]
                Input:  "Book costs $price$."
                Output: "Book costs $20."
        - in keys:
                dict ["$50", "today only 50 dollars"]
                Input:  "Price is $$50$."
                Output: "Price is today only 50 dollars."
        - NOTE! If there is a dollar in a key and an another key has same beginning
               up to $, results could be different than what you want.
               E.g. keys "today$50"  and "today" could be mixed.
               As they are stored internally $today$50$ and $today$, define the
               longer one first to be able to use both of them, and check the results.

        Key could be empty:
                dict ["", "an empty key"]
                Input:  "begin-$$-end"
                Output: "begin-an empty key-end"
                An empty key could be mixed with a key which starts with a $, like "$50",
                which are stored internally $$ and $$50$, so experiment to get what you want.

        Rule for keys with $:
                Define key with a longer name first if there is a $ inside:
                    dict ["$50", "today only 50 dollars]
                    dict ["", "an empty key"]
                Define longer key with doller earlier than key with the same start
                    dict ["price$50", "Today only 50 dollars"]
                    dict ["price", "and the price was $20"]

        Value could be empty:
                dict ["goals",""]
                Input:  "Number of goals: $goals$"
                Output: "Number of goals: "

    INPUT (stdin): Text string which contains keys marked with $name_of_the_key$
                e.g. $temp$ here comes the $name$

    OUTPUT (stdout): Text string which has all the keys replaced with the values.

    OUTPUT (stderr): No output to standard error.

    ERROR SITUATIONS:
        - illegal parameters
        - opening dictionary file failed
        - dictionary format illegal
        - memory allocation failed
        - size of the output text buffer exceeded, rest ignored
        - size of the dictionary table exceeded, rest ignored
        - duplicate keys found

    OUTPUT STATUS:  0 OK, or in errors EXIT_FAILURE

    LIMITATIONS (defined either in kdr.h or overridden in Makefile):
        MAX_DICT_KEYS      limits the number of the possible "key","value" pairs
        MAX_OUTPUT_STRING  output text buffer size

    PARAMETERS, in this order: [-dN] filename
        optional -dN defines a debug level, number 0-5, -d0=no debug (default)
        name of the dictionary text file

    NOTE
        - Output string is written into a buffer which size is limited and
          if that limit is reached, error is printed and rest of input is ignored
        - In source files TAB stops are 8 spaces (default in Linux)

    debug levels (-dN) (each level prints also lower level logs):
        - 0(default) just print the output string, also possible error messages
        - 1 print input text (useful if using scripts)
        - 2 print the dictionary
        - 3 entering and leaving subroutines, parameters and status
        - 4 parsing the lines of the dictionary, parsing the input string
        - 5 print also indexes when parsing the lines of the dictionary

--------------------------------------------------------------------------------

KATA: http://codingdojo.org/cgi-bin/index.pl?KataDictionaryReplacer

This kata is about making a simple string replacer. It is inspired by
Corey Haines Lightning talk about practicing.

Create a method that takes a string and a dictionary, and replaces every key
in the dictionary pre and suffixed with a dollar sign, with the corresponding
value from the Dictionary.

Tests

input : "", dict empty
output:""

input : "$temp$", dict ["temp", "temporary"]
output: "temporary"

input : "$temp$ here comes the name $name$", dict ["temp", "temporary"] ["name", "John Doe"]
output : "temporary here comes the name John Doe" 

********************************************************************************/
/* The above text is also in README file */


#include "kdr.h"


/*
 * Global variables for storing the dictionary and debug level.
 * XXX: Size of the dictionary is limited to MAX_DICT_KEYS (kdr.h, Makefile)
 */
int               number_of_keys = 0;
struct key_table  key_value_data[MAX_DICT_KEYS];

int               _debug_level    = 0; /* global debug level */

/*
 * Prototypes of functions from other modules:
 *      kdr_build_dictionary.c
 *      kdr_process_input.c
 * Just one routine from each module, so no separate header files.
 */
extern int kdr_build_dictionary(char *filename);
extern int kdr_process_input(char *input_string, char *output_string);



/*
 * Usage
 */
static void Usage(void)
{
        printf("Usage: kdr [-dN] dictionaryfile\n");
        printf("  Dictionaryfile contains rows of \"key\",\"value\" pairs.\n");
        printf("  Program asks an input text string which contains $key$ strings,\n");
        printf("  which are replaced by value strings found from the dictionaryfile.\n");
        printf("  Example of input:  My name is $name$. If dictionary has a row \"name\",\"John Doe\",\n");
        printf("  then the output is My name is John Doe.\n");

        printf("  Optional -dN sets debug level 0-5, default 0 (no debug), e.g. -d2 shows dictionary.\n");
        printf("  Compiled with MAX_DICT_KEYS=%d and MAX_OUTPUT_STRING=%d.\n",
                MAX_DICT_KEYS, MAX_OUTPUT_STRING);

} /* Usage */


/*
 *  main routine
 *
 *  If error at stage 1-6, jumps to 7 to cleanup/exit.
 */
/* 1. Check the parameters  */
/* 2. Read dictionary data  */
/* 3. Read input string     */
/* 4. Reserve output string */
/* 5. Process input string  */
/* 6. Write the result      */
/* 7. Cleanup, deallocate   */
/*
 * Future development ideas:
 *    - remove size limits in key table and output string.
 *    - handle several input strings, not just one 
 */

int main(int argc, char *argv[])
{
        int     status = EXIT_SUCCESS;  /* OK */

        char    *output_string = NULL;  /* result string   */
        char    *filename;              /* dictionary data */

        char    *input_string = NULL;   /* read from stdin */
        ssize_t linelen;                /* signed, for output of getline() */
        size_t  buffsize = 0;

        int     key_ind;


        do {
/* 1. Check the parameters */
                if (argc == 3) { /* check debuglevel "-dN" */
                        _debug_level = -1;
                        if (argv[1][0] == '-' && argv[1][1] == 'd' && strlen(argv[1]) == 3)
                                _debug_level = argv[1][2] - '0';
                }

                if (argc == 1 || argc > 3 || _debug_level < 0 || _debug_level > 5) {
                        Usage();
                        status = EXIT_FAILURE;
                        break;
                }

                filename = argv[argc - 1]; /* filename is the last parameter */

/* 2. Read dictionary data */
                status = kdr_build_dictionary(filename); /* possible error msg inside */
                if (status)
                        break;
                
                DEBUG(2) { /* print dictionary table */
                        for (key_ind = 0; key_ind < number_of_keys; key_ind++)
                                printf("%2d: '%s','%s'\n",
                                        key_ind,
                                        key_value_data[key_ind].keyp,
                                        key_value_data[key_ind].valuep);
                }

/* 3. Read input string */
                /*
                 * read the input string from stdin, includes EOL
                 * getline() allocates the memory for us with NULL and 0 parameters.
                 */
                linelen = getline(&input_string, &buffsize, stdin);

                if (linelen == -1) { /* if no text and CTRL-D, getline fails */
                        DEBUG(1) printf("Empty input string, OK, exit.\n");
                        break;
                }

                /* useful to print if input was read from a file kdr dict.txt < input.txt */
                DEBUG(1) printf("Input : %s", input_string);


/* 4. Reserve output string */
                if (number_of_keys) { /* only if dictionary wasn't empty */
                        output_string = malloc(MAX_OUTPUT_STRING);
                        if (output_string == NULL) {
                                printf("Allocation error (output_string %d bytes) errno %d, exit.\n",
                                    MAX_OUTPUT_STRING, errno);
                                status = EXIT_FAILURE;
                                break;
                        }
                }

/* 5. Process input string */
                if (number_of_keys) { /* only if dictionary wasn't empty */
                        status = kdr_process_input(input_string, output_string);
                        if (status)
                                printf("\nPartially successful.\n");
                }

/* 6. Write the result */
                DEBUG(1) printf("Output: ");
                /*
                 * If there were no keys, just print the input_string.
                 */
                printf("%s", number_of_keys ? output_string : input_string);
                /*
                 * If partially successful, needs to print EOL because
                 * it was not included in the output_string.
                 */
                if (status)
                        printf("\n");

        } while (0); /* execute just once, allow "break;" to exit */


/* 7. Cleanup, deallocate reserved memory, exit */
        if (input_string)
                free(input_string);
        if (output_string)
                free(output_string);
        for (key_ind = 0; key_ind < number_of_keys; key_ind++)
                if (key_value_data[key_ind].dictionary_line)
                        free(key_value_data[key_ind].dictionary_line);

        exit(status);

} /* main */

/*
 * END kdr_main.c
 */
