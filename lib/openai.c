#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include "../include/cJSON.h"

#define API_URL "https://api.openai.com/v1/chat/completions"
#define BUFFER_SIZE 4096

// Structure for API configuration
typedef struct {
    char api_key[256];
    char model[64];
    char system_role[256];
} ChatGPTConfig;

// Structure to store response data
typedef struct {
    char *data;
    size_t size;
} ResponseBuffer;

// Callback function for libcurl to handle API responses
size_t write_callback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t realsize = size * nmemb;
    ResponseBuffer *buffer = (ResponseBuffer *)userp;

    char *ptr = realloc(buffer->data, buffer->size + realsize + 1);
    if (ptr == NULL) {
        fprintf(stderr, "Out of memory!\n");
        return 0;
    }

    buffer->data = ptr;
    memcpy(&(buffer->data[buffer->size]), contents, realsize);
    buffer->size += realsize;
    buffer->data[buffer->size] = '\0';

    return realsize;
}

// Initialize the ChatGPT library with API key, model, and system role
void chatgpt_init(ChatGPTConfig *config, const char *api_key, const char *model, const char *system_role) {
    strncpy(config->api_key, api_key, sizeof(config->api_key) - 1);
    strncpy(config->model, model, sizeof(config->model) - 1);
    strncpy(config->system_role, system_role, sizeof(config->system_role) - 1);

    config->api_key[sizeof(config->api_key) - 1] = '\0';
    config->model[sizeof(config->model) - 1] = '\0';
    config->system_role[sizeof(config->system_role) - 1] = '\0';

    curl_global_init(CURL_GLOBAL_ALL);
}

// Cleanup resources
void chatgpt_cleanup() {
    curl_global_cleanup();
}

int chatgpt_send_message(ChatGPTConfig *config, const char *user_message, char *response, size_t response_size) {
    CURL *curl;
    CURLcode res;

    ResponseBuffer buffer = { .data = malloc(1), .size = 0 };
    if (buffer.data == NULL) {
        fprintf(stderr, "Failed to allocate memory\n");
        return -1;
    }

    curl = curl_easy_init();
    if (!curl) {
        fprintf(stderr, "Failed to initialize libcurl\n");
        free(buffer.data);
        return -1;
    }

    // Set up headers
    struct curl_slist *headers = NULL;
    char auth_header[BUFFER_SIZE];
    snprintf(auth_header, sizeof(auth_header), "Authorization: Bearer %s", config->api_key);
    headers = curl_slist_append(headers, "Content-Type: application/json");
    headers = curl_slist_append(headers, auth_header);

    // Set up the request body
    char request_body[BUFFER_SIZE];
    snprintf(request_body, sizeof(request_body),
             "{\"model\": \"%s\", \"messages\": ["
             "{\"role\": \"system\", \"content\": \"%s\"},"
             "{\"role\": \"user\", \"content\": \"%s\"}"
             "]}",
             config->model, config->system_role, user_message);

    // Configure curl options
    curl_easy_setopt(curl, CURLOPT_URL, API_URL);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, request_body);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);

    // Perform the request
    res = curl_easy_perform(curl);

    if (res != CURLE_OK) {
        fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        free(buffer.data);
        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);
        return -1;
    }

    // printf("Raw API Response: %s\n", buffer.data);

    // Parse JSON response
    cJSON *json = cJSON_Parse(buffer.data);
    if (!json) {
        fprintf(stderr, "Failed to parse JSON response: Malformed JSON\n");
        free(buffer.data);
        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);
        return -1;
    }

    // Check for "error" field in response
    cJSON *error = cJSON_GetObjectItemCaseSensitive(json, "error");
    if (error) {
        cJSON *message = cJSON_GetObjectItemCaseSensitive(error, "message");
        if (cJSON_IsString(message) && message->valuestring) {
            fprintf(stderr, "API Error: %s\n", message->valuestring);
        } else {
            fprintf(stderr, "API Error: Unknown error occurred\n");
        }
        cJSON_Delete(json);
        free(buffer.data);
        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);
        return -1;
    }

    // Get "choices" array
    cJSON *choices = cJSON_GetObjectItemCaseSensitive(json, "choices");
    if (!cJSON_IsArray(choices)) {
        fprintf(stderr, "Invalid response: 'choices' is not an array\n");
        cJSON_Delete(json);
        free(buffer.data);
        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);
        return -1;
    }

    // Get first choice
    cJSON *choice = cJSON_GetArrayItem(choices, 0);
    if (!choice) {
        fprintf(stderr, "Invalid response: No choices available\n");
        cJSON_Delete(json);
        free(buffer.data);
        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);
        return -1;
    }

    // Get "message" object
    cJSON *message = cJSON_GetObjectItemCaseSensitive(choice, "message");
    if (!message) {
        fprintf(stderr, "Invalid response: No 'message' object in choice\n");
        cJSON_Delete(json);
        free(buffer.data);
        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);
        return -1;
    }

    // Get "content" field
    cJSON *content = cJSON_GetObjectItemCaseSensitive(message, "content");
    if (!cJSON_IsString(content) || !content->valuestring) {
        fprintf(stderr, "Invalid response: No valid 'content' in message\n");
        cJSON_Delete(json);
        free(buffer.data);
        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);
        return -1;
    }

    // Copy the assistant's message content to the response buffer
    strncpy(response, content->valuestring, response_size - 1);
    response[response_size - 1] = '\0'; // Null-terminate explicitly

    // Cleanup
    cJSON_Delete(json);
    free(buffer.data);
    curl_easy_cleanup(curl);
    curl_slist_free_all(headers);

    return 0;
}