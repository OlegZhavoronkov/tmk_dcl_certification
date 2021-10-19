//------------------------------------------------------------------------------
//  Created     : 12.08.21
//  Author      : Alin42
//  Description : Class ImageKeeper is aimed to store flatten images.
//------------------------------------------------------------------------------
#ifndef TMK_IMAGE_HPP
#define TMK_IMAGE_HPP

#include <algorithm>
#include <cassert>
#include <cstring>
#include <stdexcept>
#include <type_traits>

namespace libdclalgo {
namespace image {
template <typename T> class ImageKeeper {
public:
  ImageKeeper() = default;
  ImageKeeper(const size_t rows, const size_t cols, const size_t channels = 1,
              const T *dataptr = nullptr) {
    recreate(rows, cols, channels, dataptr);
  }

  ~ImageKeeper() { delete[] _data; }

  ImageKeeper &operator=(const ImageKeeper &other) {
    if (this != &other) {
      recreate(other.rows(), other.cols(), other.channels(), other.data());
    }

    return *this;
  }

  const T &at(const size_t i) const {
    if (i >= _size) {
      throw std::out_of_range("ImageKeeper: out of bounds");
    }
    return _data[i];
  }

  T &at(const size_t i) {
    return const_cast<T &>(static_cast<const ImageKeeper &>(*this).at(i));
  }

  const T &at(const size_t row, const size_t col,
              const size_t channel = 0) const {
    // Сheck only during debug
    assert(row < _rows && "ImageKeeper: row is out of bounds");
    assert(col < _cols && "ImageKeeper: col is out of bounds");
    assert(channel < _channels && "ImageKeeper: channel is out of bounds");
    return at(col + row * _cols + channel * _cols * _rows);
  }

  T &at(const size_t row, const size_t col, const size_t channel = 0) {
    return const_cast<T &>(
        static_cast<const ImageKeeper &>(*this).at(row, col, channel));
  }

  [[nodiscard]] size_t size() const { return _size; }

  [[nodiscard]] size_t rows() const { return _rows; }

  [[nodiscard]] size_t cols() const { return _cols; }

  [[nodiscard]] size_t channels() const { return _channels; }

  const T *data() const { return _data; }

  template <typename T2,
            typename = std::enable_if_t<!std::is_same<T, T2>::value>>
  void convertTo(ImageKeeper<T2> &other) const {
    other.recreate(_rows, _cols, _channels);

    for (size_t i = 0; i < _size; ++i) {
      other.at(i) = static_cast<T2>(at(i));
    }
  }

  void convertTo(ImageKeeper<T> &other) const { other = *this; }

  void swap(ImageKeeper<T> &other) {
    std::swap(_size, other._size);
    std::swap(_rows, other._rows);
    std::swap(_cols, other._cols);
    std::swap(_channels, other._channels);
    std::swap(_data, other._data);
  }

  void recreate(const size_t rows, const size_t cols, const size_t channels = 1,
                const T *dataptr = nullptr) {
    if (_size != 0 || _data != nullptr) {
      clear();
    }

    _rows = rows;
    _cols = cols;
    _channels = channels;
    _size = _rows * _cols * _channels;
    _data = new T[_size];

    if (dataptr == nullptr) {
      std::fill(_data, _data + _size, T());
    } else {
      std::memcpy(_data, dataptr, _size * sizeof(T));
    }
  }

private:
  void clear() {
    _size = 0;
    _rows = 0;
    _cols = 0;
    _channels = 0;

    delete[] _data;
    _data = nullptr;
  }

private:
  size_t _size = 0;
  size_t _rows = 0;
  size_t _cols = 0;
  size_t _channels = 0;
  T *_data = nullptr;
};
} // namespace image
} // namespace libdclalgo

#endif // TMK_IMAGE_HPP
