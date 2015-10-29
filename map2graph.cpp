#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>

using namespace cv;
using namespace std;

Mat img;
unsigned char *input;
Mat img2;
unsigned char *input2;

vector<Mat> kernels;

int getPx(int i, int j, Mat &m) {
  if (j >= 0 && j < m.rows && i >= 0 && i < m.cols) {
    return ((unsigned char *)(m.data))[m.step[0] * j + m.step[1] * i];
  }

  return 0; // black
}

void setPx(int i, int j, int color) {
  if (j >= 0 && j < img.rows && i >= 0 && i < img.cols)
    input[img.step[0] * j + img.step[1] * i] = color;
}

int match(int i, int j, Mat &img, Mat &k) {
  return (getPx(i - 1, j - 1, img) == getPx(0, 0, k) &&
          getPx(i + 0, j - 1, img) == getPx(1, 0, k) &&
          getPx(i + 1, j - 1, img) == getPx(2, 0, k) &&
          getPx(i - 1, j + 0, img) == getPx(0, 1, k) &&
          getPx(i + 1, j + 0, img) == getPx(2, 1, k) &&
          getPx(i - 1, j + 1, img) == getPx(0, 2, k) &&
          getPx(i + 0, j + 1, img) == getPx(1, 2, k) &&
          getPx(i + 1, j + 1, img) == getPx(2, 2, k));
}

int main(int argc, const char **argv) {

  cout << "Loading kernels...";
  for (int i = 0; i < 256; i++) {
    stringstream ss;
    ss << "kernel_" << i << ".png";
    Mat k = imread(ss.str().c_str(), CV_LOAD_IMAGE_UNCHANGED);
    if (!k.empty()) {
      kernels.push_back(k);
    }
  }
  cout << "done.\n";

  cout << "Getting parameters...";
  int times = 1;
  if (argc > 1)
    times = atoi(argv[1]);
  cout << "done. Running erosion " << times << " times.\n";

  cout << "Loading file... ";
  img = imread("map.png", CV_LOAD_IMAGE_UNCHANGED);
  img2 = imread("map.png", CV_LOAD_IMAGE_UNCHANGED);
  if (img.empty()) // check whether the image is loaded or not
  {
    cout << "Error : Image cannot be loaded!" << endl;
    return -1;
  }
  cout << "done.\n";

  input = (unsigned char *)(img.data);
  input2 = (unsigned char *)(img2.data);

  // white & black
  cout << "Making it white & black... ";
  for (int j = 0; j < img.rows; j++) {
    for (int i = 0; i < img.cols; i++) {
      if (getPx(i, j, img2) < 255) {
        // make it black
        setPx(i, j, 0);
      } else {
        // make it white
        setPx(i, j, 255);
      }
    }
  }
  img.copyTo(img2);
  cout << "done.\n";

  cout << "Eroding...\n";
  for (int s = 0; s < times; s++) {

    cout << "\tIteration [" << (s + 1) << "/" << times << "]... ";
    int changes = 0;

    for (int j = 1; j < img.rows - 1; j++) {
      for (int i = 1; i < img.cols - 1; i++) {

        // only work on white pixels
        if (getPx(i, j, img) == 0)
          continue;

        // match all white and all black and continue if this happens without
        // kernel check
        if ((getPx(i - 1, j - 1, img) > 0 && getPx(i + 0, j - 1, img) > 0 &&
             getPx(i + 1, j - 1, img) > 0 && getPx(i - 1, j + 0, img) > 0 &&
             getPx(i + 1, j + 0, img) > 0 && getPx(i - 1, j + 1, img) > 0 &&
             getPx(i + 0, j + 1, img) > 0 && getPx(i + 1, j + 1, img) > 0) ||
            (getPx(i - 1, j - 1, img) == 0 && getPx(i + 0, j - 1, img) == 0 &&
             getPx(i + 1, j - 1, img) == 0 && getPx(i - 1, j + 0, img) == 0 &&
             getPx(i + 1, j + 0, img) == 0 && getPx(i - 1, j + 1, img) == 0 &&
             getPx(i + 0, j + 1, img) == 0 && getPx(i + 1, j + 1, img) == 0))
          continue;

        int matches = 0;
        for (vector<Mat>::iterator it = kernels.begin(); it != kernels.end();
             ++it)
          if (match(i, j, img, *it)) {
            matches++;
            break;
          }
        for (vector<Mat>::iterator it = kernels.begin(); it != kernels.end();
             ++it)
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
      cout << "done entirely, no changes in last iteration.\n";
      break;
    }

    img.copyTo(img2);
    cout << "done.\n";
  }

  cout << "done.\n";

  cout << "Writing file... ";
  imwrite("map_graph.png", img);
  cout << "done.\n";

  return 0;
}
