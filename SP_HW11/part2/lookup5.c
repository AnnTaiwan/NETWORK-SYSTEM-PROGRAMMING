/*
 * lookup5 : local file ; setup a memory map of the file
 *           Use bsearch. We assume that the words are already
 *           in dictionary order in a file of fixed-size records
 *           of type Dictrec
 *           The name of the file is what is passed as resource
 */

#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <sys/mman.h>

#include "dict.h"

/*
 * This obscure looking function will be useful with bsearch
 * It compares a word with the word part of a Dictrec
 */

int dict_cmp(const void *a,const void *b) {
    return strcmp((char *)a,((Dictrec *)b)->word);
}

int lookup(Dictrec * sought, const char * resource) {
	static Dictrec * table;
	static int numrec;
	Dictrec * found;
	static int first_time = 1;

	if (first_time) {  /* table ends up pointing at mmap of file */
		int fd;
		long filsiz;

		first_time = 0;

		/* Open the dictionary.
		 * Fill in code. */
        if ((fd = open(resource, O_RDONLY, NULL)) == -1)
            DIE("lookup5.c:open");
		/* Get record count for building the tree. */
		filsiz = lseek(fd,0L,SEEK_END);
		numrec = filsiz / sizeof(Dictrec);

		/* mmap the data.
		 * Fill in code. */
		// mapping virtual memory to src file
		table = mmap(NULL, filsiz, PROT_READ, MAP_PRIVATE, fd, 0);
        if(table == MAP_FAILED)
            DIE("lookup5.c:mmap");
		close(fd);
	}
    
	/* search table using bsearch
	 * Fill in code. */
	// using bsearch with dict_cmp compare function
    found = bsearch(sought->word, table, numrec, sizeof(Dictrec), dict_cmp); // Return NULL if not found
	if (found) {
		strcpy(sought->text,found->text);
		return FOUND;
	}

	return NOTFOUND;
}
