#ifndef CHATGPT_API_H
#define CHATGPT_API_H

#include <stddef.h>

typedef struct {
    char api_key[128];
    char model[64];
    char system_role[256];
} ChatGPTConfig;

void chatgpt_init(ChatGPTConfig *config, const char *api_key, const char *model, const char *system_role);
void chatgpt_cleanup();
int chatgpt_send_message(ChatGPTConfig *config, const char *user_message, char *response, size_t response_size);

#endif // CHATGPT_API_H
