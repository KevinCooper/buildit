/*
 * functions.h
 *
 *  Created on: Sep 1, 2014
 *      Author: kevin
 */

#ifndef FUNCTIONS_H_
#define FUNCTIONS_H_

#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <openssl/md5.h>
#include <stdlib.h>
#include <openssl/evp.h>
#include <openssl/aes.h>
#include "definitions.h"

void printMD5(char * toPrint);
off_t fsize(const char *filename);
int aes_init(unsigned char *key_data, int key_data_len, unsigned char *salt, EVP_CIPHER_CTX *e_ctx, EVP_CIPHER_CTX *d_ctx);
unsigned char *aes_encrypt(EVP_CIPHER_CTX *e, unsigned char *plaintext, int *len);
unsigned char *aes_decrypt(EVP_CIPHER_CTX *e, unsigned char *ciphertext, int *len);
void invalid();
void checkMahFile(logread_args args);
void invalid_0();
void nameOpt(char * input);

void numOpt(char * input);

#endif /* FUNCTIONS_H_ */
