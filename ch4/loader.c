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
static void load_symbols_bfd(bfd * bfd_handle, Binary * bin);
static void load_dynsym_bfd(bfd * bfd_handle, Binary * bin);
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
	(*bin) = (Binary*)calloc(1, sizeof(Binary));

	/* allocate the filename memory to a char array of the right length, then copy the string in */
	(*bin)->filename = (char*)malloc(strlen(fname)+1);
	strcpy((*bin)->filename, fname);

	/* log the entry point */
	(*bin)->entry = bfd_get_start_address(bfd_handle);

	/* log the type string */
	(*bin)->type_str = (char *)malloc(strlen(bfd_handle->xvec->name) + 1);
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
	(*bin)->arch_str = (char *)malloc(strlen(bfd_info->printable_name) + 1);
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
	load_dynsym_bfd(bfd_handle, (*bin));

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

static void load_symbols_bfd(bfd * bfd_handle, Binary * bin)
{

}

static void load_dynsym_bfd(bfd * bfd_handle, Binary * bin)
{

}

static int load_sections_bfd(bfd * bfd_handle, Binary * bin)
{
	return 0;
}
