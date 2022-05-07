#include <typeinfo>
#include <emp-tool/emp-tool.h>
#include "emp-swap/emp-swap.h"
using namespace emp;
using namespace std;


int port, party;
NetIO * io;
Bit test(void* ctx){

	Integer x(BITLENGTH,6,PROVER);
	Integer y(BITLENGTH,8,PROVER);
	Integer z(BITLENGTH,14,PROVER);
	Integer w(BITLENGTH,15,PROVER);
 
 

	Bit b1=(x+y)>=z;
	Bit b2=(y+x)==z;
	Bit b3=(x+y)<w; 

	return b1 & b2 & b3;
}

Bit ez(void* ctx){ 
	Bit b1(true,PROVER); 
	Bit b2(true,PROVER); 
	return b1 & b2;  
}

int main(int argc, char** argv) {
	parse_party_and_port(argv, &party, &port);
	io = new NetIO(party==PROVER ? nullptr : "127.0.0.1", port);

	setup_arithmetic_zk(io, party);


	if(!judge(io,party,NULL,ez)){
		error("ez1 failed");
		return 0;
	}
	puts("finish 1");

	setup_arithmetic_zk(io, party);

	if(!judge(io,party,NULL,ez)){
		error("ez2 failed");
		return 0;
	}
	puts("finish 2");

	setup_arithmetic_zk(io, party);
	
	if(!judge(io,party,NULL,test)){
		error("ez3 failed");
		return 0;
	}
	

	puts("Yes");

	delete io;
}
