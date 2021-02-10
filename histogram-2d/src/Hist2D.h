#ifndef hist2d_h
#define hist2d_h

class Hist2D {
public:
	Hist2D(int *binsInput, int n);

	void setBins(int *binsInput);
	
	void setBinCount(int n);

	int* getBins();

	int getBinCount();

	void compute(int *head, int w, int h, int stride, int *out);

private:
	int bins[100];
	int m;
};

#endif
