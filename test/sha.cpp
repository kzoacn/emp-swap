 #include <emp-tool/emp-tool.h>
#include "emp-swap/emp-swap.h"
using namespace emp;
using namespace std;


int port, party;
NetIO * io;
BristolFormat sha("/home/kzoacn/work/emp-swap/emp-swap/files/sha-256.txt");


Bit ez(void* ctx){
	Bit bit(true,PUBLIC);
    //CircuitFile sha256;


	return bit;
}

string padding(string chars){
    string ans;

    long long l=chars.length()*8;
    for(int i=0;i<chars.length();i++){
        unsigned char c=chars[i];
        for(int j=0;j<8;j++){
            ans+=(char)(c>>(7-j)&1);
        }
    }
    
    
    ans+=(char)1;
    while(ans.length()%512!=448){
        ans+=(char)0;
    }
    for(int i=0;i<8;i++){
        unsigned char c=*(((unsigned char*)&l)+7-i);
        for(int j=0;j<8;j++)
            ans+=(c>>(7-j)&1) ? (char)1 : (char)0;
    }
    return ans;
}



/*Bit sha_test(void* ctx){

    BristolFormat sha(sha_path);
    
    Bit out[256],in1[512],in2[512];
    
    for(int i=0;i<512;i++){
        in1[i]=Bit(false,PUBLIC);
        in2[i]=Bit(false,PUBLIC);
    }
    
    sha.compute(out,in1,in2);

    Bit bit(true,PUBLIC);


    for(int i=0;i<256;i++){    
        if(sha_0[i]=='1'){
            bit = bit & out[i];
        }else{
            bit = bit & !out[i];
        }
    }

    return bit;
}*/


char to_hex(int x){
    if(x<10)
        return '0'+x;
    return 'A'+x-10;
}

string to_hex(string s){
    string ans;
    for(int i=0;i<(int)s.length();i+=4){
        int x=0;
        for(int j=0;j<4;j++)
            x=x<<1 | (s[i+j]-'0');
        ans+=to_hex(x);
    }
    return ans;
}

int main(int argc, char** argv) {

	parse_party_and_port(argv, &party, &port);
	io = new NetIO(party==PROVER ? nullptr : "127.0.0.1", port);
	setup_arithmetic_zk(io, party);
    if(party==VERIFIER)return 0;

	/*if(!judge(io,party,NULL,ez)){
		error("ez failed");
		return 0;
	}
    */
   CircuitExecution::circ_exec=new AriPlainEva();


    block out[256],in1[512],in2[512];
    memset(out,0,sizeof out);
    memset(in1,0,sizeof in1);
    memset(in2,0,sizeof in2);



    string input=padding("0");
    for(int i=0;i<512;i++){
        *((char *) &in1[i]) &= 0xfe;
       	*((char *) &in1[i]) |= input[i];       
    }

    sha.compute(out,in1,in2);


    string st;
    for(int i=0;i<256;i++){
        st+= (get_val(out[i])&1)+'0';
    }
    cout<< to_hex(st);

	//puts("Yes");

	delete io;
}
