/*
 * Copyright (c) 2020 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdint.h>
#include "efi.h"
#include "printf.h"
#include <zefi-segments.h>

#define PUTCHAR_BUFSZ 128

/* The linker places this dummy last in the data memory.  We can't use
 * traditional linker address symbols because we're relocatable; the
 * linker doesn't know what the runtime address will be.  The compiler
 * has to emit code to find this thing's address at runtime via an
 * offset from RIP.  It's a qword so we can guarantee alignment of the
 * stuff after.
 */
static __attribute__((section(".runtime_data_end")))
uint64_t runtime_data_end[1] = { 0x1111aa8888aa1111L };

#define EXT_DATA_START ((void *) &runtime_data_end[1])

static struct efi_system_table *efi;

static void efi_putchar(int c)
{
	static uint16_t efibuf[PUTCHAR_BUFSZ + 1];
	static int n;

	if (c == '\n') {
		efi_putchar('\r');
	}

	efibuf[n++] = c;

	if (c == '\n' || n == PUTCHAR_BUFSZ) {
		efibuf[n] = 0;
		efi->ConOut->OutputString(efi->ConOut, efibuf);
		n = 0;
	}
}

/* FIXME: if you check the generated code, "ms_abi" calls like this
 * have to SPILL HALF OF THE SSE REGISTER SET TO THE STACK on entry
 * because of the way the conventions collide.  Is there a way to
 * prevent/suppress that?
 */
uintptr_t __abi efi_entry(void *img_handle, struct efi_system_table *sys_tab)
{
	efi = sys_tab;
	z_putchar = efi_putchar;
	printf("*** Zephyr EFI Loader ***\n");

	for (int i = 0; i < sizeof(zefi_zsegs)/sizeof(zefi_zsegs[0]); i++) {
		int nqwords = zefi_zsegs[i].sz;
		uint64_t *dst = (uint64_t *)zefi_zsegs[i].addr;

		printf("Zeroing %d bytes of memory at %p\n", 8 * nqwords, dst);
		for (int j = 0; j < nqwords; j++) {
			dst[j] = 0;
		}
	}

	for (int i = 0; i < sizeof(zefi_dsegs)/sizeof(zefi_dsegs[0]); i++) {
		int nqwords = zefi_dsegs[i].sz;
		int off = zefi_dsegs[i].off;
		uint64_t *dst = (uint64_t *)zefi_dsegs[i].addr;
		uint64_t *src = &((uint64_t *)EXT_DATA_START)[off];

		printf("Copying %d data bytes to %p from image offset %d\n",
		       8 * nqwords, dst, zefi_dsegs[i].off);
		for (int j = 0; j < nqwords; j++) {
			dst[j] = src[j];
		}
	}

	unsigned char *code = (void *)zefi_entry;

	printf("Jumping to Entry Point: %p (%x %x %x %x %x %x)\n",
	       code, code[0], code[1], code[2], code[3],
	       code[4], code[5], code[6]);

	/* The EFI console seems to be buffered, give it a little time
	 * to drain before we start banging on the same UART from the
	 * OS.
	 */
	for (volatile int i = 0; i < 50000000; i++) {
	}

	__asm__ volatile("cli; jmp *%0" :: "r"(code));

	return 0;
}

/* Trick cribbed shamelessly from gnu-efi.  We need to emit a ".reloc"
 * section into the image with a single dummy entry for the EFI loader
 * to think we're a valid PE file, gcc won't because it thinks we're
 * ELF.
 */
uint32_t relocation_dummy;
__asm__(".section .reloc\n"
	"base_relocation_block:\n"
	".long relocation_dummy - base_relocation_block\n"
	".long 0x0a\n"
	".word	0\n");
