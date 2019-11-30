#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <bfd.h>

#include "loader.h"

/* static function prototypes */
static void dtor_Symbol(Symbol ** symbol);
static void dtor_Section(Section  ** section);
static void dtor_Binary(Binary ** binary);
static bfd * open_bfd(char * filename);
static int load_symbols_bfd(bfd * bfd_handle, Binary * bin);
static int load_sections_bfd(bfd * bfd_handle, Binary * bin);

/* global function definitions */
int load_binary(char * fname, Binary ** bin)
{
	bfd * bfd_handle = NULL;
	const bfd_arch_info_type * bfd_info;

	bfd_handle = open_bfd(fname);
	if(!bfd_handle)
	{
		/* failed to open for some reason listed to stderr */
		return -1;
	}

	/* let's create our Binary struct and start filling it up */
	(*bin) = calloc(1, sizeof(Binary));
	if(!bin && sizeof(Binary))
	{
		fprintf(stderr, "out of memory, calloc of (*bin) failed\n");
		return -1;
	}

	/* allocate the filename memory to a char array of the right length, then copy the string in */
	(*bin)->filename = malloc(strlen(fname)+1);
	if(!((*bin)->filename) && (strlen(fname)+1))
	{
		dtor_Binary(bin);
		fprintf(stderr, "out of memory, malloc of (*bin)->filename failed\n");
		return -1;
	}
	strcpy((*bin)->filename, fname);

	/* log the entry point */
	(*bin)->entry = bfd_get_start_address(bfd_handle);

	/* log the type string */
	(*bin)->type_str = malloc(strlen(bfd_handle->xvec->name) + 1);
	if(!((*bin)->type_str) && (strlen(bfd_handle->xvec->name) + 1))
	{
		dtor_Binary(bin);
		fprintf(stderr, "out of memory, malloc of (*bin)->type_str failed\n");
		return -1;
	}
	strcpy((*bin)->type_str, bfd_handle->xvec->name);

	switch(bfd_handle->xvec->flavour)
	{
		case bfd_target_elf_flavour:
			(*bin)->type = BIN_TYPE_ELF;
			break;
		case bfd_target_coff_flavour:
			(*bin)->type = BIN_TYPE_PE;
			break;
		case bfd_target_unknown_flavour:
		default:
			fprintf(stderr, "unsupported binary type: %d (%s)", bfd_handle->xvec->flavour, bfd_handle->xvec->name);
			return -1;	
	}

	/* point bfd_info at the right architecture info struct */
	bfd_info = bfd_get_arch_info(bfd_handle);
	
	/* grab the architecture string */
	(*bin)->arch_str = malloc(strlen(bfd_info->printable_name) + 1);
	if(!((*bin)->arch_str) && (strlen(bfd_info->printable_name) + 1))
	{
		dtor_Binary(bin);
		fprintf(stderr, "out of memory, malloc of (*bin)->arch_str failed\n");
		return -1;
	}
	strcpy((*bin)->arch_str, bfd_info->printable_name);
	
	/* machine type */
	switch(bfd_info->mach)
	{
		case bfd_mach_i386_i386:
			(*bin)->arch = ARCH_X86;
			(*bin)->bits = 32;
			break;
		case bfd_mach_x86_64:
			(*bin)->arch = ARCH_X86;
			(*bin)->bits = 64;
			break;
		default:
			fprintf(stderr, "unsupported architecture (%s)\n", bfd_info->printable_name);
			return -1;
	}

	load_symbols_bfd(bfd_handle, (*bin));

	if(load_sections_bfd(bfd_handle, (*bin)) < 0)
		return -1;

	if(bfd_handle)
		bfd_close(bfd_handle);

	return 0;
}

void unload_binary(Binary ** bin)
{
	/* calling dtor_Binary should be sufficient because it recursively frees all levels with valid pointers */
	dtor_Binary(bin);
}

/* static function definitions */
static void dtor_Symbol(Symbol ** symbol)
{
	Symbol * sym = *symbol;

	if(sym)
	{
		if(sym->name)
		{
			free(sym->name);
			sym->name = NULL;
		}
		free(sym);
		*symbol = NULL;
	}
	else
	{
		fprintf(stderr, "ERROR: From function \"dtor_Symbol()\":\n\tnull symbol pointer\n");
		return; /* null symbol pointer */
	}

	return;
}

static void dtor_Section(Section ** section)
{
	Section * sec = *section;

	if(sec)
	{
		if(sec->name)
		{
			free(sec->name);
			sec->name = NULL;
		}
		if(sec->bytes)
		{
			free(sec->bytes);
			sec->bytes = NULL;
		}
		free(sec);
		*section = NULL;
	}
	else
	{
		fprintf(stderr, "ERROR: From function \"dtor_Section()\":\n\tnull section pointer\n");
		return; /* null section pointer pointer */
	}

	return;
}

static void dtor_Binary(Binary ** binary)
{
	Binary * bin = *binary;
	unsigned int i;

	if(bin)
	{
		if(bin->filename)
		{
			free(bin->filename);
			bin->filename = NULL;
		}
		if(bin->type_str)
		{
			free(bin->type_str);
			bin->type_str = NULL;
		}
		if(bin->arch_str)
		{
			free(bin->arch_str);
			bin->arch_str = NULL;
		}
		if(bin->sections)
		{
			for(i = 0U; i < bin->num_sections; i++)
			{
				dtor_Section(&(bin->sections[i]));		
			}
			free(bin->sections);
			bin->sections = NULL;
		}
		if(bin->symbols)
		{
			for(i = 0U; i < bin->num_symbols; i++)
			{
				dtor_Symbol(&(bin->symbols[i]));
			}
			free(bin->symbols);
			bin->symbols = NULL;
		}	
		free(bin);
		*binary = NULL;
	}
	else
	{
		fprintf(stderr, "ERROR: From function \"dtor_Binary()\":\n\tnull binary pointer\n");
	}

	return;	
}

static bfd * open_bfd(char * filename)
{
	static int bfd_initialized = 0;
	bfd * bfd_handle;

	/* initialize libbfd internal state once */
	if(!bfd_initialized++)
		bfd_init();
	
	/* get pointer to bfd struct representing file */
	bfd_handle = bfd_openr(filename, NULL);

	/* error handling for NULL pointer return from libbfd */
	if(!bfd_handle)
	{
		fprintf(stderr, "failed to open binary \"%s\"\n\tREPORTED BFD ERROR: %s\n", filename, bfd_errmsg(bfd_get_error()));
		return NULL;
	}

	/* error handling for wrong unkown file */
	if(!bfd_check_format(bfd_handle, bfd_object))
	{
		fprintf(stderr, "file \"%s\" doesn't look like an executable\n", filename);
		return NULL;
	}

	/* bfd error checking can be whacky sometimes, so if we get to this point and an error is set we unset it */
	bfd_set_error(bfd_error_no_error);

	if(bfd_get_flavour(bfd_handle) == bfd_target_unknown_flavour)
	{
		fprintf(stderr, "unrecognized format for binary \"%s\"\n\tREPORTED BFD ERROR: %s\n", filename, bfd_errmsg(bfd_get_error()));
		return NULL;
	}

	return bfd_handle;
}

static int load_symbols_bfd(bfd * bfd_handle, Binary * bin)
{
	int err = 0;
	long n, nsyms, nsyms_dyn, i, j;
        long nfuncsyms = 0L, nfuncsyms_dyn = 0L;
	asymbol ** bfd_symtab = NULL;
	asymbol ** bfd_dynsym = NULL;

	/* first we load up the static symbols */
	n = bfd_get_symtab_upper_bound(bfd_handle);
	if(n < 0)
	{
		fprintf(stderr, "failed to read symtab\n\tERROR MESSAGE: (%s)\n", bfd_errmsg(bfd_get_error()));
		return -1;
	}	
	else if(n)
	{
		bfd_symtab = malloc(n);
		if(!bfd_symtab && n)
		{
			fprintf(stderr, "out of memory, malloc of bfd_symtab failed\n");
			return -1;
		}
		
		nsyms = bfd_canonicalize_symtab(bfd_handle, bfd_symtab);
		if(nsyms < 0)
		{
			fprintf(stderr, "failed to read symbol table\n\tERROR MESSAGE: (%s)\n", bfd_errmsg(bfd_get_error()));
			free(bfd_symtab);
			bfd_symtab = NULL;
			err = 1;
		}
		/* on the first pass, we'll figure out how many function symbols we have */	
		for(i = 0; i < nsyms; i++)
		{
			if(bfd_symtab[i]->flags & BSF_FUNCTION)
			{
				nfuncsyms++;		
			}
		}
	}

	/* then we load up the dynamic symbols */
	n = bfd_get_dynamic_symtab_upper_bound(bfd_handle);
	if(n < 0)
	{
		fprintf(stderr, "failed to read dynamic symtab\n\tERROR MESSAGE: (%s)\n", bfd_errmsg(bfd_get_error()));
		return -1;
	}	
	else if(n)
	{
		bfd_dynsym = malloc(n);
		if(!bfd_dynsym && n)
		{
			fprintf(stderr, "out of memory, malloc bfd_dynsym failed\n");
			free(bfd_symtab);
			return -1;
		}
		
		nsyms_dyn = bfd_canonicalize_dynamic_symtab(bfd_handle, bfd_dynsym);
		if(nsyms_dyn < 0)
		{
			fprintf(stderr, "failed to read symbol table\n\tERROR MESSAGE: (%s)\n", bfd_errmsg(bfd_get_error()));
			free(bfd_dynsym);
			bfd_dynsym = NULL;
			err = 2;
		}
		/* on the first pass, we'll figure out how many function symbols we have */	
		for(i = 0; i < nsyms_dyn; i++)
		{
			if(bfd_dynsym[i]->flags & BSF_FUNCTION)
			{
				nfuncsyms_dyn++;		
			}
		}
	}

	/* now we can allocate a Symbol pointer array and load the function symbols from both sections */
	bin->symbols = calloc(sizeof(Symbol*) * (nfuncsyms + nfuncsyms_dyn), 1);
	if(!(bin->symbols) && (sizeof(Symbol*) * (nfuncsyms + nfuncsyms_dyn)))
	{
		fprintf(stderr, "out of memory, malloc bin->symbols failed\n");
		free(bfd_symtab);
		free(bfd_dynsym);
		return -1;
	}
	bin->num_symbols = (int)(nfuncsyms + nfuncsyms_dyn);
	for(i = 0; i < bin->num_symbols; i++)
	{
		bin->symbols[i] = malloc(sizeof(Symbol));
		if(!(bin->symbols[i]) && sizeof(Symbol))
		{
			fprintf(stderr, "out of memory, malloc bin->symbols[%ld] failed\n", i);
			free(bfd_symtab);
			free(bfd_dynsym);
			free(bin->symbols);
			bin->symbols = NULL;
			return -1;
		}
		bin->symbols[i]->type = SYM_TYPE_FUNC;
		if(i < nfuncsyms)
		{	
			bin->symbols[i]->name = malloc(strlen(bfd_symtab[i]->name) + 1);
			if(!(bin->symbols[i]->name) && (strlen(bfd_symtab[i]->name) + 1))
			{
				fprintf(stderr, "out of memory, malloc of bin->symbols[%ld]->name failed\n", i);
				free(bfd_symtab);
				free(bfd_dynsym);
				for(j = 0; j < i; j++)
				{
					free(bin->symbols[j]);
					bin->symbols[j] = NULL;
				}
				free(bin->symbols);
				bin->symbols = NULL;
				return -1;
			}
			strcpy(bin->symbols[i]->name, bfd_symtab[i]->name);
			bin->symbols[i]->addr = bfd_asymbol_value(bfd_symtab[i]);
		}
		else
		{
			bin->symbols[i]->name = malloc(strlen(bfd_dynsym[i-nfuncsyms]->name) + 1);
			if(!(bin->symbols[i]->name) && (strlen(bfd_dynsym[i-nfuncsyms]->name) + 1))
			{
				fprintf(stderr, "out of memory, malloc of bin->symbols[%ld]->name failed\n", i);
				free(bfd_symtab);
				free(bfd_dynsym);
				for(j = 0; j < i; j++)
				{
					free(bin->symbols[j]);
					bin->symbols[j] = NULL;
				}
				free(bin->symbols);
				bin->symbols = NULL;
				return -1;
			}
			strcpy(bin->symbols[i]->name, bfd_dynsym[i-nfuncsyms]->name);
			bin->symbols[i]->addr = bfd_asymbol_value(bfd_dynsym[i-nfuncsyms]);
		}
	}
	
	free(bfd_dynsym);
	free(bfd_symtab);

	return err;
}

static int load_sections_bfd(bfd * bfd_handle, Binary * bin)
{
	return 0;
}
