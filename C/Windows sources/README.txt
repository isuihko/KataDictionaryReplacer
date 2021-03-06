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