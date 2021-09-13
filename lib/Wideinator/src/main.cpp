#include "wideinator.hpp"
#include "image.hpp"
#include "cmdline.h"

#include <Magick++.h>
#include <iostream>
#include <iterator>
#include <fstream>
#include <vector>

using namespace wideinator;

int main(int argc, char** argv) {

  gengetopt_args_info args_info;
  
  if (cmdline_parser(argc, argv, &args_info)) {
    std::cerr << "Gengetopt error parsing arguments\n";
    exit(1);
  } 
 
  if (!args_info.widen_given && !args_info.split_given)  {
    std::cout << "Nothing to do. Pass either -w or -s or both\n";
    std::cout << gengetopt_args_info_help;
  }
  if (args_info.inputs_num == 0) {
    std::cout << "No file given.\n";
    std::cout << gengetopt_args_info_help;
  }

  Magick::InitializeMagick(nullptr);

  for (size_t i = 0; i < args_info.inputs_num; i++) {
    std::string filename = args_info.inputs[i];
    Image image(filename.c_str());
    std::vector<Image> out_images;
    
    if (args_info.widen_given) {
      out_images.emplace_back(stretchImage(image, args_info.widen_arg));
    } else { 
      out_images.emplace_back(image);
    }

    if (args_info.split_given) {
      std::vector<Image> split = splitImage(out_images[0], args_info.split_arg);
      out_images.insert(out_images.begin(),
          std::make_move_iterator(split.begin()),
          std::make_move_iterator(split.end()));
    }
    out_images.pop_back();
  
    for (size_t i = 0; i < out_images.size(); i++) {
      std::string name, format, num_str;
      for (size_t j = filename.size(); j > 0; j--) {
        if (filename[j] == '.') {
          name = filename.substr(0, j);
          format = filename.substr(j);
        }
      }
      if (args_info.widen_given) {
        name = "wide" + name;
      }
      if (args_info.split_given) { 
        num_str = std::to_string(i+1);
      } else {
        num_str = "";
      }

      out_images[i].write((name + num_str + format).c_str()); }  
  }

  return 0;
}
