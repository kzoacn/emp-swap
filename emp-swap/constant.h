#ifndef __CONSTANT_H_
#define __CONSTANT_H_

#define THREAD_BUFFER_SIZE 1024*1024

__thread BIGNUM *MOD;
__thread EC_GROUP *GROUP;
__thread BN_CTX *CTX;

#endif