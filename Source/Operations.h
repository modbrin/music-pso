#pragma once
#include <iostream>

//List add List, sum elements of array1 and array2 and put into dest
//size - all arrays should be same size
void lal(double* dest, double* array1, double* array2, int size) {
	for (int i = 0; i < size; i++) {
		dest[i] = array1[i] + array2[i];
	}
}
//List subtract List, subtract elements of array2 from array1 and put into dest
//size - all arrays should be same size
void lsl(double* dest, double* array1, double* array2, int size) {
	for (int i = 0; i < size; i++) {
		dest[i] = array1[i] - array2[i];
	}
}
//Multiply list by constant, multiply elements of list by scalar and put into dest
//size - all arrays should be same size
void mlc(double* dest, double* array, double constant, int size) {
	for (int i = 0; i < size; i++) {
		dest[i] = array[i] * constant;
	}
}
//Divide list by constant, divide elements of list by scalar and put into dest
//size - all arrays should be same size
void dlc(double* dest, double* array, double constant, int size) {
	for (int i = 0; i < size; i++) {
		dest[i] = array[i] / constant;
	}
}
//Round elements of array in place
void roundArray(double* arr, int size) {
	for (int i = 0; i < size; i++) {
		arr[i] = round(arr[i]);
	}
}
//copy elements of one array into another array
void copy(double* to, double* from, int size) {
	memcpy(to, from, size * sizeof(double));
}
