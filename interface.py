"""
Error printing and formatting utilities
"""

from __future__ import print_function
import sys
from transform import compare_peaks
from os.path import basename

def invalid_args(args):
    # Group args into 2-tuples
    groups = zip(*[iter(args)]*2)
    return len(args) != 4 or any(g[0] != '-f' and g[0] != '-d' for g in groups)

def print_error(error_str):
    print("ERROR: {0}".format(error_str), file=sys.stderr)

def print_error_and_exit(error_str, ret_code=1):
    print_error(error_str)
    sys.exit(ret_code)

def print_match(files, o1, o2):
    print_list = files + [o1, o2]
    print ("MATCH {0} {1} {2} {3}".format(*print_list))

def compare_peaks_and_print(data_tuple):
    f1, f2, p1, p2 = data_tuple
   # print(basename(f1), basename(f2))
    comparison = compare_peaks(p1, p2)
    if comparison[0]:
        print_match(map(basename, [f1, f2]), comparison[1], comparison[2])
