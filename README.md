# OpenAI Library
This repository contains the code for a library that makes it easy to communicate with ChatGPT API's in C. The library is in the `lib` folder.

## Functions
Here are the functions and their syntaxes. For more details you can look at `example.c` where it gets input from user and gets the answer from **ChatGPT 4o**.

### chatgpt_init()
**Usage:** Initializing the AI chatbot.

**Returns:** `void`

**Syntax:** `chatgpt_init(ChatGPTConfig *config, char *api_key, char *model, char *system_role)`

**Example:** `chatgpt_init(&config, "YOUR_API_KEY", "gpt-4", "Be a helpful assistant.")`

**What it does:** Changes the `ChatGPTConfig *config` to add the `char *api_key`, `char *model` and `char *system_role`. And then it initializes curl by running `curl_global_init(CURL_GLOBAL_ALL)`. When coding with this library please run this command at least and at maximum once.

### chatgpt_send_message()
**Usage:** Sends requested message to ChatGPT.

**Returns:** `integer`, `-1` or `0`

**Syntax:** `chatgpt_send_message(ChatGPTConfig *config, const char *user_message, char *response, size_t response_size)`

**Example:** `chatgpt_send_message(&config, user_input, answer, 4096)`

**What it does:** It sends the user_input to ChatGPT with API key in `ChatGPTConfig *config` and the maximum size of `size_t response_size` and puts the response in `char *response` and returns with `0` if success and with a `-1` if failed.

### chatgpt_cleanup()
**Usage / What it does:** Runs the command `curl_global_cleanup()` which should be ran once. If you are coding with this library, please don't forget you need to use this once at maximum and minimum.

**Returns:** `void`

**Syntax:** `chatgpt_cleanup()`

## Classes
Here are the classes created by this library.

### ChatGPTConfig
As you've seen in the functions, this function holds the structure of this entire library.

**Initialisation:** `ChatGPTConfig <name>`.

## Running the Example Script
To run the example script, first create an `.env` file and then type this in the env file:
```
API_KEY=<your api key here>
```
Replace the `<your api key here>` with your API key that you got from OpenAI Playground. Now to run the example script, while in the main folder open your terminal and type
```
make; make run
```
That should build the script and run it. `make` builds the script and `make run` runs it.

## Contributing
If you want to contribute, just create a pull request and I will try to answer them whenever I can.

## Thank You
Thank you for visiting this repository and reading this. If you want to support me you can follow this repository.