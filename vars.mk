###############################################################################
#  © Université de Lille, The Pip Development Team (2015-2025)                #
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
DIRECTORIES_MK_PATH = $(abspath directories.mk)

ifeq ("$(wildcard $(DIRECTORIES_MK_PATH))","")
    $(error "Please create $(DIRECTORIES_MK_PATH) and populate the file with FAE_DIRECTORY_PATH and PIPCORE_MPU_DIRECTORY_PATH")
endif

include directories.mk

ifeq ("$(FAE_DIRECTORY_PATH)","")
    $(error "FAE_DIRECTORY_PATH has not been defined in $(DIRECTORIES_MK_PATH)")
endif

FAE_ABSOLUTE_DIRECTORY_PATH=$(abspath $(FAE_DIRECTORY_PATH))
ifeq ("$(wildcard $(FAE_ABSOLUTE_DIRECTORY_PATH))","")
    $(error "$(FAE_ABSOLUTE_DIRECTORY_PATH) does not exist." )
endif

FAE_UTILS_DIRECTORY_PATH=$(FAE_ABSOLUTE_DIRECTORY_PATH)/fae_utils
FAE_BUILDER=$(FAE_UTILS_DIRECTORY_PATH)/build_fae.py
ifeq ("$(wildcard $(FAE_BUILDER))","")
    $(error "Missing build_fae.py in $(FAE_UTILS_DIRECTORY_PATH) path. Please check this path.")
endif


ifeq ("$(PIPCORE_MPU_DIRECTORY_PATH)","")
    $(error "PIPCORE_MPU_DIRECTORY_PATH has not been defined in $(DIRECTORIES_MK_PATH)")
endif
PIPCORE_ABSOLUTE_DIRECTORY_PATH=$(abspath $(PIPCORE_MPU_DIRECTORY_PATH))

PIPCORE_MPU_PIP_PATH=$(PIPCORE_ABSOLUTE_DIRECTORY_PATH)/pip.bin
ifeq ("$(wildcard $(PIPCORE_MPU_PIP_PATH))","")
    $(error "Missing pip.bin. Please make $(PIPCORE_MPU_PIP_PATH) first.")
endif

PIPCORE_MPU_CRT0_DIRECTORY_PATH=$(PIPCORE_ABSOLUTE_DIRECTORY_PATH)/src/partition_crt0/build
PIPCORE_MPU_CRT0_FAE_PATH=$(PIPCORE_MPU_CRT0_DIRECTORY_PATH)/crt0.fae
PIPCORE_MPU_CRT0_ELF_PATH=$(PIPCORE_MPU_CRT0_DIRECTORY_PATH)/crt0.elf
ifeq ("$(wildcard $(PIPCORE_MPU_CRT0_FAE_PATH))","")
    $(error "Missing crt0.fae. Please make $(PIPCORE_MPU_CRT0_FAE_PATH) first.")
endif
ifeq ("$(wildcard $(PIPCORE_MPU_CRT0_ELF_PATH))","")
    $(error "Missing crt0.elf. Please make $(PIPCORE_MPU_CRT0_ELF_PATH) first.")
endif
