#include <iostream>
#include <fstream>
using namespace std;

#include "Hist2D.h"


int main(int argc, char **argv) {
	int width = 2;
	int height = 4;
	int tw = 2;
	int th = 1;
	int i = 0;
	int data[8] = {0};

	/* Check Program Usage */
	if (argc != 4) {
		fprintf(stderr, "hist2d.exe error: arg count");
		return 1;
	}

	/* Open input and output files */
	ifstream pixelStream (argv[1]);
	ifstream binStream (argv[2]);
	ofstream outStream (argv[3]);

	/* Read data from bin file */
	int numBins = 2;
	int binsInput[numBins*2] = {0};
	int out[numBins] = {0};
	for (i = 0; i < numBins*2; i++) {
	  binStream >> binsInput[i];
	}
	binStream.close();
	
	/* Read data from pixel file */
	for (i = 0; i < 8; i++) {
		pixelStream >> data[i];
	}
	pixelStream.close();
	
	/* Construct a histogram */
	Hist2D *hist = new Hist2D(binsInput,2);

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
