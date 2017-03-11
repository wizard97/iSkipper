#! /usr/bin/env python3
import csv
import argparse
import itertools


# Normally there is 3 us of delay between SPI packets
# And at least 8 us when the master toggles NSS to signal the end of a packet
# So assume that after a delay of more than 6 us, the packet has ended
NSS_TOGGLE_DELAY = 6e-6
FIFO_REG = 0x0


def main():
    parser = argparse.ArgumentParser(
        description='Decodes SPI data from Saleae analyzer')
    parser.add_argument(
        'datacsv', help='SPI output CSV file from Saleae software')
    args = parser.parse_args()

    print('%s\t%s\t%s\t%s' % ('R/W', 'Addr', 'Out', 'In'))
    last_time = float('-inf')  # force a NSS toggle on the first packet
    last_addr = 0
    last_write = 0
    with open(args.datacsv, newline='') as csvfile:
        reader = csv.reader(csvfile)
        for row in itertools.islice(reader, 1, None):
            time = float(row[0])
            dt = time - last_time
            last_time = time
            mosi, miso = [
                int(s[s.find("(") + 1:s.find(")")], 0) for s in map(str.strip, row[2].split(';'))]

            addr = last_addr + 1
            write = last_write
            if dt > NSS_TOGGLE_DELAY:
                print('****************************')
                mosi_bin = '{0:08b}'.format(mosi)
                write = (mosi_bin[0] == '1')
                last_write = write
                addr = int(mosi_bin[1:], 2)
                # We didn't actually write anything this time
                # It will write to the same address next time
                last_addr = addr - 1
                print('%s\t0x%02x\t\t0x%02x' %
                      ('W' if write else 'R', addr, miso))
            else:
                last_addr = addr
                print('%s\t0x%02x\t0x%02x\t0x%02x' %
                      ('W' if write else 'R', addr, mosi, miso))

if __name__ == '__main__':
    main()
