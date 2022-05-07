#include <typeinfo>
#include <emp-tool/emp-tool.h>
#include "emp-swap/emp-swap.h"
using namespace emp;
using namespace std;


int port, party;
NetIO * io;


Integer randOP(int op,Integer a,Integer b){
	if(op==0)return a+b;
	if(op==1)return a-b;
	if(op==2)return a*b;
	return -a;
}
Number randOP(int op,Number a,Number b){
	if(op==0)return a+b;
	if(op==1)return a-b;
	if(op==2)return a*b;
	return -a;
}



Bit test(void* ctx){


	int seed=111;
	auto rnd=[&seed]{return seed=(seed*2411+12412)%10007;};

	Bit bit(true,PROVER);
	Integer bthres(BITLENGTH,1<<13,PROVER);
	Number athres=1<<13;
	for(int T=0;T<100;T++){

		const int n=16;
		Integer b[n];
		Number a[n];
		for(int i=0;i<n;i++){
			b[i]=Integer(BITLENGTH,rnd()%256-127,PROVER);
			a[i]=b2a(b[i]);
		}
		for(int k=0;k<100;k++){
			int op=rnd()%4;
			int x=rnd()%n,y=rnd()%n,z=rnd()%n;
			b[z]=randOP(op,b[x],b[y]);
			a[z]=randOP(op,a[x],a[y]);

			b[z]=b[z].select(b[z]>bthres,bthres);
			b[z]=b[z].select(b[z]<-bthres,-bthres);
			a[z]=a[z].select(a[z]>athres,athres);
			a[z]=a[z].select(a[z]<-athres,-athres);
		}

		for(int i=0;i<n;i++){
			Integer t=a2b(BITLENGTH,a[i]);
			bit=bit&(t==b[i]);
		}

	}
	return bit;

	for(int T=0;T<100;T++){
		int x=rnd()%255-127;
		int shamt=rnd()%8;
		Number tt=x;
		tt=tt>>shamt;
		Integer t2(BITLENGTH,x>>shamt,PROVER);
		bit=bit&(a2b(BITLENGTH,tt)==t2);
	}

	for(int T=0;T<100;T++){
		int _a=rnd()%1256+1;
		int _b=rnd()%1256+1;
		int _p=_a/_b;
		int _q=_a%_b;
		Number a=_a;
		Number b=_b;
		Number p=a/b;
		Number q=a%b;
		
		Number t1=_p;
		Number t2=_q;

		bit=bit&(p==t1);
		bit=bit&(q==t2);
	}


	for(int T=0;T<100;T++){
		int _a=rnd()%1111;
		int _sqrta=sqrt(_a);
		Number a=_a;
		Number sqrta=a.sqrt();
		bit=bit & (sqrta==_sqrta);
	}

    return bit;
}

int main(int argc, char** argv) {
	parse_party_and_port(argv, &party, &port);
	io = new NetIO(party==PROVER ? nullptr : "127.0.0.1", port);

	setup_arithmetic_zk(io, party);

	
	if(!judge(io,party,NULL,test)){
		error("test failed");
		return 0;
	}

	puts("Yes");

	delete io;
}
