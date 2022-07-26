format ELF64
public __switch_threads

section '.text' writable
__switch_threads:
    push rbx
	push rbp
	push r12
	push r13
	push r14
	push r15
	push rdx
	push rcx
	push r8
	pushf

	mov [rdi], rsp
	mov rsp, rsi

	popf
	pop r8
	pop rcx
	pop rdx
	pop r15
	pop r14
	pop r13
	pop r12
	pop rbp
	pop rbx

	retq

