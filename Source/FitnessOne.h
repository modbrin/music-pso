#pragma once
#include <random>
#include <ctime>

std::random_device rdc;
std::mt19937_64 gen(rdc());
std::uniform_real_distribution<double> dis(0.0, 1.0);
std::uniform_real_distribution<double> disn(-1.0, 1.0);

//Get random double in range 0:1
double randomFraction() {
	return dis(gen);
}

//Get random tonic note, i.e. number in range 0:11
double getRandomTonality() {
	return round(11 * dis(gen));
}

//Get random note of MIDI range, i.e. 0:127
double getRandomMidiNote() {
	return 127.0 * dis(gen);
}

//Get sequence of notes including tonics, subdominants, dominants of given tonality
//length:36
double* getTonalityNotes(double tonic) {
	tonic = tonic - 12;
	double* result = new double[36];
	for (int i = 0; i < 36; ) {
		result[i] = tonic;
		tonic += 5;
		i++;
		result[i] = tonic;
		tonic += 2;
		i++;
		result[i] = tonic;
		tonic += 5;
		i++;
	}
	return result;
}

//Get rangom sequence of given length
double* getRandomSequence(int number) {
	double* result = new double[number];
	for (int i = 0; i < number; i++) {
		result[i] = getRandomMidiNote();
	}
	return result;
}

//Evaluate how far is the particle from tonality notes: tonic, subdominant, dominant
double getInTonality(double* sequence, int sequenceSize, double* tonalityNotes, int tonalityNotesSize) {
	double summ = 0;
	for (int i = 0; i < sequenceSize; i++) {
		double min = 127;
		double diff;
		for (int j = 0; j < tonalityNotesSize; j++) {
			diff = abs(tonalityNotes[j] - sequence[i]);
			if (diff < min) {
				min = diff;
			}
		}
		summ += 1 - min / 3;
	}
	return summ;
}

//bounds for chord bases, 77 = 96 - 7(for chord upper notes) - 12(for melody note)
double lower = 48, upper = 77;

//Evaluate particle to be in range 48:77
double getInRange(double* sequence, int sequenceSize) {
	int inside = 0;
	for (int i = 0; i < sequenceSize; i++) {
		if (sequence[i]<upper&&sequence[i]>lower) {
			inside++;
		}
	}
	return (double)inside / (double)sequenceSize;
}

//Evaluate how fractured is the sequence, i.e. neighbour notes should not be more than 12 apart
double getFractured(double* sequence, int sequenceSize) {
	double tooBig = 0;
	for (int i = 1; i < sequenceSize; i++) {
		double diff = abs(sequence[i] - sequence[i - 1]);
		if (diff > 12) {
			tooBig += diff;
		}
	}
	return -tooBig;
}

//Evaluate last note of particle to be tonic
double getEndingWithTonic(double* sequence, int sequenceSize, double tonic) {
	double* tonics = new double[3];
	tonics[0] = tonic - 12;
	tonics[1] = tonic;
	tonics[2] = tonic + 12;
	double min = 127;
	for (int i = 0; i < 3; i++) {
		double curr = abs(((int)round(sequence[sequenceSize - 1])) % 12 - tonics[i]);
		if (curr < min) {
			min = curr;
		}
	}
	free(tonics);
	return 1 - min / 6;
}

//Evaluate particle to avoid having >4 repeated notes in row
double getRepeated(double* sequence, int sequenceSize) {
	int counter = 0;
	for (int i = 0; i < sequenceSize - 5; ++i) {
		if ((sequence[i] == sequence[i + 1]) && (sequence[i + 1] == sequence[i + 2]) && (sequence[i + 2] == sequence[i + 3]) && (sequence[i + 3] == sequence[i + 4]) && (sequence[i + 4] == sequence[i + 5])) {
			counter++;
		}
	}
	return 1.0 - ((double)counter / (double)(sequenceSize - 4));
}

//master function for fitness one, evaluates 16-dim particle of chord bases
namespace fitnessOne {
	double getFitness(double* noteArray, int noteArraySize, double tonic, double* tonalityNotes, int tonalityNotesSize, double** chordList, int chordListSize) {
		double ending = getEndingWithTonic(noteArray, noteArraySize, tonic);
		double fractured = getFractured(noteArray, noteArraySize);
		double inTonality = getInTonality(noteArray, noteArraySize - 1, tonalityNotes, tonalityNotesSize);
		double inRange = getInRange(noteArray, noteArraySize);
		double repeated = getRepeated(noteArray, noteArraySize);
		return 5 * inTonality + 600 * inRange + 30 * fractured + 15 * ending + 300000 * repeated;
	}
}

//make sure chord base notes are good (in TSD sequence), making small adjustments to their positions. If any fixing occurs it is printed to console. In this case priting is in main code.
bool chordFix(double* base, double* targetSequence, int targetSequenceSize) {
	double min = 128;
	double closestNote;
	for (int i = 0; i < targetSequenceSize; i++) {
		double curr = abs(round(targetSequence[i] - *base));
		if (curr < min) {
			min = curr;
			closestNote = targetSequence[i];
		}
	}
	if (min != 0) {
		*base = closestNote;
		return true;
	}
	else {
		return false;
	}
}
