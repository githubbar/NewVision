/*!
 * Tracklet - one visible track segment; contains an array of body coordinates, width, visibility and confidence information, one record per frame. 
 * Additionally stores the header with body id, tags and start frame
 *
 * Invisible records can be disregarded thus letting Tracklet store multiple segments of a broken path
 * 
 * Copyright (c) 2005 by Alex Leykin
*/

#if !defined( TRACKLET_H )
#define TRACKLET_H
#pragma once
#include <afxtempl.h>
#include "cv.h"
#include "Tracklet.h"
#include "ClusteringMachine.h"

class CNewVisionDoc;
class FloorRegion;

// --------------------------------------------------------------------------
class Tracklet : public CArray < CvPoint2D32f >, public INode 
{
public:
	static int	last_id;
	int			startFrame;			

	Tracklet(void);
	Tracklet(const Tracklet& that);
	Tracklet& operator=(const Tracklet& that);
	
	virtual ~Tracklet(void);

	// INode overrides
	virtual double	DistanceSQR(INode *that);	
	System::String^ ToString();
	virtual void	Serialize(CArchive& ar) {};
};
// --------------------------------------------------------------------------
#endif // !defined( TRACKLET_H )