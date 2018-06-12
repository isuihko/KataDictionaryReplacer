/********************************************************************************

    kdr_build_dictionary.c
 
    Solution for the following Kata problem
        http://codingdojo.org/cgi-bin/index.pl?KataDictionaryReplacer

    Written in C language by Ismo Suihko, Espoo Finland, 2017-01-01
   
    More information in the main module kdr_main.c

    Global routines in this module: 
        int read_dictionary_data(char *filename)

********************************************************************************/

#include "kdr.h"

/*
 * parse line 
 *      Search for the key and value pairs like
 *             "temp","temporary"  or dict ["temp", "temporary"]
 *             "name","John Doe"   or dict ["name", "John Doe"]
 *
 * If format is illegal, an error message is printed and error status is returned.
 *
 * Input:  Pointer to a key_table which contains dictionary line.
 *              E.g. "temp","temporary"
 *         line_index  used in error messages and debug printings
 *
 * Output: Return status EXIT_SUCCESS if OK, otherwise EXIT_FAILURE.
 *
 * 	   Sets key and value pointers of the key_table data structure so
 *         that they point to correct places in the dictionary line.
 *         The key is changed from "key" to $key$, because then it
 *         is easier to use for searching keys from the input string.
 *	       	keyp -> $temp$, valuep -> temporary
 *
 */
static int parse_line(struct key_table *key_tablep, int line_index)
{
	int status = EXIT_SUCCESS; /* OK */

	char *line;
	int linelen;

	int index, start_index;
	int n_of_quatations = 0;

	/*
	 * check the parameter for internal error before using it
	 */
	if (key_tablep == NULL) {
                printf(">parse_line INTERNAL ERROR, tablep NULL\n");
		return EXIT_FAILURE;
	}

	line    = key_tablep->dictionary_line;
	linelen = key_tablep->dictionary_len;

	DEBUG(3) printf(">parse_line(line %d: '%s')\n", line_index, line);

	/*     "key","value"
	 *     0123456789012
	 * 
	 *     dict ["temp", "temporary"]  creates pair key=$temp$ value=temporary
	 *     dict empty   creates empty table
	 */
	for (index = 0; index < linelen && !status; index++) {
		if (line[index] == '"') {
			DEBUG(5) printf("parse index %d, n %d\n", index, n_of_quatations);

			switch (n_of_quatations) {

			case 0: /* 1st = the start of the key */
				line[index] = '$';   /* change " into $ for a key */
				key_tablep->keyp = &line[index];
				start_index = index;
				break;

			case 1: /* 2nd = end of the key */
				if (line[index+1] != ',') {
					printf("format error: needs a comma between key and value\n");
					status = 1;
					break;
				}
				key_tablep->keylen = index - start_index + 1;
				line[index] = '$';     /* $key$ */
				line[index+1] = '\0';  /* end of the key */
				break;

			case 2: /* 3rd = start of the value, skip " */
				key_tablep->valuep = &line[index+1];
				start_index = index + 1;  /* skip " */
				break;

			case 3: /* 4th = end of the value */
				line[index] = '\0';
				key_tablep->valuelen = index - start_index;
				break;

			default:
				/* error */
				break;
			}
			n_of_quatations++;
		}
	}

	if (n_of_quatations != 4) {
		printf("Format error in line %d: should be \"key\",\"value\" in each line.\n",
			line_index);
		printf("Only empty lines and lines starting with # are ignored\n");
		status = 1;
	}
	DEBUG(3) printf("parse_line status %d\n", status);

	return status;

} /* parse_line */


/*
 * reads a file and parses dictionary data into key_value_data[] global table.
 *
 * Input:  A filename to be opened and read. Should contain a dictionary.
 * Output: Return status EXIT_SUCCESS if OK, otherwise EXIT_FAILURE.
 */
int kdr_build_dictionary(char *filename)
{
	int    status = EXIT_SUCCESS; /* OK */
	FILE   *fp;

#ifndef DONT_CHECK_DUPLICATE /* could be removed for debugging */
	int    key_ind, duplicate_key;
#endif

	/* for using getline(), initialized each time before calling */
	char   *linebuffer;
	size_t  buffsize;
	ssize_t linelen; /* output type of getline(), signed */

	int line_index = 0;

	if (filename == NULL) {
                printf(">build_dictionary INTERNAL ERROR, filename NULL\n");
		return EXIT_FAILURE;
	}

	DEBUG(3) printf(">build_dictionary_data(file '%s')\n", filename);

	/* open the dictionary file */
	errno = 0;
	fp = fopen(filename, "r");
	if (fp == NULL) {
		printf("Error opening file '%s' (errno %d)\n", filename, errno);
		return EXIT_FAILURE;
	}

	/* clear the whole dictionary table */
	bzero(key_value_data, sizeof(key_value_data));

	/*
	 * read all the lines from the dictionary file
	 * and parse the key and value pairs
	 */
	while (number_of_keys < MAX_DICT_KEYS) {
		/*
		 * getline(3) reads the entire line
		 * returns the number of characters read including EOL
		 * If given NULL pointer and zero buffsize, allocates the buffer.
		 *
		 * NOTE! The allocated memory needs to be freed before exiting the program.
		 */
		buffsize = 0;
		linebuffer = NULL;
		linelen = getline(&linebuffer, &buffsize, fp);
		line_index++;

		if (linelen < 1) /* end of file, not even EOL */ {
			if (linebuffer)
				free(linebuffer);
			break;
		}

		linelen--;
		linebuffer[linelen] = '\0'; /* clear EOL byte */

		DEBUG(4) printf("len %d : %s\n", linelen, linebuffer);

		/* skip the lines starting with #, also skip empty lines */
		if (linebuffer[0] == '#' || linelen == 0) {
			if (linebuffer)
				free(linebuffer);
			continue;
		}

		/* store the line */
		key_value_data[number_of_keys].dictionary_line = linebuffer;
		key_value_data[number_of_keys].dictionary_len  = linelen;

		/* parse the line and exit if error status */
		status = parse_line(&key_value_data[number_of_keys], line_index);
		if (status) {
			if (linebuffer)
				free(linebuffer);
			/* lines stored arelier are freed in main */
			break;
		}

#ifndef DONT_CHECK_DUPLICATE /* could be removed for debugging */
		/*
		 * check for duplicate keys
		 */
		duplicate_key = 0;
		for (key_ind = 0; key_ind < number_of_keys - 1; key_ind++) {
			if (!strcmp(key_value_data[key_ind].keyp, key_value_data[number_of_keys].keyp)) {
				duplicate_key = 1;
				break;
			}
		}
		if (duplicate_key) {
			printf("Duplicate found for key '%s' (already '%s'). "
				"Ignore value '%s' in line %d.\n",
				key_value_data[key_ind].keyp,
				key_value_data[key_ind].valuep,
				key_value_data[number_of_keys].valuep,
				line_index);
			if (linebuffer)
				free(linebuffer); /* free ignored line */

			/*
			 * Actually no need to clear the latest key_value_data table entry,
			 * but do it anyway.
			 */ 
			bzero(&key_value_data[number_of_keys], sizeof(struct key_table));

			continue; /* Continue checking, don't increase the key counter */
		}
#endif

		number_of_keys++;

		/*
		 * XXX: Reached maximum number of key_value entries.
		 * XXX: Expand the dictionary size if needed (realloc). Or use a linked list.
		 * XXX: At least can modify Makefile to compile with bigger table.
		 */
		if (number_of_keys == MAX_DICT_KEYS)
			printf("Maximum number of keys (%d), rest could be ignored after line %d.\n",
				number_of_keys, line_index);

	} /* while number_of_keys < MAX_DICT_KEYS */

	fclose(fp);

	DEBUG(3) printf(">build_dictionary_data status %d, keys read %d\n",
			status, number_of_keys);

	return status;

} /* kdr_build_dictionary */

/*
 * END kdr_build_dictionary.c
 */
