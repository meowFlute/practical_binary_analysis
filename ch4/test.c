#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

#include "loader.h"

int main(int argc, char * argv[])
{
	unsigned int i;
	Binary * bin = NULL;
	char * filename = NULL;

	if(argc == 2)
	{
		/* allocate a buffer for filename */
		filename = malloc(strlen(argv[1]) + 1);
		if(!(filename) && (strlen(argv[1]) + 1))
		{
			printf("malloc of filename failed in test.c\n");
			return 1;
		}
		strcpy(filename, argv[1]);
	
		/* pass that to the binary loader */
		load_binary(filename, &bin);

		/* don't need filename anymore, free it */
		free(filename);
		filename = NULL;

		/* what does our bin contain? */
		/* general */
		printf("file: %s\n", bin->filename);
		printf("type: %s, machine: %dbit %s, entry point: 0x%" PRIx64 "\n", bin->type_str, bin->bits, bin->arch_str, bin->entry);
		/* sections */
		printf("Sections:\n");
		printf("\t%-16s   %-8s %-30s %s\n", "ADDRESS", "SIZE", "NAME", "TYPE");
		printf("\t%-16s   %-8s %-30s %s\n", "-------", "----", "----", "----");
		for(i = 0; i < bin->num_sections; i++)
		{
			printf("\t0x%-16lx %-8lu %-30s %s\n", 
					bin->sections[i]->vma, 
					bin->sections[i]->size, 
					bin->sections[i]->name, 
					bin->sections[i]->type == SEC_TYPE_CODE ? "CODE" : "DATA");
		}
		/* symbols */
		printf("Function Symbols:\n");
		printf("\tTYPE \t%-40s \tADDRESS\n", "NAME");
		printf("\t---- \t%-40s \t-------\n", "----");
		for(i = 0; i < bin->num_symbols; i++)
		{
			if(bin->symbols[i]->type == SYM_TYPE_FUNC)
				printf("\tFUNC \t%-40s \t0x%" PRIx64 "\n", bin->symbols[i]->name, bin->symbols[i]->addr); 
		}

		/* now we destroy everything */
		unload_binary(&bin);
	}
	else
	{
		printf("give me a filename of a binary\n");
		return 1;
	}

	return 0;
}
