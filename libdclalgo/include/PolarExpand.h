#ifndef POLAR_EXPAND_H
#define POLAR_EXPAND_H

#include <array>
#include <cstdint>
#include <vector>
#include <iostream>

#include <opencv2/core.hpp>

// Contains shifts of pixel coordinate (x or y) for 0, 45, 90 and 135 degries polarisation
template<typename T>
using Shifts = std::array<T, 4>;

// Contains mosaic to create set of polarized pixels for single pixel of image
template<typename T>
struct Mosaic {
  Shifts<T> xShifts;
  Shifts<T> yShifts;
};

// System of mosaics
template<typename T>
using Mosaics = std::array<Mosaic<T>, 4>;

template<class T>
class PolarExpand {
public:
  PolarExpand();

  //TODO: create fast versions
  void expand_fast(cv::Mat image, std::vector<cv::Mat> &polarSet) {
    expand(image, polarSet);
  };

  void expandEasy_fast(cv::Mat image, std::vector<cv::Mat> &polarSet) {
    expandEasy(image, polarSet);
  };

  void expandDebayer_fast(cv::Mat image, std::vector<cv::Mat> &polarSet);
  void expand(cv::Mat image, std::vector<cv::Mat> &polarSet);
  void expandEasy(cv::Mat image, std::vector<cv::Mat> &polarSet);
  void expandLite(cv::Mat image, std::vector<cv::Mat> &polarSet);
  void expandDebayer(cv::Mat image, std::vector<cv::Mat> &polarSet);
  void
  expandDebayerWithoutBoundaries(cv::Mat image, std::vector<cv::Mat> &polarSet);

private:
  int DATA_TYPE;
  int IMAGE_DATA_MASK;
  cv::Mat mImage;
  Mosaics<T> mMosaics;

  void
  biInterpolationWithOutBoundaries(cv::Mat &image, cv::Mat &polarImage, int x,
                                   int y);
  void biInterpolation(cv::Mat &image, cv::Mat &polarImage, int x, int y);
  void biInterpolation_fast(cv::Mat &image, cv::Mat &polarImage, int x, int y);
  int calculateA(cv::Mat &image, int x, int y);
  int calculateB(cv::Mat &image, int x, int y);
  int calculateC(cv::Mat &image, int x, int y);
  int calculateD(cv::Mat &image, int x, int y);
  void
  calculateABCD(cv::Mat &image, int x, int y, int &a, int &b, int &c, int &d);
  void
  calculateABCD1(cv::Mat &image, int x, int y, int &a, int &b, int &c, int &d);
  int selectCase(int x, int y, int width, int height);
  int calculateValue(int v1, int v2);
};

template<class T>
inline int PolarExpand<T>::calculateValue(int v1, int v2) {
  int v12 = (v1 + v2) / 2;
  int deltaV = v1 - v12;
  return (deltaV > 0) ? (((v1 + deltaV / 2) & IMAGE_DATA_MASK)) : ((
  (v1 - deltaV / 2) & IMAGE_DATA_MASK));
}

template<class T>
PolarExpand<T>::PolarExpand() {
  Shifts<T> eeX, eeY, eoX, eoY,
  oeX, oeY, ooX, ooY;

  /*
  We assume default counter clockwize mosaical matrix
  _____________________
  | 90 | 45 | 90 | 45 |
  _____________________
  |135 |  0 |135 |  0 |
  _____________________
  | 90 | 45 | 90 | 45 |
  _____________________
  |135 |  0 |135 |  0 |
  _____________________

  We have to creata pixel set for each pixel from this mosaic image

  Sets of four neighbor pixels in shuch mosaic may be classified as "even" and "odd" for lines and columns:
  even         odd
  ___________  ___________
  | 90 | 45 |  | 45 | 90 |
  even ___________  ___________
  |135 |  0 |  |  0 |135 |
  ________________________


  ___________  ___________
  |135 |  0 |  |  0 |135 |
  odd	 ___________  ___________
  | 90 | 45 |  | 45 | 90 |
  ___________  ___________

  This table allows us to determinate mosaic shifts for each pixel

  */

  eeX = {1, 1, 0, 0};
  eoX = {0, 0, 1, 1};
  eeY = {1, 0, 0, 1};
  eoY = {1, 0, 0, 1};

  oeX = {1, 1, 0, 0};
  ooX = {0, 0, 1, 1};
  oeY = {0, 1, 1, 0};
  ooY = {0, 1, 1, 0};

  mMosaics = {Mosaic<T>{eeX, eeY}, Mosaic<T>{eoX, eoY}, Mosaic<T>{oeX, oeY},
              Mosaic<T>{ooX, ooY}};
  int ss = sizeof(T);
  if (ss == 2) {
    DATA_TYPE = CV_16UC1;
    IMAGE_DATA_MASK = 0xffff;
  } else {
    DATA_TYPE = CV_8UC1;
    IMAGE_DATA_MASK = 0xff;
  }
}

template<class T>
void
PolarExpand<T>::calculateABCD1(cv::Mat &image, int x, int y, int &a, int &b,
                               int &c, int &d) {
  if ((x == -1) && (y >= 0)) {
    int a1 = image.at<T>(y, x + 1);
    int a2 = image.at<T>(y, x + 3);
    int a12 = (a1 + a2) / 2;
    int deltaA = a1 - a12;
    a = (deltaA > 0) ? (((a1 + deltaA / 2) & IMAGE_DATA_MASK)) : ((
    (a1 - deltaA / 2) & IMAGE_DATA_MASK));

    b = image.at<T>(y, x + 1);

    int c1 = image.at<T>(y + 2, x + 1);
    int c2 = image.at<T>(y + 2, x + 3);
    int c12 = (c1 + c2) / 2;
    int deltaC = c1 - c12;
    c = (deltaC > 0) ? (((c1 + deltaC / 2) & IMAGE_DATA_MASK)) : ((
    (c1 - deltaC / 2) & IMAGE_DATA_MASK));

    d = image.at<T>(y + 2, x + 1);
  } else if ((y == -1) && (x >= 0)) {
    int a1 = image.at<T>(y + 1, x);
    int a2 = image.at<T>(y + 3, x);
    int a12 = (a1 + a2) / 2;
    int deltaA = a1 - a12;
    a = (deltaA > 0) ? (((a1 + deltaA / 2) & IMAGE_DATA_MASK)) : ((
    (a1 - deltaA / 2) & IMAGE_DATA_MASK));

    int b1 = image.at<T>(y + 1, x + 2);
    int b2 = image.at<T>(y + 3, x + 2);
    int b12 = (b1 + b2) / 2;
    int deltaB = b1 - b12;
    b = (deltaB > 0) ? (((b1 + deltaB / 2) & IMAGE_DATA_MASK)) : ((
    (b1 - deltaB / 2) & IMAGE_DATA_MASK));

    c = image.at<T>(y + 1, x);

    d = image.at<T>(y + 1, x + 2);
  } else if ((y == -1) && (x == -1)) {
    int a1 = image.at<T>(y + 1, x + 1);
    int a2 = image.at<T>(y + 3, x + 3);
    int a12 = (a1 + a2) / 2;
    int deltaA = a1 - a12;
    a = (deltaA > 0) ? ((a1 + deltaA / 2) & IMAGE_DATA_MASK) : (
    (a1 - deltaA / 2) & IMAGE_DATA_MASK);

    int b1 = image.at<T>(y + 1, x + 1);
    int b2 = image.at<T>(y + 3, x + 1);
    int b12 = (b1 + b2) / 2;
    int deltaB = b1 - b12;
    b = (deltaB > 0) ? (((b1 + deltaB / 2) & IMAGE_DATA_MASK)) : ((
    (b1 - deltaB / 2) & IMAGE_DATA_MASK));

    int c1 = image.at<T>(y + 1, x + 1);
    int c2 = image.at<T>(y + 1, x + 3);
    int c12 = (c1 + c2) / 2;
    int deltaC = c1 - c12;
    c = (deltaC > 0) ? (((c1 + deltaC / 2) & IMAGE_DATA_MASK)) : ((
    (c1 - deltaC / 2) & IMAGE_DATA_MASK));

    d = image.at<T>(y + 1, x + 1);
  } else {
    a = image.at<T>(y, x);
    b = image.at<T>(y, x + 2);
    c = image.at<T>(y + 2, x);
    d = image.at<T>(y + 2, x + 2);
  }
}

template<class T>
int PolarExpand<T>::selectCase(int x, int y, int width, int height) {
  int result = 0;

  if (x == -1)
    result = 1;
  else if (x == (width - 2))
    result = 2;
  else if ((x >= 0) && (x < (width - 2)))
    result = 3;

  if (y == -1)
    result += 10;
  else if (y == (height - 2))
    result += 20;
  else if ((y >= 0) && (y < (height - 2)))
    result += 30;

  return result;
}

template<class T>
void PolarExpand<T>::calculateABCD(cv::Mat &image, int x, int y, int &a, int &b,
                                   int &c, int &d) {
  cv::Size size = image.size();
  int caseSelected = selectCase(x, y, size.width, size.height);

  switch (caseSelected) {
    case 11://x == -1 && y == -1
    {
      int y2x2 = image.at<T>(y + 2, x + 2);
      int y4x4 = image.at<T>(y + 4, x + 4);
      a = calculateValue(y2x2, y4x4);

      int y4x2 = image.at<T>(y + 4, x + 2);
      b = calculateValue(y2x2, y4x2);

      int y2x4 = image.at<T>(y + 2, x + 4);
      c = calculateValue(y2x2, y2x4);

      d = y2x2;
      break;
    }
    case 21:// x==-1 && y == (height - 2)
    {
      int y0x2 = image.at<T>(y, x + 2);
      int y0x4 = image.at<T>(y, x + 4);
      a = calculateValue(y0x2, y0x4);

      int y_2x4 = image.at<T>(y - 2, x + 4);
      b = calculateValue(y0x2, y_2x4);

      c = y0x2;

      int y_4x2 = image.at<T>(y - 4, x + 2);
      d = calculateValue(y0x2, y_4x2);

      break;
    }
    case 31: //x == -1 && (y >= 0) && (y < (height-2)) 
    {
      int y0x2 = image.at<T>(y, x + 2);
      int y0x4 = image.at<T>(y, x + 4);
      a = calculateValue(y0x2, y0x4);

      b = y0x2;

      int y2x2 = image.at<T>(y + 2, x + 2);
      int y2x4 = image.at<T>(y + 2, x + 4);
      c = calculateValue(y2x2, y2x4);

      d = y2x2;
      break;
    }
    case 12://x == (width - 2) && y == -1 
    {
      int y2x0 = image.at<T>(y + 2, x);
      int y4x0 = image.at<T>(y + 4, x);
      a = calculateValue(y2x0, y4x0);

      int y4x_2 = image.at<T>(y + 4, x - 2);
      b = calculateValue(y2x0, y4x_2);

      c = y2x0;

      int y4x_4 = image.at<T>(y + 2, x - 4);
      d = calculateValue(y2x0, y4x_4);
      break;
    }
    case 22://x == (width - 2) && y == (height - 2)
    {
      int y0x0 = image.at<T>(y, x);
      a = y0x0;

      int y0x_2 = image.at<T>(y, x - 2);
      b = calculateValue(y0x0, y0x_2);

      int y_2x0 = image.at<T>(y - 2, x);
      c = calculateValue(y0x0, y_2x0);

      int y_4x_4 = image.at<T>(y - 4, x - 4);
      d = calculateValue(y0x0, y_4x_4);

      break;
    }
    case 32://x == (width - 2)&& (y >= 0) && (y < (height-2))
    {
      int y0x0 = image.at<T>(y, x);
      a = y0x0;

      int y0x_2 = image.at<T>(y, x - 2);
      b = calculateValue(y0x0, y0x_2);

      c = y0x_2;

      int y0x_4 = image.at<T>(y, x - 4);
      d = calculateValue(y0x_2, y0x_4);

      break;
    }
    case 13:// (x >= 0) && (x < (width-2)) && (y == -1)
    {
      int y2x0 = image.at<T>(y + 2, x);
      int y4x0 = image.at<T>(y + 4, x);
      a = calculateValue(y2x0, y4x0);

      int y2x2 = image.at<T>(y + 2, x + 2);
      int y4x2 = image.at<T>(y + 4, x + 2);
      b = calculateValue(y2x2, y4x2);

      c = y2x0;

      d = y2x2;

      break;
    }
    case 23://(x >= 0) && (x < (width-2))&&(y == (height - 2)
    {
      int y0x0 = image.at<T>(y, x);
      a = y0x0;

      int y0x2 = image.at<T>(y, x + 2);
      b = y0x2;

      int y_2x0 = image.at<T>(y - 2, x);
      c = calculateValue(y0x0, y_2x0);

      int y_2x2 = image.at<T>(y - 2, x + 2);
      d = calculateValue(y0x2, y_2x2);

      break;
    }
    case 33://(x >= 0) && (x < (width-2)) && (y >= 0) && (y < (height-2))
    {
      a = image.at<T>(y, x);
      b = image.at<T>(y, x + 2);
      c = image.at<T>(y + 2, x);
      d = image.at<T>(y + 2, x + 2);
      break;
    }
    default:
      break;
  }
}

template<class T>
int PolarExpand<T>::calculateA(cv::Mat &image, int x, int y) {
  int a = 0;
  if ((x == -1) && (y >= 0)) {
    int a1 = image.at<T>(y, x + 1);
    int a2 = image.at<T>(y, x + 3);
    int a12 = (a1 + a2) / 2;
    int deltaA = a1 - a12;
    int a0 = (deltaA > 0) ? (((a1 + deltaA / 2) & IMAGE_DATA_MASK)) : ((
    (a1 - deltaA / 2) & IMAGE_DATA_MASK));
    a = a0;
  } else if ((y == -1) && (x >= 0)) {
    int a1 = image.at<T>(y + 1, x);
    int a2 = image.at<T>(y + 3, x);
    int a12 = (a1 + a2) / 2;
    int deltaA = a1 - a12;
    int a0 = (deltaA > 0) ? (((a1 + deltaA / 2) & IMAGE_DATA_MASK)) : ((
    (a1 - deltaA / 2) & IMAGE_DATA_MASK));
    a = a0;
  } else if ((y == -1) && (x == -1)) {
    int a1 = image.at<T>(y + 1, x + 1);
    int a2 = image.at<T>(y + 3, x + 3);
    int a12 = (a1 + a2) / 2;
    int deltaA = a1 - a12;
    int a0 = (deltaA > 0) ? ((a1 + deltaA / 2) & IMAGE_DATA_MASK) : (
    (a1 - deltaA / 2) & IMAGE_DATA_MASK);
    a = a0;
  } else
    a = image.at<T>(y, x);
  return a;
}

template<class T>
int PolarExpand<T>::calculateB(cv::Mat &image, int x, int y) {
  int b = 0;
  if ((x == -1) && (y >= 0)) {
    b = image.at<T>(y, x + 1);
  } else if ((y == -1) && (x >= 0)) {
    int b1 = image.at<T>(y + 1, x + 2);
    int b2 = image.at<T>(y + 3, x + 2);
    int b12 = (b1 + b2) / 2;
    int deltaB = b1 - b12;
    int b0 = (deltaB > 0) ? (((b1 + deltaB / 2) & IMAGE_DATA_MASK)) : ((
    (b1 - deltaB / 2) & IMAGE_DATA_MASK));
    b = b0;
  } else if ((y == -1) && (x == -1)) {
    int b1 = image.at<T>(y + 1, x + 1);
    int b2 = image.at<T>(y + 3, x + 1);
    int b12 = (b1 + b2) / 2;
    int deltaB = b1 - b12;
    int b0 = (deltaB > 0) ? (((b1 + deltaB / 2) & IMAGE_DATA_MASK)) : ((
    (b1 - deltaB / 2) & IMAGE_DATA_MASK));
    b = b0;
  } else
    b = image.at<T>(y, x + 2);
  return b;
}

template<class T>
int PolarExpand<T>::calculateC(cv::Mat &image, int x, int y) {
  int c = 0;
  if ((x == -1) && (y >= 0)) {
    int c1 = image.at<T>(y + 2, x + 1);
    int c2 = image.at<T>(y + 2, x + 3);
    int c12 = (c1 + c2) / 2;
    int deltaC = c1 - c12;
    int c0 = (deltaC > 0) ? (((c1 + deltaC / 2) & IMAGE_DATA_MASK)) : ((
    (c1 - deltaC / 2) & IMAGE_DATA_MASK));
    c = c0;
  } else if ((y == -1) && (x >= 0)) {
    c = image.at<T>(y + 1, x);
  } else if ((y == -1) && (x == -1)) {
    int c1 = image.at<T>(y + 1, x + 1);
    int c2 = image.at<T>(y + 1, x + 3);
    int c12 = (c1 + c2) / 2;
    int deltaC = c1 - c12;
    int c0 = (deltaC > 0) ? (((c1 + deltaC / 2) & IMAGE_DATA_MASK)) : ((
    (c1 - deltaC / 2) & IMAGE_DATA_MASK));
    c = c0;
  } else
    c = image.at<T>(y + 2, x);
  return c;
}

template<class T>
int PolarExpand<T>::calculateD(cv::Mat &image, int x, int y) {
  int d = 0;
  if ((x == -1) && (y >= 0)) {
    d = image.at<T>(y + 2, x + 1);
  } else if ((y == -1) && (x >= 0)) {
    d = image.at<T>(y + 1, x + 2);
  } else if ((y == -1) && (x == -1)) {
    d = image.at<T>(y + 1, x + 1);
  } else
    d = image.at<T>(y + 2, x + 2);
  return d;
}

template<class T>
void PolarExpand<T>::biInterpolation(cv::Mat &image, cv::Mat &polarImage, int x,
                                     int y) {
  //calculate a,b,c,d
  int a, b, c, d;
  calculateABCD(image, x, y, a, b, c, d);

  cv::Size size = image.size();

  //calculate values
  if ((x >= 0) && (y >= 0))
    polarImage.at<T>(y, x) = image.at<T>(y, x);
  if (y >= 0)
    polarImage.at<T>(y, x + 1) = (int) ((2 * (a + b) + (c + d)) / 3 / 2);
  if ((y >= 0) && (x < size.width - 2))
    polarImage.at<T>(y, x + 2) = image.at<T>(y, x + 2);

  if (x >= 0)
    polarImage.at<T>(y + 1, x) = (int) ((2 * (a + c) + (b + d)) / 3 / 2);
  polarImage.at<T>(y + 1, x + 1) = (int) (((a + b) + (c + d)) / 2 / 2);
  if (x < size.width - 2)
    polarImage.at<T>(y + 1, x + 2) = (int) (((a + c) + 2 * (b + d)) / 3 / 2);

  if ((x >= 0) && (y < size.height - 2))
    polarImage.at<T>(y + 2, x) = image.at<T>(y + 2, x);
  if (y < size.height - 2)
    polarImage.at<T>(y + 2, x + 1) = (int) (((a + b) + 2 * (c + d)) / 3 / 2);
  if ((x < size.width - 2) && (y < size.height - 2))
    polarImage.at<T>(y + 2, x + 2) = image.at<T>(y + 2, x + 2);

}

template<class T>
void
PolarExpand<T>::biInterpolation_fast(cv::Mat &image, cv::Mat &polarImage, int x,
                                     int y) {
  //calculate a,b,c,d
  int a, b, c, d;
  calculateABCD(image, x, y, a, b, c, d);

  cv::Size size = image.size();

  //calculate values
  if ((x >= 0) && (y >= 0))
    polarImage.at<T>(y, x) = image.at<T>(y, x);
  if (y >= 0)
    polarImage.at<T>(y, x + 1) = (int) ((2 * (a + b) + (c + d)) / 3 / 2);
  if ((y >= 0) && (x < size.width - 2))
    polarImage.at<T>(y, x + 2) = image.at<T>(y, x + 2);

  if (x >= 0)
    polarImage.at<T>(y + 1, x) = (int) ((2 * (a + c) + (b + d)) / 3 / 2);
  polarImage.at<T>(y + 1, x + 1) = (int) (((a + b) + (c + d)) / 2 / 2);
  if (x < size.width - 2)
    polarImage.at<T>(y + 1, x + 2) = (int) (((a + c) + 2 * (b + d)) / 3 / 2);

  if ((x >= 0) && (y < size.height - 2))
    polarImage.at<T>(y + 2, x) = image.at<T>(y + 2, x);
  if (y < size.height - 2)
    polarImage.at<T>(y + 2, x + 1) = (int) (((a + b) + 2 * (c + d)) / 3 / 2);
  if ((x < size.width - 2) && (y < size.height - 2))
    polarImage.at<T>(y + 2, x + 2) = image.at<T>(y + 2, x + 2);

}

template<class T>
void PolarExpand<T>::biInterpolationWithOutBoundaries(cv::Mat &image,
                                                      cv::Mat &polarImage,
                                                      int x, int y) {
  //calculate a,b,c,d
  T *ref = &image.at<T>(y, x);
  int a = (*ref);
  int b = (*(ref + 2));
  ref = &image.at<T>(y + 2, x);
  int c = (*ref);
  int d = (*(ref + 2));

  ref = &polarImage.at<T>(y, x);
  (*ref) = a;
  (*(ref + 1)) = (2 * (a + b) + (c + d)) / 6;
  (*(ref + 2)) = b;

  ref = &polarImage.at<T>(y + 1, x);
  (*ref) = (2 * (a + c) + (b + d)) / 6;
  (*(ref + 1)) = ((a + b) + (c + d)) / 4;
  (*(ref + 2)) = ((a + c) + 2 * (b + d)) / 6;

  ref = &polarImage.at<T>(y + 2, x);
  (*ref) = c;
  (*(ref + 1)) = ((a + b) + 2 * (c + d)) / 6;
  (*(ref + 2)) = d;
}

template<class T>
void PolarExpand<T>::expandDebayerWithoutBoundaries(cv::Mat image,
                                                    std::vector<cv::Mat> &polarSet) {
  cv::Size size = image.size();
  if (polarSet.size() != 4) {
    polarSet.resize(4);
  }
  for (auto &p : polarSet) {
    if (p.size() != size || p.type() != DATA_TYPE) {
      p = cv::Mat(size, DATA_TYPE, cv::Scalar(0));
    } else {
      p.setTo(0);
    }
  }
  for (int y = 0; y < size.height - 1 - 2; y += 2) {
    for (int x = 0; x < size.width - 1 - 2; x += 2) {
      biInterpolationWithOutBoundaries(image, polarSet[0], x + 1, y + 1);//0
      biInterpolationWithOutBoundaries(image, polarSet[1], x + 1, y);//45
      biInterpolationWithOutBoundaries(image, polarSet[2], x, y);//90
      biInterpolationWithOutBoundaries(image, polarSet[3], x, y + 1);//135
    }
  }

}

template<class T>
void PolarExpand<T>::expandEasy(cv::Mat image, std::vector<cv::Mat> &polarSet) {
  cv::Size size = image.size();
  size.height = size.height / 2;
  size.width = size.width / 2;

  if (polarSet.size() != 4)
    polarSet.resize(4);
  for (auto &p : polarSet) {
    if (p.size() != size || p.type() != DATA_TYPE) {
      p = cv::Mat(size, DATA_TYPE, cv::Scalar(0));
    } else {
      p.setTo(0);
    }
  }
  for (int y = 0, y0 = 1; y < size.height; y += 1, y0 += 2) {
    for (int x = 0, x0 = 1; x < size.width; x += 1, x0 += 2) {

      // std::cout << " image.at<T>(y0, x0); " << image.at<T>(y0, x0) << std::endl;
      polarSet[0].at<T>(y, x) = image.at<T>(y0, x0); //0
      // std::cout << " polarSet[0].at<T>(y, x)  " << polarSet[0].at<T>(y, x) << std::endl;
      polarSet[1].at<T>(y, x) = image.at<T>(y0, x0 - 1); // 45
      polarSet[2].at<T>(y, x) = image.at<T>(y0 - 1, x0 - 1); //90
      polarSet[3].at<T>(y, x) = image.at<T>(y0 - 1, x0); //135
    }
  }
}

template<class T>
T toUint(unsigned int a);

template<>
uint8_t toUint(unsigned int a) {
  // underflow, for 8 additions
  if (a > 8u * (unsigned int)UINT8_MAX) {
    return 0;
  }
  // overflow
  if (a > (unsigned int)UINT8_MAX) {
    return UINT8_MAX;
  }
  return a;
}

template<>
uint16_t toUint(unsigned int a) {
  // underflow, for 8 additions
  if (a > 8u * (unsigned int)UINT16_MAX) {
    return 0;
  }
  // overflow
  if (a > (unsigned int)UINT16_MAX) {
    return UINT16_MAX;
  }
  return a;
}

// Only for gray images, does nearly the same as expandEasy
template<class T>
void PolarExpand<T>::expandLite(cv::Mat image, std::vector<cv::Mat> &polarSet) {
  cv::Size size = image.size();
  // image can be too small to be processed correctly
  if (size.height < 6 || size.width < 6) {
    expandEasy(image, polarSet);
    return;
  }

  size.height = size.height / 2;
  size.width = size.width / 2;

  if (polarSet.size() != 4)
    polarSet.resize(4);
  for (auto &p : polarSet) {
    if (p.size() != size || p.type() != DATA_TYPE) {
      p = cv::Mat(size, DATA_TYPE, cv::Scalar(0));
    } else {
      p.setTo(0);
    }
  }
  for (int y = 0, y0 = 1; y < size.height - 1; y += 1, y0 += 2) {
    for (int x = 0, x0 = 1; x < size.width - 1; x += 1, x0 += 2) {
      // leave one as it is
      polarSet[0].at<T>(y, x) = image.at<T>(y0, x0); //0

      // use  linear (or bi-linear) to get the rest
      polarSet[1].at<T>(y, x) = toUint<T>((image.at<T>(y0, x0 + 1) +
                                 image.at<T>(y0, x0 - 1)) / 2u); //45
      polarSet[2].at<T>(y, x) = toUint<T>((image.at<T>(y0 - 1, x0 - 1) +
                                 image.at<T>(y0 + 1, x0 - 1) +
                                 image.at<T>(y0 - 1, x0 + 1) +
                                 image.at<T>(y0 + 1, x0 + 1)) / 4u); //90
      polarSet[3].at<T>(y, x) = toUint<T>((image.at<T>(y0 - 1, x0) +
                                 image.at<T>(y0 + 1, x0)) / 2u); //135
    }
  }

  // boundaries
  int y = size.height - 1;
  int y0 = size.height * 2 - 1;
  for (int x = 1, x0 = 3; x < size.width - 2; x += 1, x0 += 2) {
    polarSet[0].at<T>(y, x) = image.at<T>(y0, x0); //0
    polarSet[1].at<T>(y, x) = toUint<T>((image.at<T>(y0, x0 + 1) +
                               image.at<T>(y0, x0 - 1)) / 2u); //45
    polarSet[2].at<T>(y, x) = toUint<T>((3u * image.at<T>(y0 - 1, x0 + 1) -
                               image.at<T>(y0 - 3, x0 + 3) +
                               3u * image.at<T>(y0 - 1, x0 - 1) -
                               image.at<T>(y0 - 3, x0 - 3)) / 4u); //90
    polarSet[3].at<T>(y, x) = toUint<T>((3u * image.at<T>(y0 - 1, x0) -
                               image.at<T>(y0 - 3, x0)) / 2u); //135
  }
  int x = size.width - 1;
  int x0 = size.width * 2 - 1;
  for (y = 1, y0 = 3; y < size.height - 2; y += 1, y0 += 2) {
    polarSet[0].at<T>(y, x) = image.at<T>(y0, x0); //0
    polarSet[1].at<T>(y, x) = toUint<T>((3u * image.at<T>(y0, x0 - 1) -
                               image.at<T>(y0, x0 - 3)) / 2u); //45
    polarSet[2].at<T>(y, x) = toUint<T>((3u * image.at<T>(y0 - 1, x0 - 1) -
                               image.at<T>(y0 - 3, x0 - 3) +
                               3u * image.at<T>(y0 + 1, x0 - 1) -
                               image.at<T>(y0 + 3, x0 - 3)) / 4u); //90
    polarSet[3].at<T>(y, x) = toUint<T>((image.at<T>(y0 - 1, x0) +
                               image.at<T>(y0 + 1, x0)) / 2u); //135
  }
  // near corners
  y = size.height - 2;
  y0 = size.height * 2 - 3;
  x = size.width - 1;
  x0 = size.width * 2 - 1;
  polarSet[0].at<T>(y, x) = image.at<T>(y0, x0); //0
  polarSet[1].at<T>(y, x) = toUint<T>((3u * image.at<T>(y0, x0 - 1) -
                             image.at<T>(y0, x0 - 3)) / 2u); //45
  polarSet[2].at<T>(y, x) = toUint<T>((3u * image.at<T>(y0 - 1, x0 - 1) -
                             image.at<T>(y0 - 3, x0 - 3)) / 2u); //90
  polarSet[3].at<T>(y, x) = toUint<T>((3u * image.at<T>(y0 - 1, x0) -
                             image.at<T>(y0 - 3, x0)) / 2u); //135

  y = size.height - 1;
  y0 = size.height * 2 - 1;
  x = size.width - 2;
  x0 = size.width * 2 - 3;
  polarSet[0].at<T>(y, x) = image.at<T>(y0, x0); //0
  polarSet[1].at<T>(y, x) = toUint<T>((3u * image.at<T>(y0, x0 - 1) -
                             image.at<T>(y0, x0 - 3)) / 2u); //45
  polarSet[2].at<T>(y, x) = toUint<T>((3u * image.at<T>(y0 - 1, x0 - 1) -
                             image.at<T>(y0 - 3, x0 - 3)) / 2u); //90
  polarSet[3].at<T>(y, x) = toUint<T>((3u * image.at<T>(y0 - 1, x0) -
                             image.at<T>(y0 - 3, x0)) / 2u); //135
  // corners
  y = size.height - 1;
  y0 = size.height * 2 - 1;
  x = 0;
  x0 = 1;
  polarSet[0].at<T>(y, x) = image.at<T>(y0, x0); //0
  polarSet[1].at<T>(y, x) = toUint<T>((image.at<T>(y0, x0 + 1) +
                             image.at<T>(y0, x0 - 1)) / 2u); //45
  polarSet[2].at<T>(y, x) = toUint<T>((3u * image.at<T>(y0 - 1, x0 + 1) -
                             image.at<T>(y0 - 3, x0 + 3)) / 2u); //90
  polarSet[3].at<T>(y, x) = toUint<T>((3u * image.at<T>(y0 - 1, x0) -
                             image.at<T>(y0 - 3, x0)) / 2u); //135

  y = 0;
  y0 = 1;
  x = size.width - 1;
  x0 = size.width * 2 - 1;
  polarSet[0].at<T>(y, x) = image.at<T>(y0, x0); //0
  polarSet[1].at<T>(y, x) = toUint<T>((3u * image.at<T>(y0, x0 - 1) -
                             image.at<T>(y0, x0 - 3)) / 2u); //45
  polarSet[2].at<T>(y, x) = toUint<T>((3u * image.at<T>(y0 + 1, x0 - 1) -
                             image.at<T>(y0 + 3, x0 - 3)) / 2u); //90
  polarSet[3].at<T>(y, x) = toUint<T>((image.at<T>(y0 - 1, x0) +
                             image.at<T>(y0 + 1, x0)) / 2u); //135

  y = size.height - 1;
  y0 = size.height * 2 - 1;
  x = size.width - 1;
  x0 = size.width * 2 - 1;
  polarSet[0].at<T>(y, x) = image.at<T>(y0, x0); //0
  polarSet[1].at<T>(y, x) = toUint<T>((3u * image.at<T>(y0, x0 - 1) -
                             image.at<T>(y0, x0 - 3)) / 2u); //45
  polarSet[2].at<T>(y, x) = toUint<T>((3u * image.at<T>(y0 - 1, x0 - 1) -
                             image.at<T>(y0 - 3, x0 - 3)) / 2u); //90
  polarSet[3].at<T>(y, x) = toUint<T>((3u * image.at<T>(y0 - 1, x0) -
                             image.at<T>(y0 - 3, x0)) / 2u); //135
}

template<class T>
void PolarExpand<T>::expandDebayer_fast(cv::Mat image,
                                        std::vector<cv::Mat> &polarSet) {
  cv::Size size = image.size();
  if (polarSet.size() != 4) {
    polarSet.resize(4);
  }
  for (auto &p : polarSet) {
    if (p.size() != size || p.type() != DATA_TYPE) {
      p = cv::Mat(size, DATA_TYPE, cv::Scalar(0));
    } else {
      p.setTo(0);
    }
  }
  for (int y = 0; y < size.height - 1 - 2; y += 2) {
    for (int x = 0; x < size.width - 1 - 2; x += 2) {
      biInterpolationWithOutBoundaries(image, polarSet[0], x + 1, y + 1);//0
      biInterpolationWithOutBoundaries(image, polarSet[1], x + 1, y);//45
      biInterpolationWithOutBoundaries(image, polarSet[2], x, y);//90
      biInterpolationWithOutBoundaries(image, polarSet[3], x, y + 1);//135
    }
  }

  {
    int y = 0;
    for (int x = 0; x < size.width - 1; x += 2) {
      biInterpolation_fast(image, polarSet[0], x - 1, y - 1);//0
      biInterpolation_fast(image, polarSet[1], x - 1, y);//45
      biInterpolation_fast(image, polarSet[2], x, y);//90
      biInterpolation_fast(image, polarSet[3], x, y - 1);//135
    }
  }
  {
    int y = size.height - 2;
    for (int x = 0; x < size.width - 1; x += 2) {
      biInterpolation_fast(image, polarSet[0], x - 1, y - 1);//0
      biInterpolation_fast(image, polarSet[1], x - 1, y);//45
      biInterpolation_fast(image, polarSet[2], x, y);//90
      biInterpolation_fast(image, polarSet[3], x, y - 1);//135
    }
  }

  for (int y = 0; y < size.height - 1; y += 2) {
    {
      int x = size.width - 2;
      biInterpolation_fast(image, polarSet[0], x - 1, y - 1);//0
      biInterpolation_fast(image, polarSet[1], x - 1, y);//45
      biInterpolation_fast(image, polarSet[2], x, y);//90
      biInterpolation_fast(image, polarSet[3], x, y - 1);//135
    }
  }

  for (int y = 0; y < size.height - 1; y += 2) {
    {
      int x = 0;
      biInterpolation_fast(image, polarSet[0], x - 1, y - 1);//0
      biInterpolation_fast(image, polarSet[1], x - 1, y);//45
      biInterpolation_fast(image, polarSet[2], x, y);//90
      biInterpolation_fast(image, polarSet[3], x, y - 1);//135
    }
  }/**/
}

template<class T>
void
PolarExpand<T>::expandDebayer(cv::Mat image, std::vector<cv::Mat> &polarSet) {
  cv::Size size = image.size();
  if (polarSet.size() != 4) {
    polarSet.resize(4);
  }
  for (auto &p : polarSet) {
    if (p.size() != size || p.type() != DATA_TYPE) {
      p = cv::Mat(size, DATA_TYPE, cv::Scalar(0));
    } else {
      p.setTo(0);
    }
  }
  for (int y = 0; y < size.height - 1; y += 2) {
    for (int x = 0; x < size.width - 1; x += 2) {
      biInterpolation(image, polarSet[0], x - 1, y - 1);//0
      biInterpolation(image, polarSet[1], x - 1, y);//45
      biInterpolation(image, polarSet[2], x, y);//90
      biInterpolation(image, polarSet[3], x, y - 1);//135
    }
  }
}

template<class T>
void PolarExpand<T>::expand(cv::Mat image, std::vector<cv::Mat> &polarSet) {
  cv::Size size = image.size();
  if (polarSet.size() != 4) {
    polarSet.resize(4);
  }
  for (auto &p : polarSet) {
    if (p.size() != size || p.type() != DATA_TYPE) {
      p = cv::Mat(size, DATA_TYPE, cv::Scalar(0));
    } else {
      p.setTo(0);
    }
  }
  for (int y = 0; y < size.height - 1; ++y) {
    for (int x = 0; x < size.width - 1; ++x) {
      int sX = x % 2;
      int sY = y % 2;
      Mosaic<T> m = mMosaics[sY * 2 + sX];
      for (int i = 0; i < 4; ++i) {
        polarSet[i].at<T>(y, x) = image.at<T>(y + m.yShifts[i],
                                              x + m.xShifts[i]);
      }
    }
  }
}

#endif //POLAR_EXPAND_H
