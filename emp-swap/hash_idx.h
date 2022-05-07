#ifndef HASH_IDX_H
#define HASH_IDX_H

#include "emp-tool/utils/aes.h"
#include <assert.h>
#include <string.h>
namespace emp {


static inline block hash_with_idx(block A,uint64_t idx, const AES_KEY *key) {
	block HA;
	block tweak;

	tweak = makeBlock(2 * idx, (uint64_t) 0);

	{
		block tmp, mask;

		tmp = xorBlocks(double_block(A), tweak);
		mask = tmp;
		AES_ecb_encrypt_blks(&tmp, 1, key);
		HA = xorBlocks(tmp, mask);
	}

    return HA;
}

static inline void hash_with_indices(int length,block *H,const block *A,uint64_t idx, const AES_KEY *key) {//TOOD buffer

	block* tweak=new block[length];

	for(int i=0;i<length;i++)
		tweak[i] = makeBlock(2 * (idx+i), (long) 0);

	
	block *masks=new block[length];
	block *keys=new block[length];

	for(int i=0;i<length;i++)
		masks[i] = keys[i] = xorBlocks(double_block(A[i]), tweak[i]);

	AES_ecb_encrypt_blks(keys, length, key);
	for(int i=0;i<length;i++)
		H[i] = xorBlocks(keys[i], masks[i]);
	
	delete[] tweak;
	delete[] masks;
	delete[] keys;
}

static inline void hash_with_delta(int length,block *H,const block *A,uint64_t idx, block delta,const AES_KEY *key) {

	block* tweak=new block[length];

	for(int i=0;i<length;i++)
		tweak[i] = makeBlock(2 * (idx+i), (long) 0);

	
	block *masks=new block[length*2];
	block *keys=new block[length*2];

	for(int i=0;i<length;i++)
		keys[i] = xorBlocks(double_block(A[i]), tweak[i]);
	for(int i=length;i<length*2;i++)
		keys[i] = xorBlocks(double_block(xorBlocks(A[i-length],delta)), tweak[i-length]);

	memcpy(masks, keys, sizeof(block)*length*2);

	AES_ecb_encrypt_blks(keys, length*2, key);
	for(int i=0;i<length*2;i++)
		H[i] = xorBlocks(keys[i], masks[i]);
	
	delete[] tweak;
	delete[] masks;
	delete[] keys;	
}


}
#endif
