#pragma once

#include "Operations.h"
#include "FitnessChord.h"

class ChordParticle {
public:
	double* chord;
	double* velocity;
	double* personalBestNotes;
	double personalBestFitness;
	double currentFitness;
	const int chordSize = 3;

	double baseNote;

	double vfactor = 0.001;
	double phiPersonal = 2;
	double phiGlobal = 2;
	double k = 0.5;
	double phi = phiPersonal + phiGlobal;
	double divCompensation = (2 * k) / abs(2 - phi - sqrt(phi*phi - 4 * phi));
	bool isMajor;

	ChordParticle() {}
	//Destructor
	~ChordParticle() {
		delete[] velocity;
		delete[] chord;
		delete[] personalBestNotes;
	}
	//construct particle values
	void initParticle(double* chord, double baseNote, bool isMajor) {
		this->chord = chord;
		this->baseNote = baseNote;
		this->isMajor = isMajor;
		velocity = new double[chordSize]();
		personalBestNotes = new double[chordSize];
		copy(personalBestNotes, chord, chordSize);
		currentFitness = fitone::getFitness(chord, baseNote, isMajor);
		personalBestFitness = currentFitness;
	}
	//update velocity of particle
	void updateVelocity(double* globalBestNotes) {
		double* personalDifference = new double[chordSize];
		double* globalDifference = new double[chordSize];
		lsl(personalDifference, personalBestNotes, chord, chordSize);
		lsl(globalDifference, globalBestNotes, chord, chordSize);
		mlc(personalDifference, personalDifference, phiPersonal * uniform(genc), chordSize);
		mlc(globalDifference, globalDifference, phiGlobal*uniform(genc), chordSize);
		mlc(velocity, velocity, vfactor, chordSize);
		lal(velocity, velocity, personalDifference, chordSize);
		lal(velocity, velocity, globalDifference, chordSize);
		mlc(velocity, velocity, divCompensation, chordSize);
		delete[] personalDifference;
		delete[] globalDifference;
	}
	//change position of current particle by applying velocity
	void move() {
		lal(chord, chord, velocity, chordSize);
		currentFitness = fitone::getFitness(chord, baseNote, isMajor);
		if (currentFitness < personalBestFitness) {
			personalBestFitness = currentFitness;
			copy(personalBestNotes, chord, chordSize);
		}
	}

};

class PSO1 {

public:
	ChordParticle* particleArray;
	int particleArraySize;
	double* globalBestNotes;
	double globalBestFitness;
	int chordSize = 3;

	double baseNote;

	double* targetSequence;
	int targetSequenceSize;
	double upperLimit;
	double lowerLimit;
	bool isMajor;

	//create PSO object and initialize all particles
	PSO1(int samples, double baseNote, bool isMajor) {
		particleArraySize = samples;
		this->isMajor = isMajor;
		this->baseNote = baseNote;
		globalBestFitness = INFINITY;
		particleArray = new ChordParticle[samples];
		globalBestNotes = new double[chordSize];
		for (int i = 0; i < samples; i++) {
			particleArray[i].initParticle(fitone::getRandomChord(), baseNote, isMajor);
		}
		for (int i = 0; i < particleArraySize; i++) {
			if (particleArray[i].personalBestFitness < globalBestFitness) {
				globalBestFitness = particleArray[i].personalBestFitness;
				copy(globalBestNotes, particleArray[i].personalBestNotes, chordSize);
			}
		}
	}
	//Destructor
	~PSO1() {
		delete[] particleArray;
		delete[] globalBestNotes;
	}
	//get next iteration by updating speed and applying to position
	void nextIteration() {
		for (int i = 0; i < particleArraySize; i++) {
			particleArray[i].updateVelocity(globalBestNotes);
			particleArray[i].move();
			if (particleArray[i].personalBestFitness < globalBestFitness) {
				globalBestFitness = particleArray[i].personalBestFitness;
				copy(globalBestNotes, particleArray[i].personalBestNotes, chordSize);
			}
		}
	}
};

