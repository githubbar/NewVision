#if !defined( BODYMODEL_H )
#define BODYMODEL_H

#pragma once
#include <afx.h>
#include "cv.h"
#include "Model.h"
#include "SmartPtrArray.h"
#include "LABHistogram2D.h"
#include "Globals.h"
#include "Body.h"


class BodyPath;
class SwarmActivityModel;
class Object3D;

enum Z_MODE {BODIES_ONLY=0, OBSTACLES_ONLY=1, BODIES_MINUS_OBSTACLES=2, ALL=3};
enum F_MODE {F_BODIES_ONLY=0, F_OBSTACLES_ONLY=1, F_ALL=3};
enum MUTATION {M_DIFFUSE=1, M_MOVE=2, M_EXCHANGE=3, M_DELETE=4, M_NEW=5};

class BodyMap : public CMap<int, int, Body*, Body* > {
public: 
	BodyMap( ) {
	}

	BodyMap( const BodyMap &that ) {
		POSITION pos = that.GetStartPosition();
		while (pos) {
			int i; Body *b;
			that.GetNextAssoc(pos, i, b);
			this->SetAt(i, new Body(*b));
		}
	}

	BodyMap& operator=( const BodyMap &that )  { 
		RemoveAll();
		POSITION pos = that.GetStartPosition();
		while (pos) {
			int i; Body *b;
			that.GetNextAssoc(pos, i, b);
			this->SetAt(i, new Body(*b));
		}
		return *this;
	}
	
	Body* GetRandomValue() {
		int i_random = cvRandInt(&rnd_seed) % GetCount(); 
		POSITION pos = GetStartPosition();
		int i=0;
		while (pos) {
			int id; Body *b;
			GetNextAssoc(pos, id, b);
			if (i == i_random)
				return b;
			i++;
		}
		return NULL;
	}

	(Body*)& operator[](int key) {
		Body *b;
		if (Lookup(key, b))
			delete b;
		return CMap<int, int, Body*, Body*>::operator[](key);
	}

	void SetAt(int key, Body* value) {
		Body *b;
		if (Lookup(key, b))
			delete b;
		CMap<int, int, Body*, Body*>::SetAt(key, value);
	}

	void RemoveKey(int id) {
		Body *b;
		if (Lookup(id, b)) {
			delete b;
			CMap<int, int, Body*, Body*>::RemoveKey(id);
		}
	}

	void RemoveAll() {
		POSITION pos = GetStartPosition();
		while (pos) {
			int id; Body *b;
			GetNextAssoc(pos, id, b);
			delete b;
		}
		CMap<int, int, Body*, Body*>::RemoveAll();
	}

	void ChangeID(int id_old, int id_new) {
		if (id_old == id_new)
			return;

		Body *b_old, *b_new;
		if (Lookup(id_old, b_old)) {
			if (Lookup(id_new, b_new))
				RemoveKey(id_new);
			b_new = new Body(*b_old);
			b_new->id = id_new;
			SetAt(id_new, b_new);
			RemoveKey(id_old);
		}
	}

	bool SwapID(int id1, int id2) {
		if (id1 == id2)
			return true;

		Body *b1, *b2;
		if (!Lookup(id1, b1) || !Lookup(id2, b2))
			return false;
		Body *b1copy = new Body(*b1);
		Body *b2copy = new Body(*b2);
		RemoveKey(id1);
		RemoveKey(id2);
		b1copy->id = id2;
		b2copy->id = id1;
		SetAt(id1, b2copy);
		SetAt(id2, b1copy);
		return true;
	}
};
// ================================================================================================
class BodyModel : public Model
{
	DECLARE_SERIAL(BodyModel)
public:
	// --------------- Persistant variables -----------------------
	bool	m_initialized;
	int 	m_heightM;
	int 	m_heightS;
	int 	m_minHeight;
	int 	m_maxHeight;
	int 	m_widthM;
	int 	m_widthS;
	int 	m_minWidth;
	int 	m_maxWidth;
	int 	m_iterations;
	int 	m_iterationsPerBody;
	int 	m_invisibleFor;// number of frames not to have any underlying blobs to be considered for deletion
	int 	m_dormantFor;  // number of frames not to have any motion or color change to be considered for deletion
	int 	m_outsiderFor; // number of frames not to have any underlying blobs to be considered for deletion
	double	m_distanceConfidence;	// accuracy (cm) while performing distance related operations
	int		m_saturationTime;		// time (in frames) to start trusting sizes and locations
	double	m_maxHistChange;		// maximum percetage change in color histogram to be deemed as dormant
	double	m_maxPosChange;			// maximum change in position (cm) to be deemed as dormant
	int		m_maxDeletedAge;		// maximum age of deleted bodies to recover (also the time to keep in body_deleted array)
	double	m_maxDeletedDist;		// maximum distance of deleted bodies from new candidates to recover
	int		m_histUpdateFrequency;

	// tracking parameters
	double	m_randomness;
	double	m_maxDoorDistance; // maximum distance from the door to perform add/delete mutations
	int		last_id;
	int		last_group_id;
	
	BodyMap body;
	BodyMap body_deleted;

	// prior probability penalties
	double 	p_BodyOverlap; // penalty for overlapping with other bodies
	double 	m_BodyMinDist; // minimal distance to other body to consider an overlap
	double 	p_ObstacleOverlap; // penalty for overlapping with obstacles 
	double 	p_number; // probability of no objects present in the scene
	double 	p_distance; // penalty for the distance from the Kalman predicted position 
	double 	p_height;
	double 	p_width;
	double 	p_new; // probability of creating a new body (should descrese further from doors)
	double 	p_delete; // probability of deleting an old body (should descrese further from doors)
	double 	p_color, p_background, p_foreground; // color consistency, object attraction and BG exclustion relative weights
	double 	p_abnormalW; // penalty for deviation from the mean width
	double 	p_abnormalH; // penalty for deviation from the mean height
	int		m_UseColor;
	// jump-diffusion probabilities
	double 	p_add;
	double 	p_remove;
	double 	p_exchange;
	double 	p_update;
	double 	p_diffuse;
	
	// Kalman parameters
	double 	m_posModelNoise;
	double 	m_posMeasurementNoise;
	double 	m_orientModelNoise;
	double 	m_orientMeasurementNoise;

	BodyModel(void);
	bool Initialize();
	void DeInitialize();
	bool IsInitialized() { return m_initialized; };
	virtual ~BodyModel(void);
	
	bool			IsOverlapping(Body* b, BodyMap& bmap);
	void 			UpdateHistograms(BodyMap& body);
	void 			RefreshHistograms(BodyMap& body);
	void 			RefreshOpticalFlow(BodyMap& body);
	void 			UpdateZBuffer(IplImage* z, BodyMap& body, SmartPtrArray<Object3D>& obstacle, Z_MODE mode=ALL);
	void 			UpdateFBuffer(IplImage* f, BodyMap* body, SmartPtrArray<Object3D>& obstacle, F_MODE mode=F_ALL);
	void 			Predict();
	void			PredictOrientation();
	void 			Iterate();
	void 			RasterizeFrame(IplImage* frame, CvScalar color=cvScalar(1));
	void 			RasterizeFloor(IplImage* frame, CvScalar color=cvScalar(1));
	void 			RasterizeNonDormant(IplImage* frame, CvScalar color=cvScalar(1));
	
	void			DrawFrame(IplImage* frame, CvScalar color);
	void			DrawFrame(CDC* pDC , int ox, int oy, CPen* pen);
	void			DrawBrowsingMarks(IplImage* frame, CvScalar color);
	void			DrawColorizingConfidence(IplImage* frame, CvScalar colorYes, CvScalar colorNo);
	void			DrawFloor(IplImage* frame, CvScalar color);
	void			DrawHeight(IplImage* frame, CvScalar color);
	void			DrawInfo(IplImage* frame, CvScalar color);
	void			DrawGroups(IplImage* frame, CvScalar color);
	void			DrawWeightMasks(IplImage* frame);
	void			DrawZBuffer(IplImage* frame);
	void			DrawHistograms(IplImage* frame);
	void			DrawMotionOrientation(IplImage* srcFrame, IplImage* tgtFrame);

	double			Prior(BodyMap& arrCurr, BodyMap& arrPrev);
	double			Likelyhood(BodyMap& arr);

	// --------------------------------------------------------------------------
	Body* 			FindBodyMatch(BodyMap& arr, CvPoint3D32f bot, int R=5);
	Body* 			FindBodyMatch(BodyMap& arr, CvPoint2D32f bot, int R=5);
	Body* 			FindBodyMatchByAgeAndDistance(BodyMap& arr, Body* b, int d_age=10, double R=5);
	Body* 			FindBodyMatchTop(BodyMap& arr, CvPoint2D32f top, int R=5);
	int 			FindBodyIDTop(BodyMap& arr, CvPoint2D32f top, int R=5);
	int 			FindBodyIDTag(BodyMap& arr, CvPoint2D32f p);
	void			UpdateRecentlyDeleted(BodyMap& arr_after, BodyMap& arr_before, BodyMap& arr_deleted, int frame);
	void			ReplaceNewWithRecentlyDeleted(BodyMap& arr_after, BodyMap& arr_before, BodyMap& arr_deleted, int delta_age, double delta_distance);
	void			ReorderIDs(int last_id_before, BodyMap& arr);


	// Body model mutations
	int				GetRandomMutationNumber();
	bool 			MutateNew(BodyMap& proposed);
	bool 			MutateDiffuse(BodyMap& proposed);
	bool 			MutateMove(BodyMap& proposed);
	bool 			MutateExchange(BodyMap& proposed);
	bool 			MutateDelete(BodyMap& proposed);

	void 			RemoveEmpty(BodyMap& accepted);
	void 			RemoveOutsiders(BodyMap& accepted);
	void 			NewForEachBlob(BodyMap& accepted);
};
#endif // !defined( BODYMODEL_H )