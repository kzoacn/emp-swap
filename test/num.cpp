#include <typeinfo>
#include <emp-tool/emp-tool.h>
#include "emp-swap/emp-swap.h"
using namespace std;
using namespace emp;


int port, party;
NetIO * io;


Bit test(void* ctx){
  
    Number a(BITLENGTH,4,PROVER);  
    Integer b(BITLENGTH,5,PROVER);
    Number c(BITLENGTH,11,PUBLIC);  
    Number d(BITLENGTH,9,PUBLIC);  
 
    Number e=a*b; 
    Number f=c+d; 

    e.assert_eq(f);

    Bit ans(true,PUBLIC); 
    return ans;
}



int main(int argc, char** argv) {
	parse_party_and_port(argv, &party, &port);
	io = new NetIO(party==PROVER ? nullptr : "127.0.0.1", port);

	setup_arithmetic_zk(io, party);
 
	if(!judge(io,party,NULL,test)){
        error("failed");
		return 0;
	}

	puts("Yes");


    


	delete io;
}
