#include "utils.h"

#include <zel.h>

std::string encrypt(const std::string &plain_text) {

    zel::crypto::Des des;

    char plain_text_bcd[256], input[256], cipher_text[256], ivec[16];
    memset(plain_text_bcd, 0, sizeof(plain_text_bcd));
    memset(input, 0, sizeof(input));
    memset(cipher_text, 0, sizeof(cipher_text));
    memset(ivec, 0, sizeof(ivec));
    // memset(key, 0, sizeof(key));
    char key[256] = "7B02D262D6462BD95AEB5C7646CE4248";

    // des.char2HexStr((char *) plain_text.c_str(), plain_text_bcd, plain_text.size());

    // sprintf((char *) input, "%02d%s", plain_text.size(), plain_text_bcd);
    // int pt_len = strlen((const char *) input), key_len = strlen(key);

    int ret = des.Run3Des(ENCRYPT, CBC, plain_text.c_str(), key, cipher_text, ivec);

    return std::string((char *) cipher_text);
}

std::string decrypt(const std::string &cipher_text) {

    zel::crypto::Des des;

    char cipher_text_bcd[256], input[256], plain_text[256], ivec[16];
    memset(cipher_text_bcd, 0, sizeof(cipher_text_bcd));
    memset(input, 0, sizeof(input));
    memset(plain_text, 0, sizeof(plain_text));
    memset(ivec, 0, sizeof(ivec));

    char key[256] = "7B02D262D6462BD95AEB5C7646CE4248";

    // des.char2HexStr((char *) cipher_text.c_str(), cipher_text_bcd, cipher_text.size());

    // sprintf((char *) input, "%02d%s", cipher_text.size(), cipher_text_bcd);
    // int pt_len = strlen((const char *) input), key_len = strlen(key);

    int ret = des.Run3Des(DECRYPT, CBC, cipher_text.c_str(), key, plain_text, ivec);

    return std::string((char *) plain_text);
}