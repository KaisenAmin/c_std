#include "dir/dir.h"
#include "crypto/crypto.h"
#include <stdio.h>
#include <stdint.h>

int main() {
    const char* filePath = "C:\\Users\\Science\\Desktop\\new_one.txt";
    const char* password = "aminamin";
    
    uint8_t iv[DES_BLOCK_SIZE]; // IV for CBC mode
    crypto_generate_random_iv(iv, DES_BLOCK_SIZE); 

    if (dir_encrypt_file(filePath, password, iv)) {
        printf("File encrypted successfully.\n");
    } 
    else {
        printf("Failed to encrypt the file.\n");
    }

    if (dir_decrypt_file(filePath, password, iv)) {
        printf("File decrypted successfully.\n");
    } 
    else {
        printf("Failed to decrypt the file.\n");
    }

    return 0;
}