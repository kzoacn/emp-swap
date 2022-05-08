#include <typeinfo>
#include <emp-tool/emp-tool.h>
#include "emp-swap/emp-swap.h"
using namespace std;
using namespace emp;


int port, party;
NetIO * io;


Bit test(void* ctx){
  

    BIGNUM *num=BN_new();
    BN_set_word(num,124);
    EC_POINT *P=EC_POINT_new(GROUP);
    EC_POINT_mul(GROUP,P,num,NULL,NULL,CTX);

    Number a(BITLENGTH,124,PROVER);  
 
    a.assert_dlog(P);



    Bit ans(true,PUBLIC); 
    return ans;
}

Bit test_fail(void* ctx){
  

    BIGNUM *num=BN_new();
    BN_set_word(num,123);
    EC_POINT *P=EC_POINT_new(GROUP);
    EC_POINT_mul(GROUP,P,num,NULL,NULL,CTX);

    Number a(BITLENGTH,124,PROVER);  
 
    a.assert_dlog(P);



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


	setup_arithmetic_zk(io, party);
 
	if(judge(io,party,NULL,test_fail)){
        if(party==VERIFIER)
            error("failed");
		return 0;
	}

	puts("Yes");


    


	delete io;
}
