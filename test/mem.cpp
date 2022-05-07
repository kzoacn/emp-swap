#include <typeinfo>
#include <emp-tool/emp-tool.h>
#include "emp-swap/emp-swap.h"
using namespace emp;
using namespace std;


int port, party;
NetIO * io;
Bit mem(void* ctx){
 

    Number n;
    for(int i=0;i<1000000;i++)
        n=i;

    Bit bit(true);
    return bit;
} 

int main(int argc, char** argv) {
	parse_party_and_port(argv, &party, &port);
	io = new NetIO(party==PROVER ? nullptr : "127.0.0.1", port);

	setup_arithmetic_zk(io, party);


	if(!judge(io,party,NULL,mem)){
		error("mem failed");
		return 0;
	} 
	

	puts("Yes");

	delete io;
}
