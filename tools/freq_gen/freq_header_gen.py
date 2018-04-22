#! /usr/bin/env python3

import freq_gen
import argparse
import os

letters = ['a', 'b', 'c', 'd']
fstep=61.0

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
    }; //%s

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
        sval=[int(x, 16) for x in send]
        rval=[int(x, 16) for x in receive]
        sval.reverse()
        rval.reverse()
        sendfreq = sum([v << 8*i for i,v in enumerate(sval)])*fstep/1000
        recvfreq = sum([v << 8*i for i,v in enumerate(rval)])*fstep/1000
        s="Send (KHz): %f, Recv (KHz):%f" % (sendfreq/1000, recvfreq/1000)
        parts += [struct_format % tuple(map(str.upper, [freq] + send + receive+[s]))]

    parts += [end]
    print(''.join(parts))
    # print(freqs)

if __name__ == '__main__':
    main()
