#ifndef ARITHMETICZK_EVA_H__
#define ARITHMETICZK_EVA_H__
#include <emp-tool/emp-tool.h>
#include <emp-ot/emp-ot.h>
#include "emp-swap/ari_privacy_free_eva.h"
#include "emp-swap/thread_buffer.h"

namespace emp {
template<typename IO>
class ArithmeticZKEva: public ProtocolExecution {
public:
	IO* io = nullptr;
	IKNP<IO>* ot;
	AriPrivacyFreeEva<IO> * gc;
	PRG shared_prg;
	
	block seed;
	bool setup;

	ArithmeticZKEva(IO *io, AriPrivacyFreeEva<IO> * gc): ProtocolExecution(PROVER) {
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
	~ArithmeticZKEva() {
		delete ot;
	} 
	ThreadBuffer<bool> *buf;
	std::queue<block> lbuf;
	bool tmp[THREAD_BUFFER_SIZE];
	block btmp[THREAD_BUFFER_SIZE];
	vector<int>IO_send;
	long long all_size;
	void feed(block * label, int party, const bool* b, int length) {

		if(!setup){
			setup=true;
			ot->setup_recv();
		}
		
		if(length>(int)lbuf.size()){

			buf->greedy_ensure_size(length-(int)lbuf.size(),all_size);

			//int size=std::max(length-(int)lbuf.size(),buf->size());
			
			int size=std::min(all_size,(long long)THREAD_BUFFER_SIZE);
			
			std::cerr<<"batched buffer size : "<<size<<std::endl;

			all_size-=size;
			
			buf->get(tmp,size);
			
	
			io->send_data(&size,4);
			ot->recv_cot(btmp, tmp, size);
			for(int i=0;i<size;i++){
				*((char*)&btmp[i])&=0xfe;
				*((char*)&btmp[i])|=tmp[i];
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