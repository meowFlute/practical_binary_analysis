# Chapter 4 - A Simple Binary Loader
For this project I decided to experiment with getopt_long and some other things to make it useful. 
As a result, the following is what you get when you use the program
```console
scott@Chromebook:~/practicalBinaryAnalysisBook/ch4$ ./loader --h
Usage: loader [options] binary
Options:
	-f (--func_symbols)                      display function symbols
	-d (--data_symbols)                      display data symbols
	-s (--important_sections)                display code and data sections
	-x (--hex_dump) <.section_name>          display hex dump of section passed as argument
	-h (--h or --help)                       display this information and return with no action

scott@Chromebook:~/practicalBinaryAnalysisBook/ch4$ ./loader -s ./loader
file: ./loader
type: elf64-x86-64, machine: 64bit i386:x86-64, entry point: 0xed0
Sections:
	ADDRESS            SIZE     NAME                           TYPE
	-------            ----     ----                           ----
	0x238              28       .interp                        DATA
	0x254              32       .note.ABI-tag                  DATA
	0x274              36       .note.gnu.build-id             DATA
	0x298              68       .gnu.hash                      DATA
	0x2e0              984      .dynsym                        DATA
	0x6b8              496      .dynstr                        DATA
	0x8a8              82       .gnu.version                   DATA
	0x900              80       .gnu.version_r                 DATA
	0x950              264      .rela.dyn                      DATA
	0xa58              648      .rela.plt                      DATA
	0xce0              23       .init                          CODE
	0xd00              448      .plt                           CODE
	0xec0              8        .plt.got                       CODE
	0xed0              9762     .text                          CODE
	0x34f4             9        .fini                          CODE
	0x3500             2562     .rodata                        DATA
	0x3f04             132      .eh_frame_hdr                  DATA
	0x3f88             568      .eh_frame                      DATA
	0x204cd8           8        .init_array                    DATA
	0x204ce0           8        .fini_array                    DATA
	0x204ce8           512      .dynamic                       DATA
	0x204ee8           280      .got                           DATA
	0x205000           16       .data                          DATA
scott@Chromebook:~/practicalBinaryAnalysisBook/ch4$ ./loader -x .interp ./loader
file: ./loader
type: elf64-x86-64, machine: 64bit i386:x86-64, entry point: 0xed0
Hex Dump of .interp (section type: DATA, bytes dumped: 28):

VMA                 Hex Data                                                                          Equivalent Chars
---                 --------                                                                          ----------------
0x0000000000000238:                                                             2f6c 6962 3634 2f6c   /lib64/l
0x0000000000000240: 642d 6c69 6e75 782d 7838 362d 3634 2e73 6f2e 3200                                 d-linux-x86-64.so.2.
scott@Chromebook:~/practicalBinaryAnalysisBook/ch4$ ./loader -fd ./loader
file: ./loader
type: elf64-x86-64, machine: 64bit i386:x86-64, entry point: 0xed0
Function Symbols:
	TYPE   	NAME                                     	ADDRESS
	----   	----                                     	-------
	FUNC 	deregister_tm_clones                     	0xf00
	FUNC 	register_tm_clones                       	0xf40
	FUNC 	__do_global_dtors_aux                    	0xf90
	FUNC 	frame_dummy                              	0xfd0
	FUNC 	open_bfd                                 	0x1672
	FUNC 	dtor_Binary                              	0x14e4
	FUNC 	load_symbols_bfd                         	0x1779
	FUNC 	load_sections_bfd                        	0x1f3d
	FUNC 	dtor_Symbol                              	0x13b8
	FUNC 	dtor_Section                             	0x1438
	FUNC 	print_help                               	0x33c2
	FUNC 	__libc_csu_fini                          	0x34f0
	FUNC 	free@@GLIBC_2.2.5                        	0x0
	FUNC 	putchar@@GLIBC_2.2.5                     	0x0
	FUNC 	abort@@GLIBC_2.2.5                       	0x0
	FUNC 	strcpy@@GLIBC_2.2.5                      	0x0
	FUNC 	puts@@GLIBC_2.2.5                        	0x0
	FUNC 	bfd_openr                                	0x0
	FUNC 	load_binary                              	0xfda
	FUNC 	_fini                                    	0x34f4
	FUNC 	bfd_get_section_contents                 	0x0
	FUNC 	strlen@@GLIBC_2.2.5                      	0x0
	FUNC 	__stack_chk_fail@@GLIBC_2.4              	0x0
	FUNC 	unload_binary                            	0x139d
	FUNC 	getopt_long@@GLIBC_2.2.5                 	0x0
	FUNC 	printf@@GLIBC_2.2.5                      	0x0
	FUNC 	bfd_get_error                            	0x0
	FUNC 	bfd_set_error                            	0x0
	FUNC 	__libc_start_main@@GLIBC_2.2.5           	0x0
	FUNC 	bfd_init                                 	0x0
	FUNC 	bfd_get_arch_info                        	0x0
	FUNC 	calloc@@GLIBC_2.2.5                      	0x0
	FUNC 	strcmp@@GLIBC_2.2.5                      	0x0
	FUNC 	fprintf@@GLIBC_2.2.5                     	0x0
	FUNC 	memcpy@@GLIBC_2.14                       	0x0
	FUNC 	__libc_csu_init                          	0x3480
	FUNC 	malloc@@GLIBC_2.2.5                      	0x0
	FUNC 	_start                                   	0xed0
	FUNC 	main                                     	0x27a7
	FUNC 	access@@GLIBC_2.2.5                      	0x0
	FUNC 	bfd_close                                	0x0
	FUNC 	bfd_errmsg                               	0x0
	FUNC 	sprintf@@GLIBC_2.2.5                     	0x0
	FUNC 	fwrite@@GLIBC_2.2.5                      	0x0
	FUNC 	bfd_check_format                         	0x0
	FUNC 	__cxa_finalize@@GLIBC_2.2.5              	0x0
	FUNC 	_init                                    	0xce0
	FUNC 	__ctype_b_loc@@GLIBC_2.3                 	0x0
	FUNC 	free                                     	0x0
	FUNC 	putchar                                  	0x0
	FUNC 	abort                                    	0x0
	FUNC 	strcpy                                   	0x0
	FUNC 	puts                                     	0x0
	FUNC 	bfd_openr                                	0x0
	FUNC 	bfd_get_section_contents                 	0x0
	FUNC 	strlen                                   	0x0
	FUNC 	__stack_chk_fail                         	0x0
	FUNC 	getopt_long                              	0x0
	FUNC 	printf                                   	0x0
	FUNC 	bfd_get_error                            	0x0
	FUNC 	bfd_set_error                            	0x0
	FUNC 	__libc_start_main                        	0x0
	FUNC 	bfd_init                                 	0x0
	FUNC 	bfd_get_arch_info                        	0x0
	FUNC 	calloc                                   	0x0
	FUNC 	strcmp                                   	0x0
	FUNC 	fprintf                                  	0x0
	FUNC 	memcpy                                   	0x0
	FUNC 	malloc                                   	0x0
	FUNC 	access                                   	0x0
	FUNC 	bfd_close                                	0x0
	FUNC 	bfd_errmsg                               	0x0
	FUNC 	sprintf                                  	0x0
	FUNC 	fwrite                                   	0x0
	FUNC 	bfd_check_format                         	0x0
	FUNC 	__cxa_finalize                           	0x0
	FUNC 	__ctype_b_loc                            	0x0
	FUNC 	_init                                    	0xce0
	FUNC 	_fini                                    	0x34f4
Data Symbols:
	TYPE        	NAME                                     	ADDRESS
	----        	----                                     	-------
	LOCAL DATA  	completed.7697                           	0x205048
	LOCAL DATA  	__do_global_dtors_aux_fini_array_entry   	0x204ce0
	LOCAL DATA  	__frame_dummy_init_array_entry           	0x204cd8
	LOCAL DATA  	bfd_initialized.7504                     	0x20504c
	LOCAL DATA  	__FRAME_END__                            	0x41bc
	LOCAL DATA  	_DYNAMIC                                 	0x204ce8
	LOCAL DATA  	_GLOBAL_OFFSET_TABLE_                    	0x204ee8
	GLOBAL DATA 	optind@@GLIBC_2.2.5                      	0x205020
	GLOBAL DATA 	optarg@@GLIBC_2.2.5                      	0x205028
	GLOBAL DATA 	__dso_handle                             	0x205008
	GLOBAL DATA 	_IO_stdin_used                           	0x3500
	GLOBAL DATA 	__TMC_END__                              	0x205010
	GLOBAL DATA 	stderr@@GLIBC_2.2.5                      	0x205040
	GLOBAL DATA 	stderr                                   	0x205040
	GLOBAL DATA 	optind                                   	0x205020
	GLOBAL DATA 	optarg                                   	0x205028
scott@Chromebook:~/practicalBinaryAnalysisBook/ch4$ 
```
