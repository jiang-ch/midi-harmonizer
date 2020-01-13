import sys
import re

############################### Parameters ######################################

# Length of each bar in MIDI ticks (This changes the speed)
BAR = 2000
# Name of the text file with output from http://flashmusicgames.com/midi/mid2txt.php
INPUT = "midi.txt"
# Name of the output file (Will be overwritten if exists)
OUTPUT = "out.txt"

# Note:
# Input file has to be in in the same directory, and also note_frequencies.txt

#################################################################################

# Load the input file and filter the unnecessary lines
with open(INPUT, "r") as inpF:
    lines = inpF.readlines()
    # Remove the lines that are irrelevant
    lines = filter(lambda x: "On ch=" in x, lines)

    # Removing 'On ch=xx n=' from the line, split the line
    # by spaces, we now have the format
    #     [ ['tick', 'note id', 'v=ww'], ... ]
    lines = map(lambda x: re.sub("On ch=[0-9]* n=", '', x).split(" "), lines)

# Open up the notes and load it into an array of size 100
# Ignores the frequencies, we only care about the names
#       [ 'C0', 'C0#', 'D0',  ... , 'D8#']
notes = open('note_frequencies.txt', 'r').readlines()
notes = list(map(lambda x: x.split('\t')[0], notes))

# Open output file
with open(OUTPUT, "w") as outF:
    for line in lines:
        # Get the bar and index from the MIDI tick
        time = int(line[0])
        bar, index = (time // BAR), (time % BAR) / float(BAR)
        # Get the correct index into notes table (note offset)
        noteInd = int(line[1]) - 11
        # If not isn't valid ignore the line
        if not 0 <= noteInd < 100:
            continue
        note = notes[noteInd]
        # Write the line to the output file
        outF.write(f"{bar}\t{index}\t{note}\n")
