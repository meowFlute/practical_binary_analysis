#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

#include "loader.h"

int main(int argc, char * argv[])
{
	Binary * bin = NULL;
	char * filename = NULL;

	if(argc == 2)
	{
		printf("file to open: %s\n", argv[1]);
	
		printf("filename pre allocation and copy %s\n", filename);

		filename = (char *)malloc(strlen(argv[1]) + 1);
		strcpy(filename, argv[1]);
	
		printf("filename after allocation and copy %s\n", filename);
	
		printf("\nBinary pointer prior to load_binary(): %p\n", bin);
		printf("return from loading: %d\n", load_binary(filename, &bin));

		free(filename);
		filename = NULL;

       		printf("Binary pointer after load_binary(): %p\n", bin);

		/* what does our bin contain? */
		printf("bin->filename: %s\n", bin->filename);
		printf("bin->type: %d (%s)\n", bin->type, bin->type_str);
		printf("bin->arch: %d (%s)\n", bin->arch, bin->arch_str);
		printf("bin->bits: %d\n", bin->bits);
		printf("bin->entry: 0x%" PRIx64 "\n", bin->entry);

		/* now we destroy everything */
		unload_binary(&bin);
		printf("\nBinary pointer after unload_binary(): %p\n", bin);
	}
	else
	{
		printf("give me a filename of a binary\n");
		return 1;
	}

	return 0;
}
