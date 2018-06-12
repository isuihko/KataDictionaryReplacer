/********************************************************************************

    kdr.h
 
    Solution for the following Kata problem
        http://codingdojo.org/cgi-bin/index.pl?KataDictionaryReplacer

    Written in C language by Ismo Suihko, Espoo Finland, 2017-01-01
   
    More information in the main module kdr_main.c

********************************************************************************/

#ifndef _KDR_H_
#define _KDR_H_

#include <stdio.h>
#include <stdlib.h> /* malloc,free,exit */
#include <string.h> /* strlen */
#include <errno.h>  /* errno  */

/*
 * set some limits, could be overridden in Makefile (CDEFS)
 */
#ifndef MAX_DICT_KEYS
#define MAX_DICT_KEYS     100   /* max number of key,value pairs in a dictionary */
#endif

#ifndef MAX_OUTPUT_STRING
#define MAX_OUTPUT_STRING 128   /* max length of the output string buffer */
#endif

#define DEBUG(n) if (_debug_level >= n)

/*
 * Data structure for storing the dictionary entries
 */
struct key_table {
        /* line read from a dictionary file and length in bytes */
        char *dictionary_line;
        int  dictionary_len;

        /* pointer to a key in format $key$ and length in bytes */
        char *keyp;
        int  keylen;

        /* pointer to a value and length in bytes */
        char *valuep;
        int  valuelen;
};

/*
 * Global table in main module kdr_main.c for storing the dictionary
 */
extern int    number_of_keys;
extern struct key_table key_value_data[MAX_DICT_KEYS];

extern int    _debug_level; /* global debug level */

#endif /* _KDR_H_ */

/*
 * END kdr.h
 */
