#include <typeinfo>
#include <emp-tool/emp-tool.h>
#include "emp-swap/emp-swap.h"
#include "emp-tool/circuits/number.h"
using namespace emp;
using namespace std;

int port, party;
NetIO * io;

int last;

Bit int_mul(void* ctx){
	int seed=123;
	auto rnd=[&seed]{return seed=(seed*123+12)&0xff;};
	int k=rnd()&0xff;
	Integer a(BITLENGTH,rnd()&0xff,PUBLIC);
	Integer b(BITLENGTH,rnd()&0xff,PUBLIC);
	Integer c(BITLENGTH,rnd()&0xff,PUBLIC);
	Integer x(BITLENGTH,k,PROVER);
	Integer num(BITLENGTH,k,PUBLIC);
	Integer zero(BITLENGTH,0,PUBLIC);


	Bit bit=(a*x*x*x+(b-num*a)*x*x+(c-num*b)*x-num*c)==zero;

	return bit;
}
Bit num_mul(void *ctx){
	int seed=123;
	auto rnd=[&seed]{return seed=(seed*123+12)&0xff;};
	int k=rnd()&0xff;
	Integer _a(BITLENGTH,rnd()&0xff,PUBLIC);
	Integer _b(BITLENGTH,rnd()&0xff,PUBLIC);
	Integer _c(BITLENGTH,rnd()&0xff,PUBLIC);
	Integer _x(BITLENGTH,k,PROVER);
	Integer _num(BITLENGTH,k,PUBLIC);
	Integer _zero(BITLENGTH,0,PUBLIC);

	Number a=b2a(_a);
	Number b=b2a(_b);
	Number c=b2a(_c);
	Number x=b2a(_x);
	Number num=b2a(_num);
	Number zero=b2a(_zero);


	Bit bit=(a*x*x*x+(b-num*a)*x*x+(c-num*b)*x-num*c)==zero;


	return bit;
}

Bit num_test(void *ctx){
	int seed=123;
	auto rnd=[&seed]{return seed=(seed*123+12)&0xff;};

	Bit ans(true,PUBLIC);

	for(int i=0;i<10;i++){
		for(int j=0;j<10;j++){

			Integer _a(BITLENGTH,i,PROVER);
			Integer _b(BITLENGTH,j,PROVER);
			Number a=b2a(_a);
			Number b=b2a(_b);

			Bit b1=_a<_b;
			Bit b2=a<b;
			Bit bit;
			bit=b1==b2;

			ans=ans&bit;
				
			b1=_a<=_b;
			b2=a<=b;
			bit=b1==b2;

			ans=ans&bit;

			b1=_a>_b;
			b2=a>b;
			bit=b1==b2;

			ans=ans&bit;

			b1=_a>=_b;
			b2=a>=b;
			bit=b1==b2;

			ans=ans&bit;

		}
	}
	for(int i=0;i<10;i++){
		for(int j=0;j<10;j++){

			Integer _a(BITLENGTH,i,PROVER);
			Integer _b(BITLENGTH,j,PROVER);
			Number a=b2a(_a);
			Number b=b2a(_b);

			Integer _c=_a+_b;
			Number c=a+b;

			Integer t=a2b(BITLENGTH,c);

			Bit bit=(t==_c);

			ans=ans&bit;

			Integer _d=_a*_b;
			Number d=a*b;

			t=a2b(BITLENGTH,d);

			bit=(t==_d);

			ans=ans&bit;

		}
	}

	for(int i=0;i<10;i++){
		for(int j=0;j<10;j++){

			Integer _a(BITLENGTH,rnd()%(1<<28)-(1<<27),PROVER);
			Integer _b(BITLENGTH,rnd()%(1<<28)-(1<<27),PROVER);
			Number a=b2a(_a);
			Number b=b2a(_b);

			Integer _c=_a+_b;
			Number c=a+b;

			Integer t=a2b(BITLENGTH,c);

			Bit bit=(t==_c);

			ans=ans&bit;
		}
	}
	return ans;
}



int main(int argc, char** argv) {
	parse_party_and_port(argv, &party, &port);
	io = new NetIO(party==PROVER ? nullptr : "127.0.0.1", port);
	
	setup_arithmetic_zk(io, party);

	//prove (ax^2+bx+c)(x-k)=0
	//prove ax^3+(b-ka)x^2+(c-kb)x-kc=0

    
	if(!judge(io,party,NULL,int_mul)){
		error("int_mul failed");
		return 0;
	}

	setup_arithmetic_zk(io, party);

	if(!judge(io,party,NULL,num_mul)){
		error("num_mul failed");
		return 0;
	}
	setup_arithmetic_zk(io, party);

	if(!judge(io,party,NULL,num_test)){
		error("num_test failed");
		return 0;
	}

	puts("Yes");

	delete io;
}
