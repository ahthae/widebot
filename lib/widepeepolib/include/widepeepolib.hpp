#pragma once

#include <image.hpp>

#include <vector>

namespace widepeepolib{
  
  extern Image stretchImage(const Image &image, const int &multiplier);
  extern std::vector<Image> splitImage(const Image &image, const int &num_splits);

} //namespace widepeepolib
