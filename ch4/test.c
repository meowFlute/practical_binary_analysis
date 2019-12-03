#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <ctype.h>
#include <unistd.h>
#include <getopt.h>

#include "loader.h"

static void print_help(void);

int main(int argc, char * argv[])
{
	unsigned char s_flag = 0U, d_flag = 0U, f_flag = 0U, x_flag = 0U, x_read_flag = 0U;
	char* x_secname = NULL;
	unsigned int i, j;
	uint64_t x_addr, x_addr_start, x_idx, x_idx_start;
	int c, index, x_buf_idx;
	Binary * bin = NULL;
	char * filename = NULL;	
	char x_buffer[81];

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

	/* check for only one additional argument and see if it is a readable file you can access */
	if(((argc - optind) == 1) && (access(argv[optind], R_OK) != -1) && (s_flag || d_flag || f_flag || (x_flag && x_secname)))
	{
		/* rename it for clarity */
		filename = argv[optind];
		
		/* pass that to the binary loader */
		load_binary(filename, &bin);

		/* what does our bin contain? */
		/* general */
		printf("file: %s\n", bin->filename);
		printf("type: %s, machine: %dbit %s, entry point: 0x%" PRIx64 "\n", bin->type_str, bin->bits, bin->arch_str, bin->entry);
		
		/* displaying sections */
	        if(s_flag)
		{
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
		}

		/* function symbols */
		if(f_flag)
		{
			printf("Function Symbols:\n");
			printf("\tTYPE \t%-40s \tADDRESS\n", "NAME");
			printf("\t---- \t%-40s \t-------\n", "----");
			for(i = 0; i < bin->num_symbols; i++)
			{
				if(bin->symbols[i]->type == SYM_TYPE_FUNC)
					printf("\tFUNC \t%-40s \t0x%" PRIx64 "\n", bin->symbols[i]->name, bin->symbols[i]->addr); 
			}
		}
		
		/* data symbols */
		if(d_flag)
		{
			fprintf(stderr, "%s: data symbol reading not implemented yet!\n", argv[0]);
		}
		
		/* hex dumps of sections */
		if(x_flag)
		{
			/* find the section and dump its contents in some readable way */
			for(i = 0; i < bin->num_sections; i++)
			{
				if(strcmp(x_secname, bin->sections[i]->name) == 0)
				{
					x_read_flag = 1;

					printf("Hex Dump of %s (section type: %s, bytes dumped: %ld):\n\n", 
							bin->sections[i]->name, 
							bin->sections[i]->type == SEC_TYPE_CODE ? "CODE" : "DATA",
							bin->sections[i]->size);
					printf("%-20s%-82s%s\n", "VMA", "Hex Data", "Equivalent Chars");						
					printf("%-20s%-82s%s\n", "---", "--------", "----------------");
					
					x_addr = bin->sections[i]->vma;
					x_addr_start = x_addr;
					x_buf_idx = 0;
					for(x_idx = 0; x_idx < bin->sections[i]->size; x_idx++)
					{
						/* print in logical blocks with the virtual memory address annotated */
						if((x_idx != 0) && ((x_addr % 32) == 0))
						{
							x_buffer[++x_buf_idx] = '\0'; /* null terminate end of string */
							/* reprint as chars on the right like other hex dumps I've seen */
							printf("0x%016lx:", x_addr_start); 	/* 20 character virtual address */
							printf("%80s   ", x_buffer);
							memcpy(x_buffer, (bin->sections[i]->bytes + x_idx_start), x_idx - x_idx_start);
							for(j = 0; j < (x_idx - x_idx_start); j++)
							{
								if(isprint(x_buffer[j]))
									printf("%c", x_buffer[j]);
								else
									printf(".");
							}
							printf("\n");
							x_idx_start = x_idx;
							x_addr_start = x_addr;
							x_buf_idx = 0;
						}
						
						/* build up byte buffer in blocks of twos */
						if((x_addr % 2) == 0)
						{
							x_buffer[x_buf_idx++] = ' ';
						}

						/* always print a block */
						sprintf((x_buffer + x_buf_idx), "%02x", bin->sections[i]->bytes[x_idx]);
						x_buf_idx += 2;

						/* for every byte printed, increment the address by one */
						x_addr++;
					}
					x_buffer[++x_buf_idx] = '\0'; /* null terminate end of string */
					/* reprint as chars on the right like other hex dumps I've seen */
					printf("0x%016lx:", x_addr_start); 	/* 20 character virtual address */
					printf("%-80s   ", x_buffer);
					memcpy(x_buffer, (bin->sections[i]->bytes + x_idx_start), x_idx - x_idx_start);
					for(j = 0; j < (x_idx - x_idx_start); j++)
					{
						if(isprint(x_buffer[j]))
							printf("%c", x_buffer[j]);
						else
							printf(".");
					}
					printf("\n");
				}
			}

			if(!x_read_flag)
			{
				fprintf(stderr, "%s: section %s not found in %s for hex dump\n", argv[0], x_secname, filename);
			}
		}

		/* now we destroy everything */
		unload_binary(&bin);
	}
	else
	{
		print_help();
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
