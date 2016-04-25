#if !defined( PERSISTANTFORMATS_H )
#define PERSISTANTFORMATS_H
#pragma once
#include <afxtempl.h>
#include <cxcore.h>

class CNewVisionDoc;
class BodyPath;
class SwarmEvent;
class SwarmActivity;

class BodyPathMap : public CMap<int, int, BodyPath*, BodyPath* > {
public: 
	BodyPathMap( );
	BodyPathMap( const BodyPathMap &that );
	BodyPathMap& operator=( const BodyPathMap &that );
	BodyPath* GetRandomValue();
	BodyPath*& operator[](int key);
	void SetAt(int key, BodyPath* value);
	void RemoveKey(int id);
	void RemoveAll();
	void ChangeID(int id_old, int id_new);
	bool SwapID(int id1, int id2);
	int GetMaxID();
};
template <> void AFXAPI SerializeElements <BodyPath*> ( CArchive& ar, BodyPath** p, INT_PTR nCount );

class TrackingData {
public:
	CNewVisionDoc* doc;
	CString   m_sourcename;
	CTime     m_startTime;
	BodyPathMap data;
	bool m_initialized;

	// Validation data
	int m_added;     // number of bodies manually added 
	int m_deleted;   // number of bodies manually deleted 
	int m_swapped;   // number of manual ID swaps
	

	TrackingData();
	~TrackingData();
	void Initialize(CNewVisionDoc* doc);
	void DeInitialize();
	void Put(int nFrame);
	void Get(int nFrame);
	int GetLastRecordedFrame();
	int GetLastRecordedID();
	void Clear();
	int CountVisibleAt(int nFrame, int zoneID=-1);
	int CountPresentAt(int nFrame);
	bool ChangeID(int id_old, int id_new);
	bool MergeID(int id_master, int id_slave);
	bool AppendID(int id_master, int id_slave);
	bool SwapID(int id1, int id2);
	void ReNumberIDs();
	void ImportFromCVML(CString fname);
	void ExportToCVML(CString fname);
	void ExportToXML(CString fname);
	void ImportFromCSV(CString fname);
	void ExportToCSV(CString fname);
	void ExportToCSVStatic(CString fname);
	
	virtual void Serialize(CArchive& ar);
	void DrawFloor(IplImage* frame, CvScalar color);
	void DrawTracksFloor(IplImage* frame, CvScalar color);
	void DrawTimeTagsFloor(IplImage* frame, CvScalar color);
	void GenerateTrackHeatmap(REFERENCE_TIME start=0, REFERENCE_TIME stop=0, int segments = 1);
	void GenerateDwellHeatmap();
	// Methods with pop-up prompts
	void AutoMerge(int maxFrames, double maxDist, double minOtherDist);
	void EditInfoID(int id);
	void MergeID(int id);
	void AppendID(int id);
	void ChangeID(int id);
	void DeleteID(int id);
	void ShiftTime(int frames);
	void ExportToX3D(CString fname);
	void ExportToX3DStatic(CString fname);
	void ExportToX3DClusteredTraffic(CString fname);
};

class HeatmapData {
public:
	bool m_initialized;
	CString   m_sourcename;
	CTime     m_startTime;
	CNewVisionDoc* doc;
	IplImage *map, *maptemp, *count, *counttemp;

	HeatmapData();
	~HeatmapData();
	void Initialize(CNewVisionDoc* doc);
	void DeInitialize();
	void Put(int nFrame);
	void Get(int nFrame);
	void ImportFromCVML(CString fname);
	void ExportToCVML(CString fname);
	void ImportFromCSV(CString fname);
	void ExportToCSV(CString fname);
	void ExportToX3D(CString fname, bool useCount = true);
	void ExportToX3DStatic(CString fname, double scale=1, bool useCount = true);
	virtual void Serialize(CArchive& ar);
	void DrawHeatMap(IplImage* frame, bool useCount = true, bool useColorTable = false, double maxAll=0);
};

class ActivityData {
public:
	bool m_initialized;
	CNewVisionDoc* doc;
	// event/activity history
	CMap<int, int, SwarmEvent*, SwarmEvent* > sEvent;
	CMap<int, int, SwarmActivity*, SwarmActivity* > sActivity;

	void 			Initialize(CNewVisionDoc* doc);
	void			DeInitialize();
	void 			Put(int nFrame);
	void 			Get(int nFrame);
	void 			Clear();
	void 			ImportFromCVML(CString fname);
	void 			ExportToCVML(CString fname);
	void 			ImportFromCSV(CString fname);
	void 			ExportToCSV(CString fname);
	int  			GetLastEventID();
	int  			GetLastActivityID();
	virtual void	Serialize(CArchive& ar);
};

#endif // !defined( PERSISTANTFORMATS_H )