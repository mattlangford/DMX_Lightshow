#!/usr/bin/env python
import argparse
import numpy as np
import os
import wave
import pickle
import struct

import cv2

from matplotlib import pyplot as plt

def load_audio(path):
    # Load audio files that match the label_names
    data = {}
    for file_name in os.listdir(os.path.abspath(path)):
        if file_name.endswith(".wav"):
            data[file_name[:-4]] = wave.open(path + file_name, 'r')

    return data


def read_audio_file(audio_file):
    length = audio_file.getnframes()
    audio_data = np.zeros(shape=(length))
    for i in range(length):
        frame = audio_file.readframes(1)
        ch1 = struct.unpack("<h", frame[:2])[0]
        ch2 = struct.unpack("<h", frame[-2:])[0]

        audio_data[i] = 0.5 * (ch1 + ch2)

    return audio_data


def create_sonogram(audio_file, fft_size_samples, bin_length_seconds):
    frames_per_bin = audio_file.getframerate() * bin_length_seconds
    audio_data = read_audio_file(audio_file)

    bins = []
    frame_num = 0
    data_left = len(audio_data)

    while data_left > 0:
        frames_processed = min(len(audio_data[frame_num:]), frames_per_bin)
        data_left -= frames_processed
        frame_num += int(frames_per_bin)

        fft_data_complex = np.fft.fft(audio_data[frame_num - fft_size_samples:frame_num], fft_size_samples)
        fft_data = np.absolute(fft_data_complex)[:fft_size_samples / 2]

        bins.append(np.copy(fft_data))

    sonogram = np.vstack(bins)
    return sonogram.T


def get_args():
    parser = argparse.ArgumentParser()
    parser.add_argument("audio_folder", help="Folder where audio files are")
    parser.add_argument("--show", action="store_true", default=False, help="Show the resulting sonograms")

    args = parser.parse_args()

    if not os.path.exists(args.audio_folder):
        raise Exception("Not a valid path:", args.audio_folder)

    return args


def main():
    args = get_args()

    audio = load_audio(args.audio_folder)

    FFT_SIZE_SAMPLES = 2 ** 13 # 8k
    BIN_LENGTH_SECONDS = 0.01 # this is as accurate as the labels
    for file_name, audio_file in audio.iteritems():
        print "Computing sonogram for:", file_name
        sonogram = create_sonogram(audio_file, FFT_SIZE_SAMPLES, BIN_LENGTH_SECONDS)
        pickle.dump(sonogram, open(args.audio_folder + file_name + ".pkl", 'w'), 2)

        if args.show:
            to_show = np.flipud(sonogram[:185, :])
            to_show = np.clip(to_show / np.percentile(to_show, 99), 0.0, 1.0)
            cv2.imshow(file_name, cv2.resize(to_show, (0, 0), fy=1.0, fx=0.5))
            cv2.waitKey(0)


if __name__ == "__main__":
    main()
