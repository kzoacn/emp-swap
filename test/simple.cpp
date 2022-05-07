#include <typeinfo>
#include <emp-tool/emp-tool.h>
#include "emp-swap/emp-swap.h"
using namespace emp;
using namespace std;


int port, party;
NetIO * io;


Bit ez(void* ctx){
	Bit bit(true,PUBLIC);
//    Number a=1;
//    Number b=2;
	return bit;
}
Bit shouldbewrong(void* ctx){
	Bit bit(false,PUBLIC);
//    Number a=1;
//    Number b=2;
	return bit;
}

int main(int argc, char** argv) {
	parse_party_and_port(argv, &party, &port);
	io = new NetIO(party==PROVER ? nullptr : "127.0.0.1", port);

	setup_arithmetic_zk(io, party);


	if(!judge(io,party,NULL,ez)){
		error("ez failed");
		return 0;
	}

	if(judge(io,party,NULL,shouldbewrong)){
		if(party==VERIFIER)
			error("shouldbewrong failed");
	}
    

	puts("Yes");

	delete io;
}
