#ifndef DBPG_FILESYSTEM_H
#define DBPG_FILESYSTEM_H

#include <opencv2/core/mat.hpp>

#include <filesystem>

namespace tmk {
namespace fs = std::filesystem;
} // namespace tmk

namespace tmk::dbpg {

fs::path fsDatabaseRoot(); // TODO:
cv::Mat readImage(const fs::path &);
void saveImage(const cv::Mat &image, const fs::path &);

} // namespace tmk::dbpg

#endif // DBPG_FILESYSTEM_H
