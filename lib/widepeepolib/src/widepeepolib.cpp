#include "widepeepolib.hpp"

#include <Magick++.h>
#include <vector>

namespace widepeepolib{

  Image stretchImage(const Image &image, const int &multiplier) {
    Image stretched_image = image;

    Magick::Geometry stretched_size = image.size();
    stretched_size.aspect(true);
    stretched_size.width(stretched_size.width() * multiplier);
    stretched_image.resize(stretched_size);

    return stretched_image;
  }

  std::vector<Image> splitImage(const Image &image, const int& num_splits) {
    std::vector<Image> splits;
  
    Magick::Geometry split_size = image.size();
    split_size.width(split_size.width() / num_splits);
    size_t x_offset = 0;
    for (int i = 0; i < num_splits; i++) {
       split_size.xOff(x_offset);
       Image split_image = image;
       split_image.crop(split_size);
       split_image.repage();
       splits.emplace_back(split_image);
       x_offset += split_size.width();
    }    

    return splits;
  }
} //namespace wwidepeepolib 
