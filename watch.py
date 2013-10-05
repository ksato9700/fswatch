#!/usr/bin/env python
import sys
from itertools import izip

flags_map = (
    (0x00000200, "Removed"),
    (0x00000400, "InodeMetaMod"),
    (0x00000800, "Renamed"),
    (0x00001000, "Modified"),
    (0x00002000, "FinderInfoMod"),
    (0x00004000, "ChangeOwner"),
    (0x00008000, "XattrMod"),
    (0x00010000, "IsFile"),
    (0x00020000, "IsDir"),
    (0x00040000, "IsSymlink"),
)


def parse_flags(flags):
    flags = int(flags, 16)
    return [x[1] for x in flags_map if x[0] & flags]


def main():
    iargv = iter(sys.argv[1:])

    for path, flags in izip(iargv, iargv):
        flags = parse_flags(flags)
        print path, "|".join(flags)

if __name__ == "__main__":
    main()
