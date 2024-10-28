/*
 * lookup1 : straight linear search through a local file
 * 	         of fixed length records. The file name is passed
 *	         as resource.
 */
#include <string.h>
#include "dict.h"
#include <stdlib.h>
int lookup(Dictrec * sought, const char * resource) {
	Dictrec dr;
	static FILE * in;
	static int first_time = 1;

	if (first_time) { 
		first_time = 0;
		/* open up the file
		 *
		 * Fill in code. */
		 if ((in = fopen(resource,"r")) == NULL){DIE(resource);}
	}

	/* read from top of file, looking for match
	 *
	 * Fill in code. */
	rewind(in); // The `rewind()` function sets the file position indicator for the stream pointed to by stream to the beginning of the file.
	while(fread(&dr, sizeof(Dictrec), 1, in) != 0) 
	{
		/* Fill in code. */
		//printf("WORD: %s\n", sought -> word);
		//printf("RRRR: %s\n", dr.word);
		if(strcmp(sought -> word, dr.word) == 0) // found the word
		{
		    strcpy(sought -> text, dr.text);
		    return FOUND;    
		}
	}

	return NOTFOUND;
}
