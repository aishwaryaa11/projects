#!/usr/bin/env python

from src.interface import (print_error, print_match, invalid_args,
                           print_error_and_exit, compare_peaks_and_print)
from src.loader import load_file_set
from os import listdir
from os.path import basename, isdir, isfile, join, islink, realpath
import os, sys, itertools, multiprocessing

def invalid_file(fpath):
    """
    Load MP3 or WAVE
    WAVE must be
    - (RIFF)
    - PCM encoding (AudioFormat 1)
    - stereo or mono
    - 8 or 16-bit samples
    - 11.025, 22.05, 44.1, or 48 kHz sampling rate
    TODO implement...
    """
    return not (fpath.endswith('.mp3') or fpath.endswith('.wav'))

def error_invalid_file(f):
    print_error("{} is an invalid audio file type".format(f))

def do(args):
    """
    Does everything.
    """
    any_errors = False

    if invalid_args(args):
        print_error_and_exit("incorrect command line")

    # Group args into 2-tuples
    groups = zip(*[iter(args)]*2)

    file_lists = []
    for flag, name in groups:
        if flag == '-d':
            if not isdir(name):
                print_error("Invalid directory: {}".format(name))
                any_errors = True
            else:
                all_files = [realpath(join(name,f)) for f in listdir(name)
                            if isfile(join(name,f)) or islink(join(name,f))]
                invalid_files = filter(invalid_file, all_files)
                if invalid_files:
                    any_errors = True
                    map(error_invalid_file, invalid_files)
                file_lists.append(all_files)
        else:
            if invalid_file(name):
                error_invalid_file(name)
                any_errors = True
            file_lists.append([realpath(name)])

    if any_errors:
        sys.exit(1)

    all_files_set = set(file_lists[0] + file_lists[1])

    peaks_dict = load_file_set(all_files_set)
    if not peaks_dict:
        # we encountered an error and it's been printed, exit
        sys.exit(1)

    def generate_comparison_tuples(files):
        f1, f2 = files
        p1 = peaks_dict[f1]
        p2 = peaks_dict[f2]
        return (f1, f2, p1, p2)

    file_pairs = itertools.product(*file_lists)
    comparison_tuples = map(generate_comparison_tuples, file_pairs)
    # each tuple in this list is (file_one, file_two, peaks_one, peaks_two)

    map(compare_peaks_and_print, comparison_tuples)

def main():
    # Drop the script name
    args = sys.argv[1:]
    do(args)

if __name__ == "__main__":
    main()
