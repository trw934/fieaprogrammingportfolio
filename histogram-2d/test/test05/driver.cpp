#include <iostream>
#include <fstream>
using namespace std;

#include "Hist2D.h"


int main(int argc, char **argv) {
	int width = 4;
	int height = 3;
	int tw = 4;
	int th = 1;
	int i = 0;
	int data[12] = {100,29,8,222,187,91,43,161,1,115,200,32};

	/* Check Program Usage */
	if (argc != 2) {
		fprintf(stderr, "hist2d.exe error: arg count");
		return 1;
	}

	/* Open output file */
	ofstream outStream (argv[1]);
	/* Set up bins */
	int numBins = 4;
	int binsInput[8] = {0,63,64,127,128,191,192,255};
	int out[numBins] = {0};
	
	/* Construct a histogram */
	Hist2D *hist = new Hist2D(binsInput,4);

	/* Build the histogram from the data set */
	int *head = &data[0];
	hist->compute(head, tw, th, width, out);

	/* Write the result to the output file */
	for (i = 0; i < 4; i++) {
		outStream << "H(" << (i+1) <<") = " << out[i] << " ";
	}
	
	/* Wrapup */
	outStream.close();
	return 0;
}
