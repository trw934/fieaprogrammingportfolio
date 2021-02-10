#include "Hist2D.h"
#include <iostream>

/* Initializes the bins of the histogram and the
 * number of bins.
 */
Hist2D::Hist2D(int *binsInput, int n) {
	m = n;
	int i = 0;
	for (i = 0; i < 2*m; i++) {
		bins[i] = binsInput[i];
	}
}

// Mutator method for bins
void Hist2D::setBins(int *binsInput) {
	int i = 0;
	for (i = 0; i < 2*m; i++) {
		bins[i] = binsInput[i];
	}
}

// Mutator method for number of bins
void Hist2D::setBinCount(int n) {
	m = n;
}

// Accessor method for bins
int * Hist2D::getBins() {
	return bins;
}

// Accessor method for number of bins
int Hist2D::getBinCount() {
	return m;
}

/* Computes the histogram for the image entered. 'Head' represents the 
 * first pixel in the tile,'w' is the width of the tile, 'h' is the height
 * of the tile, 'stride' is the width of the image, and 'out' is the 
 * output of the histogram.
 */
void Hist2D::compute(int *head, int w, int h, int stride, int *out) {
	int i = 0;
	int j = 0;
	int bin = 0;
	int k = 0;
	int data = head[k];
	
	for (j = 0; j < h; j++) {
	  /*Checks all rows in each column*/
		for (i = 0; i < w; i++) {
			for (bin = 0; bin < 2*m; bin = bin + 2) {
			  int data = head[k];
				/* If it belongs in the current bin, adds 1 to
				current bin value and moves to next data input*/
				if (data >= bins[bin] && data <= bins[bin+1]) {
					out[bin/2]++;
					bin = 2*m;
				}
			}
			k++;
		}
		k = k + stride - w;
	}
}
