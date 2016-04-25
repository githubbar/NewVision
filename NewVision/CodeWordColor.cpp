#include "StdAfx.h"
#include "CodeWordColor.h"

IMPLEMENT_SERIAL( CodeWordColor, CObject, 1 )
// --------------------------------------------------------------------------
CodeWordColor::CodeWordColor(void) {
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
// --------------------------------------------------------------------------
CodeWordColor::CodeWordColor(Pixel* p, int frame) {
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
// --------------------------------------------------------------------------
CodeWordColor::CodeWordColor(const CodeWordColor& c) {
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
// --------------------------------------------------------------------------
CodeWordColor& CodeWordColor::operator=( const CodeWordColor &c )  { 
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
// --------------------------------------------------------------------------
double CodeWordColor::MatchProbabilistic(Pixel* p, int frame, double t_cd, double alpha, double beta, int minAge) {
	int i_2 = (p->R*p->R+p->G*p->G+p->B*p->B); 
	int i = p->R+p->G+p->B;
	double d = (R*p->R+G*p->G+B*p->B);
	// color distortion
	double cd = sqrt(abs(i_2 - d*d/(R*R+G*G+B*B+1)));
	if (i < alpha*i_max || i > min(beta*i_max, i_min/alpha)) 
		return 1;
	else if (cd > t_cd)
		return 1;
	else
		return cd/t_cd;
}
// --------------------------------------------------------------------------
bool CodeWordColor::Match(Pixel* p, int frame, double t_cd, double alpha, double beta, int minAge) {
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
// --------------------------------------------------------------------------
void CodeWordColor::Update(Pixel* p, int frame) {		
	R = (age*R+p->R)/(age+1);
	G = (age*G+p->G)/(age+1);
	B = (age*B+p->B)/(age+1);
	i_min = min(p->R+p->G+p->B, i_min);
	i_max = max(p->R+p->G+p->B, i_max);
	age = age + 1;
	// reset stale counter
	stale = 0; 
	//if (frame != -1) {
	//	stale = max(stale, frame - last);
	//	last = frame;
	//}
}
// --------------------------------------------------------------------------
void CodeWordColor::Serialize( CArchive& archive ) {
	CObject::Serialize( archive );
	if( archive.IsStoring() )
	    archive << R << G << B << i_min << i_max << age << stale << first << last;
	else 
		archive >> R >> G >> B >> i_min >> i_max >> age >> stale >> first >> last;
}
// --------------------------------------------------------------------------
template <> void AFXAPI SerializeElements <CodeWordColor> ( CArchive& ar, CodeWordColor* p, INT_PTR nCount )
{
    for ( int i = 0; i < nCount; i++, p++ )
        p->Serialize( ar );
}