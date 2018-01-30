# Training data format
I am using [this repo](https://github.com/CarlSouthall/MDBDrums) to get labeled data. The labelled data should match that: a set of timestamps and which drum is active at that timestamp. Here is a list of the possible drums:
 - KD: kick drum
 - SD: snare drum
 - HH: high hat
 - TT: tom
 - CY: cymbol
 - OT: other

Point the script to two folder, one with training data, one with audio data (wav files). The name of the label txt file should be the same as the wav file.
