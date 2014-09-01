/*
 * functions.h
 *
 *  Created on: Aug 31, 2014
 *      Author: kevin
 */

#ifndef FUNCTIONS_H_
#define FUNCTIONS_H_

#include "dbg.h"
#include "args.h"
#include "definitions.h"
#include "hash.h"
#include "argv.h"
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <openssl/md5.h>
#include <stdlib.h>
#include <openssl/evp.h>
#include <openssl/aes.h>

HT* allMahHashes;

off_t fsize(const char *filename);
int check_logic(logappend_args * args);
int aes_init(unsigned char *key_data, int key_data_len, unsigned char *salt, EVP_CIPHER_CTX *e_ctx, EVP_CIPHER_CTX *d_ctx);
unsigned char *aes_encrypt(EVP_CIPHER_CTX *e, unsigned char *plaintext, int *len);
unsigned char *aes_decrypt(EVP_CIPHER_CTX *e, unsigned char *ciphertext, int *len);
void invalid();
void printMD5(char * toPrint);


#endif /* FUNCTIONS_H_ */
