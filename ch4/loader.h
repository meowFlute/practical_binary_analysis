#ifndef LOADER_H
#define LOADER_H

#include <stdint.h>

/* Loader.h
 * 
 * Description: This header file is for the loader from chapter 
 * 4 of "Practical Binary Analysis" written by Dennis Andriesse, 
 * but it is written in C instead of C++ 
 * 
 * Date Created: 11/26/2019
 * Author: Scott Christensen */

/* types */
/* types: enums */
/* types: enums: SymbolType
 * 	This enumeration is for describing the types of symbols we encounter
 * 	in the symbol table during static analysis of a binary */
typedef enum 
{
	SYM_TYPE_UKN = 0,	/* unknown symbol type */
	SYM_TYPE_FUNC = 1	/* function symbol type */
} SymbolType;
/* types: enums: SectionType 
 * 	This enumeration is for describing the types of sections we encounter
 * 	in the section header during static analysis of a binary */
typedef enum 
{
	SEC_TYPE_NONE = 0,	/* no section type */
	SEC_TYPE_CODE = 1,	/* code section */
	SEC_TYPE_DATA = 2	/* data section */
} SectionType;
/* types: enums: BinaryType
 * 	This enumeration is for describing the types of binary files we parse */
typedef enum
{
	BIN_TYPE_AUTO = 0,	/* tell the program to attempt to figure it out */
	BIN_TYPE_ELF = 1,	/* ELF format binary file */
	BIN_TYPE_PE = 2		/* PE format binary file */
} BinaryType;
/* types: enums: BinaryArch
 * 	This enumeration is for describing the architecture that the binary was 
 * 	built for. */
typedef enum
{
	ARCH_NONE = 0, 	/* unkown architecture */
	ARCH_X86 = 1,	/* X86 architecture is target of binary */
} BinaryArch;

/* types: structs
 *  	first we need to typedef all of our structs so they can all point at each other 
 *  	below in their definitions */
typedef struct _Section Section;
typedef struct _Binary Binary;
typedef struct _Symbol Symbol;

/* types: structs: Binary
 * 	This is the main object to hold the data for static analysis of our binary 
 * 	files */
struct _Binary {
	/* data */
	char 		*filename; 	/* name of binary file */
	BinaryType 	type;		/* type of binary file */
	char 		*type_str;	/* string representation of type */
	BinaryArch 	arch;		/* architecture target of binary */
	char 		*arch_str;	/* string representation of arch */
	unsigned char 	bits;		/* differences between 64 and 32 bit platforms */
	uint64_t 	entry;		/* this is going to be an address (entry point of the program) */
	unsigned int 	num_sections; 	/* number of entries in section array "sections[]" */
	Section 	**sections; 	/* array of section pointers */
	unsigned int 	num_symbols; 	/* number of entries in symbol array "symbols[] */
	Symbol 		**symbols; 	/* array of symbol pointers */
	
	/* function pointers */
	Section * (*get_text_section)();
};

/* types: structs: Section
 * 	This is the section storage unit */
struct _Section {
	/* data */
	Binary 		*binary; 	/* pointer to papa */
	char 		*name;		/* string name of section */
	SectionType 	type;		/* type using SectionType enum */
	uint64_t 	vma;		/* virtual memory address of section */
	uint64_t 	size;		/* size of section (also size of bytes array) */
	uint8_t 	*bytes;		/* bytes array */

	/* function pointers */
	int (*contains)(uint64_t addr); 
};

struct _Symbol {
	/* data */
	char 		*name;		/* name of the symbol */
	SymbolType 	type;		/* type of symbol */
	uint64_t 	addr;		/* address of symbol */
};

/* global function prototypes */
int load_binary(char * fname, Binary ** bin);
void unload_binary(Binary ** bin);

#endif /* LOADER_H */
