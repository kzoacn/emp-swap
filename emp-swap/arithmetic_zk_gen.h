#ifndef ARITHMETICZK_GEN_H__
#define ARITHMETICZK_GEN_H__
#include <emp-tool/emp-tool.h>
#include <emp-ot/emp-ot.h>
#include "emp-swap/ari_privacy_free_gen.h"
#include <iostream>
#include "emp-swap/thread_buffer.h"

namespace emp {
template<typename IO>
class ArithmeticZKGen: public ProtocolExecution {
public:
	IO* io;
	IKNP<IO> * ot;//TODO SHOT/MOT
	PRG prg, shared_prg;
	AriPrivacyFreeGen<IO> *gc;
    block seed;
	bool setup;
	ArithmeticZKGen(IO* io, AriPrivacyFreeGen<IO>* gc): ProtocolExecution(VERIFIER) {
		this->io = io;
		ot = new IKNP<IO>(io);		
		setup=false;
		
		this->gc = gc;	
		shared_prg.reseed(&zero_block);
		
#ifdef EMP_USE_RANDOM_DEVICE
			int * data = (int *)(&seed);
			std::random_device rand_div;
			for (size_t i = 0; i < sizeof(block) / sizeof(int); ++i)
				data[i] = rand_div();
#else
			unsigned long long r0, r1;
			_rdseed64_step(&r0);
			_rdseed64_step(&r1);
			seed = makeBlock(r0, r1);
#endif
		ot->prg.reseed(&seed);
	}
	~ArithmeticZKGen() {
		delete ot;
	} 
	std::queue<block> lbuf;
	bool tmp[THREAD_BUFFER_SIZE];
	block btmp[THREAD_BUFFER_SIZE];

	void feed(block * label, int party, const bool* b, int length) {
		if(!setup){
			setup=true;
			bool bdelta[128];
			block_to_bool(bdelta,gc->delta);
			ot->setup_send(bdelta);
		}
		
		if(length>(int)lbuf.size()){
			int size;
			io->recv_data(&size,4);
			ot->send_cot(btmp, size);
			for(int i=0;i<size;i++){
				*((char*)&btmp[i])&=0xfe;
				lbuf.push(btmp[i]);
			}
			
		}
		for(int i=0;i<length;i++){
			label[i]=lbuf.front(); 
			lbuf.pop();
		}

	}

	void reveal(bool * b, int party, const block * label, int length) {
		throw;
	}
};
}
#endif