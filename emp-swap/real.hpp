
inline Real Real::operator+(const Real& rhs) const {
	return a2r(num+rhs.num,true);
}
inline Real Real::operator-(const Real& rhs) const {
	return a2r(num-rhs.num,true);
}
inline Real Real::select(Bit bit,Real rhs)const{
    return a2r(num.select(bit,rhs.num),true);
}


inline Real a2r(Number n,bool semantic=false){
    Real r;
    if(semantic)
        r.num=n;
    else
        r.num=n<<EXP;
    return r;
}

inline Real Real::operator*(const Real& rhs) const{
	return a2r(num*rhs.num>>EXP,true);
}
inline Real Real::operator/(const Real& rhs) const{
	return a2r((num<<EXP)/rhs.num,true);
}
inline Real Real::operator-() const{
	return a2r(-num,true);
}

inline Bit Real::operator==(const Real& rhs) const{
	return num==rhs.num;
}

inline Bit Real::operator!=(const Real &rhs) const{
	return num!=rhs.num;
}

inline Bit Real::operator<(const Real &rhs) const{
	return num<rhs.num;
}

inline Bit Real::operator<=(const Real &rhs) const{
	return !(*this>rhs);
}

inline Bit Real::operator>(const Real &rhs) const{
	return rhs<*this;
}

inline Bit Real::operator>=(const Real &rhs) const{
	return !(*this<rhs);
}
inline Real Real::sqrt()const{
	return a2r(num.sqrt()<<(EXP/2),true);
}


inline Number Real::ceil()const{
	Real t=*this - a2r(1,true);
	Number one(BITLENGTH,1,PUBLIC);
	return t.floor()+one;
}

inline Number Real::floor()const{
	return num>>EXP;
}

inline Number Real::round()const{
	Real t=*this + 0.5;
	return t.floor();
}