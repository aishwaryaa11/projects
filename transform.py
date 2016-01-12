import sys
import operator
import numpy as np
import hashlib
import matplotlib.pyplot as plt
from matplotlib.gridspec import GridSpec
from matplotlib.mlab import specgram, window_hanning
from scipy.ndimage.filters import maximum_filter, minimum_filter
from scipy.ndimage.morphology import (generate_binary_structure,
                                      iterate_structure)
from collections import defaultdict

# Minimum amplitude to be considered a peak
PEAK_MINIMUM_AMPLITUDE = 10

# Minimum number of matches for an offset-bin to be considered a match
MIN_DIFFS = 350

# How far apart 5 offsets should be in order for it to be considered a match
# start
OFFSET_DELTA = 0.5

# How long a contiguous offset should be in order for it to be considered a
# match
MATCH_LENGTH = 5

def compare_peaks(p1, p2):
    # p1, p2 are (hash, offset) lists
    p1 = dict(p1)
    p2 = dict(p2)
    array = []
    for localhash in p1:
        offset = p1[localhash]
        if localhash in p2:
            second_offset = p2[localhash]
            offset_difference = second_offset - offset
            array.append((localhash, offset_difference, offset, second_offset))
    sorted_array = sorted(array, key=lambda arr: arr[2])
    return determine_neighbors(sorted_array)

def round_to_five(num):
    return int(round(num/5.0)*5.0)

def _find_first_over_delta(offset_list):
    for i in xrange(0, len(offset_list)-5):
        if abs(offset_list[i+5] - offset_list[i]) < OFFSET_DELTA:
            return round(offset_list[i], 1)
    return None

def _get_match_start(offset_list):
    o_list = sorted(offset_list)
    return _find_first_over_delta(o_list)

def _get_match_end(offset_list):
    o_list = sorted(offset_list, reverse=True)
    return _find_first_over_delta(o_list)

def process_offset_list(offset_list):
    return (_get_match_start(offset_list), _get_match_end(offset_list))

def determine_neighbors(sorted_array):
    if not sorted_array:
        return (False, 0, 0)
    diffs = defaultdict(lambda: [0, [], []])
    for (localhash, offset_diff, o1, o2) in sorted_array:
        rounded_diff = round_to_five(offset_diff)
        diffs[rounded_diff][0] += 1
        diffs[rounded_diff][1].append(o1)
        diffs[rounded_diff][2].append(o2)
    diff_list = diffs.values()
    diff_list.sort(key=operator.itemgetter(0), reverse=True)
    diff_count, o1_list, o2_list = tuple(diff_list[0])
    o1, o1_end = process_offset_list(o1_list)
    o2, o2_end = process_offset_list(o2_list)
    if (o1 is None or o2 is None
        or o1_end is None or o2_end is None
        or (o1_end - o1) < MATCH_LENGTH
        or (o2_end - o2) < MATCH_LENGTH
        or diff_count <= MIN_DIFFS):
        return (False, 0, 0)
    return (True, o1, o2)

class Spectrogram():
    def __init__(self, sample_rate, spectrograms, times):
        self.sample_rate = sample_rate
        self.spectrograms = spectrograms
        self.times = times

    def mono(self):
        return len(self.spectrograms) == 1

    def get_time_frequency_array(self, peak_array):
        # This returns a 2D of peaks in [time, frequency] format,
        # sorted by time
        unsorted = np.transpose(np.flipud(np.where(peak_array)))
        # this is a horrible hack but it sorts by the time column
        sorted_peaks = unsorted[unsorted[:,0].argsort()]
        timed_peaks = map(lambda (s,f): [self.times[s], f], sorted_peaks)
        return timed_peaks

    def get_peaks_coords(self):
        # bottleneck method
        def extract_peak_coords():
            return map(lambda p: self.get_time_frequency_array(p),
                       map(lambda s: self.get_peaks(s), self.spectrograms))

        if self.mono():
            return extract_peak_coords()[0]
        else:
            return np.vstack(extract_peak_coords())

    @classmethod
    def fromwav(cls, wav_file):
        np.seterr(divide='ignore')
        sample_rate, values = wav_file
        sgrams = []
        if values.ndim == 1:
            sgram, _, times = specgram(values, NFFT=512,
                                       Fs=sample_rate,
                                       window=window_hanning,
                                       noverlap=256)
            sgrams.append(np.where(sgram > 0, 10 * np.log10(sgram), 0))
        elif values.ndim == 2:
            for channel in xrange(values.ndim):
                ch_values = values[:,channel]
                sgram, _, times = specgram(ch_values, NFFT=512,
                                           Fs=sample_rate,
                                           window=window_hanning,
                                           noverlap=256)
                sgrams.append(np.where(sgram > 0, 10 * np.log10(sgram), 0))
        else:
            print_error_and_exit("{} has more than 2 channels, invalid WAV")

        return cls(sample_rate, sgrams, times)

    def get_peaks(self, array):
        # This is a huge mess but returns an array where it's True
        # everywhere there's a peak

        binary_struct = generate_binary_structure(2, 1)
        # This generates a diamond shaped boolean array, i.e.
        # False True False
        # True  True True
        # False True False

        footprint = iterate_structure(binary_struct, 20)
        # This inflates the structure to 40x40 while maintaining the same
        # pattern

        filtered_array = maximum_filter(array, footprint=footprint)
        # This replaces each point in the array with the largest point
        # within the footprint centered at the original point's location

        # If the filtered value equals the original value, that value is the
        # maximum within the footprint area

        local_maxima = (filtered_array == array)
        # This is now a boolean array where True marks points of local
        # maxima. However, this still includes areas where there is no
        # information (0) in that 40x40 diagonal; let's filter those out

        no_data = (array == 0)
        # This is True everywhere the input array is 0

        no_data_min = minimum_filter(no_data, footprint=footprint)
        # This is now True only where there is a 40x40 diagonal region
        # of only zero around each point in question in the original array
        # this point would be detected as a peak by our above algorithm, but
        # it isn't actually a peak, so we'll exclude it.

        no_data_neg = np.logical_not(no_data_min)
        detected_peaks = np.logical_and(local_maxima, no_data_neg)
        # Negate this and then AND with the calculated local maxima to
        # exclude these zero point regions from the final peak calculation
        # result

        # Now let's filter out all areas that are below the amplitude cutoff
        above_amp_min = (array > PEAK_MINIMUM_AMPLITUDE)
        detected_peaks_filtered = np.logical_and(detected_peaks,
                                             above_amp_min)
        # This is now True only where there's a peak with an amplitude above
        # the cutoff
        output = detected_peaks_filtered

        return output
