#include <emp-tool/emp-tool.h>
#include "emp-swap/emp-swap.h"
using namespace emp;
using namespace std;


int port, party;
NetIO * io; 

int main(int argc, char** argv) {
	parse_party_and_port(argv, &party, &port);
	io = new NetIO(party==PROVER ? nullptr : "127.0.0.1", port);
    

    char s[100];

    if(party==PROVER){
        io->send_data("hi",2);
        io->recv_data(s,2);
        cout<<s<<endl;
        io->send_data("ho",2);

    }else{
        io->recv_data(s,2);
        cout<<s<<endl;

        io->send_data("vv",2);

        io->recv_data(s,2);
        cout<<s<<endl;

    }

	delete io;
}
