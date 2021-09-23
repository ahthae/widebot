#pragma once

#include <Magick++.h>
#include <list>
#include <iostream>

namespace widepeepolib {

  // Mostly a wrapper to simplify the representation of gifs as a single object
  class Image {
  public:
    Image(const char* filename) {
      Magick::readImages(&frames, filename);
      Magick::coalesceImages(&frames, frames.begin(), frames.end());

      printf("Frames: %lu\n", frames.size());
    }
    Image(Magick::Blob &blob) {
      Magick::readImages(&frames, blob);
      Magick::coalesceImages(&frames, frames.begin(), frames.end());
    }

    void addFrame(const Magick::Image &image) { frames.emplace_back(image); }
    std::list<Magick::Image> getFrames() const { return frames; }

    // Magick::Image functions wrappers
    Magick::Geometry size() const {
        if (!frames.empty()) return frames.front().size();
        else return Magick::Geometry();
    }
    std::string magick() const { 
        if (!frames.empty()) return frames.front().magick();
        else return "";
    }
    void resize(Magick::Geometry geometry) { for(auto& frame : frames) frame.resize(geometry); }
    void crop(Magick::Geometry geometry) { for(auto& frame : frames) frame.crop(geometry); }
    void repage() { for (auto& frame : frames) frame.repage(); }
    void write(const char* filename, bool adjoin = true) {
      try {
        Magick::writeImages(frames.begin(), frames.end(), filename, adjoin);
      }
      catch(Magick::WarningCoder &w) {
        std::cerr << "Magick codec warning: " << w.what() << std::endl;
      }
    }
    void write(Magick::Blob &blob, bool adjoin = true) {
      try {
        Magick::writeImages(frames.begin(), frames.end(), &blob, adjoin);
      }
      catch(Magick::WarningCoder &w) {
        std::cerr << "Magick codec warning: " << w.what() << std::endl;
      }
    }

  private:
    std::list<Magick::Image> frames;
  };

} //namespace widepeepolib 
