#! /usr/bin/env python3
"""
./analyze.py ../../data/acks/*.txt
"""
import argparse


def idhex(id):
    return "{0:#0{1}x}".format(id, 10)


def ackhex(ack):
    return "{0:#0{1}x}".format(ack, 16)


def parse_ack_line(line):
    parts = line.split()
    for i in range(len(parts)):
        if len(parts[i]) == 1:
            parts[i] = '0' + parts[i]
    return int(''.join(parts), 16)


def parse_ack_file(f):
    with open(f, 'r') as s:
        header = s.readline()
        header = header[header.index('(') + 1:header.index(')')]
        header = header.replace(', ', '')
        header = int(header, 16)
        acks = []
        for line in s:
            acks += [parse_ack_line(line)]
    return header, acks


def const_check(acks):
    hmask = 0xFFFFFFFFFFFFFF
    lmask = 0xFFFFFFFFFFFFFF
    for ack in acks:
        hmask &= ack
        lmask &= ~ack
    return hmask | lmask


def main():
    parser = argparse.ArgumentParser(
        description='Analyzes 7 byte ack data')
    parser.add_argument(
        'ackfiles', nargs='+',
        help='List of files where each file contains acks from 1 ID')
    args = parser.parse_args()
    acks = {}
    for ackfile in args.ackfiles:
        header, ack = parse_ack_file(ackfile)
        acks[header] = ack
        print('Const check: %s: %s' % (idhex(header), ackhex(const_check(ack))))


if __name__ == '__main__':
    main()
