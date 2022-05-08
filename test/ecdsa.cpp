#include <typeinfo>
#include <emp-tool/emp-tool.h>
#include "emp-swap/emp-swap.h"
using namespace std;
using namespace emp;


int port, party;
NetIO * io;

EC_POINT *PK;
BIGNUM *sk;
EC_POINT *P; //kG
BIGNUM *r,*s;

void keygen(){
    sk=BN_new();
    BN_set_word(sk,123456);
    PK=EC_POINT_new(GROUP);
    EC_POINT_mul(GROUP,PK,sk,NULL,NULL,CTX);
}

void sign(){
    BIGNUM *z,*k,*tmp,*kinv;
    z=BN_new();
    k=BN_new();
    r=BN_new();
    s=BN_new();
    tmp=BN_new();
    kinv=BN_new();

    BN_set_word(z,123);
    BN_set_word(k,124);

    P=EC_POINT_new(GROUP);
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
}

void veify(){
    BIGNUM *z,*tmp,*sinv,*u1,*u2;
    z=BN_new();
    tmp=BN_new();
    sinv=BN_new();
    u1=BN_new();
    u2=BN_new();

    BN_set_word(z,123);
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

Bit ecdsa(void* ctx){ 
    bool bits[256]; 
    BIGNUM *sinv=BN_new(),*z=BN_new();
    BN_mod_inverse(sinv,s,MOD,CTX);

    bn2bool(sinv,bits);
    Number num_sinv(BITLENGTH,bits,PROVER);

    BN_set_word(z,123);
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



int main(int argc, char** argv) {
	parse_party_and_port(argv, &party, &port);
	io = new NetIO(party==PROVER ? nullptr : "127.0.0.1", port);

	setup_arithmetic_zk(io, party);
 
    keygen();
    sign();
    veify();


	if(!judge(io,party,NULL,ecdsa)){
        error("failed");
		return 0;
	} 
	puts("Yes");


    


	delete io;
}
