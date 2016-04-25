#if !defined( Node2DPoint_H )
#define Node2DPoint_H
#pragma once
#include <afxtempl.h>
#include "ClusteringMachine.h"

class Node2DPoint : public INode {
	DECLARE_DYNAMIC( Node2DPoint )
	public:
		static int last_id;
		// Attributes
		double x, y;   // x,y coordinates
		Node2DPoint ();
		Node2DPoint (double x, double y);
		double Spread();
		void Add(Node2DPoint *t);

		// INode overrides
		virtual void Serialize(CArchive& ar) {};
		virtual double	DistanceSQR(INode *that);	
		System::String^ ToString();
};
#endif // !defined( Node2DPoint_H )