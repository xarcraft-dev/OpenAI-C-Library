#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "lib/openai.h"
#include "include/env.h"

#define MAX_INPUT_SIZE 1024
#define MAX_RESPONSE_SIZE 4096

int main() {
    system("clear");
    ChatGPTConfig config;
    char api_key[256];
    char user_input[MAX_INPUT_SIZE];
    char response[MAX_RESPONSE_SIZE];

    if (load_env(".env", "API_KEY", api_key, sizeof(api_key)) != 0) {
        fprintf(stderr, "Failed to load API key\n");
        return 1;
    }

    chatgpt_init(&config, api_key, "gpt-4o", "Your name is CodeGPT. You are a helpful assistant. Your main job is to help people to code.");

    printf("\033[1;33mChatGPT Interactive Mode (type 'exit' to quit):\033[0m\n\n");

    // Interactive loop
    while (1) {
        printf("\033[1;34mYou: \033[0m");
        if (fgets(user_input, sizeof(user_input), stdin) == NULL) {
            printf("\033[1;31mERROR:\033[0mError reading input.\n");
            break;
        }

        // Remove trailing newline from input
        user_input[strcspn(user_input, "\r\n")] = '\0';

        // Exit condition
        if (strcmp(user_input, "exit") == 0) {
            printf("\033[1;35mINFO: \033[0mExiting...\n");
            system("clear");
            break;
        }

        // Send the message to ChatGPT
        if (chatgpt_send_message(&config, user_input, response, sizeof(response)) == 0) {
            printf("\033[1;34mCodeGPT: \033[0m%s\n\n", response);
        } else {
            printf("\n\033[1;31mERROR:\033[0m Failed to get a response from ChatGPT.\n");
        }
    }

    // Cleanup
    chatgpt_cleanup();
    return 0;
}
