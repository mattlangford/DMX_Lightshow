#!/usr/bin/env python
import argparse
import numpy as np
import os
import pickle
from collections import defaultdict

from matplotlib import pyplot as plt
import cv2

def load_labels(path):
    # Load all .txt files from the folder
    data = {}
    for file_name in os.listdir(os.path.abspath(path)):
        if file_name.endswith(".txt"):
            with open(path + file_name) as f:
                data[file_name[:-4]] = f.readlines()

    return data


def load_audio_sonogrames(path, label_names):
    # Load audio files that match the label_names
    data = {}
    for file_name in os.listdir(os.path.abspath(path)):
        if file_name.endswith(".pkl") and file_name[:-4] in label_names:
            data[file_name[:-4]] = pickle.load(open(path + file_name, 'r'))

    return data


def parse_labels(labels, sonogram_length, bin_length_seconds):
    drums = defaultdict(list)
    for l in labels:
        time, drum = l.split('\t')
        time = float(time)
        drum = drum.replace(' ', '').replace('\n', '')
        drums[drum].append(time)

    drums_sorted = sorted(drums.keys())
    parsed_labels = np.zeros(shape=(len(drums_sorted), sonogram_length))

    for drum_index in range(len(drums_sorted)):
        drum = drums_sorted[drum_index]
        times = drums[drum]

        for t in times:
            index = int(t / float(bin_length_seconds))
            parsed_labels[drum_index, index] = 1

    return parsed_labels


def get_args():
    parser = argparse.ArgumentParser()
    parser.add_argument("-x", "--audio-folder", dest="audio_folder", required=True, help="Folder where audio sonograms are")
    parser.add_argument("-y", "--label-folder", dest="label_folder", required=True, help="Folder where label data is")

    args = parser.parse_args()

    if not os.path.exists(args.audio_folder):
        raise "Not a valid path:", args.audio_folder
    if not os.path.exists(args.label_folder):
        raise "Not a valid path:", args.label_folder

    return args


def main():
    args = get_args()

    labels = load_labels(args.label_folder)
    audio = load_audio_sonogrames(args.audio_folder, labels.keys())

    BIN_LENGTH_SECONDS = 0.01
    for file_name, audio_data in audio.iteritems():
        print "Parsing labels from:", file_name
        labeled_sonogram = parse_labels(labels[file_name], audio_data.shape[1], BIN_LENGTH_SECONDS)
        pickle.dump(labeled_sonogram, open(args.label_folder + file_name + ".pkl", 'w'), 2)

if __name__ == "__main__":
    main()
