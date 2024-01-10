/*******************************************************************************/
/*  © Université de Lille, The Pip Development Team (2015-2024)                */
/*                                                                             */
/*  This software is a computer program whose purpose is to run a minimal,     */
/*  hypervisor relying on proven properties such as memory isolation.          */
/*                                                                             */
/*  This software is governed by the CeCILL license under French law and       */
/*  abiding by the rules of distribution of free software.  You can  use,      */
/*  modify and/ or redistribute the software under the terms of the CeCILL     */
/*  license as circulated by CEA, CNRS and INRIA at the following URL          */
/*  "http://www.cecill.info".                                                  */
/*                                                                             */
/*  As a counterpart to the access to the source code and  rights to copy,     */
/*  modify and redistribute granted by the license, users are provided only    */
/*  with a limited warranty  and the software's author,  the holder of the     */
/*  economic rights,  and the successive licensors  have only  limited         */
/*  liability.                                                                 */
/*                                                                             */
/*  In this respect, the user's attention is drawn to the risks associated     */
/*  with loading,  using,  modifying and/or developing or reproducing the      */
/*  software by the user in light of its specific status of free software,     */
/*  that may mean  that it is complicated to manipulate,  and  that  also      */
/*  therefore means  that it is reserved for developers  and  experienced      */
/*  professionals having in-depth computer knowledge. Users are therefore      */
/*  encouraged to load and test the software's suitability as regards their    */
/*  requirements in conditions enabling the security of their systems and/or   */
/*  data to be ensured and,  more generally, to use and operate it in the      */
/*  same conditions as regards security.                                       */
/*                                                                             */
/*  The fact that you are presently reading this means that you have had       */
/*  knowledge of the CeCILL license and that you accept its terms.             */
/*******************************************************************************/

#include <stdint.h>

#include "interface.h"

/* The following code performs the initialization work required before
 * calling the main of the root partition. It starts by copying the
 * .data section from the ROM to the RAM. Then, it initializes the .bss
 * section with zeros. Finally, it copies and reallocates each entry of
 * the Global Offset Table (GOT) from the ROM to the RAM. The following
 * diagram shows the performed operations:
 *
 *                      ROM                                       RAM
 *             +--------------------+                    +--------------------+
 *             |                    |                    |                    |
 *             |                    |                    |                    |
 *             |        PIP         |                    |        PIP         |
 *             |                    |                    |                    |
 *             |                    |                    |                    |
 * romStart -> +--------------------+                    +--------------------+
 *             |       .text        |                    |       .stack       |
 *             +--------------------+        ramStart -> +--------------------+
 *             |       .got         |--+     +---------> |       .data        |
 *             +--------------------+  |     |           +--------------------+
 *             |       .data        |--+-----+           |       .bss         |
 *             +--------------------+  |                 +--------------------+
 *             |       .bss         |  +---------------> |       .got         |
 *             +--------------------| ramUnusedStart ->  +--------------------+
 *             |                    |                    |                    |
 *             |                    |                    |                    |
 *             |                    |                    |                    |
 *                      ...                                       ...
 *             |                    |                    |                    |
 *             |                    |                    |                    |
 *             |                    |                    |                    |
 *             +--------------------+                    +--------------------+
 *
 * WARNING: Because the crt0.c file reallocates the GOT, no global
 * variable must be declared in this file.
 */

/*!
 * \brief The start address of the .text section expressed in relation
 *        to the start of the root partition binary.
 *
 * \see The declaration of the symbol in the link.ld file.
 */
extern void *_stext;

/*!
 * \brief The end address of the .text section expressed in relation to
 *        the start of the root partition binary.
 *
 * \see The declaration of the symbol in the link.ld file.
 */
extern void *_etext;

/*!
 * \brief The start address of the .got section expressed in relation
 *        to the start of the root partition binary.
 *
 * \see The declaration of the symbol in the link.ld file.
 */
extern void *_sgot;

/*!
 * \brief The end address of the .got section expressed in relation to
 *        the start of the root partition binary.
 *
 * \see The declaration of the symbol in the link.ld file.
 */
extern void *_egot;

/*!
 * \brief The start address of the .data section expressed in relation
 *        to the start of the root partition binary.
 *
 * \see The declaration of the symbol in the link.ld file.
 */
extern void *_sdata;

/*!
 * \brief The end address of the .data section expressed in relation to
 *        the start of the root partition binary.
 *
 * \see The declaration of the symbol in the link.ld file.
 */
extern void *_edata;

/*!
 * \brief The start address of the .bss section expressed in relation
 *        to the start of the root partition binary.
 *
 * \see The declaration of the symbol in the link.ld file.
 */
extern void *_sbss;

/*!
 * \brief The end address of the .bss section expressed in relation to
 *        the start of the root partition binary.
 *
 * \see The declaration of the symbol in the link.ld file.
 */
extern void *_ebss;

/*
 * The main of the root partition.
 */
extern void main(interface_t *interface);

/*!
 * \brief The _start function performs the initialization work required
 *        before calling the main of the root partition.
 *
 * \param pip_interface The interface that PIP provides to the root
 *        partition.
 */
extern void  __attribute__((section(".crt0"), noreturn))
_start(interface_t *interface)
{
	/* PIP provides the start address of the unused RAM and the
	 * start address of the unused ROM in the structure. */
	uint32_t romStart = (uint32_t) interface->binaryEntryPoint;
	uint32_t ramStart = (uint32_t) interface->unusedRamStart;
	uint32_t ramUnusedStart = ramStart;

	/* Copies the .data section of the root partition word by word
	 * from ROM to RAM. This assumes that the section is word
	 * aligned. */
	uint32_t romDataStart = (uint32_t) &_sdata;
	uint32_t romDataEnd   = (uint32_t) &_edata;
	uint32_t ramDataStart = ramUnusedStart;
	uint32_t dataIndex    = romDataStart;

	while (dataIndex < romDataEnd)
	{
		*((uint32_t *) ramUnusedStart) =
			*((uint32_t *)(romStart + dataIndex));

		dataIndex      += sizeof(void *);
		ramUnusedStart += sizeof(void *);
	}

	/* Initialize the .bss section of the root partition to zero.
	 * This assumes that the section is word aligned. */
	uint32_t symBssStart = (uint32_t) &_sbss;
	uint32_t symBssEnd   = (uint32_t) &_ebss;
	uint32_t ramBssStart = ramUnusedStart;
	uint32_t bssIndex    = symBssStart;

	while (bssIndex < symBssEnd)
	{
		*((uint32_t *) ramUnusedStart) = 0;

		bssIndex       += sizeof(void *);
		ramUnusedStart += sizeof(void *);
	}

	/* Reallocate each entry in the Global Offset Table (GOT). */
	uint32_t romGotStart  = (uint32_t) &_sgot;
	uint32_t romGotEnd    = (uint32_t) &_egot;
	uint32_t ramGotStart  = ramUnusedStart;
	uint32_t gotIndex     = romGotStart;

	uint32_t romTextStart = (uint32_t) &_stext;
	uint32_t romTextEnd   = (uint32_t) &_etext;

	while (gotIndex < romGotEnd)
	{
		uint32_t gotEntry = *((uint32_t *)(romStart + gotIndex));

		if (gotEntry >= romTextStart && gotEntry < romTextEnd)
		{
			*((uint32_t *) ramUnusedStart) =
				romStart + gotEntry - romTextStart;
		}
		else if (gotEntry >= romDataStart && gotEntry < romDataEnd)
		{
			*((uint32_t *) ramUnusedStart) =
				ramDataStart + gotEntry - romDataStart;
		}
		else if (gotEntry >= symBssStart && gotEntry < symBssEnd)
		{
			*((uint32_t *) ramUnusedStart) =
				ramBssStart + gotEntry - symBssStart;
		}
		else
		{
			/* A GOT entry refers to an unknown section. It
			 * is impossible to know how to reallocate this
			 * entry. */
			for (;;);
		}

		gotIndex       += sizeof(void *);
		ramUnusedStart += sizeof(void *);
	}

	/* Update the unused RAM start. */
	interface->unusedRamStart = (void *) ramUnusedStart;

	/* Set the PIC register to the address of the GOT. */
	asm volatile
	(
		"mov r10, %0"
		:
		: "r" (ramGotStart)
		: "r10"
	);

	/* Branch to the main of the root partition. */
	main(interface);

	/* Loop forever if the main function returns. */
	for (;;);
}
