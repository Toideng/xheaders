;  build: nasm -g -F dwarf -f elf64 aesops.s -o aesops.o
;  link:  gcc -march=native main.c aesops.o

BITS 64


SECTION .text



global aes128_prepare_key
;int aes128_prepare_key( rdi == byte roundkeys[0x200], rsi == byte key[16] );
aes128_prepare_key:

	; copy first words verbatim to the first encrypt rkey and
	; the last decrypt rkey

	movups xmm0, [rsi + 0x000]
	movups [rdi + 0x000], xmm0
	movups [rdi + 0x1f0], xmm0

	; now make the remaining 40 words
	; edx == temp

%macro round_convoluted 2
	mov eax, [rdi + (4 * (%1 - 4))]
	mov edx, [rdi + (4 * (%1 - 1))]
	rol rdx, 32
	movq xmm0, rdx
	aeskeygenassist xmm0, xmm0, %2
	movq rdx, xmm0
	ror rdx, 32
	xor eax, edx
	mov [rdi + (4 * %1)], eax
%endmacro

%macro round_plain 1
	mov eax, [rdi + (4 * (%1 - 4))]
	mov edx, [rdi + (4 * (%1 - 1))]
	xor eax, edx
	mov [rdi + (4 * %1)], eax
%endmacro

%macro megaround 2
	round_convoluted (4 * %1), %2
	round_plain (4 * %1 + 1)
	round_plain (4 * %1 + 2)
	round_plain (4 * %1 + 3)
	movups xmm0, [rdi + (16 * %1)]
%if %1 <> 10
	aesimc xmm0, xmm0
%endif
	movups [rdi + (0x1f0 - (16 * %1))], xmm0
%endmacro

	megaround  1, 0x01
	megaround  2, 0x02
	megaround  3, 0x04
	megaround  4, 0x08
	megaround  5, 0x10
	megaround  6, 0x20
	megaround  7, 0x40
	megaround  8, 0x80
	megaround  9, 0x1b
	megaround 10, 0x36

	ret
%unmacro round_convoluted 2
%unmacro round_plain 1
%unmacro megaround 1



global aes192_prepare_key
;int aes192_prepare_key( rdi == byte roundkeys[0x200], rsi == byte key[16] );
aes192_prepare_key:

	; utility macros

%macro round_convoluted 2
	mov eax, [rdi + (4 * (%1 - 6))]
	mov edx, [rdi + (4 * (%1 - 1))]
	rol rdx, 32
	movq xmm0, rdx
	aeskeygenassist xmm0, xmm0, %2
	movq rdx, xmm0
	ror rdx, 32
	xor eax, edx
	mov [rdi + (4 * %1)], eax
%endmacro

%macro round_plain 1
	mov eax, [rdi + (4 * (%1 - 6))]
	mov edx, [rdi + (4 * (%1 - 1))]
	xor eax, edx
	mov [rdi + (4 * %1)], eax
%endmacro

%macro invert_key 1
	movups xmm0, [rdi + 0x10 * %1]
	aesimc xmm0, xmm0
	movups [rdi + (0x1f0 - (0x10 * %1))], xmm0
%endmacro

	; main routine

	movups xmm0, [rsi + 0x000]
	movups [rdi + 0x000], xmm0
	movups [rdi + 0x1f0], xmm0
	mov rax, [rsi + 0x010]
	mov [rdi + 0x010], rax

	round_convoluted  6, 0x01
	round_plain       7
	invert_key 0x01
	round_plain       8
	round_plain       9
	round_plain      10
	round_plain      11
	invert_key 0x02
	round_convoluted 12, 0x02
	round_plain      13
	round_plain      14
	round_plain      15
	invert_key 0x03
	round_plain      16
	round_plain      17
	round_convoluted 18, 0x04
	round_plain      19
	invert_key 0x04
	round_plain      20
	round_plain      21
	round_plain      22
	round_plain      23
	invert_key 0x05
	round_convoluted 24, 0x08
	round_plain      25
	round_plain      26
	round_plain      27
	invert_key 0x06
	round_plain      28
	round_plain      29
	round_convoluted 30, 0x10
	round_plain      31
	invert_key 0x07
	round_plain      32
	round_plain      33
	round_plain      34
	round_plain      35
	invert_key 0x08
	round_convoluted 36, 0x20
	round_plain      37
	round_plain      38
	round_plain      39
	invert_key 0x09
	round_plain      40
	round_plain      41
	round_convoluted 42, 0x40
	round_plain      43
	invert_key 0x0a
	round_plain      44
	round_plain      45
	round_plain      46
	round_plain      47
	invert_key 0x0b
	round_convoluted 48, 0x80
	round_plain      49
	round_plain      50
	round_plain      51

	; First and last round keys are not IMC'd

	movups xmm0, [rdi + 0xc0]
	movups [rdi + 0x130], xmm0

	ret
%unmacro round_convoluted 2
%unmacro round_plain 1
%unmacro invert_key 1



global aes256_prepare_key
;int aes256_prepare_key( rdi == byte roundkeys[0x200], rsi == byte key[16] );
aes256_prepare_key:

	; utility macros

%macro round_convoluted 2
	mov eax, [rdi + (4 * (%1 - 8))]
	mov edx, [rdi + (4 * (%1 - 1))]
	rol rdx, 32
	movq xmm0, rdx
	aeskeygenassist xmm0, xmm0, %2
	movq rdx, xmm0
	ror rdx, 32
	xor eax, edx
	mov [rdi + (4 * %1)], eax
%endmacro

%macro round_sub 1
	mov eax, [rdi + (4 * (%1 - 8))]
	mov edx, [rdi + (4 * (%1 - 1))]
	rol rdx, 32
	movq xmm0, rdx
	aeskeygenassist xmm0, xmm0, 0
	movq rdx, xmm0
	xor eax, edx
	mov [rdi + (4 * %1)], eax
%endmacro

%macro round_plain 1
	mov eax, [rdi + (4 * (%1 - 8))]
	mov edx, [rdi + (4 * (%1 - 1))]
	xor eax, edx
	mov [rdi + (4 * %1)], eax
%endmacro

%macro invert_key 1
	movups xmm0, [rdi + 0x10 * %1]
	aesimc xmm0, xmm0
	movups [rdi + (0x1f0 - (0x10 * %1))], xmm0
%endmacro

%macro doublekey 2
	round_convoluted  (8 * %1 + 0), %2
	round_plain       (8 * %1 + 1)
	round_plain       (8 * %1 + 2)
	round_plain       (8 * %1 + 3)
	invert_key (2 * %1)
	round_sub         (8 * %1 + 4)
	round_plain       (8 * %1 + 5)
	round_plain       (8 * %1 + 6)
	round_plain       (8 * %1 + 7)
	invert_key (2 * %1 + 1)
%endmacro

	; main routine

	movups xmm0, [rsi + 0x000]
	movups [rdi + 0x000], xmm0
	movups [rdi + 0x1f0], xmm0
	movups xmm0, [rsi + 0x010]
	movups [rdi + 0x010], xmm0
	aesimc xmm0, xmm0
	movups [rdi + 0x1e0], xmm0

	doublekey 1, 0x01
	doublekey 2, 0x02
	doublekey 3, 0x04
	doublekey 4, 0x08
	doublekey 5, 0x10
	doublekey 6, 0x20

	round_convoluted 56, 0x40
	round_plain      57
	round_plain      58
	round_plain      59

	; First and last round keys are not IMC'd

	movups xmm0, [rdi + 0xe0]
	movups [rdi + 0x110], xmm0

	ret
%unmacro round_convoluted 2
%unmacro round_plain 1
%unmacro invert_key 1
%unmacro doublekey 2










global aes128_encrypt
;void aes128_encrypt( rdi == byte block[16], rsi == byte roundkeys[0x200] );
aes128_encrypt:
	movups xmm0, [rdi]
	xorps xmm0, [rsi + 0x00] ; first round key must be just xor'd
	aesenc xmm0, [rsi + 0x10]
	aesenc xmm0, [rsi + 0x20]
	aesenc xmm0, [rsi + 0x30]
	aesenc xmm0, [rsi + 0x40]
	aesenc xmm0, [rsi + 0x50]
	aesenc xmm0, [rsi + 0x60]
	aesenc xmm0, [rsi + 0x70]
	aesenc xmm0, [rsi + 0x80]
	aesenc xmm0, [rsi + 0x90]
	aesenclast xmm0, [rsi + 0xa0]
	movups [rdi], xmm0
	ret



global aes192_encrypt
;void aes192_encrypt( rdi == byte block[16], rsi == byte roundkeys[0x200] );
aes192_encrypt:
	movups xmm0, [rdi]
	xorps xmm0, [rsi + 0x00] ; first round key must be just xor'd
	aesenc xmm0, [rsi + 0x10]
	aesenc xmm0, [rsi + 0x20]
	aesenc xmm0, [rsi + 0x30]
	aesenc xmm0, [rsi + 0x40]
	aesenc xmm0, [rsi + 0x50]
	aesenc xmm0, [rsi + 0x60]
	aesenc xmm0, [rsi + 0x70]
	aesenc xmm0, [rsi + 0x80]
	aesenc xmm0, [rsi + 0x90]
	aesenc xmm0, [rsi + 0xa0]
	aesenc xmm0, [rsi + 0xb0]
	aesenclast xmm0, [rsi + 0xc0]
	movups [rdi], xmm0
	ret



global aes256_encrypt
;void aes256_encrypt( rdi == byte block[16], rsi == byte roundkeys[0x200] );
aes256_encrypt:
	movups xmm0, [rdi]
	xorps xmm0, [rsi + 0x00] ; first round key must be just xor'd
	aesenc xmm0, [rsi + 0x10]
	aesenc xmm0, [rsi + 0x20]
	aesenc xmm0, [rsi + 0x30]
	aesenc xmm0, [rsi + 0x40]
	aesenc xmm0, [rsi + 0x50]
	aesenc xmm0, [rsi + 0x60]
	aesenc xmm0, [rsi + 0x70]
	aesenc xmm0, [rsi + 0x80]
	aesenc xmm0, [rsi + 0x90]
	aesenc xmm0, [rsi + 0xa0]
	aesenc xmm0, [rsi + 0xb0]
	aesenc xmm0, [rsi + 0xc0]
	aesenc xmm0, [rsi + 0xd0]
	aesenclast xmm0, [rsi + 0xe0]
	movups [rdi], xmm0
	ret










global aes128_decrypt
;void aes128_decrypt( rdi == byte block[16], rsi == byte roundkeys[11][16] );
aes128_decrypt:
	add rsi, (0x200 - (0x10 * 0xb))
	movups xmm0, [rdi]
	xorps xmm0, [rsi + 0x00] ; first round key must be just xor'd
	aesdec xmm0, [rsi + 0x10]
	aesdec xmm0, [rsi + 0x20]
	aesdec xmm0, [rsi + 0x30]
	aesdec xmm0, [rsi + 0x40]
	aesdec xmm0, [rsi + 0x50]
	aesdec xmm0, [rsi + 0x60]
	aesdec xmm0, [rsi + 0x70]
	aesdec xmm0, [rsi + 0x80]
	aesdec xmm0, [rsi + 0x90]
	aesdeclast xmm0, [rsi + 0xa0]
	movups [rdi], xmm0
	ret



global aes192_decrypt
;void aes192_decrypt( rdi == byte block[16], rsi == byte roundkeys[11][16] );
aes192_decrypt:
	add rsi, (0x200 - (0x10 * 0xd))
	movups xmm0, [rdi]
	xorps xmm0, [rsi + 0x00] ; first round key must be just xor'd
	aesdec xmm0, [rsi + 0x10]
	aesdec xmm0, [rsi + 0x20]
	aesdec xmm0, [rsi + 0x30]
	aesdec xmm0, [rsi + 0x40]
	aesdec xmm0, [rsi + 0x50]
	aesdec xmm0, [rsi + 0x60]
	aesdec xmm0, [rsi + 0x70]
	aesdec xmm0, [rsi + 0x80]
	aesdec xmm0, [rsi + 0x90]
	aesdec xmm0, [rsi + 0xa0]
	aesdec xmm0, [rsi + 0xb0]
	aesdeclast xmm0, [rsi + 0xc0]
	movups [rdi], xmm0
	ret



global aes256_decrypt
;void aes256_decrypt( rdi == byte block[16], rsi == byte roundkeys[11][16] );
aes256_decrypt:
	add rsi, (0x200 - (0x10 * 0xf))
	movups xmm0, [rdi]
	xorps xmm0, [rsi + 0x00] ; first round key must be just xor'd
	aesdec xmm0, [rsi + 0x10]
	aesdec xmm0, [rsi + 0x20]
	aesdec xmm0, [rsi + 0x30]
	aesdec xmm0, [rsi + 0x40]
	aesdec xmm0, [rsi + 0x50]
	aesdec xmm0, [rsi + 0x60]
	aesdec xmm0, [rsi + 0x70]
	aesdec xmm0, [rsi + 0x80]
	aesdec xmm0, [rsi + 0x90]
	aesdec xmm0, [rsi + 0xa0]
	aesdec xmm0, [rsi + 0xb0]
	aesdec xmm0, [rsi + 0xc0]
	aesdec xmm0, [rsi + 0xd0]
	aesdeclast xmm0, [rsi + 0xe0]
	movups [rdi], xmm0
	ret
