# WideBot

Discord bot to create images in the style of wide variations of Twitch emotes such as [peepoHappy]() [widepeepoHappy]().

## Building

Requirements:
- cURLpp
- ImageMagick

1. Create a bot at https://discord.com/developers.

2. In the OAuth2 section, enable the Bot scope and "Send Messages" and "Attach Files" bot permissions, then go to the generated URL and invite the bot to your server.

4. Build and run the bot
```
git clone --recursive https://github.com/tarrem/WideBot.git
```
Put the bot token into token.txt
```
mkdir build && cd build
cmake ..
cmake --build .
./WideBot
```

### Build notes:
- Make sure the ImageMagick library and headers are the same version, otherwise it causes linking issues.
- See https://yourwaifu.dev/sleepy-discord/setup-standard-cli.html#common-errors for errors with Sleepy Discord.
- Ubuntu builds seems to be affected by https://askubuntu.com/questions/1233186/ubuntu-20-04-how-to-set-lower-ssl-security-level.

## Usage

In Discord, attach and caption and image or gif with `!wide [num_splits]` and the bot will upload the modified images.

## To-Do
- [ ] Re-optimize gifs transformation
- [ ] Option to automatically upload guild emoji
- [ ] Option to stretch without splitting
  - [x] ImageProcessor class (::split, ::stretch)
- [ ] Delete cached images
- [x] Custom command prefix
- [ ] Handle quotations and code blocks in prefix command
- [ ] Role-based num_splits limit
- [x] Split arguments into array
