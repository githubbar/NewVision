#if !defined( SWARMEVENT_H )
#define SWARMEVENT_H
#pragma once
#include <afxtempl.h>
#include "ClusteringMachine.h"

class SwarmEvent : public INode {
	DECLARE_DYNAMIC( SwarmEvent )
	public:
		static int last_id;
		// Attributes
		CString type;   // type of event (GROUPING, LEADER FOLLOWING, LINING UP, CO-DWELLING, etc)
		int start;  // frame event started 
		int stop;   // frame event stopped
		double p;   // event likelihood/confidence
		CMap<int, int, double, double> actors;			 // [participant IDs:confidence]
		SwarmEvent ();
		SwarmEvent (int f, CString t);
		void AddActor(int aid, double p);
		bool OccurredAt(int nFrame);
		int GetAverageFrameDistance(SwarmEvent *that);
		bool AreActorsUnique(SwarmEvent *that);
		virtual void Serialize(CArchive& ar);
		double Spread();
		void Add(SwarmEvent *t);

		// INode overrides
		virtual double	DistanceSQR(INode *that);	
		System::String^ ToString();
};
template <> void AFXAPI SerializeElements <SwarmEvent*> ( CArchive& ar, SwarmEvent** p, INT_PTR nCount );
#endif // !defined( SWARMEVENT_H )