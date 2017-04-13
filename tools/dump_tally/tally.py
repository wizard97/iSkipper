#! /usr/bin/env python3

import argparse
import collections
import csv

choices = list(map(chr, range(ord('A'), ord('E')+1)))

def parse_dump(f):
    table = {}
    with open(f, 'r') as stream:
        for line in stream.readlines():
            if line.startswith('Captured:'):
                parts = line.replace('(','').replace(')','').replace(',','').split()
                answer = parts[1]
                student_id = ''.join(parts[2:])
                table[student_id] = answer
    return table

def tally(table):
    count = collections.OrderedDict()
    for choice in choices:
        count[choice] = 0
    for student_id, answer in table.items():
        if answer in count:
            count[answer] += 1;
    return count

def save(f, table):
    with open(f, 'w') as csv_file:
        writer = csv.writer(csv_file)
        for key, value in table.items():
           writer.writerow([key, value])

def main():
    parser = argparse.ArgumentParser(
        description='Tallies raw class dump data and optionaly outputs a CSV matching IDs to answers')
    parser.add_argument(
        'dump', help='raw dump file')
    parser.add_argument('-t', '--table' , help='saves table to file', nargs=1)
    args = parser.parse_args()
    table = parse_dump(args.dump)
    count = tally(table)
    print('Students: %i' % len(table))
    for choice in choices:
        print('%s\t%i' % (choice, count[choice]))

    if args.table:
        save(args.table[0], table)


if __name__ == '__main__':
    main()
