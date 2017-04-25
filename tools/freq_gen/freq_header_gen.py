#! /usr/bin/env python3

import freq_gen
import argparse
import os

letters = ['a', 'b', 'c', 'd']

header = """
namespace iClickerChannels
{
    // MSB to LSB RegFrf

"""

struct_format = """
    const iClickerChannel_t %s =
    {
        { 0x%s, 0x%s, 0x%s },
        { 0x%s, 0x%s, 0x%s },
    };

"""

end = """
}
"""

def main():
    parser = argparse.ArgumentParser(
        description='Generates frequency code header file')
    parser.add_argument(
        'folder', help='folder with files')
    args = parser.parse_args()

    freqs = [a + b for a in letters for b in letters]
    parts = [header]
    for freq in freqs:
        send, receive = freq_gen.extract_freqs(os.path.join(args.folder, '%s_2.csv' % freq))
        parts += [struct_format % tuple(map(str.upper, [freq] + send + receive))]

    parts += [end]
    print(''.join(parts))
    # print(freqs)

if __name__ == '__main__':
    main()
