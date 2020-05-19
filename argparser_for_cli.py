import argparse

import sys

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--x', type=float, default=1.0,
                        help='What is the first number?')
    parser.add_argument('--y', type=float, default=1.0,
                        help='What is the second number?')
    parser.add_argument('--operation', type=str, default='add',
                        help='What is the first number?')
    args = parser.parse_args()
    print(args)
    # func(args)
    # print(func(args))
    sys.stdout.write(str(func(args)))

def func(args):
    print(args)
    print(type(args))
    print(args.x)
    if args.operation=='mul':
        return args.x * args.y
    

if __name__=='__main__':
    main()
