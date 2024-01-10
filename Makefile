###############################################################################
#  © Université de Lille, The Pip Development Team (2015-2024)                #
#                                                                             #
#  This software is a computer program whose purpose is to run a minimal,     #
#  hypervisor relying on proven properties such as memory isolation.          #
#                                                                             #
#  This software is governed by the CeCILL license under French law and       #
#  abiding by the rules of distribution of free software.  You can  use,      #
#  modify and/ or redistribute the software under the terms of the CeCILL     #
#  license as circulated by CEA, CNRS and INRIA at the following URL          #
#  "http://www.cecill.info".                                                  #
#                                                                             #
#  As a counterpart to the access to the source code and  rights to copy,     #
#  modify and redistribute granted by the license, users are provided only    #
#  with a limited warranty  and the software's author,  the holder of the     #
#  economic rights,  and the successive licensors  have only  limited         #
#  liability.                                                                 #
#                                                                             #
#  In this respect, the user's attention is drawn to the risks associated     #
#  with loading,  using,  modifying and/or developing or reproducing the      #
#  software by the user in light of its specific status of free software,     #
#  that may mean  that it is complicated to manipulate,  and  that  also      #
#  therefore means  that it is reserved for developers  and  experienced      #
#  professionals having in-depth computer knowledge. Users are therefore      #
#  encouraged to load and test the software's suitability as regards their    #
#  requirements in conditions enabling the security of their systems and/or   #
#  data to be ensured and,  more generally, to use and operate it in the      #
#  same conditions as regards security.                                       #
#                                                                             #
#  The fact that you are presently reading this means that you have had       #
#  knowledge of the CeCILL license and that you accept its terms.             #
###############################################################################

ARCH_DEPENDENT  = arm-none-eabi

CC              = $(ARCH_DEPENDENT)-gcc
LD              = $(ARCH_DEPENDENT)-gcc
BI              = $(ARCH_DEPENDENT)-objcopy

CFLAGS_COMMON   = -Wall
CFLAGS_COMMON  += -Wextra
CFLAGS_COMMON  += -Werror
CFLAGS_COMMON  += -mthumb
CFLAGS_COMMON  += -mcpu=cortex-m4
CFLAGS_COMMON  += -ffreestanding
CFLAGS_COMMON  += -Iinclude
CFLAGS_COMMON  += -Os

CFLAGS_PIC      = $(CFLAGS_COMMON)
CFLAGS_PIC     += -msingle-pic-base
CFLAGS_PIC     += -mpic-register=r10
CFLAGS_PIC     += -mno-pic-data-is-text-relative
CFLAGS_PIC     += -fPIC

LDFLAGS         = -nostartfiles
LDFLAGS        += -nodefaultlibs
LDFLAGS        += -nolibc
LDFLAGS        += -nostdlib
LDFLAGS        += -T link.ld

STARTFILE_SRC   = crt0.c
PARTITION_SRC   = $(filter-out $(STARTFILE_SRC), $(wildcard *.c))

STARTFILE_OBJ  = $(STARTFILE_SRC:.c=.o)
PARTITION_OBJ  = $(PARTITION_SRC:.c=.o)

EXEC           = $(shell basename $$(pwd))

all: $(EXEC).bin

$(EXEC).bin: $(EXEC).elf
		$(BI) -O binary $< $@

$(EXEC).elf: $(STARTFILE_OBJ) $(PARTITION_OBJ)
		$(LD) $(LDFLAGS) -o $@ $^

$(STARTFILE_OBJ): $(STARTFILE_SRC)
		$(CC) $(CFLAGS_COMMON) -c $< -o $@

%.o: %.c
		$(CC) $(CFLAGS_PIC) -c $< -o $@

realclean:
		$(RM) $(STARTFILE_OBJ) $(PARTITION_OBJ) $(EXEC).elf $(EXEC).bin

.PHONY: all realclean
