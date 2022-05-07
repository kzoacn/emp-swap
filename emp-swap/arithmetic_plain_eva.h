#ifndef ARITHMETIC_PLAIN_EVA_H__
#define ARITHMETIC_PLAIN_EVA_H__
#include <emp-tool/emp-tool.h>
#include <emp-ot/emp-ot.h>
#include "emp-swap/ari_plain_eva.h"
#include "emp-swap/thread_buffer.h"
namespace emp {

class ArithmeticPlainEva: public ProtocolExecution {
public:
	AriPlainEva * gc;
	ArithmeticPlainEva(AriPlainEva * gc): ProtocolExecution(PROVER) {
		this->gc = gc;	
	}
	~ArithmeticPlainEva() {

	}
	ThreadBuffer<bool> *buf;
	void feed(block * label, int party, const bool* b, int length) {

		buf->push(b,length);

		for(int i=0;i<length;i++){
			*((char*)&label[i])&=0xfe;
			*((char*)&label[i])|=b[i];
        }
	}

	void reveal(bool * b, int party, const block * label, int length) {
        throw;
	}

};


class CountPlainEva: public ProtocolExecution {
public:
	AriPlainEva * gc;
	long long counter;
	CountPlainEva(AriPlainEva * gc): ProtocolExecution(PROVER) {
		this->gc = gc;	
		counter=0;
	}
	~CountPlainEva() {

	} 
	void feed(block * label, int party, const bool* b, int length) {

		counter+=length;

		for(int i=0;i<length;i++){
			*((char*)&label[i])&=0xfe;
			*((char*)&label[i])|=b[i];
        }
	}

	void reveal(bool * b, int party, const block * label, int length) {
        throw;
	}

};
}

#endif