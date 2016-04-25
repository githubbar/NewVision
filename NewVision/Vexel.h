
/*!
 * 
 * Copyright : Alex Leykin 2007
 *
 *	Email : alexleykin@gmail.com
 *
 *	This code may be used in compiled form in any way you desire. This
 *	file may be redistributed unmodified by any means PROVIDING it is 
 *	not sold for profit without the authors written consent, and 
 *	providing that this notice and the authors name is included.
 *
 *	Description  
 *  ----------- 
*/

#if !defined( VEXEL_H )
#define VEXEL_H
#pragma once
#include <afxtempl.h>
#include "cv.h"
#include "Vexel.h"
#include "ClusteringMachine.h"

class CNewVisionDoc;
class FloorRegion;

// --------------------------------------------------------------------------
class Vexel : public INode 
{
	DECLARE_DYNAMIC( Vexel )
public:
	static int	last_id;
	CvPoint2D32f p; // position
	CvPoint2D32f v; // velocity

	Vexel(void);
	Vexel(const Vexel& that);
	Vexel& operator=(const Vexel& that);
	
	virtual ~Vexel(void);

	// INode overrides
	virtual double	DistanceSQR(INode *that);	
	System::String^ ToString();
	virtual void	Serialize(CArchive& ar) {};
};
// --------------------------------------------------------------------------
#endif // !defined( VEXEL_H )