#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <ctype.h>
#include <getopt.h>

#include "loader.h"

static void print_help(void);

int main(int argc, char * argv[])
{
	unsigned char s_flag = 0U, d_flag = 0U, f_flag = 0U, x_flag = 0U;
	char* x_secname = NULL;
	unsigned int i;
	int c, index;
	Binary * bin = NULL;
	char * filename = NULL;	

	struct option long_options[] = 
	{
		{"func_symbols", 	no_argument, 		NULL, 	'f'},
		{"important_sections", 	no_argument, 		NULL, 	's'},
		{"data_symbols", 	no_argument, 		NULL, 	'd'},
		{"hex_dump", 		required_argument, 	NULL, 	'x'},
		{"h", 			no_argument, 		NULL, 	'h'},
		{"help",		no_argument,		NULL, 	'h'},
		{0, 0, 0, 0}
	};

	while ((c = getopt_long(argc, argv, "sdfx:h", long_options, &index)) != -1)
	{
    		switch (c)
		{
			case 's':
				s_flag = 1;
				break;
			case 'd':
				d_flag = 1;
				break;
			case 'f':
				f_flag = 1;
				break;
			case 'x':
				x_flag = 1;
				x_secname = optarg;
				break;
			case 'h':
				print_help();
				return 0;
			case '?':
				print_help();
				return 1;
			default:
				abort();	
		}
	}

	printf ("s_flag = %d, d_flag = %d, f_flag = %d, x_flag = %d, x_secname = %s\n", s_flag, d_flag, f_flag, x_flag, x_secname);

  	for (index = optind; index < argc; index++)
		printf ("Non-option argument %s\n", argv[index]);

	printf("optind = %d, argc = %d\n", optind, argc);

  	return 0;
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

static void print_help(void)
{
	printf("Usage: loader [options] binary\n");
	printf("Options:\n");
	printf("\t%-40s %s\n", "-f (--func_symbols)", "display function symbols");
	printf("\t%-40s %s\n", "-d (--data_symbols)", "display data symbols");
	printf("\t%-40s %s\n", "-s (--important_sections)", "display code and data sections");
	printf("\t%-40s %s\n", "-x (--hex_dump) <.section_name>", "display hex dump of section passed as argument");
	printf("\t%-40s %s\n\n", "-h (--h or --help)", "display this information and return with no action"); 
}
