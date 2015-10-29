#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <iostream>

using namespace cv;
using namespace std;

void setPx(Mat &img, unsigned char *input, int i, int j, int color) {
  if (j >= 0 && j < img.rows && i >= 0 && i < img.cols)
    input[img.step[0] * j + img.step[1] * i] = color;
}

int main(int argc, const char **argv) {

  for (int i = 0; i < 256; i++) {
    Mat img(3, 3, CV_8UC1, Scalar(255));
    unsigned char *input = (unsigned char *)(img.data);
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

    cout << "Writing file " << i << " ... ";
    stringstream ss;
    ss << "kernel_" << i << ".png";
    imwrite(ss.str().c_str(), img);
    cout << " done.\n";
  }

  return 0;
}
