#!/usr/bin/env python
import argparse
import os
import pickle
import numpy as np


def load_labels(path):
    data = {}
    for file_name in os.listdir(os.path.abspath(path)):
        if file_name.endswith(".pkl"):
            data[file_name[:-4]] = pickle.load(open(path + file_name, 'r'))

    return data


def load_audio_sonogrames(path, label_names):
    data = {}
    for file_name in os.listdir(os.path.abspath(path)):
        if file_name.endswith(".pkl") and file_name[:-4] in label_names:
            data[file_name[:-4]] = pickle.load(open(path + file_name, 'r'))

    return data


def get_args():
    parser = argparse.ArgumentParser()
    parser.add_argument("out", help="Where to save training data too")
    parser.add_argument("-x", "--audio-folder", dest="audio_folder", required=True, help="Folder where audio sonograms are")
    parser.add_argument("-y", "--label-folder", dest="label_folder", required=True, help="Folder where label data is")
    parser.add_argument("--show", action="store_true", default=False, help="Show the resulting sonograms")

    args = parser.parse_args()

    if not os.path.exists(args.audio_folder):
        raise Exception("Not a valid path: " +  args.audio_folder)
    if not os.path.exists(args.label_folder):
        raise Exception("Not a valid path: " + args.label_folder)

    return args


def main():
    args = get_args()

    labels = load_labels(args.label_folder)
    audio = load_audio_sonogrames(args.audio_folder, labels.keys())

    if len(labels) != len(audio):
        raise Exception("The number of labels and audio bits don't match!")

    # ignore the first few samples (samples) each sample is 1/44100 s
    TRIM_START = 20
    # trim frequencies (bins) each bin is like 5hz
    FREQ_TRIM_LOW = 4
    FREQ_TRIM_HIGH = 100

    # number of samples to delay a response by
    HIT_DELAY = 50
    HIT_WIDTH = 5
    # how wide to make each image (in samples)
    IMAGE_SIZE = 100
    FRAMES_PER_IMAGE = 3

    SAMPLE_RATE = 44100

    training_images = []
    training_labels = []

    i = 0
    for file_name, audio_data in audio.iteritems():
        i += 1
        print "Generating for:", file_name, i
        label_data = labels[file_name][:, TRIM_START:]
        audio_data = audio_data[FREQ_TRIM_LOW:FREQ_TRIM_HIGH, TRIM_START:]
        print "Data loaded! Generating images now"

        if args.show:
            import cv2
            drums = sorted(['KD', 'SD', 'HH', 'TT', 'CY', 'OT'])
            kick_index = drums.index('KD')
            kick_label = label_data[kick_index]

            to_show = audio_data / np.percentile(audio_data, 99)
            to_show = np.clip(to_show, 0.0, 1.0)
            to_show = cv2.resize(to_show, (0, 0), fx=1.0, fy=1.0)

            for col in range(audio_data.shape[1]):
                if (col > audio_data.shape[1] - HIT_DELAY):
                    break

                if kick_label[col] == 1:
                    to_show[:, col + HIT_DELAY] = 1.0

            half = audio_data.shape[1] / 2
            cv2.line(to_show, (half, 0), (half, audio_data.shape[0]), 1.0, 1)
            cv2.line(to_show, (half + IMAGE_SIZE, 0), (half + IMAGE_SIZE, audio_data.shape[0]), 1.0, 1)

            cv2.imshow(file_name, to_show)
            cv2.waitKey(0)

        drums = sorted(['KD', 'SD', 'HH', 'TT', 'CY', 'OT'])
        kick_drum = label_data[drums.index('KD')]

        cols = audio_data.shape[1]
        this_col = IMAGE_SIZE
        while this_col < cols - HIT_WIDTH:
            kick_hit = bool(kick_drum[this_col - HIT_DELAY])
            for offset in range(1, HIT_WIDTH):
                if kick_hit == True:
                    break

                if kick_drum[this_col - HIT_DELAY - offset] == 1:
                    kick_hit = True
                elif kick_drum[this_col - HIT_DELAY + offset] == 1:
                    kick_hit = True

            this_image = audio_data[:, this_col - IMAGE_SIZE:this_col]
            # the first row is the label, either all zero or all one
            label = np.zeros(shape=(1, this_image.shape[1])) + int(kick_hit)
            this_image = np.vstack((label, this_image))

            training_images.append(np.copy(this_image))
            training_labels.append(kick_hit)

            this_col += FRAMES_PER_IMAGE

        images = np.dstack(training_images)
        print "Saving data... {}x{}x{}".format(*images.shape)
        np.save(open(args.out, 'w'), images)


if __name__ == "__main__":
    main()
