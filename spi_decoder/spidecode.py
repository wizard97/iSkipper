#! /usr/bin/env python3
import csv
import argparse
import itertools


def main():
    parser = argparse.ArgumentParser(
        description='Decodes SPI data from Saleae analyzer')
    parser.add_argument(
        'datacsv', help='SPI output CSV file from Saleae software')
    args = parser.parse_args()
    print('%s\t%s\t%s\t%s' % ('R/W', 'Addr', 'MOSI', 'MISO'))
    with open(args.datacsv, newline='') as csvfile:
        reader = csv.reader(csvfile)
        for row in itertools.islice(reader, 1, None):
            time = row[0]
            mosi, miso = [
                int(s[s.find("(") + 1:s.find(")")], 0) for s in map(str.strip, row[2].split(';'))]
            mosi_bin = '{0:08b}'.format(mosi)
            write = (mosi_bin[0] == '1')
            addr = int(mosi_bin[1:], 2)
            print('%s\t0x%02x\t0x%02x\t0x%02x' %
                  ('W' if write else 'R', addr, mosi, miso))

if __name__ == '__main__':
    main()
