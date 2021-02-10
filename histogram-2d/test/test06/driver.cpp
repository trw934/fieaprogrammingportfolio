#include <iostream>
#include <fstream>
using namespace std;

#include "Hist2D.h"


int main(int argc, char **argv) {
	int width = 1;
	int height = 3;
	int tw = 1;
	int th = 3;
	int i = 0;
	int data[3] = {0};

	for (i = 0; i < 3; i++) {
		data[i] = (i+1)*6;
	}

	/* Check Program Usage */
	if (argc != 2) {
		fprintf(stderr, "hist2d.exe error: arg count");
		return 1;
	}

	/* Open output file */
	ofstream outStream (argv[1]);
	/* Set up bins */
	int numBins = 2;
	int binsInput[numBins*2];
	int out[numBins] = {0};
	int j = 0;

	for (i = 0; i < numBins*2; i++) {
		if(i % 2 == 0) {
			binsInput[i] = j+1;
			j+=1;
		} else {
			binsInput[i] = j+9;
			j+=9;
		}
	}
	
	/* Construct a histogram */
	Hist2D *hist = new Hist2D(binsInput,4);

	/* Build the histogram from the data set */
	int *head = &data[0];
	hist->compute(head, tw, th, width, out);

	/* Write the result to the output file */
	for (i = 0; i < 2; i++) {
		outStream << "H(" << (i+1) <<") = " << out[i] << " ";
	}
	
	/* Wrapup */
	outStream.close();
	return 0;
}
