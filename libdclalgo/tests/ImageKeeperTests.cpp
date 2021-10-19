#include "ImageKeeper.h"

#include <gtest/gtest.h>

#include <algorithm>
#include <vector>

TEST(ImageKeeperTests, Creations) {
  libdclalgo::image::ImageKeeper<double> m1(1, 2);
  std::vector<double> m2_data({1, 2, 3, 4, 5, 6});
  libdclalgo::image::ImageKeeper<double> m2(2, 3, 1, m2_data.data());
  m2.at(2) = 10;
  for (int i = 0; i < 6; ++i) {
    EXPECT_EQ(m2_data[i], i + 1);
    if (i != 2) {
      EXPECT_EQ(m2.at(i), i + 1);
    } else {
      EXPECT_EQ(m2.at(i), 10);
    }
  }
}

TEST(ImageKeeperTests, MethodAt) {
  libdclalgo::image::ImageKeeper<double> m(3, 4);
  m.at(0, 0) = 5.;
  m.at(2, 1) = 10.;
  EXPECT_EQ(abs(m.at(0) - 5.) > std::numeric_limits<double>::epsilon(), 0);
  EXPECT_EQ(abs(m.at(9) - 10.) > std::numeric_limits<double>::epsilon(),
            0); // 9 = 1 + 2 * 4
}

TEST(ImageKeeperTests, Converting) {
  libdclalgo::image::ImageKeeper<double> m(5, 5);
  std::fill(const_cast<double *>(m.data()),
            const_cast<double *>(m.data() + m.size()), 3.);
  libdclalgo::image::ImageKeeper<int> mInt;
  m.convertTo(mInt);
  EXPECT_EQ(mInt.rows(), 5);
  EXPECT_EQ(mInt.cols(), 5);
  for (size_t i = 0; i < mInt.rows() * mInt.cols() * mInt.channels(); ++i) {
    EXPECT_EQ(mInt.at(i) != 3, 0);
  }
  libdclalgo::image::ImageKeeper<double> mDouble;
  mInt.convertTo(mDouble);
  EXPECT_EQ(mDouble.rows(), 5);
  EXPECT_EQ(mDouble.cols(), 5);
  for (size_t i = 0; i < mDouble.rows() * mDouble.cols() * mDouble.channels();
       ++i) {
    EXPECT_EQ(abs(mDouble.at(i) - 3.), 0);
  }
}
