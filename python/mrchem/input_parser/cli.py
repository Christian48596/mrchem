# -*- coding: utf-8 -*-

# This file was automatically generated by parselglossy on 2022-10-25
# Editing is *STRONGLY DISCOURAGED*

import argparse

from .api import parse
from .plumbing.utils import default_outfile


def cli():
    cli = argparse.ArgumentParser()
    cli.add_argument("infile", help="the input file to parse")
    cli.add_argument(
        "--dump-ir",
        dest="dumpir",
        help="whether to dump intermediate representation to JSON",
        action="store_true",
        default=False,
    )
    cli.add_argument(
        "--outfile",
        dest="outfile",
        help="name or path for the parsed JSON output file",
        type=str,
        action="store",
    )

    args = cli.parse_args()

    if args.outfile is None:
        outfile = default_outfile(fname=args.infile, suffix="_fr.json")

    parse(infile=args.infile, dump_ir=args.dumpir, outfile=outfile)
