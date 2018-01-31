# Training data format
I am using [this repo](https://github.com/CarlSouthall/MDBDrums) to get labeled data. The labelled data should match that: a set of timestamps and which drum is active at that timestamp. Here is a list of the possible drums:
 - KD: kick drum
 - SD: snare drum
 - HH: high hat
 - TT: tom
 - CY: cymbol
 - OT: other


There are three scripts that take different data:
 - `generate_sonograms.py`: Take wav audio file, generate pickle file containing a numpy sonogram
 - `parse_labeled_data.py`: Take those sonograms and then generate another pickle containing another numpy array which encodes which drum is active
 - `generate_training_data.py` : Take the sonograms and labels and generate a numpy data file that contains images and labels for each label

