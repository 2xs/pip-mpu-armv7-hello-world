#!/usr/bin/env python3

# Copyright (C) 2024 Université de Lille
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.


"""gdbinit script"""


import sys


from elftools.elf.elffile import ELFFile
from elftools.elf.sections import SymbolTableSection


def usage():
    """Print how to to use the script and exit"""
    print(f'usage: {sys.argv[0]} <PIP_PATH> <CRT0_PATH> <SOFTWARE_PATH> <METADATA_SIZE>...')
    sys.exit(1)


def die(message):
    """Print error message and exit"""
    print(f'\033[91;1m{sys.argv[0]}: {message}\033[0m', file=sys.stderr)
    sys.exit(1)


def process_file(elf, symnames):
    """Parse the symbol table sections to extract the st_value"""
    sh = elf.get_section_by_name('.symtab')
    if not sh:
        die(f'.symtab: no section with this name found')
    if not isinstance(sh, SymbolTableSection):
        die(f'.symtab: is not a symbol table section')
    if sh['sh_type'] != 'SHT_SYMTAB':
        die(f'.symtab: is not a SHT_SYMTAB section')
    xs = []
    for symname in symnames:
        symbols = sh.get_symbol_by_name(symname)
        if not symbols:
            die(f'.symtab: {symname}: no symbol with this name')
        if len(symbols) > 1:
            die(f'.symtab: {symname}: more than one symbol with this name')
        xs.append(symbols[0].entry['st_value'])
    return xs


if __name__ == '__main__':
    if len(sys.argv) >= 5:
        pip_path = sys.argv[1]
        crt0_path = sys.argv[2]
        soft_path = sys.argv[3]
        crt0_meta_size = int(sys.argv[4])
        with open(pip_path, 'rb') as f:
            xs = process_file(ELFFile(f), [
                '__root',
                '__unusedRamStart',
            ])
        with open(soft_path, 'rb') as f:
            ys = process_file(ELFFile(f), [
                '__gotSize',
                '__romRamSize',
            ])
        bin_addr = xs[0]
        rom_addr = bin_addr + crt0_meta_size
        rel_got_addr = xs[1]
        rel_rom_ram_addr = rel_got_addr + ys[0]
        rel_ram_addr = rel_rom_ram_addr + ys[1]
        print(f'symbol-file {pip_path}')
        print(f'add-symbol-file {crt0_path} '
              f'-s .text {bin_addr}')
        print(f'add-symbol-file {soft_path} '
              f'-s .rom {rom_addr} '
              f'-s .got {rel_got_addr} '
              f'-s .rom.ram {rel_rom_ram_addr} '
              f'-s .ram {rel_ram_addr}')
        sys.exit(0)
    usage()
