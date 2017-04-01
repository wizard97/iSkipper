#! /usr/bin/env python3
import csv
import argparse
import itertools
import os
import os.path
import collections

# Normally there is 3 us of delay between SPI packets
# And at least 8 us when the master toggles NSS to signal the end of a packet
# So assume that after a delay of more than 6 us, the packet has ended
NSS_TOGGLE_DELAY = 6e-6
FIFO_REG = 0x0
REG_DATA_FILE = os.path.join(os.path.dirname(
    os.path.realpath(__file__)), 'register_data.csv')

RegInfo = collections.namedtuple(
    'RegInfo', 'name reset recommended description')


def load_register_data():
    reg_data = {}
    with open(REG_DATA_FILE) as csvfile:
        reader = csv.reader(csvfile)
        for row in itertools.islice(reader, 1, None):
            raw_address = row[0]
            if '-' in raw_address:
                min_raw_address, max_raw_address = raw_address.split('-', 2)
            else:
                min_raw_address = raw_address
                max_raw_address = raw_address

            if raw_address == '????':
                other = row
            else:
                for address in range(int(min_raw_address, 0), int(max_raw_address, 0) + 1):
                    reg_data[address] = RegInfo(name=row[1], reset=int(
                        row[2], 0), recommended=int(row[3], 0), description=row[4])

    for i in range(0, 256):
        if i not in reg_data:
            reg_data[i] = RegInfo(name=other[1], reset=0,
                                  recommended=0, description=other[4])
    return reg_data


def nice_reg_data(data):
    return '%s(0x%02x, 0x%02x)\t%s' % (data.name.ljust(18), data.reset, data.recommended, data.description)


def main():
    parser = argparse.ArgumentParser(
        description='Decodes SPI data from Saleae analyzer')
    parser.add_argument(
        'datacsv', help='SPI output CSV file from Saleae software')
    args = parser.parse_args()

    reg_data = load_register_data()
    # print(reg_data)
    print('%s\t%s\t%s\t%s' % ('R/W', 'Addr', 'Out', 'In'))
    last_time = float('-inf')  # force a NSS toggle on the first packet
    last_addr = 0
    last_write = 0
    fifo = False
    with open(args.datacsv) as csvfile:
        reader = csv.reader(csvfile)
        for row in itertools.islice(reader, 1, None):
            time = float(row[0])
            dt = time - last_time
            last_time = time
            mosi, miso = [
                int(s[s.find("(0x") + 1:s.find(")", s.find("(0x"))], 0) for s in map(str.strip, row[2].split(';'))]

            addr = last_addr if fifo else last_addr + 1
            write = last_write
            if dt > NSS_TOGGLE_DELAY:
                mosi_bin = '{0:08b}'.format(mosi)
                write = (mosi_bin[0] == '1')
                last_write = write
                addr = int(mosi_bin[1:], 2)

                if addr == FIFO_REG:
                    print('FIFO ================== FIFO')
                    fifo = True
                else:
                    print('****************************')
                    fifo = False

                # We didn't actually write anything this time
                # It will write to the same address next time
                last_addr = addr if fifo else addr - 1
                print('%s\t0x%02x\t\t0x%02x' %
                      ('W' if write else 'R', addr, miso))
            else:
                last_addr = addr
                print('%s\t0x%02x\t0x%02x\t0x%02x\t%s' %
                      ('W' if write else 'R', addr, mosi, miso, nice_reg_data(reg_data[addr])))

if __name__ == '__main__':
    main()
