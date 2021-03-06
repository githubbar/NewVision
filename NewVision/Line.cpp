#include "stdafx.h"
#include "line.h"

IMPLEMENT_SERIAL(CLine, CObject, 1)
// ------------------------------------------------------------------------
CLine::CLine() {
}
// ------------------------------------------------------------------------
CLine& CLine::operator=( const CLine &c )  { 
	this->p1 = c.p1;
	this->p2 = c.p2;
	return *this; 
}
// ------------------------------------------------------------------------
CLine::CLine( const CLine &c )  { 
	this->p1 = c.p1;
	this->p2 = c.p2;
}
// ------------------------------------------------------------------------
CLine::CLine(CPoint p1, CPoint p2) {
	this->p1 = p1;
	this->p2 = p2;
}
// ------------------------------------------------------------------------


