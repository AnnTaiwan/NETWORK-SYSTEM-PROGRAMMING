/*
 * convert.c : take a file in the form 
 *  word1
 *  multiline definition of word1
 *  stretching over several lines, 
 * followed by a blank line
 * word2....etc
 * convert into a file of fixed-length records
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "dict.h"
#define BIGLINE 512

int main(int argc, char **argv) {
	FILE *in;
	FILE *out;        /* defaults */
	char line[BIGLINE];
	static Dictrec dr; //, blank; // unused
	
	/* If args are supplied, argv[1] is for input, argv[2] for output */
	if (argc==3) {
		if ((in =fopen(argv[1],"r")) == NULL){DIE(argv[1]);}
		if ((out =fopen(argv[2],"w")) == NULL){DIE(argv[2]);}	
	}
	else{
		printf("Usage: convert [input file] [output file].\n");
		return -1;
	}

	/* Main reading loop : read word first, then definition into dr */

	/* Loop through the whole file. */
	while (!feof(in)) {
		
		/* Create and fill in a new blank record.
		 * First get a word and put it in the word field, then get the definition
		 * and put it in the text field at the right offset.  Pad the unused chars
		 * in both fields with nulls.
		 */
		
		/* Read word and put in record.  Truncate at the end of the "word" field.
		 *
		 * Fill in code. */
		fgets(line, sizeof(line), in); // line contains '\n' in the end.
		if(strcmp(line, "\n") == 0) // no more word
		    break;
		line[strlen(line) - 1] = '\0'; // remove the '\n'
                strcpy(dr.word, line);
                for(int i = strlen(line); i < WORD; i++)
                {
                    dr.word[i] = '\0'; // padded to 32 bytes with null
                }
		/* Read definition, line by line, and put in record.
		 *
		 * Fill in code. */
		memset(dr.text, 0, sizeof(dr.text)); // initialize the text field
                while(fgets(line, sizeof(line), in) && strcmp(line, "\n") != 0) // read line until reaching blank line
		{
		    
		    line[strlen(line) - 1] = '\0'; // remove the '\n'
                    int left_space = TEXT - strlen(dr.text) - 1; // save one byte for last '\0'
                    int num = (left_space > strlen(line)) ? strlen(line) : left_space;
                    strncat(dr.text, line, num);
                }
                // padding the rest of text field
                for(int i = strlen(dr.text); i < TEXT; i++)
                {
                     dr.text[i] = '\0'; // padded to 32 bytes with null
                }
		/* Write record out to file.
		 *
		 * Fill in code. */
		if (fwrite(&dr, sizeof(Dictrec), 1, out) != 1) 
		{
                    perror("Failed to write record");
                    fclose(in);
                    fclose(out);
                    return -1;
                }

                /* Clear `dr` for the next record, setting it to a blank state */
                memset(&dr, 0, sizeof(Dictrec));
	}

	fclose(in);
	fclose(out);
	return 0;
}
