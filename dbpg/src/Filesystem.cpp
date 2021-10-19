#include <dbpg/Filesystem.h>

#include <opencv2/imgcodecs.hpp>

#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/iostreams/filter/zlib.hpp>
#include <boost/iostreams/filtering_streambuf.hpp>

#include <fstream>

namespace tmk::dbpg {

void saveImage(const cv::Mat &image, const fs::path &path) {
  int channels = image.channels();
  int depth = image.depth();
  if ((depth == CV_8U || depth == CV_16U) &&
      (channels == 1 || channels == 3 || channels == 4)) {
    cv::imwrite(path.string() + ".png", image);
  } else {
    std::ofstream ofs(path.string() + ".dat", std::ios::binary);
    {
      boost::iostreams::filtering_ostreambuf fos;

      // push the ofstream and the compressor
      fos.push(boost::iostreams::zlib_compressor(
          boost::iostreams::zlib::best_compression));
      fos.push(ofs);

      // start the archive on the filtering buffer:
      boost::archive::binary_oarchive bo(fos);

      size_t dataSize = image.step[0] * image.rows;
      bo << image.cols << image.rows << image.type();
      for (size_t i = 0; i < dataSize; i++) {
        bo << image.data[i];
      }
    }
  }
}

cv::Mat readImage(const fs::path &path) {
  cv::Mat image;
  if (fs::exists(path.string() + ".png")) {
    image = cv::imread(path.string() + ".png");
  } else if (fs::exists(path.string() + ".dat")) {
    std::ifstream ifs(path.string() + ".dat", std::ios::binary);

    boost::iostreams::filtering_istreambuf fis;

    // push the ifstream and the decompressor
    fis.push(boost::iostreams::zlib_decompressor());
    fis.push(ifs);

    // start the archive on the filtering buffer:
    boost::archive::binary_iarchive bi(fis);
    int cols, rows, type;
    bi >> cols >> rows >> type;
    image.create(cv::Size2i{cols, rows}, type);
    size_t dataSize = image.step[0] * image.rows;
    for (size_t i = 0; i < dataSize; i++) {
      bi >> image.data[i];
    }
  }
  return image;
}

} // namespace tmk::dbpg
