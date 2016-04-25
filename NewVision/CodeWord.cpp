#include "StdAfx.h"
#include "CodeWord.h"

IMPLEMENT_SERIAL( CodeWord, CObject, 1 )

CodeWord::CodeWord(void) {
	this->R = 0;
	this->G = 0;
	this->B = 0;
	this->i_min = 0;
	this->i_max = 0;
	this->age = 0;
	this->stale = 0;
	this->first = 0;
	this->last = 0;
}

CodeWord::CodeWord(Pixel* p, int frame) {
	this->R = p->R;
	this->G = p->G;
	this->B = p->B;
	this->i_min = p->R+p->G+p->B;
	this->i_max = p->R+p->G+p->B;
	this->age = 0;
	this->stale = 0;
	this->first = frame;
	this->last = frame;
}

CodeWord::CodeWord(const CodeWord& c) {
	this->R = c.R;
	this->G = c.G;
	this->B = c.B;
	this->i_min = c.i_min;
	this->i_max = c.i_max;
	this->age = c.age;
	this->stale = c.stale;
	this->first = c.first;
	this->last = c.last;
}

CodeWord& CodeWord::operator=( const CodeWord &c )  { 
	this->R = c.R;
	this->G = c.G;
	this->B = c.B;
	this->i_min = c.i_min;
	this->i_max = c.i_max;
	this->age = c.age;
	this->stale = c.stale;
	this->first = c.first;
	this->last = c.last;
	return *this; 
}

//
//char CodeWord::Match3(Pixel* p, int frame) {
//	double t_cd = 20, alpha = 0.7, beta = 1.2;
//	double t_cd3 = 30, alpha3 = 0.5, beta3 = 1.4;
//	
//	int i_2 = (p->R*p->R+p->G*p->G+p->B*p->B); 
//	int i = p->R+p->G+p->B;
//	double d = (R*p->R+G*p->G+B*p->B);
//	
//	
//	// background pixel
//	if (abs(i_2 - d*d/(R*R+G*G+B*B+1)) < t_cd*t_cd && i > alpha*i_max && i < min(beta*i_max, i_min/alpha))
//		return 0;
//
//	// possible foreground pixel
//	else if (abs(i_2 - d*d/(R*R+G*G+B*B+1)) < t_cd3*t_cd3 && i > alpha3*i_max && i < min(beta3*i_max, i_min/alpha3))
//		return 1;
//	// foreground pixel
//	
//	else 
//		return 2;
//}

bool CodeWord::Match(Pixel* p, int frame, double t_cd, double alpha, double beta, int minAge) {
	int i_2 = (p->R*p->R+p->G*p->G+p->B*p->B); 
	int i = p->R+p->G+p->B;
	double d = (R*p->R+G*p->G+B*p->B);
	// color distortion
	if (abs(i_2 - d*d/(R*R+G*G+B*B+1)) > t_cd*t_cd) 
		return false;
	
	// brightness distortion
	if (i < alpha*i_max || i > min(beta*i_max, i_min/alpha))
		return false;

	return true;
}

void CodeWord::Update(Pixel* p, int frame) {		
	R = (age*R+p->R)/(age+1);
	G = (age*G+p->G)/(age+1);
	B = (age*B+p->B)/(age+1);
	i_min = min(p->R+p->G+p->B, i_min);
	i_max = max(p->R+p->G+p->B, i_max);
	age = age + 1;
	stale = max(stale, frame - last);
	last = frame;
}

void CodeWord::Serialize( CArchive& archive ) {
	CObject::Serialize( archive );
	if( archive.IsStoring() )
	    archive << R << G << B << i_min << i_max << age << stale << first << last;
	else 
		archive >> R >> G >> B >> i_min >> i_max >> age >> stale >> first >> last;
}


template <> void AFXAPI SerializeElements <CodeWord> ( CArchive& ar, CodeWord* p, INT_PTR nCount )
{
    for ( int i = 0; i < nCount; i++, p++ )
        p->Serialize( ar );
}