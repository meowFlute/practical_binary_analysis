	.file	"test.c"
	.text
	.globl	x
	.bss
	.align 4
	.type	x, @object
	.size	x, 4
x:
	.zero	4
	.globl	y
	.align 4
	.type	y, @object
	.size	y, 4
y:
	.zero	4
	.section	.rodata
.LC0:
	.string	"response = %f\n"
	.text
	.globl	main
	.type	main, @function
main:
.LFB5:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$16, %rsp
	movl	%edi, -4(%rbp)
	movq	%rsi, -16(%rbp)
	movq	-16(%rbp), %rdx
	movl	-4(%rbp), %eax
	movq	%rdx, %rsi
	movl	%eax, %edi
	movl	$0, %eax
	call	process_cmd_input
	movl	$0, %eax
	call	divide
	cvtss2sd	%xmm0, %xmm0
	leaq	.LC0(%rip), %rdi
	movl	$1, %eax
	call	printf@PLT
	movl	$0, %eax
	call	add
	cvtss2sd	%xmm0, %xmm0
	leaq	.LC0(%rip), %rdi
	movl	$1, %eax
	call	printf@PLT
	movl	$0, %eax
	call	multiply
	cvtss2sd	%xmm0, %xmm0
	leaq	.LC0(%rip), %rdi
	movl	$1, %eax
	call	printf@PLT
	movl	$0, %eax
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE5:
	.size	main, .-main
	.section	.rodata
	.align 8
.LC1:
	.string	"just put some args in for fun plz"
.LC2:
	.string	"args:"
.LC3:
	.string	"\targ %d = %s\n"
	.text
	.globl	process_cmd_input
	.type	process_cmd_input, @function
process_cmd_input:
.LFB6:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$32, %rsp
	movl	%edi, -20(%rbp)
	movq	%rsi, -32(%rbp)
	movb	$0, -5(%rbp)
	cmpl	$1, -20(%rbp)
	jne	.L4
	leaq	.LC1(%rip), %rdi
	call	puts@PLT
	movb	$1, -5(%rbp)
	jmp	.L5
.L4:
	cmpl	$3, -20(%rbp)
	jne	.L6
	movq	-32(%rbp), %rax
	addq	$8, %rax
	movq	(%rax), %rax
	movq	%rax, %rdi
	call	atof@PLT
	cvtsd2ss	%xmm0, %xmm0
	movss	%xmm0, x(%rip)
	movq	-32(%rbp), %rax
	addq	$16, %rax
	movq	(%rax), %rax
	movq	%rax, %rdi
	call	atof@PLT
	cvtsd2ss	%xmm0, %xmm0
	movss	%xmm0, y(%rip)
	jmp	.L5
.L6:
	leaq	.LC2(%rip), %rdi
	call	puts@PLT
	movl	$0, -4(%rbp)
	jmp	.L7
.L8:
	movl	-4(%rbp), %eax
	cltq
	leaq	0(,%rax,8), %rdx
	movq	-32(%rbp), %rax
	addq	%rdx, %rax
	movq	(%rax), %rdx
	movl	-4(%rbp), %eax
	movl	%eax, %esi
	leaq	.LC3(%rip), %rdi
	movl	$0, %eax
	call	printf@PLT
	addl	$1, -4(%rbp)
.L7:
	movl	-4(%rbp), %eax
	cmpl	-20(%rbp), %eax
	jl	.L8
.L5:
	movzbl	-5(%rbp), %eax
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE6:
	.size	process_cmd_input, .-process_cmd_input
	.section	.rodata
.LC4:
	.string	"divide %f * %f = %f\n"
	.text
	.globl	multiply
	.type	multiply, @function
multiply:
.LFB7:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$16, %rsp
	movss	x(%rip), %xmm1
	movss	y(%rip), %xmm0
	mulss	%xmm1, %xmm0
	movss	%xmm0, -4(%rbp)
	cvtss2sd	-4(%rbp), %xmm2
	movss	y(%rip), %xmm0
	cvtss2sd	%xmm0, %xmm1
	movss	x(%rip), %xmm0
	cvtss2sd	%xmm0, %xmm0
	leaq	.LC4(%rip), %rdi
	movl	$3, %eax
	call	printf@PLT
	movss	-4(%rbp), %xmm0
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE7:
	.size	multiply, .-multiply
	.section	.rodata
.LC5:
	.string	"divide %f + %f = %f\n"
	.text
	.globl	add
	.type	add, @function
add:
.LFB8:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$16, %rsp
	movss	x(%rip), %xmm1
	movss	y(%rip), %xmm0
	addss	%xmm1, %xmm0
	movss	%xmm0, -4(%rbp)
	cvtss2sd	-4(%rbp), %xmm2
	movss	y(%rip), %xmm0
	cvtss2sd	%xmm0, %xmm1
	movss	x(%rip), %xmm0
	cvtss2sd	%xmm0, %xmm0
	leaq	.LC5(%rip), %rdi
	movl	$3, %eax
	call	printf@PLT
	movss	-4(%rbp), %xmm0
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE8:
	.size	add, .-add
	.section	.rodata
.LC6:
	.string	"divide %f / %f = %f\n"
	.text
	.globl	divide
	.type	divide, @function
divide:
.LFB9:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$16, %rsp
	movss	x(%rip), %xmm0
	movss	y(%rip), %xmm1
	divss	%xmm1, %xmm0
	movss	%xmm0, -4(%rbp)
	cvtss2sd	-4(%rbp), %xmm2
	movss	y(%rip), %xmm0
	cvtss2sd	%xmm0, %xmm1
	movss	x(%rip), %xmm0
	cvtss2sd	%xmm0, %xmm0
	leaq	.LC6(%rip), %rdi
	movl	$3, %eax
	call	printf@PLT
	movss	-4(%rbp), %xmm0
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE9:
	.size	divide, .-divide
	.ident	"GCC: (Ubuntu 7.4.0-1ubuntu1~18.04.1) 7.4.0"
	.section	.note.GNU-stack,"",@progbits
