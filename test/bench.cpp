#include <typeinfo>
#include <emp-tool/emp-tool.h>
#include "emp-swap/emp-swap.h"
#include "emp-tool/circuits/number.h"
using namespace std;
using namespace emp;


int port, party;
NetIO * io;


struct Clock{
    double st;
    double ed;
};


Bit num_mul(void *ctx){
    Clock* clk=(Clock*)ctx;
    Integer _a(BITLENGTH,13,PROVER);
    Integer _b(BITLENGTH,11,PROVER);

    Number a=b2a(_a);
    //Number b=b2a(_b);
    Number c;
    clk->st=clock();
    for(int i=0;i<500000;i++){
        c=a*_b;
    }
    clk->ed=clock();
    return c>=a;
}
Bit num_sel(void *ctx){
    Clock* clk=(Clock*)ctx;
    Integer _a(BITLENGTH,13,PROVER);
    Integer _b(BITLENGTH,11,PROVER);

    Bit bit(true,PROVER);
    Number a=b2a(_a);
    Number b=b2a(_b);
    Number c;
    clk->st=clock();
    for(int i=0;i<1000000;i++){
        c=b.select(bit,a);
    }
    clk->ed=clock();
    return c>=a;
}
Bit int_mul(void *ctx){
    Clock* clk=(Clock*)ctx;
    Integer _a(BITLENGTH,13,PROVER);
    Integer _b(BITLENGTH,11,PROVER);
    Integer _c;
    clk->st=clock();
    for(int i=0;i<500000;i++){
        _c=_a*_b;
    }
    clk->ed=clock();
    return _c>=_a;
}

Bit int_sel(void *ctx){
    Clock* clk=(Clock*)ctx;
    Integer _a(BITLENGTH,13,PROVER);
    Integer _b(BITLENGTH,11,PROVER);
    Integer _c;
    Bit bit(true,PROVER);
    clk->st=clock();
    for(int i=0;i<500000;i++){
        _c.select(bit,_a);
    }
    clk->ed=clock();
    return _c>=_a;
}

int main(int argc, char** argv) {
	parse_party_and_port(argv, &party, &port);
	io = new NetIO(party==PROVER ? nullptr : "127.0.0.1", port);

	setup_arithmetic_zk(io, party);

    Clock *ctx=new Clock;
	if(!judge(io,party,ctx,num_mul)){
		puts("failed");
		//return 0;
	}
    std::cout<<"500000 times num x int mul: "<<(ctx->ed-ctx->st)/CLOCKS_PER_SEC<<std::endl;
    //std::cout<<modcnt<<std::endl;

/*
	if(!judge(io,party,ctx,num_sel)){
		puts("failed");
		return 0;
	}
    std::cout<<"1000000 times num sel: "<<(ctx->ed-ctx->st)/CLOCKS_PER_SEC<<std::endl;
*/

	if(!judge(io,party,ctx,int_mul)){
		puts("failed");
		//return 0;
	}
    std::cout<<"500000 times int x int mul: "<<(ctx->ed-ctx->st)/CLOCKS_PER_SEC<<std::endl;

	puts("Yes");


    


	delete io;
}
