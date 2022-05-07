#include <typeinfo>
#include <emp-tool/emp-tool.h>
#include "emp-swap/emp-swap.h"
using namespace std;
using namespace emp;


int port, party;
NetIO * io;
Number min(Number a,Number b){
    Bit bit=a<b;
    return b.select(bit,a);
}
Number max(Number a,Number b){
    Bit bit=a<b;
    return a.select(bit,b);
}

void swap(Bit bit,Number &a,Number &b){
    Number t1=a.select(bit,b);
    Number t2=a.select(!bit,b);
    a=t1;
    b=t2;
}


Bit sort(void* ctx){
    int n=16;
    Number *a=new Number[n];

    for(int i=0;i<n;i++){
        a[i]=i^(i>>1);
    }
    for(int i=0;i<n;i++){
        for(int j=0;j+1<n;j++){
            Bit s=(a[j]>a[j+1]);
            swap(s,a[j],a[j+1]);
        }
    }

    Bit ans(true,PUBLIC);
    for(int i=0;i<n;i++){

        Bit bit= a[i]==i;

        ans=ans&bit;
    }
    return ans;
}



int main(int argc, char** argv) {
	parse_party_and_port(argv, &party, &port);
	io = new NetIO(party==PROVER ? nullptr : "127.0.0.1", port);

	setup_arithmetic_zk(io, party);

	if(!judge(io,party,NULL,sort)){
        error("failed");
		return 0;
	}

	puts("Yes");


    


	delete io;
}
