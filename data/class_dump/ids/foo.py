#! /usr/bin/env python3

def ids():
    with open('ids.txt', 'r') as stream:
        for line in stream.readlines():
            foo = line.strip()
            if(foo != ''):
                yield map(str.strip, foo[foo.index('(') + 1:foo.index(')')].split(','))

def things():
    for thing in ids():
        yield '{' + ', '.join(['0x'+bar for bar in thing]) + '}'
    

def main():
    print('{' + ', '.join(things()) + '}')

if __name__ == '__main__':
    main()
