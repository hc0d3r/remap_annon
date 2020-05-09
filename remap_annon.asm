%define SYS_mmap     9
%define SYS_mprotect 10
%define SYS_munmap   11

%define PROT_READ  1
%define PROT_WRITE 2

%define MAP_ANONYMOUS 32
%define MAP_FIXED     16
%define MAP_PRIVATE   2

BITS 64

;void remap_annonymous(
;	uint64_t addr,     // (rdi)
;	uint64_t size,     // (rsi)
;	uint64_t data,     // (rdx)
;	uint64_t perms     // (rcx)
;);
remap_annonymous:
	; perms
	push rcx

	; data
	push rdx

	; munmap(addr, size)
	xor rax, rax
	mov al, SYS_munmap
	syscall

	; mmap(addr, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED, -1, 0)
	xor r10, r10
	mov r9, r10
	mov r8, r9
	mul r10

	dec r8

	mov dl, PROT_READ | PROT_WRITE
	mov r10b, MAP_ANONYMOUS | MAP_FIXED | MAP_PRIVATE
	mov al, SYS_mmap
	syscall

	; memcpy(addr, data, size)
	mov r11, rdi ; addr
	pop r10      ; data
	mov rax, rsi ; size

loop:
	mov r8, qword [r10]
	mov [r11], r8

	add r10, 8
	add r11, 8

	sub rax, 8
	jnz loop


	; mprotect(addr, size, perms)
	xor rax, rax
	mov al, SYS_mprotect
	pop rdx
	syscall

	ret
