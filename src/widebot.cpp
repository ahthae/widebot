#include "sleepy_discord/common_return_types.h"
#include <curl/curl.h>
#include <widebot.hpp>

#include <curlpp/Options.hpp>
#include <curlpp/Exception.hpp>

#include <filesystem>
#include <fstream>
#include <iostream>
#include <list>
#include <sstream>
#include <vector>

using namespace SleepyDiscord;
using namespace cURLpp;
using namespace widepeepolib;

namespace widebot {

  void WideBot::init() { 
    std::filesystem::path cache = std::filesystem::current_path() / cache_dir_;
    std::filesystem::create_directory(cache);
    std::cout << "Created cache directory: " << cache << std::endl;
  }

  void WideBot::onMessage(Message message) {
    if (message.startsWith(prefix_)) { 
      std::cout << "Recieved command: " << message.content << std::endl;
      std::string cmd;
      std::vector<std::string> args;
      parseCommand(message.content, &cmd, &args);
      std::cout << "Command: " << cmd << std::endl;
      std::cout << "Args: ";
      for (const auto& arg : args) {
        std::cout << arg << ' ';
      }
      std::cout << std::endl;

      if(cmd == commands.HELP) {
        sendMessage(message.channelID, "Caption and image attachment with `" + prefix_ + "wide [num_splits] {w}` to split and widen and image for emojis!");
      } else if (cmd == commands.PREFIX) {
        if (args.empty()) {
          prefix_ = "!";
          sendMessage(message.channelID, "Resetting prefix to `" + prefix_ + "`.");
        } else {
          prefix_ = args[0];
          sendMessage(message.channelID, "Setting the command prefix to `" + prefix_ + "`.");
        }
      } else if (cmd == commands.WIDE) {
        if (message.attachments.empty()) {
          sendMessage(message.channelID, "Error: no attachment found");
          return;
        }
        if (args.size() < 1) {
          sendMessage(message.channelID, "Error: missing arguments");
          return;
        }

        bool create_emojis = true; //TODO
        bool no_split = *--args.end() == "w" ? true : false;

        int num_splits;
        try {
          num_splits = std::stoi(args[0].c_str());
        }
        catch (std::invalid_argument &e) {
          sendMessage(message.channelID, "Error: invalid argument");
          std::cerr << "Invalid argument: \n" << e.what();
          return;
        }
        std::cout << "Using " << num_splits << " splits.\n";

        std::cout << "Recieved attachment: " << std::endl;
        for (const auto& a : message.attachments) {
          std::cout << '\t' << a.filename << " at " << a.url << std::endl;

          std::stringstream out;
          try { 

            curl_->setOpt(Options::Url(a.url));
            curl_->setOpt(Options::WriteStream(&out));
            curl_->perform();
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

          Image image(blob);

          std::vector<Image> splits; 
          // TODO: Make this an argument option; actually implement optionally splitting
          if (no_split) { // TESTING
            splits.emplace_back(stretchImage(image, num_splits));
          } else { // END TESTING
            splits = splitImage(stretchImage(image, num_splits), num_splits);
          }

          std::cout << "Writing and uploading... \n";
          for (std::size_t i = 0; i < splits.size(); i++) {
            // Construct the filename
            std::string name, format;
            for (size_t i = a.filename.length(); i > 0; i--) {
              if (a.filename[i] == '.') {
                name = a.filename.substr(0, i);
                format = a.filename.substr(i, std::string::npos);
                break;
              }
            }
            std::string out_name = "wide" + name + std::to_string(i+1);
            std::string out_filename = out_name + format;

            if (create_emojis) {
              std::vector<Snowflake<Role>> roles; // role have yet to be implemented in sleepy_discorda
              try {
                createEmoji(message.serverID, out_name, a.content_type, splits[i], roles, RequestMode(~ThrowError));
              }
              catch (Magick::ErrorCoder &e) {
                std::cout << "ImageMagick error: " << e.what();
                create_emojis = false;
              }
              catch (std::exception &e) {
                std::cout << "Error uploading emoji: " << e.what();
                create_emojis = false;
              }
            }

            std::filesystem::path p = std::filesystem::current_path() / cache_dir_ / out_filename;
            std::cout << "Writing " << p << std::endl;
            splits[i].write(p.c_str());
            uploadFile(message.channelID, p, "");
          }
        }
      }
    }
  }

  int WideBot::parseCommand(const std::string& msg, std::string* cmd, std::vector<std::string>* args) {
    std::istringstream ss(msg.substr(prefix_.length()));
    std::string word;
    std::getline(ss, *cmd, ' ');
    while (std::getline(ss, word, ' ')) {
      args->emplace_back(word);
    }
    return 0;
  }

  ObjectResponse<Emoji> WideBot::createEmoji(SleepyDiscord::Snowflake<SleepyDiscord::Server> serverID, const std::string &name, const std::string &mime_type, Image &image, std::vector<SleepyDiscord::Snowflake<SleepyDiscord::Role>> roles, RequestSettings<ObjectResponse<Emoji>> settings) {
    std::string image_data_uri = "data:" + mime_type + ";base64,";

    Magick::Blob blob;
    image.write(blob);
    image_data_uri += blob.base64();

    return createServerEmoji(serverID, name, image_data_uri, roles, settings);
  }

} //namespace widebot
