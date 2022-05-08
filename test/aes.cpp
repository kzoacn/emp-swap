#include <emp-tool/emp-tool.h>
#include "emp-swap/emp-swap.h"

using namespace emp;
using namespace std;


const char path[]="/home/kzoacn/work/emp-swap/emp-swap/files/AES-non-expanded.txt";
int port, party;
NetIO * io;
// Enc(0,0) = 
const char answer[]="01100110111010010100101111010100111011111000101000101100001110111000100001001100111110100101100111001010001101000010101100101110";
// 66E94BD4EF8A2C3B884CFA59CA342B2E



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


Bit aes_test(void* ctx){

    BristolFormat _aes(path);
    
    Bit out[128],in1[128],in2[128];
    
    for(int i=0;i<128;i++){
        in1[i]=Bit(false,PUBLIC);
        in2[i]=Bit(false,PUBLIC);
    }
    
    _aes.compute(out,in1,in2);

    Bit bit(true,PUBLIC);


    for(int i=0;i<128;i++){    
        if(answer[i]=='1'){
            bit = bit & out[i];
        }else{
            bit = bit & !out[i];
        }
    }

    return bit;
}

int main(int argc, char** argv) {
	parse_party_and_port(argv, &party, &port);
	io = new NetIO(party==PROVER ? nullptr : "127.0.0.1", port);
	setup_arithmetic_zk(io, party);
    //if(party==VERIFIER)return 0;

	if(!judge(io,party,NULL,aes_test)){
		error("failed");
		return 0;
	}
    puts("Yes");
    return 0;
    
    CircuitExecution::circ_exec=new AriPlainEva();

    block out[128],in1[128],in2[128];
    memset(out,0,sizeof out);
    memset(in1,0,sizeof in1);
    memset(in2,0,sizeof in2);



    //string input=padding("0");
    for(int i=0;i<128;i++){
        *((char *) &in1[i]) &= 0xfe;
       	*((char *) &in1[i]) |= 0;       
    }

    BristolFormat aes(path);
    aes.compute(out,in1,in2);


    string st;
    for(int i=0;i<128;i++){
        st+= (get_val(out[i])&1)+'0';
    }
    cout<< st <<endl;
    cout<< to_hex(st) <<endl;

	//puts("Yes");

	delete io;
}
