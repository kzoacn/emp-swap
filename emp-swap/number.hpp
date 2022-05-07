
Number::Number(int length, long long input, int party = PROVER){
        Integer t(length,input,party);  
        Number c=b2a(t); 
		val = BN_new();
		mask = BN_new();
		BN_copy(val,c.val);
		BN_copy(mask,c.mask);
}
Number::Number(const long long &x){
        Integer t(BITLENGTH,x,PUBLIC);
        Number c=b2a(t);
		val = BN_new();
		mask = BN_new();
		BN_copy(val,c.val);
		BN_copy(mask,c.mask);
}
inline Number Number::operator+(const Number& rhs) const {
	Number res;
	ArithmeticExecution::ari_exec->add_gate(res,*this,rhs);
	return res;
}
inline Number Number::operator-(const Number& rhs) const {
	Number res;
	ArithmeticExecution::ari_exec->sub_gate(res,*this,rhs);
	return res;
}

inline Number b2a(Integer x){
	return ArithmeticExecution::ari_exec->b2a_gate(x);
}

inline Number Number::select(Bit bit,Number rhs)const{
	Number res;
	ArithmeticExecution::ari_exec->sel_gate(res,bit,rhs,*this);
	return res;
}

inline void selects(int length,Number *c,const Bit *bit,Number *a,Number *b){
	 
	ArithmeticExecution::ari_exec->sels_gate(length,c,bit,a,b);
	
}
inline Number& Number::operator=(const Number& rhs){
	BN_copy(val,rhs.val);
	BN_copy(mask,rhs.mask);
	return *this;
}

inline Number Number::operator*(const Integer& rhs) const{
	Number res,t=*this;
	Number c[BITLENGTH];
	Number t2k[BITLENGTH];
	for(int i=0;i<rhs.size();i++){
		t2k[i]=t;
		t=t+t;
	}
	//t2k[rhs.size()-1]=-t2k[rhs.size()-1];

	selects(rhs.size(),c,rhs.bits.data(),t2k,c);

	for(int i=0;i<rhs.size();i++)
		res=res+c[i];

	return res;
}

inline Number Number::operator-() const{
	Number zero;
	return zero-*this;
}
 
inline void Number::assert_eq(const Number& rhs) const{
	ArithmeticExecution::ari_exec->eq(*this,rhs);
}