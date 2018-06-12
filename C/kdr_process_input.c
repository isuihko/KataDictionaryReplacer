/********************************************************************************
 
    kdr_process_input.c
 
    Solution for the following Kata problem
        http://codingdojo.org/cgi-bin/index.pl?KataDictionaryReplacer

    Written in C language by Ismo Suihko, Espoo Finland, 2017-01-01
   
    More information in the main module kdr_main.c

    Global routines in this module: 
        int kdr_process_input(char *input_string, char *output_string)

********************************************************************************/

#include "kdr.h"

/*
 * This routine goes through the input_string and replaces all the keys
 * marked as $name_of_the_key$ with the value associated with it and
 * builds the output_string.
 *
 * XXX: TODO: expand the size of the output_string buffer if needed, realloc()
 *
 * Input:  pointer to input string = text given by a user
 *         output string pointer for new text string
 *
 * Output: Return status EXIT_SUCCESS if OK, otherwise EXIT_FAILURE.
 *
 * Note! output_string has been reserved before coming here.
 */
int kdr_process_input(char *input_string, char *output_string)
{
	int   status = EXIT_SUCCESS;

	/* input buffer pointer and size */
	char  *input_ptr;
	int   input_size;

	/* output buffer pointer and sizes */
	char  *output_ptr;
	int   output_size;
	int   max_output_size = MAX_OUTPUT_STRING - 1; /* leave space for '\0' */

	/* flags */
	int   dollar_found, key_found;

	int   start_index, input_offset, key_ind;

	/* pointers and lengths of key,value */
	char   *search_key;
	int    keylen;
	char   *value;
	int    valuelen;

	DEBUG(3) printf(">process_input()\n");

	/*
	 * check the parameters
	 */
	if (input_string == NULL || output_string == NULL) {
		printf(">process_input() INTERNAL ERROR, NULL parameter %x,%x\n",
			input_string, output_string);
		return EXIT_FAILURE;
	}

	/* Search keys from the input string and replace them with the value.
	 * Keys are inside '$' like $key$, $name$, $price$ and even $price of car$
	 *
	 * Note! Key can have $, so even a key "price $50" might have been
	 *       defined in a dictionary.
	 *
	 * Build output_string. Check that the max length of the buffer is not reached.
	 */
	output_size = 0;
	start_index = 0;
	input_offset = 0;

	/* use temporary pointers */
	input_ptr  = input_string;
	input_size = strlen(input_string);
	output_ptr = output_string;

	while (output_size < max_output_size) {

		dollar_found = 0;
		while ((input_offset < input_size) && (output_size < max_output_size)) {
			if (*input_ptr == '$') {
				dollar_found = 1;
				break; /* dollar not stored, if it starts a key */
			}
			*output_ptr++ = *input_ptr++;		/* STORE */
			output_size++;
			input_offset++;

			DEBUG(5) putchar('.');
		}

		if (output_size >= max_output_size) {
			printf("\nOutput buffer exceeded (1)\n");
			break;
		}

		if (!dollar_found)  /* nothing to replace (at all or any more) */
			break;      /* Ready! */

		/* input_ptr points now to a possible key */
		DEBUG(4) printf("is there a key in offset %d '%s'\n",
				input_offset, input_ptr);

		/*
		 * input_offset has index to $key$ in input string,
		 * will be replace with the corresponding value.
		 *
		 * Compare all the known keys to the current text string
		 * until a first match is found.
		 */
		for (key_ind = 0; key_ind < number_of_keys; key_ind++) {
			/*
			 * check for each search key
			 */
			search_key = key_value_data[key_ind].keyp;
			keylen     = key_value_data[key_ind].keylen;

			DEBUG(4) printf("Is it [%d] '%s', len %d\n", key_ind, search_key, keylen);

			/*
			 * find the keys from the input,
			 * use int strncmp(string, key, keylen) to compare
			 * returns zero, if same strings, and key_found is set to TRUE
			 */
			if (key_found = !strncmp(input_ptr, search_key, keylen))
				break;
		}

		/*
		 * '$' was found but it didn't start a known key.
		 * Store or not '$' to output_string? Decided to store!
		 */
		if (!key_found) {
			/*
			 * THESE THREE LINES ARE SAME AS IN WHILE LOOP EARLIER
			 */
			*output_ptr++ = *input_ptr++;		/* STORE */
			output_size++;
			input_offset++;

			DEBUG(5) putchar('$');
			continue;
		}

		value     = key_value_data[key_ind].valuep;
		valuelen  = key_value_data[key_ind].valuelen;

		DEBUG(4) printf("Found '%s' len %d from %s, offset %d, store to %d\n",
			    search_key, valuelen, input_ptr, input_offset, output_size);

		/*
		 * is there enough space for the value?
		 */
		if (output_size + valuelen < max_output_size) {
			/*
			 * search_key,value  keylen,valuelen
			 * pointer to input_string = beginning of the key
			*/
			strncpy(output_ptr, value, valuelen);	/* STORE */
			DEBUG(5) putchar('*');

			input_ptr += keylen; /* increase by length of the key */

			/* output pointer is increased by length of the value */
			output_ptr  += valuelen;
			output_size += valuelen;
		} else {
			output_size = max_output_size; /* exit loop, print error */
			printf("\nOutput buffer exceeded (2)\n");
			break;
		}
	}

	/* finish the text string, write zero after the last stored character */
	*output_ptr = '\0';

	if (output_size == max_output_size) {
		printf("MAX OUTPUT BUFFER SIZE REACHED (%d), ignored rest (%s)\n",
			max_output_size, input_ptr);
		status = EXIT_FAILURE;
	}

	DEBUG(3) printf("process_input status %d\n", status);

	return status;

} /* kdr_process_input */

/*
 * END kdr_process_input.c
 */
