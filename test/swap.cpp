 #include <emp-tool/emp-tool.h>
#include "emp-swap/emp-swap.h"
using namespace emp;
using namespace std;


int port, party;
NetIO * io;
const char sha_path[]="/home/kzoacn/work/emp-swap/emp-swap/files/sha-256.txt";

// SHA(0) = 
const char answer[] ="1101101001010110100110001011111000010111101110011011010001101001011000100011001101010111100110010111011110011111101111101100101010001100111001011101010010010001110000001101001001100010010000111011101011111110111110011110101000011000001101111010100111011000";
// DA5698BE17B9B46962335799779FBECA8CE5D491C0D26243BAFEF9EA1837A9D8

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


Bit sha_test(void* ctx){

    BristolFormat sha(sha_path);
    
    Bit out[256],in1[512],in2[512];
    
    for(int i=0;i<512;i++){
        in1[i]=Bit(false,PUBLIC);
        in2[i]=Bit(false,PUBLIC);
    }
    
    sha.compute(out,in1,in2);

    Bit bit(true,PUBLIC);


    for(int i=0;i<256;i++){    
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

	if(!judge(io,party,NULL,sha_test)){
		error("failed");
		return 0;
	}
    puts("Yes");
   /*CircuitExecution::circ_exec=new AriPlainEva();


    block out[256],in1[512],in2[512];
    memset(out,0,sizeof out);
    memset(in1,0,sizeof in1);
    memset(in2,0,sizeof in2);

    for(int i=0;i<512;i++){
        *((char *) &in1[i]) &= 0xfe;
       	*((char *) &in1[i]) |= 0;       
    }

    sha.compute(out,in1,in2);


    string st;
    for(int i=0;i<256;i++){
        st+= (get_val(out[i])&1)+'0';
    }

    cout << st << endl;
    cout<< to_hex(st) << endl;
*/
	//puts("Yes");

	delete io;
}
