from midiutil import MIDIFile
import datetime

now = datetime.datetime.now()
if __name__ == "__main__":

    track = 0
    channel = 0
    duration = 1  # In beats
    tempo = 120  # In BPM
    volume = 80  # 0-127, as per the MIDI standard

    MyMIDI = MIDIFile(2, adjust_origin=True)
    MyMIDI.addTempo(track, 0, tempo)
    with open('data.txt') as f:
        lines = f.readlines()
        for i in range(0, 16):
            chord = [int(n) for n in lines[2*i].split()]
            MyMIDI.addNote(track, channel, chord[0], i, duration, volume)
            MyMIDI.addNote(track, channel, chord[1], i, duration, volume)
            MyMIDI.addNote(track, channel, chord[2], i, duration, volume)
        counter = 0
        volume = 110
        for i in range(16, 48):
            melody = [int(n) for n in lines[2*i].split()]
            MyMIDI.addNote(track, channel, melody[0], counter, duration, volume)
            counter+=0.5

        MyMIDI.addNote(track, channel, 200, 17, duration, volume)
    name = "music_"+str(now.month)+"-"+str(now.day)+"_"+str(now.hour)+"-"+str(now.minute)+".mid"
    with open(name, "wb") as output_file:
        MyMIDI.writeFile(output_file)




