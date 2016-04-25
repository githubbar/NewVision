#if !defined( X3DDRIVER_H )
#define X3DDRIVER_H
#pragma once

#include "x3d-3.1.hxx"
#include "xercesc/framework/LocalFileFormatTarget.hpp"
//#include "xercesc/framework/StdOutFormatTarget.hpp"

class X3DDriver {
public: 
	X3DDriver();
	~X3DDriver();
	void AddSound(CString fileName);
	void LoadX3D(CString fileName);
	void SaveX3D(CString fileName);
	void SetViewPoint();
	void ClearScene();
	void AddFloorImage(CString imageFile, CString minmaxExt);
	void AddObstacle(int id, CString center, CString size, CString rotation, CString label);
	void AddObstacle(int id, CString coords, CString label);
	void AddArrow(CString center, CString rotation, CString color, float bottomRadius, float height);
	void AddTrackAvatar(int id, CString urlAvatar, CString coords, CString ctimes, CString orients, CString otimes, CString tcoords, CString ttimes);
	void AddTrackCurve(int id, bool female, CString coords);
	void AddNurbsPatchSurface(CString coords, int udims, int vdims);
	void AddTimeSensor(CString name, CString duration);
private:
	std::auto_ptr<x3d::X3D> x;
	xml_schema::namespace_infomap map;
};
#endif // !defined( X3DDRIVER_H )