#include <typeinfo>
#include <emp-tool/emp-tool.h>
#include "emp-swap/emp-swap.h"
using namespace emp;
using namespace std;


int port, party;
NetIO * io;


Bit test_sqrt(void* ctx){


	int seed=123;
	auto rnd=[&seed]{return seed=(seed*123+13)&0xffffff;};

	Bit bit(true,PUBLIC);

	for(int T=0;T<100;T++){

		double x=rnd()%1000;
		//std::cerr<<x<<std::endl;
		Real lower=sqrt(x)-0.1;
		Real upper=sqrt(x)+0.1;	


		Real sqrtn;

		Real n=x;
		Real l=1;
		Real r=n;

		for(int i=0;i<20;i++){
			Real mid=(l+r)/2;
			Bit bit=mid*mid<n;
			l=l.select(bit,mid);
			r=r.select(!bit,mid);
		}
			
		sqrtn=l;

		bit=bit& (lower<sqrtn) & (sqrtn<upper);

		bit=bit& (lower<n.sqrt()) & (n.sqrt()<upper);
	}

	for(int T=0;T<100;T++){

		double x=(rnd()%10000-5000)/20;

		int f=floor(x);
		int c=ceil(x);
		int r=round(x);

		Real t=x;
		bit=bit & (t.ceil()==c);
		bit=bit & (t.floor()==f);
		bit=bit & (t.round()==r);

	}
    return bit;
}

int main(int argc, char** argv) {
	parse_party_and_port(argv, &party, &port);
	io = new NetIO(party==PROVER ? nullptr : "127.0.0.1", port);

	setup_arithmetic_zk(io, party);

	
	if(!judge(io,party,NULL,test_sqrt)){
		error("test failed");
		return 0;
	}

	puts("Yes");

	delete io;
}
