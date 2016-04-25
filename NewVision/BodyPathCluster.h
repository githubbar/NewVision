#if !defined( BODYPATHCLUSTER_H )
#define BODYPATHCLUSTER_H
#pragma once
#include <afxtempl.h>
#include "cv.h"

class BodyPath;
class Step;

class BodyPathCluster
{
public:
	CArray<BodyPath*> path;
	int id;
	int first, last; // first and last frame

	BodyPathCluster(void);
	BodyPathCluster(BodyPath* path);
	void AddCluster(BodyPathCluster* path);
	void Add(BodyPath* path);
	void GetCenter(CArray<Step>& center);
	virtual ~BodyPathCluster(void);
	double Compare(BodyPathCluster* that, int mature_path, int delta, int step);
	bool IsInCluster(int id);
};
#endif // !defined( BODYPATHCLUSTER_H )