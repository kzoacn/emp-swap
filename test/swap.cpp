 #include <emp-tool/emp-tool.h>
#include "emp-swap/emp-swap.h"
using namespace emp;
using namespace std;


int port, party;
NetIO * io;
const char sha_path[]="/home/kzoacn/work/emp-swap/emp-swap/files/sha-256.txt";
const char aes_path[]="/home/kzoacn/work/emp-swap/emp-swap/files/AES-non-expanded.txt";
// SHA(0) = 
const char sha_0[]="1101101001010110100110001011111000010111101110011011010001101001011000100011001101010111100110010111011110011111101111101100101010001100111001011101010010010001110000001101001001100010010000111011101011111110111110011110101000011000001101111010100111011000";
// SHA(1) = 
const char sha_1[]="1110001110110000110001000100001010011000111111000001110000010100100110101111101111110100110010001001100101101111101110010010010000100111101011100100000111100100011001001001101110010011010011001010010010010101100110010001101101111000010100101011100001010101";

const char ct[]="1101101011110001001011101101100110000101001000011110110111001001100100001000010110011100011111100100000110111001011010001011011100111000110001100010100100000010111000011100110111010011010001100110001010101011111010100010001010000001010011111011101100000111";

const char s_key[]="1000101011111010110000010110100101110111100000010000000111001101010110001011100101110110001011001010111101101111001111110000000101001110110010010000101110000111000101011100100110111100100110011001010101010111111101011011000110010111001111010001000101111101";

const char s_sinv[]="1101100100101001011111010011001101111010111001110111100001001100101000100010110000101101010111100000100101010111001010011010100010101001101011110101101000111000101100101110111010011001100001100011100011001011010010001111000010001110110101010111100111100011";

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
 

namespace ECDSA{

EC_POINT *PK;
BIGNUM *sk;
EC_POINT *P; //kG
BIGNUM *r,*s;
BIGNUM *z;

const int _sk = 1234567;
const int message = 123;
const int _k=124;

void keygen(){
    PK=EC_POINT_new(GROUP);
    sk=BN_new();
    if(party==PROVER){
        BN_set_word(sk,_sk);
        EC_POINT_mul(GROUP,PK,sk,NULL,NULL,CTX);
        send_pt(io,PK);
    }else{
        recv_pt(io,PK);
    }
}

void sign(){
    z=BN_new();
    BN_set_word(z,message);
    r=BN_new();
    s=BN_new();
    P=EC_POINT_new(GROUP); 
    if(party==PROVER){
         
        BIGNUM *k,*tmp,*kinv;
        k=BN_new();
        tmp=BN_new();
        kinv=BN_new();

        BN_set_word(k,_k);

        
        EC_POINT_mul(GROUP,P,k,NULL,NULL,CTX);
        EC_POINT_get_affine_coordinates_GFp(GROUP,P,r,NULL,CTX);

        BN_mod(r,r,MOD,CTX);
        // if r==0, then k is not a valid secret key
        if(BN_is_zero(r)){
            error("failed sign");
            return;
        }
        BN_mod_mul(tmp,r,sk,MOD,CTX);
        BN_mod_add(tmp,z,tmp,MOD,CTX);
        BN_mod_inverse(kinv,k,MOD,CTX);
        BN_mod_mul(s,tmp,kinv,MOD,CTX);

        send_bn(io,r);
        //send_bn(io,s);
        send_pt(io,P); 
    }else{ 
        recv_bn(io,r);
        //recv_bn(io,s);
        recv_pt(io,P);
    } 
}

void veify(){
    BIGNUM *tmp,*sinv,*u1,*u2;
    tmp=BN_new();
    sinv=BN_new();
    u1=BN_new();
    u2=BN_new();

    if(BN_is_zero(r)){
        error("verify failed");
        return;
    }
    BN_mod_inverse(sinv,s,MOD,CTX);
    BN_mod_mul(u1,z,sinv,MOD,CTX);
    BN_mod_mul(u2,r,sinv,MOD,CTX);
    EC_POINT *Q=EC_POINT_new(GROUP);
    EC_POINT_mul(GROUP,Q,u1,PK,u2,CTX);
    
    EC_POINT_get_affine_coordinates_GFp(GROUP,Q,tmp,NULL,CTX);
    BN_mod(tmp,tmp,MOD,CTX);
    if(BN_cmp(tmp,r)!=0){
        error("verify failed");
        return;
    }

    puts("Yes");
}

Bit ecdsa(Bit *_sinv,int length){ 

    bool bits[256];  

    //BIGNUM *sinv=BN_new();
    //BN_mod_inverse(sinv,s,MOD,CTX);
    //bn2bool(sinv,bits);
    vector<Bit> vec_sinv;
    for(int i=0;i<length;i++)
        vec_sinv.push_back(_sinv[i]);
    Integer int_sinv(vec_sinv);
    Number num_sinv=b2a(int_sinv);//(BITLENGTH,bits,PROVER);

    bn2bool(z,bits);
    Integer int_z;
    int_z.init(bits,BITLENGTH,PUBLIC);

    bn2bool(r,bits);
    Integer int_r;
    int_r.init(bits,BITLENGTH,PUBLIC);

    Number u1,u2;
    u1=num_sinv*int_z;
    u2=num_sinv*int_r;

    Integer int_sk;
    bn2bool(sk,bits);    
    int_sk.init(bits,BITLENGTH,PROVER);

    Number num_sk;
    num_sk=b2a(int_sk);
    num_sk.assert_dlog(PK);

    Number k=u1+u2*int_sk;

    k.assert_dlog(P);

    Bit ans(true,PUBLIC);
    return ans;
} 



}


Bit C(int length,Bit *m){
    return ECDSA::ecdsa(m,length);
}

Bit check(int length,Bit *bits,const char *s){
    Bit bit(true,PUBLIC);
    for(int i=0;i<length;i++){
        if(s[i]=='1'){
            bit = bit & bits[i];
        }else{
            bit = bit & !bits[i];
        }
    }
    return bit;
}

Bit swap(void *ctx){
 
    Bit result(true,PUBLIC);

    BristolFormat aes(aes_path);
    BristolFormat sha(sha_path);
    vector<Bit> key;
    vector<Bit> m;
    
    for(int i=0;i<128;i++){ 
        key.push_back(Bit(s_key[i]=='1',PROVER));
    }

    for(int i=0;i<256;i++){ 
        m.push_back(Bit(s_sinv[i]=='1',PROVER));
    }

    vector<Bit> out;
    out.resize(256);

    aes.compute(out.data(),key.data(),m.data());
    aes.compute(out.data()+128,key.data(),m.data()+128);

    /*if(party==PROVER){
        for(int i=0;i<256;i++){
            cout<< getLSB(out[i].bit);
        }
        cout<<endl;
    }*/

    Bit bit = check(out.size(),out.data(),ct);
    result = result & bit;

    Bit cbit = C(m.size(),m.data());
    result = result & cbit;

    for(int i=0;i<128;i++){
        Bit com[256],input[512],input2[512];
        for(int j=0;j<512;j++){
            input[j]=Bit(false,PROVER);//randomness all zero
        }
        input[0]=key[i];
        sha.compute(com,input,input2);
        Bit sbit;
        if(s_key[i]=='1'){
            sbit=check(256,com,sha_1);
        }else{
            sbit=check(256,com,sha_0);
        }
        result = result & sbit;
    }


    return result;
}

int main(int argc, char** argv) {

	parse_party_and_port(argv, &party, &port);
	io = new NetIO(party==PROVER ? nullptr : "127.0.0.1", port);
	setup_arithmetic_zk(io, party);
    //if(party==VERIFIER)return 0;


    ECDSA::keygen();
    ECDSA::sign(); 

	if(!judge(io,party,NULL,swap)){
		error("failed");
		return 0;
	}
    std::cout << io->counter << std:: endl;
    puts("Yes");
   
 /*   CircuitExecution::circ_exec=new AriPlainEva();

    BristolFormat sha(sha_path);

    block out[256],in1[512],in2[512];
    memset(out,0,sizeof out);
    memset(in1,0,sizeof in1);
    memset(in2,0,sizeof in2);

    for(int i=0;i<512;i++){
        *((char *) &in1[i]) &= 0xfe;
       	*((char *) &in1[i]) |= 0;       
    }

    *((char *) &in1[0]) |= 1;

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
