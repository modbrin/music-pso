#pragma once

#include "Operations.h"
#include "FitnessTwo.h"

class Particle {
public:
	double(*fitnessFunction)(double* noteArray, int noteArraySize, double tonic, double* tonalityNotes, int tonalityNotesSize, double** chordList, int chordListSize);
	double* noteArray;
	double* velocity;
	double* personalBestNotes;
	int noteArraySize;
	double personalBestFitness;
	double currentFitness;
	int chordListSize;

	double tonic;
	double* tonalityNotes;
	int tonalityNotesSize;
	double** chordList;

	double phiPersonal = 0.5;
	double phiGlobal = 0.6;
	//disabled due to convergence negative influence
	//double k = 0.5;
	//double phi = phiPersonal + phiGlobal;
	//double divCompensation = (2 * k) / abs(2 - phi - sqrt(phi*phi - 4 * phi));
	double intertion_coeff = 0.7;
	Particle() {}

	~Particle() {
		delete[] noteArray;
		delete[] velocity;
		delete[] personalBestNotes;
	}

	//construct particle values
	void initParticle(double* noteArray, int noteArraySize, double tonic, double* tonalityNotes, int tonalityNotesSize, double(*fitnessFunction)(double* noteArray, int noteArraySize, double tonic, double* tonalityNotes, int tonalityNotesSize, double** chordList, int chordListSize), double** chordList, int chordListSize) {
		this->noteArray = noteArray;
		this->noteArraySize = noteArraySize;
		this->tonic = tonic;
		this->tonalityNotes = tonalityNotes;
		this->tonalityNotesSize = tonalityNotesSize;
		this->fitnessFunction = fitnessFunction;
		this->chordList = chordList;
		this->chordListSize = chordListSize;
		velocity = new double[noteArraySize]();
		for (int i = 0; i < noteArraySize; ++i) {
			velocity[i] = disn(gen);
		}
		personalBestNotes = new double[noteArraySize];
		copy(personalBestNotes, noteArray, noteArraySize);
		currentFitness = fitnessFunction(noteArray, noteArraySize, tonic, tonalityNotes, tonalityNotesSize, chordList, chordListSize);
		personalBestFitness = currentFitness;
	}

	//update velocity of particle
	void updateVelocity(double* globalBestNotes) {
		double* personalDifference = new double[noteArraySize];
		double* globalDifference = new double[noteArraySize];
		lsl(personalDifference, personalBestNotes, noteArray, noteArraySize);
		lsl(globalDifference, globalBestNotes, noteArray, noteArraySize);
		mlc(personalDifference, personalDifference, phiPersonal*randomFraction(), noteArraySize);
		mlc(globalDifference, globalDifference, phiGlobal*randomFraction(), noteArraySize);
		lal(velocity, velocity, personalDifference, noteArraySize);
		lal(velocity, velocity, globalDifference, noteArraySize);
		//disabled due to convergence negative influence
		//mlc(velocity, velocity, divCompensation, noteArraySize);
		mlc(velocity, velocity, intertion_coeff, noteArraySize);
		delete[] personalDifference;
		delete[] globalDifference;
	}

	//change position of current particle by applying velocity
	void move() {
		lal(noteArray, noteArray, velocity, noteArraySize);
		currentFitness = fitnessFunction(noteArray, noteArraySize, tonic, tonalityNotes, tonalityNotesSize, chordList, chordListSize);
		if (currentFitness > personalBestFitness) {
			personalBestFitness = currentFitness;
			copy(personalBestNotes, noteArray, noteArraySize);
		}
	}

};

class PSO {

public:
	double(*fitnessFunction)(double* noteArray, int noteArraySize, double tonic, double* tonalityNotes, int tonalityNotesSize, double** chordList, int chordListSize);
	Particle* particleArray;
	int particleArraySize;
	double* globalBestNotes;
	int noteArraySize;
	double globalBestFitness;

	double tonic;
	double* tonalityNotes;
	int tonalityNotesSize = 36;

	//create PSO object and initialize all particles
	PSO(int samples, double tonic, int size, double(*fitnessFunction)(double* noteArray, int noteArraySize, double tonic, double* tonalityNotes, int tonalityNotesSize, double** chordList, int chordListSize), double** chordList, int chordListSize) {
		particleArraySize = samples;
		noteArraySize = size;
		this->tonic = tonic;
		tonalityNotes = getTonalityNotes(tonic);
		globalBestFitness = -INFINITY;
		particleArray = new Particle[samples];
		globalBestNotes = new double[noteArraySize];
		for (int i = 0; i < samples; i++) {
			particleArray[i].initParticle(getRandomSequence(noteArraySize), noteArraySize, tonic, tonalityNotes, tonalityNotesSize, fitnessFunction, chordList, chordListSize);
		}
		for (int i = 0; i < particleArraySize; i++) {
			if (particleArray[i].personalBestFitness > globalBestFitness) {
				globalBestFitness = particleArray[i].personalBestFitness;
				copy(globalBestNotes, particleArray[i].personalBestNotes, noteArraySize);
			}
		}
	}
	//Destructor
	~PSO() {
		delete[] particleArray;
		delete[] globalBestNotes;
	}

	//get next iteration by updating speed and applying to position
	void nextIteration() {
		for (int i = 0; i < particleArraySize; i++) {
			particleArray[i].updateVelocity(globalBestNotes);
			particleArray[i].move();
			if (particleArray[i].personalBestFitness > globalBestFitness) {
				globalBestFitness = particleArray[i].personalBestFitness;
				copy(globalBestNotes, particleArray[i].personalBestNotes, noteArraySize);
			}
		}
	}
};
