"""
Tools for loading a set of files into a dictionary
mapping file to peaks.
"""

import tempfile, os, errno, multiprocessing, sys, hashlib
from .interface import print_error
from .transform import Spectrogram
from contextlib import contextmanager
from os.path import basename
from scipy.io import wavfile
import wave
import numpy as np

try:
    import subprocess32 as sp
except Exception:
    import subprocess as sp


# WAV file restrictions
SAMPLE_WIDTHS = [1, 2]
FRAME_RATES = [11025, 22050, 44100, 48000]


def _get_lame_path():
    pid = sp.Popen(['which','lame'], stdout=sp.PIPE, stderr=sp.PIPE)
    out, _ = pid.communicate()
    if len(out) == 0:
        lame_path = "/course/cs4500f14/bin/lame"
    else:
        lame_path = out.replace("\n", "")
    return lame_path
LAME_PATH = _get_lame_path()

def get_temp_file():
    "returns filename of a temp file that is guaranteed to be OK to use"
    return tempfile.mkstemp(suffix='.wav', dir='/tmp')[1]

@contextmanager
def convert_mp3(input_file_name):
    """
    Returns a wav filename. The file is deleted when the context exits.
    """
    filename = get_temp_file()
    try:
        sp.check_call([LAME_PATH,
                       '--decode', input_file_name,
                       '--silent',
                       '-o', filename])
        yield filename
    except Exception as e:
        print_error(("{} is not a valid mp3 {}"
                     .format(os.path.basename(input_file_name), e)))
        yield False
    finally:
        os.remove(filename)


def convert_and_load_file(fpath):
    """ fpath is guaranteed to end in .wav or .mp3 """
    if fpath.endswith('.wav'):
        return load_wav(fpath, restrict=True)
    with convert_mp3(fpath) as wav_path:
        # hacky way to make sure we return False if wav_path is False,
        # i.e. conversion failed
        return wav_path and load_wav(wav_path)

def check_wav_format(wav_path):
    """
    Checks for the following restrictions.
    WAVE must be
    - (RIFF)
    - PCM encoding (AudioFormat 1)
    - stereo or mono
    - 8 or 16-bit samples
    - 11.025, 22.05, 44.1, or 48 kHz sampling rate
    """
    wav = wave.open(wav_path)
    if wav.getsampwidth() not in SAMPLE_WIDTHS:
        print_error("invalid sample bitness: {0}".format(wav_path))
    elif wav.getframerate() not in FRAME_RATES:
        print_error("invalid sampling rate: {0}".format(wav_path))
    else:
        return True
    return False

def load_wav(wav_path, restrict=False):
    """
    Loads a WAV and returns its samples as a numpy array.
    If restrict=True, then the wav file is checked for restrictions.
    """
    try:
        if restrict and not check_wav_format(wav_path):
            return False
        return wavfile.read(wav_path)
    # TODO check for wav file specs here, print error and return false
    except IOError as e:
        if e.errno == errno.ENOENT:
            print_error("file not found: {0}".format(basename(wav_path)))
        else:
            print_error(str(e))
    except (EOFError, ValueError, TypeError) as e:
        print_error(("Error reading wav file: {0}"
                     .format(basename(wav_path))))
    except Exception as e:
        print_error(("Error reading wav file: {0}: {1}"
                     .format(basename(wav_path), e)))
    return False


def get_peaks_from_file(file_path):
    wav_data = convert_and_load_file(file_path)
    if not wav_data:
        return False
    sgram = Spectrogram.fromwav(wav_data)
    #rid_peaks_of_duplicate_times(sgram.get_peaks_coords())
    #get_fingerprints_from_peaks(sgram.get_peaks_coords())
    return (file_path, get_fingerprints_from_peaks(sgram.get_peaks_coords()))

def load_file_set(file_set):
    """
    Loads files using multiprocessing.
    Returns a dictionary of file path to peak array.
    """
    pool = multiprocessing.Pool()
    results = pool.map(get_peaks_from_file, file_set)
    pool.close()
    pool.join()

    # if anything is False, something has gone wrong, an error has been
    # printed, and we should abort
    if not all(results):
        return False
    return dict(results)

def get_fingerprints_from_peaks(peak_coords):
    peak_length = len(peak_coords)
    prints = []
    for i in range(peak_length):
        for j in range(1, 20):
            if (i + j) < peak_length:
                offset_1 = peak_coords[i][0]
                offset_2 = peak_coords[i + j][0]
                time_difference = offset_2 - offset_1
                if time_difference >= 0 and time_difference <= 200:
                    f1 = peak_coords[i][1]
                    f2 = peak_coords[i + j][1]
                    h = hashlib.sha1(
                        "%s|%s|%s" % (str(f1), str(f2), str(time_difference)))
                    prints.append((h.hexdigest()[0:20], offset_1))
    return prints
