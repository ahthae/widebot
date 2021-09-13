#include <widebot.hpp>

#include <curlpp/Options.hpp>
#include <curlpp/Exception.hpp>

#include <fstream>
#include <iostream>
#include <list>
#include <sstream>
#include <vector>

using namespace SleepyDiscord;
using namespace cURLpp;
using namespace wideinator;

namespace widebot {

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
            std::cout << "Writing " << out_filename << std::endl;
              splits[i].write(out_filename.c_str());

            if (create_emojis) {
              std::vector<Snowflake<Role>> roles; // role have yet to be implemented in sleepy_discord
//            roles.push_back(Snowflake<Role>("801225009636442133"));
              if (!createEmoji(message.serverID, out_name, a.content_type, splits[i], roles)) {
                create_emojis = false;
              }
            }

            uploadFile(message.channelID, out_filename, "");
          }
        }
      }
    }
  }

  int WideBot::parseNumSplits(const std::string& args) {
    int num_splits = 0;
    // assumes the number of splits is the first integer argument
    for(size_t i = 0; i < args.length(); i++) {
      try {
        num_splits = std::stoi(args.substr(i));
        return num_splits;
      }
      catch (std::invalid_argument &e) {
        std::cerr << "Unable to parse splits argument\n";
      }
    }
    return  num_splits;
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

  int WideBot::createEmoji(SleepyDiscord::Snowflake<SleepyDiscord::Server> serverID, const std::string &name, const std::string &mime_type, const Image &image, std::vector<SleepyDiscord::Snowflake<SleepyDiscord::Role>> roles) {
    std::string image_data_uri = "data:" + mime_type + ";base64,";

    std::cout << "Emoji MIME type: " << image_data_uri << std::endl;

    for (auto frame : image.getFrames()) {
      Magick::Blob blob;
      try {
        frame.write(&blob);
      }
      catch(Magick::WarningCoder &w) {
        std::cerr << "Magick codec warning: " << w.what() << std::endl;
      }
      image_data_uri += blob.base64();
    }
    try{
      createServerEmoji(serverID, name, image_data_uri, roles);
    }
    catch(std::exception& e) {
      std::cerr << e.what() << std::endl;
      return false;
    }
    catch (SleepyDiscord::ErrorCode& e) {
      std::cerr << e << std::endl;
      return false;
    }
    return true;
  }

} //namespace widebot
