#! /usr/bin/env python3
import subprocess
import os
import argparse

tool_dir = os.path.dirname(os.path.dirname(os.path.realpath(__file__)))
decoder_tool = os.path.join(tool_dir, 'spi_decoder', 'spidecode.py')

def main():
    parser = argparse.ArgumentParser(
        description='Generates frequency codes')
    parser.add_argument(
        'datacsv', help='SPI output CSV file from Saleae software')
    args = parser.parse_args()

    table = subprocess.check_output([decoder_tool, args.datacsv]).decode('utf-8')
    print(table)
    
if __name__ == '__main__':
    main()
