#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Function to load an environment variable from .env file
int load_env(const char *filename, const char *key, char *value, size_t value_size) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error opening .env file");
        return -1;
    }

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        char *equals_sign = strchr(line, '=');
        if (!equals_sign) continue;

        *equals_sign = '\0'; // Split the line into key and value
        char *env_key = line;
        char *env_value = equals_sign + 1;

        // Remove trailing newline from value
        env_value[strcspn(env_value, "\r\n")] = '\0';

        if (strcmp(env_key, key) == 0) {
            strncpy(value, env_value, value_size - 1);
            value[value_size - 1] = '\0'; // Ensure null termination
            fclose(file);
            return 0;
        }
    }

    fclose(file);
    fprintf(stderr, "Key '%s' not found in .env file\n", key);
    return -1;
}
