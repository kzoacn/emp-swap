
Number::Number(int length, long long input, int party = PROVER){
        Integer t(length,input,party);
        *this=b2a(t);
}
Number::Number(const long long &x){
        Integer t(BITLENGTH,x,PROVER);
        *this=b2a(t);
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
inline Integer a2b(int length,const Number &x){

	Integer res=ArithmeticExecution::ari_exec->a2b_gate(length,x);
	Number tmp=b2a(res);

	if(!ArithmeticExecution::ari_exec->eq(x,tmp))
		throw;

	return res;
}
inline Number Number::select(Bit bit,Number rhs)const{
	Number res;
	ArithmeticExecution::ari_exec->sel_gate(res,bit,rhs,*this);
	return res;
}

inline void selects(int length,Number *c,const Bit *bit,Number *a,Number *b){
	
	
	ArithmeticExecution::ari_exec->sels_gate(length,c,bit,a,b);
	
}


inline Number Number::operator*(const Number& rhs) const{
	Integer x=a2b(BITLENGTH,rhs);
	return (*this) * x;
}
inline Number Number::operator*(const Integer& rhs) const{
	Number res,t=*this;
	Number c[BITLENGTH];
	Number t2k[BITLENGTH];
	for(int i=0;i<rhs.size();i++){
		t2k[i]=t;
		t=t+t;
	}
	t2k[rhs.size()-1]=-t2k[rhs.size()-1];

	selects(rhs.size(),c,rhs.bits.data(),t2k,c);

	for(int i=0;i<rhs.size();i++)
		res=res+c[i];

	return res;
}

inline Number Number::operator-() const{
	Number zero;
	return zero-*this;
}

inline Bit Number::operator==(const Number& rhs) const{
	Integer t=a2b(BITLENGTH,*this-rhs);
	Integer zero(BITLENGTH,0,PUBLIC);
	return t==zero;
}

inline Bit Number::operator!=(const Number &rhs) const{
	Integer t=a2b(BITLENGTH,*this-rhs);
	Integer zero(BITLENGTH,0,PUBLIC);
	return t!=zero;
}

inline Bit Number::operator<(const Number &rhs) const{
	Integer x=a2b(BITLENGTH,*this);
	Integer y=a2b(BITLENGTH,rhs);
	return x<y;
}

inline Bit Number::operator<=(const Number &rhs) const{
	return !(*this>rhs);
}

inline Bit Number::operator>(const Number &rhs) const{
	return rhs<*this;
}

inline Bit Number::operator>=(const Number &rhs) const{
	return !(*this<rhs);
}


inline Number Number::operator>>(int shamt) const{
	Integer t=a2b(BITLENGTH,*this);
	Integer res=t;
	Bit sign=t[t.size()-1];
	if(shamt >t.size()) {
		for(int i = 0; i < t.size(); ++i)
			res.bits[i] = false;
	}
	else {
		for(int i = shamt; i < t.size(); ++i)
			res.bits[i-shamt] = t.bits[i];
		for(int i = t.size()-shamt; i < t.size(); ++i)
			res.bits[i] = sign;
	}
	return b2a(res);	
}
inline Number Number::operator<<(int shamt) const{
	Number t=*this;
	while(shamt-->0)
		t=t+t;
	return t;	
}

std::pair<Number,Number> Number::div(const Number &rhs)const{
	std::pair<Number,Number> res;

	long long a=get_val(val),b=get_val(rhs.val);
	long long _p=0,_q=0;
	if(b){
		_p=a/b;
		_q=a%b;
	}
	Number p(BITLENGTH,_p,PROVER);
	Number q(BITLENGTH,_q,PROVER);
	
	Number zero;
	ArithmeticExecution::ari_exec->is_true(zero<=q);
	ArithmeticExecution::ari_exec->is_true(q<rhs);
	Number t=p*rhs+q;
	ArithmeticExecution::ari_exec->eq(*this,t);

	return std::pair<Number,Number>(p,q);
}

inline Number Number::operator/(const Number &rhs) const{
	return div(rhs).first;
}


inline Number Number::operator%(const Number &rhs) const{
	return div(rhs).second;
}

inline Number Number::operator<<(const Number& shamt)const{
	Integer a=a2b(BITLENGTH,*this);
	Integer b=a2b(BITLENGTH,shamt);
	return b2a(a<<b);
}
inline Number Number::operator>>(const Number& shamt)const{
	Integer a=a2b(BITLENGTH,*this);
	Integer b=a2b(BITLENGTH,shamt);
	return b2a(a>>b);
}
inline Number Number::operator&(const Number& rhs)const{
	Integer a=a2b(BITLENGTH,*this);
	Integer b=a2b(BITLENGTH,rhs);
	return b2a(a&b);
}
inline Number Number::operator|(const Number& rhs)const{
	Integer a=a2b(BITLENGTH,*this);
	Integer b=a2b(BITLENGTH,rhs);
	return b2a(a|b);
}

inline Number Number::sqrt()const{
	long long _v=get_val(val);
	long long _sqrtv=0;
	if(_v>=0){
		_sqrtv=(long long)(::sqrt(_v)-1e-20);
		while((_sqrtv+1)*(_sqrtv+1)<=_v)
			_v++;
	}
	Number sqrtv=_sqrtv;
	Number one(BITLENGTH,1,PUBLIC);
	ArithmeticExecution::ari_exec->is_true(sqrtv>=0);
	ArithmeticExecution::ari_exec->is_true(sqrtv*sqrtv<= *this);
	ArithmeticExecution::ari_exec->is_true(*this<(sqrtv+one)*(sqrtv+one));
	return sqrtv;
}