#if !defined( SWARMACTIVITY_H )
#define SWARMACTIVITY_H
#pragma once

#include "ClusteringMachine.h"
class SwarmEvent;
class TrackingData;
class SwarmActivity : public IFuzzyCluster {
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
						SwarmActivity(CArray<INode*> &node);
		virtual int		EqualActors(SwarmActivity *that);
		virtual void	Merge(SwarmActivity *that);
				void	Normalize();
				void	UpdateStartStop(TrackingData *tr);
				void	FilterActors( double m_minActorWeight );
		System::String^ ToString();

		// IFuzzyCluster overrides
		virtual void	Serialize(CArchive& ar);
		virtual void	Add(INode* that);				// add element
	
};
template <> void AFXAPI SerializeElements <SwarmActivity*> ( CArchive& ar, SwarmActivity** p, INT_PTR nCount );
#endif // !defined( SWARMACTIVITY_H )