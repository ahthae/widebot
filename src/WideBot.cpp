#include <WideBot.hpp>

#include <curlpp/Options.hpp>
#include <curlpp/Exception.hpp>
#include <Magick++.h>

#include <fstream>
#include <iostream>
#include <sstream>
#include <list>
#include <vector>

using namespace SleepyDiscord;
using namespace cURLpp;

void WideBot::onMessage(Message message) {
  if (message.startsWith("!hello")) {
    std::string response = "Hello, " + message.author.username + "!";
    sendMessage(message.channelID, response);
  }

  if (!message.attachments.empty()) {
    std::cout << "Recieved attachment: " << std::endl;
    for (const auto& f : message.attachments) {
      std::cout << '\t' << f.filename << " at " << f.url << std::endl;
   
      std::stringstream out;
      try { 

        curl->setOpt(Options::Url(f.url));
        curl->setOpt(Options::WriteStream(&out));
        curl->perform();
      }
      catch (cURLpp::RuntimeError& e) {
        std::cerr << e.what() << std::endl;
        sendMessage(message.channelID, e.what()); // TODO make this friendly
      }
      catch (cURLpp::LogicError& e) {
        std::cerr << e.what() << std::endl;
        sendMessage(message.channelID, e.what()); // TODO make this friendly
      }

      out.seekg(0, std::ios::end);
      long size = out.tellg();
      out.seekg(0, std::ios::beg);
      char data[size];
      out.read(data, size);

      Magick::Blob blob(data, size);

      std::list<Magick::Image> images;
      try {
          Magick::readImages(&images, blob);
      }
      catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
      }
      Magick::Geometry geoOrig(f.width, f.height);
      Magick::Geometry geoWide1(f.width / 2, f.height);
      Magick::Geometry geoWide2(f.width, f.height, f.width / 2, 0);
      for (auto& image : images) {
//        image.crop(geoWide1);
//        image.resize(geoOrig); 
      }
        Magick::writeImages(images.begin(), images.end(), "test_"+f.filename);
    }
  }
}