#include <ctime>
#include <fstream>
#include "PSO1.h"
#include "PSO.h"

int main()
{
	//Initial setup
	int particles = 5000;
	int iterations = 400;
	//Generate tonality and mode
	bool isMajor = getRandomBool();
	double tonic = getRandomTonality();
	//At first we generate sequence of base notes for chords, i.e. 16-dim particles

	//PSO for generationg chord bases starts
	clock_t begin = clock();
	//Create PSO object
	PSO* leftHand = new PSO(particles, tonic, 16, &fitnessOne::getFitness, nullptr, 0);

	for (int i = 0; i < iterations; i++) {
		leftHand->nextIteration();
		if (i % ((iterations / 100 == 0) ? 1 : iterations / 100) == 0) {
			std::cout << "\rPSO executing chord bases: " << (i / ((iterations / 100 == 0) ? 1 : iterations / 100)) << " %, Current best fitness: " << leftHand->globalBestFitness << "            " << std::flush;
		}
	}
	clock_t end = clock();
	//PSO for generating chord bases ends
	std::cout << "\rChord bases execution finished. Best fitness achieved: " << leftHand->globalBestFitness << "\n" << std::endl;
	std::cout << "Sequence of tonics, subdominants, dominants: ";
	for (int i = 0; i < 36; i++) {
		if (leftHand->tonalityNotes[i] >= 0 && leftHand->tonalityNotes[i] <= 127) {
			std::cout << leftHand->tonalityNotes[i] << " ";
		}
	}

	std::cout << std::endl;
	double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
	std::cout << "Tonic: " << leftHand->tonic << " , Mode: " << ((isMajor) ? "Major" : "Minor") << std::endl;
	std::cout << "\nPSO elapsed time: " << elapsed_secs << " seconds" << std::endl;
	//print infor about tonality and elapsed time

	//get rid of decimal places by rounding to integer number
	roundArray(leftHand->globalBestNotes, 16);

	//checking chord notes, actual fixes appear very rarely
	double* tsdds = leftHand->tonalityNotes;
	double* tonics = fitone::getTonicNotes(leftHand->tonic);

	for (int i = 0; i < 15; i++) {
		std::cout << leftHand->globalBestNotes[i];
		if (chordFix(&leftHand->globalBestNotes[i], tsdds, 36)) {
			std::cout << " fixed to " << leftHand->globalBestNotes[i];
		}
		std::cout << std::endl;
	}
	std::cout << leftHand->globalBestNotes[15];
	if (chordFix(&leftHand->globalBestNotes[15], tonics, 12)) {
		std::cout << " fixed to " << leftHand->globalBestNotes[15];
	}
	std::cout << std::endl;
	//checking finished

	//Actual chords generation start
	//For each chord base run PSO and take one chord at a time
	double** chords = new double*[16];
	begin = clock();

	for (int i = 0; i < 16; i++) {
		PSO1* chordGen = new PSO1(1000, leftHand->globalBestNotes[i], isMajor);
		std::cout << "\rPSO generating chords for bases: " << i + 1 << "/16";
		for (int i = 0; i <= 1000; i++) {
			chordGen->nextIteration();
		}
		chords[i] = new double[3];
		//take generated chord and add to main chord list
		memcpy(chords[i], chordGen->globalBestNotes, sizeof(double) * 3);
		roundArray(chords[i], 3);
		delete chordGen;
	}
	end = clock();

	double elapsed_secs2 = double(end - begin) / CLOCKS_PER_SEC;
	std::cout << "\nPSO elapsed time: " << elapsed_secs2 << " seconds, " << (elapsed_secs2 / 16) << " per chord." << std::endl;
	//chords generation finished

	//write chords to file
	std::ofstream myfile;
	myfile.open("data.txt");
	for (int i = 0; i < 16; i++) {
		for (int j = 0; j < 3; j++) {
			std::cout << chords[i][j] << " ";
			myfile << (int)chords[i][j] << " ";
		}
		myfile << "\r\n";
		std::cout << std::endl;
	}
	delete leftHand;
	//PSO for generating melody starts
	begin = clock();
	PSO rightHand((int)round(particles * 3), tonic, 32, &fitnessTwo::getFitness, chords, 16);
	iterations *= 3;//increase number of iterations and particles for that case
	for (int i = 0; i < iterations; i++) {
		rightHand.nextIteration();
		if (i % ((iterations / 100 == 0) ? 1 : iterations / 100) == 0) {
			std::cout << "\rPSO2 executing melody: " << (i / ((iterations / 100 == 0) ? 1 : iterations / 100)) << " %, Current best fitness: " << rightHand.globalBestFitness << "         " << std::flush;
		}
	}
	end = clock();
	std::cout << "\rMelody execution finished. Best fitness achieved: " << rightHand.globalBestFitness << std::endl;
	double elapsed_secs3 = double(end - begin) / CLOCKS_PER_SEC;
	std::cout << "Melody generation elapsed time: " << elapsed_secs3 << " seconds" << std::endl;
	//PSO for generating melody finished

	//get scale for checking
	double* scale = getScale(tonic, isMajor);
	roundArray(rightHand.globalBestNotes, rightHand.noteArraySize);

	//melody check start
	for (int i = 0; i < rightHand.noteArraySize; i += 2) {
		melodyFixOdd(&rightHand.globalBestNotes[i], chords[(int)(i / 2)], 3);
		if (i != rightHand.noteArraySize - 1) {
			melodyFixEven(&rightHand.globalBestNotes[i + 1], scale);
		}
	}
	//melody check finished

	//write melody to file
	for (int i = 0; i < rightHand.noteArraySize; i++) {
		myfile << (int)round(rightHand.globalBestNotes[i]) << " ";
		myfile << "\r\n";
	}
	myfile.close();

	//generate midi
	system("python midi_gen.py");
	return 0;
}
