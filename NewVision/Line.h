#pragma once
#include <afxtempl.h>
// ---------- CLine class -------------------
class CLine : public CObject {
	DECLARE_SERIAL(CLine);
	public:
		CPoint p1, p2;
		CLine();
		CLine& operator=( const CLine &c );
		CLine( const CLine &c );
		CLine(CPoint p1, CPoint p2);
};

template <> void AFXAPI SerializeElements <CLine> ( CArchive& ar, CLine* l, INT_PTR nCount );