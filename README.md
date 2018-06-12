# KataDictionaryReplacer

Files:
    kdr_main.c    main module
	  Makefile
	  kdr.h
	  kdr_build_dictionary.c
	  kdr_process_input.c
	  README
 
    Solution for the following Kata problem
        http://codingdojo.org/kata/DictionaryReplacer/

    Written in C language by Ismo Suihko, Espoo Finland, 2017-01-01
    Updates:  2017-01-23 (minor) Small adjustments to comments in the beginning
                          of kdr_main.c, so also README was updated.

   Compiled and tested in Fedora Linux using GNU C
	 gcc (GCC) 6.3.1 20161221 (Red Hat 6.3.1-1)


!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
!  Some changes to Kata problem:
!!
!!	  - dictionary is read from the text file (easy to modify) and in format
!!	         "key","value"
!!	    Also text file can have entries in format (to be compatible)
!!                dict ["temp", "temporary"]
!!
!!	  - "dict empty" means no key,value pair in a file i.e. empty file
!!	    or there are only comment lines starting with # or empty lines.
!!	    Then no conversions are made to the input text.
!!
!!	Input string is read from stdin and has one line. Can be read also
!!	from a file using <inputfile as in Linux usually.
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!


See README for more information about the usage and examples.
