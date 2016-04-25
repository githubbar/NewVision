#if !defined( SWARMACTIVITY_H )
#define SWARMACTIVITY_H
#pragma once

#include "ClusteringMachine.h"
class SwarmEvent;
class SwarmActivity : public ICluster {
	public:
		static int last_id;
		// Attributes
		CString type;   // type of activityImg (GROUPING, LEADER FOLLOWING, LINING UP, CO-DWELLING, etc)
		int start;		// frame activity started 
		int stop;		// frame activity stopped
		double p;		// activity likelihood/confidence
		CMap<int, int, double, double> actors;			 // [actors participation in this activity :: confidence of belonging to this activity]
		CMap<int, int, double, double> events;			 // [events contributing to this activity :: confidence of belonging to this activity]

		SwarmActivity();
		~SwarmActivity();

		SwarmActivity(CArray<INode*> &node);
		virtual void	Serialize(CArchive& ar);
		virtual bool	EqualActors(SwarmActivity *that);
		virtual void	AddActors(SwarmActivity *that);
		System::String^ ToString();

		// ICluster overrides
		virtual double	Distance(INode *that);	
		virtual void	Add(INode *el);
};
template <> void AFXAPI SerializeElements <SwarmActivity*> ( CArchive& ar, SwarmActivity** p, INT_PTR nCount );
#endif // !defined( SWARMACTIVITY_H )