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
 *  BODYCLUSTER - represents bodies grouped, based on some proximity measure
 *  Has most basic traits of a body, like position, orientation, etc. 
 *  These represent the centroid of the cluster.
 *  
 *  Has an array of elements 'sub' which are essentially just bodies. 
 *  Note: this thing is not hierarchical or recursive: i.e. only only level of sub-clusters
*/


#if !defined( BODYCLUSTER_H )
#define BODYCLUSTER_H 
#pragma once

#include "cv.h"    
#include "Body.h"  
#include "SmartPtrArray.h"
#include "SwarmActivityDetectors.h"

class BodyClusterElement {
	public:
		// Attributes
		int id;                     // body ID
		CvPoint3D32f   bot;         // floor position (x and y)
		double         orientation; // orientation angle [0 = looking along X-axis; increasing counter-clockwise to PI, decreasing clockwise to -PI]
		double         dwell;       // dwell state (from 0 to 1)
		BodyClusterElement();
		~BodyClusterElement();
		BodyClusterElement(const BodyClusterElement& that);
		BodyClusterElement& operator=(const BodyClusterElement& that);
		BodyClusterElement(Body *b);
		BodyClusterElement(int id,	CvPoint3D32f bot, double orientation, double dwell);
};

class BodyCluster : public BodyClusterElement {
	public:
		// Attributes
		SmartPtrArray<BodyClusterElement> sub;		// an array of bodies contained in this cluster

		BodyCluster(int id,	CvPoint3D32f bot, double orientation, double dwell);
		BodyCluster(const BodyCluster& that);
		BodyCluster(BodyClusterElement* el);
		BodyCluster& operator=(const BodyCluster& that);
		BodyCluster();
		~BodyCluster();
		BodyCluster(Body *b);
		BodyClusterElement* Find(int id);
		bool In(int id);
		void Add(Body *b);
		void Add(BodyCluster *b);
		void Add(BodyClusterElement *b);
		void Clear();

		static double ComputeDistance(SmartPtrArray<BodyCluster>& cl, SwarmEventDetector* comp);
};

#endif // !defined( BODYCLUSTER_H )