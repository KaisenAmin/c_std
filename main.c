#include "fmt/fmt.h"
#include "secrets/secrets.h"
#include "vector/vector.h"
#include "string/std_string.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define PASSWORD_LENGTH 10
#define NUM_PASSWORDS 5

bool check_password_requirements(const char* password) {
    int has_lower = 0, has_upper = 0, digit_count = 0;

    for (size_t i = 0; i < PASSWORD_LENGTH; i++) {
        if (islower(password[i])) {
            has_lower = 1;
        }
        if (isupper(password[i])) {
            has_upper = 1;
        }
        if (isdigit(password[i])) {
            digit_count++;
        }
    }

    return (has_lower && has_upper && digit_count >= 3);
}

int main() {
    Vector* passwords = vector_create(sizeof(String*));
    size_t ascii_len = string_length_cstr(STRING_ASCII_LETTERS);
    size_t digits_len = string_length_cstr(STRING_DIGITS);
    char* alphabet = (char*)malloc(sizeof(char) * (ascii_len + digits_len + 1));

    if (!alphabet) {
        fmt_fprintf(stderr, "Error: Cannot allocate memory\n");
        exit(EXIT_FAILURE);
    }

    strcpy(alphabet, STRING_ASCII_LETTERS);
    strcat(alphabet, STRING_DIGITS);

    size_t alphabet_len = string_length_cstr(alphabet);

    for (size_t i = 0; i < NUM_PASSWORDS; i++) {
        String* password = string_create("");

        while (1) {
            string_clear(password);

            for (size_t j = 0; j < PASSWORD_LENGTH; j++) {
                char* random_char = (char*)secrets_choice(alphabet, alphabet_len, sizeof(char));
                string_push_back(password, *random_char);
            }
            if (check_password_requirements(password->dataStr)) {
                break; 
            }
        }

        vector_push_back(passwords, &password);
    }

    fmt_printf("Generated Secure Passwords:\n");
    for (size_t i = 0; i < vector_size(passwords); i++) {
        String** password = (String**)vector_at(passwords, i);
        fmt_printf("%zu: %s\n", i + 1, (*password)->dataStr);
    }

    for (size_t i = 0; i < vector_size(passwords); i++) {
        String** password = (String**)vector_at(passwords, i);
        string_deallocate(*password);
    }

    vector_deallocate(passwords);
    free(alphabet);
    return 0;
}
