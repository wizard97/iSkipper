#! /usr/bin/env python3

import subprocess
import os
import argparse

tool_dir = os.path.dirname(os.path.dirname(os.path.realpath(__file__)))
decoder_tool = os.path.join(tool_dir, 'spi_decoder', 'spidecode.py')
fifo_marker = 'FIFO ================== FIFO'

def target_prefix(addr):
    return 'W\t' + addr + '\t0x'

target_regs = ['0x07', '0x08', '0x09']
fstep = 61.0

def extract_freqs(spi_dump_file):
    table = subprocess.check_output([decoder_tool, spi_dump_file]).decode('utf-8')
    sections = table.split(fifo_marker)
    section_map = {}
    count = 0
    for section in sections[:2]:
        section_map[count] = {}
        for line in section.splitlines():
            for target_reg in target_regs:
                if line.startswith(target_prefix(target_reg)):
                    if target_reg not in section_map[count]:
                        section_map[count][target_reg] = line[len(target_prefix(target_reg)):len(target_prefix(target_reg))+2]
        count += 1

    return [section_map[0][x] for x in target_regs], [section_map[1][x] for x in target_regs]

def main():
    parser = argparse.ArgumentParser(
        description='Generates frequency codes')
    parser.add_argument(
        'datacsv', help='SPI output CSV file from Saleae software')
    args = parser.parse_args()
    send, receive = extract_freqs(args.datacsv)

    sval=[int(x, 16) for x in send]
    rval=[int(x, 16) for x in receive]
    sval.reverse()
    rval.reverse()
    sendfreq = sum([v << 8*i for i,v in enumerate(sval)])*fstep
    recvfreq = sum([v << 8*i for i,v in enumerate(rval)])*fstep

    print(send)
    print(receive)
    print("Send (KHz): %f, Recv (KHz):%f" % (sendfreq/1000, recvfreq/1000))

if __name__ == '__main__':
    main()
