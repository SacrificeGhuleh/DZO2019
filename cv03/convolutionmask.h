#include <utility>

//
// Created by zvone on 26-Sep-19.
//

#ifndef DZO2019_CONVOLUTIONMASK_H
#define DZO2019_CONVOLUTIONMASK_H

template<unsigned int T_SIZE>
class ConvolutionMask {
public:
  inline explicit ConvolutionMask(const std::array<int, T_SIZE * T_SIZE> &kernel, const std::string &name = "") :
      size{T_SIZE},
      offset{T_SIZE / 2},
      kernel_{kernel},
      name{name} {
    scale = computeScale();
  }
  
  
  inline ConvolutionMask(const std::array<int, T_SIZE * T_SIZE> &kernel, float scale) :
      size{T_SIZE},
      offset{T_SIZE / 2},
      kernel_{kernel},
      scale{scale},
      name{""} {
  }
  
  inline ConvolutionMask(const std::array<int, T_SIZE * T_SIZE> &kernel, float scale, const std::string &name) :
      size{T_SIZE},
      offset{T_SIZE / 2},
      kernel_{kernel},
      scale{scale},
      name{name} {
  }
  
  constexpr const int &operator[](int idx) const noexcept {
    return kernel_[idx];
  }
  
  constexpr const int &at(int row, int col) const noexcept {
    return kernel_[T_SIZE * col + row];
  }
  
  constexpr const int &at(int idx) const noexcept {
    return kernel_[idx];
  }
  
  std::string name;
  float scale;
  const unsigned int size;
  const int offset;
  std::array<int, T_SIZE * T_SIZE> kernel_;
  
  inline void print() const {
    std::cout << name << ":" << std::endl;
    std::cout << "Mask size: " << size << " offset: " << offset << " scale: " << scale << std::endl;
    std::cout << "Kernel:" << std::endl;
    for (int maskCol = -offset; maskCol <= offset; maskCol++) {
      for (int maskRow = -offset; maskRow <= offset; maskRow++) {
        std::cout << at(offset + maskRow, offset + maskCol) << " ";
      }
      std::cout << std::endl;
    }
  }

protected:

private:
  inline float computeScale() {
    float sum = 0;
    for (int i : kernel_) {
      sum += abs(static_cast<float>(i));
    }
    return sum;
  }
};

class BoxBlur3x3ConvolutionMask : public ConvolutionMask<3> {
public:
  inline BoxBlur3x3ConvolutionMask() : ConvolutionMask({1, 1, 1, 1, 1, 1, 1, 1, 1}, "Box blur 3x3 mask") {
  }
};

class GausianBlur3x3ConvolutionMask : public ConvolutionMask<3> {
public:
  inline GausianBlur3x3ConvolutionMask() : ConvolutionMask({1, 2, 1, 2, 4, 2, 1, 2, 1}, "Gausian blur 3x3 mask") {
  }
};

class GausianBlur5x5ConvolutionMask : public ConvolutionMask<5> {
public:
  inline GausianBlur5x5ConvolutionMask() : ConvolutionMask(
      {1, 4, 6, 4, 1, 4, 16, 24, 16, 4, 6, 24, 36, 24, 6, 4, 16, 24, 16, 4, 1, 4, 6, 4, 1}, "Gausian blur 5x5 mask") {
  }
};

#endif //DZO2019_CONVOLUTIONMASK_H
