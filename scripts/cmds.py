#!/usr/bin/env python3

import pathlib
import re
import zlib

DOCDIR = pathlib.Path("/usr/local/share/vim/vim91/doc")
DELIM = "  "
RE1 = "^(:.+?)(?:\t.*)?$"
#      ^~~~~~~~~~~~~~~~~ Match from : at the beginning of a line to the first \t or EOL
RE2 = ":(?:(?:\[.+?\])|(?:\{.+?\}))?(.+?(?:]|$))"
#                                    ^~~~~~~~~~ Match everything until the first ] or EOL
#       ^~~~~~~~~~~~~~~~~~~~~~~~~~~~ Skip over leading [...] or {...}


class CmdLine:
    __slots__ = ["_id", "_line", "_cmd", "_full", "_first", "_impl"]

    def __init__(self, line_nr, line):
        match = re.match(RE1, line)

        self._line = line_nr
        self._full = match.groups()[0]
        self._first = self._full.split(" ")[0]
        self._build_id()
        self._build_cmd()
        self._build_impl()

    def _build_id(self):
        tmp = zlib.crc32(self._full.encode("utf-8"))
        tmp = str(tmp)
        tmp = tmp.zfill(10)
        self._id = tmp

    def _build_cmd(self):
        tmp = re.search(RE2, self._first).groups()[0]
        tmp = tmp.replace("[", "")
        tmp = tmp.replace("]", "")
        tmp = ":" + tmp
        self._cmd = tmp

    def _build_impl(self):
        for path in pathlib.Path("src").glob("*.cpp"):
            with open(path, "r") as f:
                for i, line in enumerate(f.readlines()):
                    match = re.match(f"^// {self._id}$", line)
                    if match is None:
                        self._impl = ""
                    else:
                        self._impl = str(path) + ":" + str(i + 1)
                        return

    def has_impl(self):
        return self._impl != ""


def main():
    ipath = 0
    for path in pathlib.Path(DOCDIR).glob("*.txt"):

        cmdlines = []
        cmd_len = 0
        ln_len = 0

        if path.name.startswith("usr_"):
            continue

        if path.name.startswith("version"):
            continue

        if ipath > 0:
            print("")

        print(f"{path}:")
        with open(path, "r") as f:
            for i, line in enumerate(f.readlines()):
                match = re.match(RE1, line)
                if match is None:
                    continue

                cmdline = CmdLine(i + 1, line)

                cmd_len = max(len(cmdline._cmd), cmd_len)
                ln_len = max(len(str(i)), ln_len)

                cmdlines.append(cmdline)

        for line in cmdlines:
            print("  ", end="")
            print(f"{'Y' if line.has_impl() else ' '}{DELIM}", end="")
            print(f"{line._id}{DELIM}", end="")
            print(f"{line._line:<{ln_len}}{DELIM}", end="")
            print(f"{line._cmd:<{cmd_len}}{DELIM}", end="")
            print(f"{line._full}")

        ipath += 1


if __name__ == "__main__":
    main()
