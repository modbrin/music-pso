#pragma once
#include "FitnessOne.h"

//Get real offset of melody note to closest chord note
double getInChordOffset(double note, double* chordNotes, int chordNotesSize) {
	double min = 128;
	for (int i = 0; i < chordNotesSize; i++) {
		double curr = fmod(chordNotes[i], 12) - fmod(note, 12);
		if (abs(min) > abs(curr)) {
			min = curr;
		}
	}
	for (int i = 0; i < chordNotesSize; i++) {
		double curr = fmod(chordNotes[i], 12) - 12 - fmod(note, 12);
		if (abs(min) > abs(curr)) {
			min = curr;
		}
	}
	for (int i = 0; i < chordNotesSize; i++) {
		double curr = fmod(chordNotes[i], 12) + 12 - fmod(note, 12);
		if (abs(min) > abs(curr)) {
			min = curr;
		}
	}
	return min;
}

//Evaluate how far is melody note from closest chord note
double getInChordSingle(double note, double* chordNotes, int chordNotesSize) {
	double min = 128;
	for (int i = 0; i < chordNotesSize; i++) {
		double curr = abs(fmod(chordNotes[i], 12) - fmod(note, 12));
		if (min > curr) {
			min = curr;
		}
	}
	for (int i = 0; i < chordNotesSize; i++) {
		double curr = abs(fmod(chordNotes[i], 12) - 12 - fmod(note, 12));
		if (min > curr) {
			min = curr;
		}
	}
	for (int i = 0; i < chordNotesSize; i++) {
		double curr = abs(fmod(chordNotes[i], 12) + 12 - fmod(note, 12));
		if (min > curr) {
			min = curr;
		}
	}
	return -min;
}

//Evaluate melody sequence of particle (even indexes) to be one of according chord notes
double getInChords(double* sequence, int sequenceSize, double** chords, int chordsCount) {
	double sum = 0;
	for (int i = 0; i < sequenceSize; ++i) {
		sum += getInChordSingle(sequence[i], chords[(int)(i / 2)], 3);
	}
	return sum;
}
//upper limit for melody is 96, lower limit is upper note of according chord
double upper2 = 96;

//Evaluate particle to be in range
double getInRangeOnChords(double* sequence, int sequenceSize, double** chords, int chordsSize) {
	double inside = 0;
	for (int i = 0; i < sequenceSize; i += 2) {
		if (round(sequence[i]) > chords[(int)(i / 2)][2] && round(sequence[i]) < upper2) {
			inside++;
		}
		else if (round(sequence[i]) > upper2) {
			inside -= abs(sequence[i] - upper2);
		}
		else if (round(sequence[i]) < chords[(int)(i / 2)][2]) {
			inside -= abs(sequence[i] - chords[(int)(i / 2)][2]);
		}
		if (round(sequence[i + 1]) > chords[(int)(i / 2)][2] && round(sequence[i + 1]) < upper2) {
			inside++;
		}
		else if (round(sequence[i + 1]) > upper2) {
			inside -= abs(sequence[i + 1] - upper2);
		}
		else if (round(sequence[i + 1]) < chords[(int)(i / 2)][2]) {
			inside -= abs(sequence[i + 1] - chords[(int)(i / 2)][2]);
		}
	}
	return (double)inside;
}

//Generate sequence of tonality scale notes in respect to mode
//length:9
double* getScale(double tonic, bool isMajor) {
	double* scale = new double[9];
	if (isMajor) {
		scale[0] = fmod(tonic - 1, 12);
		scale[1] = fmod(tonic, 12);
		scale[2] = fmod(tonic + 2, 12);
		scale[3] = fmod(tonic + 4, 12);
		scale[4] = fmod(tonic + 5, 12);
		scale[5] = fmod(tonic + 7, 12);
		scale[6] = fmod(tonic + 9, 12);
		scale[7] = fmod(tonic + 11, 12);
		scale[8] = fmod(tonic + 12, 12);
	}
	else {
		scale[0] = fmod(tonic - 2, 12);
		scale[1] = fmod(tonic, 12);
		scale[2] = fmod(tonic + 2, 12);
		scale[3] = fmod(tonic + 3, 12);
		scale[4] = fmod(tonic + 5, 12);
		scale[5] = fmod(tonic + 7, 12);
		scale[6] = fmod(tonic + 8, 12);
		scale[7] = fmod(tonic + 10, 12);
		scale[8] = fmod(tonic + 12, 12);
	}
	return scale;
}

//Get real offset of melody note from closest scale note
double getScaleOffset(double note, double* scale) {
	double min = 128;
	double curr;
	for (int i = 0; i < 9; i++) {
		curr = scale[i] - fmod(note, 12);
		if (abs(curr) < abs(min)) {
			min = curr;
		}
	}
	return min;
}

//Evaluate how far is melody note from closest scale note
double getInScaleSingle(double note, double tonic, bool isMajor, double* scaleNotes) {
	double min = 128;
	double curr;
	for (int i = 0; i < 9; i++) {
		curr = abs(fmod(note, 12) - scaleNotes[i]);
		if (curr < min) {
			min = curr;
		}
	}
	return min;
}

//Evaluate how far is melody from scale notes
double getInScale(double* noteArray, int noteArraySize, double tonic, bool isMajor, double* scaleNotes) {
	double sum = 0;
	for (int i = 1; i < noteArraySize; i += 2) {
		sum += getInScaleSingle(noteArray[i], tonic, isMajor, scaleNotes);
	}
	return -sum;
}

namespace fitnessTwo {
	double getFitness(double* noteArray, int noteArraySize, double tonic, double* tonalityNotes, int tonalityNotesSize, double** chordList, int chordListSize) {
		bool isMajor = round(abs(chordList[0][0] - chordList[0][1])) == 4;
		double* scaleNotes = getScale(tonic, isMajor);
		double inScale = getInScale(noteArray, noteArraySize - 1, tonic, isMajor, scaleNotes);
		delete scaleNotes;
		double inChord = getInChords(noteArray, noteArraySize, chordList, chordListSize);
		double ending = getEndingWithTonic(noteArray, noteArraySize, tonic);
		double fractured = getFractured(noteArray, noteArraySize);
		double repeated = getRepeated(noteArray, noteArraySize);
		double inRange = getInRangeOnChords(noteArray, noteArraySize, chordList, chordListSize);
		return 12170.99 * inChord + 5020.227 * inRange + 1000 * fractured + 400 * ending + 300000 * repeated + 6270.996 * inScale;
	}
}

//make sure even melody notes are good (in Scale), makin small adjustments to their positions. If any fixing occurs it is printed to console.
bool melodyFixEven(double* base, double* scale) {
	double previous = *base;
	std::cout << previous;
	*base += getScaleOffset(*base, scale);
	if (previous != *base) {
		std::cout << " fixed to " << *base << std::endl;
		return true;
	}
	else {
		std::cout << std::endl;
		return false;
	}
}

//make sure odd melody notes are good (in Chords), making small adjustments to their positions. If any fixing occurs it is printed to console.
bool melodyFixOdd(double* base, double* chordNotes, int chordNotesSize) {
	double previous = *base;
	std::cout << previous;
	*base += getInChordOffset(*base, chordNotes, chordNotesSize);
	if (previous != *base) {
		std::cout << " fixed to " << *base << std::endl;
		return true;
	}
	else {
		std::cout << std::endl;
		return false;
	}
}