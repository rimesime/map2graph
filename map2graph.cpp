#include <iostream>
#include <cstdio>

#include <opencv2/opencv.hpp>
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

using namespace cv;
using namespace std;

Mat img;
unsigned char *input;
Mat img2;
unsigned char *input2;

vector<Mat> kernels;

inline int getPx(int i, int j, Mat &m) {
	if (j >= 0 && j < m.rows && i >= 0 && i < m.cols) {
		return ((unsigned char *) (m.data))[m.step[0] * j + m.step[1] * i];
	}
	return 0; // black
}

inline void setPx(int i, int j, int color) {
	if (j >= 0 && j < img.rows && i >= 0 && i < img.cols)
		input[img.step[0] * j + img.step[1] * i] = color;
}

inline void setPx(Mat &img, unsigned char *input, int i, int j, int color) {
	if (j >= 0 && j < img.rows && i >= 0 && i < img.cols)
		input[img.step[0] * j + img.step[1] * i] = color;
}

inline int match(int i, int j, Mat &img, Mat &k) {
	return (getPx(i - 1, j - 1, img) == getPx(0, 0, k)
			&& getPx(i + 0, j - 1, img) == getPx(1, 0, k)
			&& getPx(i + 1, j - 1, img) == getPx(2, 0, k)
			&& getPx(i - 1, j + 0, img) == getPx(0, 1, k)
			&& getPx(i + 1, j + 0, img) == getPx(2, 1, k)
			&& getPx(i - 1, j + 1, img) == getPx(0, 2, k)
			&& getPx(i + 0, j + 1, img) == getPx(1, 2, k)
			&& getPx(i + 1, j + 1, img) == getPx(2, 2, k));
}

int run_algorithm(const char* mapfilename, int times) {
	printf("Loading kernels...");
	for (int i = 0; i < 256; i++) {
		stringstream ss;
		ss << "kernel_" << i << ".png";
		Mat k = imread(ss.str().c_str(), CV_LOAD_IMAGE_UNCHANGED);
		if (!k.empty()) {
			kernels.push_back(k);
		}
	}
	printf("done.\n");

	printf("Loading file... ");
	img = imread(mapfilename, CV_LOAD_IMAGE_GRAYSCALE);
	img2 = imread(mapfilename, CV_LOAD_IMAGE_GRAYSCALE);
	if (img.empty()) // check whether the image is loaded or not
	{
		printf("Error : Image cannot be loaded!\n");
		return -1;
	}
	printf("done.\n");

	input = (unsigned char *) (img.data);
	input2 = (unsigned char *) (img2.data);

	// white & black
	printf("Making it white & black... ");
	for (int j = 0; j < img.rows; j++) {
		for (int i = 0; i < img.cols; i++) {
			if (getPx(i, j, img2) < 128) {
				// make it black
				setPx(i, j, 0);
			} else {
				// make it white
				setPx(i, j, 255);
			}
		}
	}
	img.copyTo(img2);
	printf("done.\n");

	for (int s = 0; s < times; s++) {

		printf("Eroding... Iteration [%d / %d]... ", s + 1, times);
		fflush(stdout);
		int changes = 0;

		for (int j = 1; j < img.rows - 1; j++) {
			for (int i = 1; i < img.cols - 1; i++) {

				// only work on white pixels
				if (getPx(i, j, img) == 0)
					continue;

				// match all white and all black and continue if this happens without
				// kernel check
				if ((getPx(i - 1, j - 1, img) > 0
						&& getPx(i + 0, j - 1, img) > 0
						&& getPx(i + 1, j - 1, img) > 0
						&& getPx(i - 1, j + 0, img) > 0
						&& getPx(i + 1, j + 0, img) > 0
						&& getPx(i - 1, j + 1, img) > 0
						&& getPx(i + 0, j + 1, img) > 0
						&& getPx(i + 1, j + 1, img) > 0)
						|| (getPx(i - 1, j - 1, img) == 0
								&& getPx(i + 0, j - 1, img) == 0
								&& getPx(i + 1, j - 1, img) == 0
								&& getPx(i - 1, j + 0, img) == 0
								&& getPx(i + 1, j + 0, img) == 0
								&& getPx(i - 1, j + 1, img) == 0
								&& getPx(i + 0, j + 1, img) == 0
								&& getPx(i + 1, j + 1, img) == 0))
					continue;

				int matches = 0;
				for (vector<Mat>::iterator it = kernels.begin();
						it != kernels.end(); ++it)
					if (match(i, j, img, *it)) {
						matches++;
						break;
					}
				for (vector<Mat>::iterator it = kernels.begin();
						it != kernels.end(); ++it)
					if (match(i, j, img2, *it)) {
						matches++;
						break;
					}

				if (matches == 2) {
					setPx(i, j, 0);
					changes++;
				}
			}
		}

		if (changes == 0) {
			printf("done entirely, no changes in last iteration.\n");
			break;
		} else if (s + 1 < times) {
			printf("\33[2K\r");
			img.copyTo(img2);
		} else {
			printf("done.\n");
		}

	}

	printf("Writing file... ");
	string suffix = "-graph";
	char* mapfilename_new = (char*) malloc(
			(strlen(mapfilename) + suffix.length()) * sizeof(char));
	if (mapfilename_new == NULL)
		return -1;
	char* tok = strtok((char*) mapfilename, ".");
	strcpy(mapfilename_new, tok);
	strcat(mapfilename_new, suffix.c_str());
	strcat(mapfilename_new, ".");
	tok = strtok(NULL, ".");
	strcat(mapfilename_new, tok);
	imwrite(mapfilename_new, img);
	free(mapfilename_new);
	printf("done.\n");

	return 0;
}

int create_kernel() {
for (int i = 0; i < 256; i++) {
	Mat img(3, 3, CV_8UC1, Scalar(255));
	unsigned char *input = (unsigned char *) (img.data);
	setPx(img, input, 1, 1, 0);

	if (i & 0b10000000)
		setPx(img, input, 0, 0, 0);
	if (i & 0b01000000)
		setPx(img, input, 0, 1, 0);
	if (i & 0b00100000)
		setPx(img, input, 0, 2, 0);
	if (i & 0b00010000)
		setPx(img, input, 1, 0, 0);
	if (i & 0b00001000)
		setPx(img, input, 1, 2, 0);
	if (i & 0b00000100)
		setPx(img, input, 2, 0, 0);
	if (i & 0b00000010)
		setPx(img, input, 2, 1, 0);
	if (i & 0b00000001)
		setPx(img, input, 2, 2, 0);

	printf("Writing file %d ... ", i);
	stringstream ss;
	int num_zeros = (i < 10 ? 2 : (i < 100 ? 1 : 0));
	ss << "kernel_";
	for (int j = 0; j < num_zeros; j++)
		ss << "0";
	ss << i << ".png";
	imwrite(ss.str().c_str(), img);
	printf(" done.\n");
}
return 0;
}

int main(int argc, const char **argv) {

if (argc < 2) {
	cout << "Usage:" << endl;
	cout << "\tCreate kernels:" << endl;
	cout << "\t\t" << argv[0] << " k" << endl;
	cout << "\tRun on map:" << endl;
	cout << "\t\t" << argv[0] << " m mapfilename" << endl;
	cout << "\t\t" << argv[0] << " m mapfilename iterations" << endl;
	return EXIT_SUCCESS;
}

if (strcmp(argv[1], "k") == 0) {
	return create_kernel();
}

if (strcmp(argv[1], "m") == 0 && argc >= 3) {
	if (argc >= 4)
		return run_algorithm(argv[2], atoi(argv[3]));
	return run_algorithm(argv[2], 1);
}

return EXIT_FAILURE;
}
