#pragma once
#include <random>
#include "Operations.h"

std::random_device rd;
std::uniform_real_distribution<double> uniform(0.0, 1.0);
std::mt19937_64 genc(rd());

//get random boolean
double getRandomBool() {
	return ((bool)round(uniform(genc)));
}

//set of functions required for chord generationg PSO
namespace fitone {
	//get random note of midi range[0-127]
	double getRandomNote() {
		return uniform(genc) * 127;
	}

	//get array of three random[0-127] notes
	double* getRandomChord() {
		double* chord = new double[3];
		for (int i = 0; i < 3; i++) {
			chord[i] = getRandomNote();
		}
		return chord;
	}

	//get array of all tonics in octaves, i.e. 12 notes
	double* getTonicNotes(double tonic) {
		tonic = tonic - 12;
		double* result = new double[12];
		for (int i = 0; i < 12; i++) {
			result[i] = tonic;
			tonic += 12;
		}
		return result;
	}

	//get penalty of chord
	double getChordShapePenalty(double* chord, double baseNote, bool isMajor) {
		double fstarget;
		if (isMajor) {
			fstarget = baseNote + 4;
		}
		else {
			fstarget = baseNote + 3;
		}
		double fttarget = baseNote + 7;
		return abs(chord[0] - baseNote) + abs(round(chord[1]) - fstarget) + abs(round(chord[2]) - fttarget);
	}
	//get fitness of chord
	double getFitness(double* chord, double baseNote, bool isMajor) {
		double chordShapePenalty = getChordShapePenalty(chord, baseNote, isMajor);
		return chordShapePenalty;
	}
}
