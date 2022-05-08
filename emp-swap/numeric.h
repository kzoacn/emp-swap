#ifndef NUMERIC_H
#define NUMERIC_H
#include "emp-tool/emp-tool.h"

namespace emp {

const static int PROVER = 1;
const static int VERIFIER = 2;

 
#define LOGMOD 256

#ifndef BITLENGTH

#define BITLENGTH 256

#endif 

void print(char *s1,BIGNUM *n){
	char *s = BN_bn2dec(n);
	printf("%s %s\n",s1,s);
}

block make_delta(block delta){ 
    *((char *) &delta) |= 0x01;
    return delta;
}
__attribute__((target("sse2")))
inline block double_block(block bl) {
	const __m128i mask = _mm_set_epi32(135,1,1,1);
	__m128i tmp = _mm_srai_epi32(bl, 31);
	tmp = _mm_and_si128(tmp, mask);
	tmp = _mm_shuffle_epi32(tmp, _MM_SHUFFLE(2,1,0,3));
	bl = _mm_slli_epi32(bl, 1);
	return _mm_xor_si128(bl,tmp);
}
__attribute__((target("sse2")))
inline block xorBlocks(block x, block y) {
	return _mm_xor_si128(x,y);
}
__attribute__((target("sse2")))
inline block andBlocks(block x, block y) {
	return _mm_and_si128(x,y);
}


void printBlock(block var)  {
    int64_t *v64val = (int64_t*) &var;
    printf("%.16llx %.16llx\n", (long long)v64val[1], (long long)v64val[0]);
}
 

long long get_val(const block &val) {
    long long *vv=(long long*)&val;
    return *vv;
}

template<class IO>
void send_bn(IO *io, const BIGNUM *bn) {
	int len = BN_num_bytes(bn);
	io->send_data(&len,4);
	unsigned char tmp[32];
	BN_bn2bin(bn, tmp);
	io->send_data(tmp, len);
}

template<class IO>
void recv_bn(IO *io, BIGNUM *&bn) {
	int len;
	io->recv_data(&len,4);
	unsigned char tmp[32];
	io->recv_data(tmp, len);
	BN_bin2bn(tmp,len,bn);
}

}
#endif
