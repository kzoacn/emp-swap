#include <typeinfo>
#include <emp-tool/emp-tool.h>
#include "emp-swap/emp-swap.h"
using namespace std;
using namespace emp;


int port, party;
NetIO * io;


Bit test(void* ctx){
  
    Number a(BITLENGTH,6,PROVER);  
    //Number b(BITLENGTH,8,PROVER);
    Number c(BITLENGTH,6,PUBLIC);

    a.assert_eq(c);
    //Number d=a+b;

    //d.assert_eq(c);

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
