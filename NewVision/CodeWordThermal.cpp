#include "StdAfx.h"
#include "CodeWordThermal.h"

IMPLEMENT_SERIAL( CodeWordThermal, CObject, 1 )
// --------------------------------------------------------------------------
CodeWordThermal::CodeWordThermal(void) {
	this->i_min = 0;
	this->i_max = 0;
	this->age = 0;
	this->stale = 0;
	this->first = 0;
	this->last = 0;
}
// --------------------------------------------------------------------------
CodeWordThermal::CodeWordThermal(int p, int frame) {
	this->i_min = p;
	this->i_max = p;
	this->age = 0;
	this->stale = 0;
	this->first = frame;
	this->last = frame;
}
// --------------------------------------------------------------------------
CodeWordThermal::CodeWordThermal(const CodeWordThermal& c) {
	this->i_min = c.i_min;
	this->i_max = c.i_max;
	this->age = c.age;
	this->stale = c.stale;
	this->first = c.first;
	this->last = c.last;
}
// --------------------------------------------------------------------------
CodeWordThermal& CodeWordThermal::operator=( const CodeWordThermal &c )  { 
	this->i_min = c.i_min;
	this->i_max = c.i_max;
	this->age = c.age;
	this->stale = c.stale;
	this->first = c.first;
	this->last = c.last;
	return *this; 
}
// --------------------------------------------------------------------------
double CodeWordThermal::MatchProbabilistic(int p, int frame, int m_ThermalMin, int m_ThermalMax, int minAge) {
	//if (p < alpha*i_max || p > min(beta*i_max, i_min/alpha)) 
	//	return 1;

	// if not in range [i_min-m_ThermalMin :  i_max+m_ThermalMax)]
	if (p < i_min-m_ThermalMin || p > i_max+m_ThermalMax)
		return 1;
	else
		return abs((double)(p-(i_min-m_ThermalMin + i_max+m_ThermalMax)/2))/(i_max+m_ThermalMax-(i_min-m_ThermalMin) );
}
// --------------------------------------------------------------------------
bool CodeWordThermal::MatchByFactor(int p, int frame, double alpha, double beta, int minAge) {
	// brightness distortion
	if (p < alpha*i_max || p > min(beta*i_max, i_min/alpha))
		return false;

	return true;
}
// --------------------------------------------------------------------------
bool CodeWordThermal::MatchByDeviation(int p, int frame, int m_ThermalMin, int m_ThermalMax, int minAge) {
	// brightness distortion
	const int MAX_RANGE = 50; // maximum range of the codeword (e.g. 50 degrees of temperature)
	if (p < i_min-m_ThermalMin || p > i_max+m_ThermalMax || p-i_min > MAX_RANGE || i_max-p > MAX_RANGE)
		return false;

	return true;
}
// --------------------------------------------------------------------------
void CodeWordThermal::Update(int p, int frame) {	
	i_min = min(p, i_min);
	i_max = max(p, i_max);
	age = age + 1;
	// reset stale counter
	stale = 0; 
	//if (frame != -1) {
	//	stale = max(stale, frame - last);
	//	last = frame;
	//}
}
// --------------------------------------------------------------------------
void CodeWordThermal::Serialize( CArchive& archive ) {
	CObject::Serialize( archive );
	if( archive.IsStoring() )
	    archive << i_min << i_max << age << stale << first << last;
	else 
		archive >> i_min >> i_max >> age >> stale >> first >> last;
}
// --------------------------------------------------------------------------
template <> void AFXAPI SerializeElements <CodeWordThermal> ( CArchive& ar, CodeWordThermal* p, INT_PTR nCount )
{
    for ( int i = 0; i < nCount; i++, p++ )
        p->Serialize( ar );
}