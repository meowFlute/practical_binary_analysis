#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
       		printf("Binary pointer after load_binary(): %p\n", bin);
	}
	else
	{
		printf("give me a filename of a binary\n");
		return 1;
	}

	return 0;
}
