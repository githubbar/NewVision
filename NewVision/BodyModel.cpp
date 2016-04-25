#include "StdAfx.h"
#include "BodyModel.h"
#include "NewVisionDoc.h"
#include "Globals.h"
#include "SwarmActivityModel.h"
#include "BodyActivityModel.h"
#include "SwarmActivity.h"
#include "Blob.h"
#include "BodyPath.h"
#include "BodyPathCluster.h"

#using <mscorlib.dll>
#using <System.dll>
using namespace System;
using namespace System::Diagnostics;

IMPLEMENT_SERIAL(BodyModel, Model, 1)
double pbg =0, pfg=0;
// --------------------------------------------------------------------------
BodyModel::BodyModel(void) {
	m_initialized = false;
	m_histUpdateFrequency = 5;
}
// --------------------------------------------------------------------------
BodyModel::~BodyModel(void) {
}
// --------------------------------------------------------------------------
bool BodyModel::Initialize() {
	if (m_initialized)
		DeInitialize();
	cvZero(zbuffer);
	cvZero(body_zbuffer);
	last_id = 0;
	m_initialized = true;

	return true;
}
// --------------------------------------------------------------------------
void BodyModel::DeInitialize() {
	if (!m_initialized)
		return;
	m_initialized = false;
	body.RemoveAll();
	body_deleted.RemoveAll();
}
// --------------------------------------------------------------------------
void BodyModel::Predict() {
	// update floor occupancy map
	UpdateFBuffer(fbuffer, NULL, doc->obstaclemodel.obstacle, F_OBSTACLES_ONLY);
	// search the solution space
	Iterate();
}
// --------------------------------------------------------------------------
bool BodyModel::IsOverlapping(Body* b, BodyMap& bmap) {
	for (POSITION pos = bmap.GetStartPosition();pos;) {
		int i; Body *b2;
		bmap.GetNextAssoc(pos, i, b2);
		if (b->id == b2->id)
			continue;
		if (b->IsOverlapping(b2))
			return false;
	}
	return true;
}
// --------------------------------------------------------------------------
void BodyModel::UpdateHistograms(BodyMap& bmap) {
	// update body histograms
	for (POSITION pos = bmap.GetStartPosition();pos;) {
		int i; Body *b;
		bmap.GetNextAssoc(pos, i, b);
		if (!IsOverlapping(b, bmap))
			b->HistogramUpdate(imgFrame);	
	}
}
// --------------------------------------------------------------------------
void BodyModel::RefreshHistograms(BodyMap& bmap) {
	// update body histograms
	for (POSITION pos = bmap.GetStartPosition();pos;) {
		int i; Body *b;
		bmap.GetNextAssoc(pos, i, b);
		LABHistogram2D obsColor;
		b->HistogramCompute(imgFrame, obsColor.h, 0); 
		b->dHist = obsColor.Compare(&b->color);
		b->color = obsColor;
		//b->HistogramCompute(imgFrame, b->color.h, 0/*do not accumulate values*/);
	}
}
// --------------------------------------------------------------------------
void BodyModel::RefreshOpticalFlow(BodyMap& bmap) {
	// update body histograms
	for (POSITION pos = bmap.GetStartPosition();pos;) {
		int i; Body *b;
		bmap.GetNextAssoc(pos, i, b);
		b->m_dFlow = b->GetImageOpticalFlow(imgFrame);
		// set to the current flow for the first time
		if (b->m_dAbsAvgFlow.x == 0) {
			b->m_dAbsAvgFlow.x = b->m_dFlow.x;
			b->m_dAbsAvgFlow.y = b->m_dFlow.y;
		}
		else {
			b->m_dAbsAvgFlow.x = b->m_dAbsAvgFlow.x*(1-FLOW_UPDATE_SPEED) + abs(b->m_dFlow.x)*FLOW_UPDATE_SPEED;
			b->m_dAbsAvgFlow.y = b->m_dAbsAvgFlow.y*(1-FLOW_UPDATE_SPEED) + abs(b->m_dFlow.y)*FLOW_UPDATE_SPEED;
		}
	}
}
// --------------------------------------------------------------------------
/**
Update z-buffer: 0 = BG, 1...N - from farthest to the closest object
*
* \param z 
* \param body 
* \param obstacle 
* \param mode 
*/
void BodyModel::UpdateZBuffer(IplImage* z, BodyMap& bmap, SmartPtrArray<Object3D>& obstacle, Z_MODE mode) {
	// combine bodies and obstacles in a single array
	int s = bmap.GetCount()+doc->obstaclemodel.obstacle.GetCount();

	// add bodies 
	Object3D** all = new Object3D*[s];
	
	int i1=0;
	for (POSITION pos = bmap.GetStartPosition();pos;) {
		int id; Body *b;
		bmap.GetNextAssoc(pos, id, b);
		all[i1] = b;
		i1++;
	}

	// add obstacles
	for (int i2=0; i2 < doc->obstaclemodel.obstacle.GetCount();i2++)
		all[i2+i1] = doc->obstaclemodel.obstacle[i2];

	// sort the elements
	for (int i=0;i<s;i++)
		for (int j=1;j<s-i;j++) {
			if (all[j-1]->Compare(all[j]) > 0) {
				Object3D* temp = all[j-1];
				all[j-1] = all[j];
				all[j] = temp;
			}
		}

	// update zbuffer: 0 = BG, 1...N - from farthest to the closest object
	cvZero(z);
	for (int i=s-1;i >=0;i--) {
		switch (mode) {
			case BODIES_ONLY: {
				if (all[i]->IsKindOf(RUNTIME_CLASS( Body ))) {
					all[i]->depthZ = (s-i);
					all[i]->RasterizeFrame(z, cvScalar(s-i));
				}
				break;
			}
			case OBSTACLES_ONLY: {
				if (all[i]->IsKindOf(RUNTIME_CLASS( Obstacle ))) {
					all[i]->depthZ = (s-i);
					all[i]->RasterizeFrame(z, cvScalar(s-i));
				}
				break;
			}
			case BODIES_MINUS_OBSTACLES: {
				all[i]->depthZ = (s-i);
				//echo(gcnew String(prt->m_lpszClassName));
				//if (all[i]->GetRuntimeClass()->IsDerivedFrom(RUNTIME_CLASS( Obstacle ))) {
				if (all[i]->IsKindOf(RUNTIME_CLASS( Obstacle ))) {
					all[i]->RasterizeFrame(z, cvScalar(0));
				}
				else {
					all[i]->RasterizeFrame(z, cvScalar(s-i));
				}
				break;
			}
			case ALL: {
				all[i]->depthZ = (s-i);
				all[i]->RasterizeFrame(z, cvScalar(s-i));
			}
		}
	}
	delete [] all;
}
// --------------------------------------------------------------------------
void BodyModel::UpdateFBuffer(IplImage* floor, BodyMap* bmap, SmartPtrArray<Object3D>& obstacle, F_MODE mode) {
	// update fbuffer: 1=object present  0 = object absent
	cvZero(floor);
	switch (mode) {
		case F_BODIES_ONLY: {
			for (POSITION pos = bmap->GetStartPosition();pos;) {
				int id; Body *b;
				bmap->GetNextAssoc(pos, id, b);
				b->RasterizeFloor(floor, cvScalar(1));
			}
			break;
		}
		case F_OBSTACLES_ONLY: {
			doc->obstaclemodel.RasterizeFloor(floor, cvScalar(1));
			break;
		}
		case F_ALL: {
			for (POSITION pos = bmap->GetStartPosition();pos;) {
				int id; Body *b;
				bmap->GetNextAssoc(pos, id, b);
				b->RasterizeFloor(floor, cvScalar(1));
			}
			doc->obstaclemodel.RasterizeFloor(floor, cvScalar(1));
		}
	}
}							
							
// --------------------------------------------------------------------------
int BodyModel::GetRandomMutationNumber() {
		double random = cvRandReal(&rnd_seed);
		int choice = 0;
		double P1 = 0, P2 = p_diffuse;
		if (random > P1 && random < P2)
			choice = 1;
		P1 = P2;
		P2 += p_update;
		if (random > P1 && random < P2)
			choice = 2;
		P1 = P2;
		P2 += p_exchange;
		if (random > P1 && random < P2)
			choice = 3;
		P1 = P2;
		P2 += p_remove;
		if (random > P1 && random < P2)
			choice = 4;
		P1 = P2;
		P2 += p_add;
		if (random > P1 && random < P2)
			choice = 5;
		
		return choice;
}
// --------------------------------------------------------------------------
void BodyModel::Iterate() {
	// don't bother if nothing is in the scene
	if (!doc->blobmodel.blob.GetCount() && !body.GetCount())
		return;

	// generate foreground and background images
	cvZero(fg);
	doc->blobmodel.Rasterize(fg, cvScalar(1));
	cvSet(bg, cvScalar(1));
	doc->blobmodel.Rasterize(bg, cvScalar(0));

	// update histograms of the accepted model
	if (m_UseColor) {
		UpdateHistograms(body); 
		doc->bodyactivitymodel.UpdateSalesRepHist();
	}

	// update Kalman filter state and generate predicted state
	for (POSITION pos = body.GetStartPosition();pos;) {
		int i; Body *b;
		body.GetNextAssoc(pos, i, b);
		b->KalmanObserve();
		b->KalmanPredict();
		b->bot_prev = b->bot;

		//b->predicted = b->bot;
	}

	// remove the bodies completely empty of FG pixels for prolonged periods of time
	RemoveEmpty(body);
	
	// remove bodies outside of interest region
	if (doc->doormodel.m_doorType == DOORS_INTEREST_REGION || doc->doormodel.m_doorType == DOORS_INTEREST_BOUNDARY)
		RemoveOutsiders(body);

	// create default candidate model 
	BodyMap accepted = body;

	//echo(String::Concat(__box(accepted[0]->bot.x), S", ", __box(accepted[0]->bot.y)));
	// add a body to each completely unoccupied blob and reflect changes in Z-buffer
	//NewForEachBlob(accepted);

/*
	// Apply mean-shift to all bodies
	for (POSITION pos = accepted.GetStartPosition();pos;) {
		int i; Body *b;
		accepted.GetNextAssoc(pos, i, b);
		b->MoveTo(b->MeanShift(imgFrame, fg));
		//CvPoint3D32f center = doc->cameramodel.coordsImage2Real(b->MeanShiftImage(imgFrame, fg), (b->top.z+b->bot.z)/2);
		//b->MoveTo(cvPoint3D32f(center.x, center.y, b->bot.z));
	}
*/
	// sort bodies based on the camera proximity (depth) and update Z-buffer (do not include obstacles!)
	UpdateZBuffer(body_zbuffer, accepted, doc->obstaclemodel.obstacle, BODIES_MINUS_OBSTACLES);

	// compute default model likelihood
	double p_accepted = Prior(accepted, body)*Likelyhood(accepted);	

	// create proposed candidate model 
	BodyMap proposed = accepted;

	int nFailedMutations = 0;
	// apply jump/diffusion dynamics to "shake" the candidate model
	int last_id_before = last_id;
	for (int nIter=0;nIter<m_iterations+accepted.GetCount()*m_iterationsPerBody && nFailedMutations < 10*m_iterations; nIter++) {
		int choice = GetRandomMutationNumber();
		bool result = false;
		switch (choice) {
			case M_DIFFUSE: 
				result = MutateDiffuse(proposed); 
				break;
			case M_MOVE: 
				result = MutateMove(proposed); 
				break;
			case M_EXCHANGE:
				result = MutateExchange(proposed); 
				break;
			case M_DELETE: 
				result = MutateDelete(proposed); 
				break;
			case M_NEW: 
				result = MutateNew(proposed);
				;
		}
		if (!result) {
			nFailedMutations++;
			nIter--;
			continue;
		}

		// sort proposed bodies based on the camera proximity (depth) and update Z-buffer (do not include obstacles!)
		UpdateZBuffer(body_zbuffer, proposed, doc->obstaclemodel.obstacle, BODIES_MINUS_OBSTACLES);

		// compute model likelihood
		double pr = this->Prior(proposed, body);
		double li = this->Likelyhood(proposed);
		double p_proposed = pr*li;
		
		// Metropolis-Hastings assuming symmetric transition probability density
		
		// probabilistically accept/reject the candidate model
		double pRandom = 2*m_randomness*cvRandReal(&rnd_seed)-m_randomness;
		if (p_proposed > p_accepted) {
			accepted = proposed;
			p_accepted = p_proposed;
		}
		else {
			proposed = accepted;
			UpdateZBuffer(body_zbuffer, accepted, doc->obstaclemodel.obstacle, BODIES_MINUS_OBSTACLES);
		}
	}
	// store accepted model as the current model
	UpdateRecentlyDeleted(accepted, body, body_deleted, (int) doc->trackermodel.m_frameNumber);
	ReplaceNewWithRecentlyDeleted(accepted, body, body_deleted, m_maxDeletedAge, m_maxDeletedDist);
	RemoveEmpty(accepted);
	body = accepted;

	ReorderIDs(last_id_before, body);
	proposed.RemoveAll();
	accepted.RemoveAll();
}
// --------------------------------------------------------------------------
void BodyModel::PredictOrientation() {
	// sort bodies based on the camera proximity (depth) and update Z-buffer (do not include obstacles!)
	UpdateZBuffer(body_zbuffer, body, doc->obstaclemodel.obstacle, BODIES_MINUS_OBSTACLES);
	
	// update histograms of the accepted model: do not accumulate
	if (m_UseColor) {
		RefreshHistograms(body); 
	}
	// update Kalman filter state and generate predicted state
	for (POSITION pos = body.GetStartPosition();pos;) {
		int i; Body *b;
		body.GetNextAssoc(pos, i, b);
		b->KalmanObserve();
		b->KalmanPredict();
	}
}
// --------------------------------------------------------------------------
double BodyModel::Prior(BodyMap& arrCurr, BodyMap& arrPrev) {
	double pModel= 1;

	double allpixels = doc->cameramodel.w*doc->cameramodel.h;	

	for (POSITION pos = arrCurr.GetStartPosition();pos;) {
		int i; Body *b;
		arrCurr.GetNextAssoc(pos, i, b);
		double p = 1; // prior on the body parameters

		// disallow the out of bounds bodies!
		CvRect area = cvRect(0,0,doc->cameramodel.w, doc->cameramodel.h);
		if (doc->cameramodel.m_cameraType == PROJECTION && !cvPointInRect(cvPointFrom32f(b->GetImageHead()), area) 
			&& !cvPointInRect(cvPointFrom32f(b->GetImageFoot()), area)  
			&& !cvPointInRect(cvPointFrom32f(b->GetImageCenter()), area))
			p *= 0;
		// body size prior
		b->pWidth = probNormalTruncated(b->GetWidth(), m_widthM, m_widthS, m_minWidth, m_maxWidth, p_abnormalW);;
		b->pHeight = probNormalTruncated(b->GetHeight(), m_heightM, m_heightS, m_minHeight, m_maxHeight, p_abnormalH);
		p *= b->pWidth;
		p *= b->pHeight;
		// punish for overlaps with the obstacles 
		if (doc->floormodel.InBounds(b->bot)) {
			CvPoint f_bot = doc->floormodel.coordsReal2Floor(b->bot);
			if ((byte)fbuffer->imageData[f_bot.y*fbuffer->widthStep+f_bot.x] == 1)
				p *= exp(-p_ObstacleOverlap);
		}

		// punish for overlaps with other bodies
		double pBodyOverlap = 1;
		for (POSITION pos2 = arrCurr.GetStartPosition();pos2;) {
			int j; Body *b2;
			arrCurr.GetNextAssoc(pos2, j, b2);
			if (j==i)
				continue;
			double d_body = d(b->bot, b2->bot);
			double d_norm = m_BodyMinDist*(b->GetWidth()+b2->GetWidth());
			if (d_body < d_norm) {
				if (d_body == 0)
					pBodyOverlap = 0;
				else
					pBodyOverlap = exp(-p_BodyOverlap*d_norm/d_body);
			}
			b->pBodyOverlap = pBodyOverlap;
			p *= pBodyOverlap;
		}

		// ------ Temporal priors ------
		Body* b_old;
		if (arrPrev.Lookup(b->id, b_old)) {
			// === computation confidence depends on the age of the body (the duration of track)
			int age = (int)doc->trackermodel.m_frameNumber - b->startFrame;
			double dCamera = b->GetDistanceFromCamera();
			//double confidence = (1-exp(-1.0*age/m_saturationTime));
			double confidence = 1;
			
			// === distance from the Kalman predicted location
			double distDeviation = m_distanceConfidence*(dCamera/doc->cameramodel.m_minBodyDistance);
			double pPosition = probNormalTruncated(d(b_old->predicted, b->bot), 0, distDeviation, 0, distDeviation, p_distance);
			//double pPosition = probNormalTruncated(d(b_old->bot, b->bot), 0, distDeviation, 0, distDeviation, p_distance);

			// === difference in width
			double pWidth = probNormalTruncated(b->GetWidth(), b_old->GetWidth(), m_widthS, b_old->GetWidth()-m_widthS, b_old->GetWidth()+m_widthS, p_width);

			// === difference in height
			double pHeight = probNormalTruncated(b->GetHeight(), b_old->GetHeight(), m_heightS, b_old->GetHeight()-m_heightS, b_old->GetHeight()+m_heightS, p_height);

			// === distance from the color histogram 
			double pColor = 1; 
			if (m_UseColor) {
				LABHistogram2D obsColor;
				b->HistogramCompute(imgFrame, obsColor.h, 0); // NOTE: b_old->color is the same as b->color at this point
				b->dHist = obsColor.Compare(&b_old->color);
				pColor = exp(-p_color*(1-b->dHist)); 
			}
			// === combine all priors
			p = p*(1-confidence)*1+ p*confidence
				* pPosition
				* pWidth
				* pHeight
				* pColor;
			b->pDoor = 1;
			b->pTempColor = pColor;
			b->pTempWidth = pWidth;
			b->pTempHeight = pHeight;
			b->pTempPosition = pPosition;
		}
		else { 
			// penalty for inserting a body 
			double dDoor = doc->doormodel.GetDistanceFromClosestDoor(b->bot);
			double pDoor = probNormalTruncated(dDoor, 0, m_maxDoorDistance, 0, m_maxDoorDistance, p_new);
			p *= pDoor;
			b->pDoor = pDoor;
		}
		b->confidence = p;
		pModel *= p;
	}
	
	// penalty for deleting bodies
	for (POSITION pos = arrPrev.GetStartPosition();pos;) {
		int i; Body *b;
		arrPrev.GetNextAssoc(pos, i, b);
		Body* b_new;
		if (!arrCurr.Lookup(b->id, b_new)) {
			double dDoor = doc->doormodel.GetDistanceFromClosestDoor(b->bot);
			double p = probNormalTruncated(dDoor, 0, m_maxDoorDistance, 0, m_maxDoorDistance, p_delete);
			pModel *= p;
		}
	}

	// punish for excessive number of bodies
	pModel *= exp(-p_number*(double)arrCurr.GetCount());

	return pModel;
}
// --------------------------------------------------------------------------
/**
 Computes over- and under-coverage.
 See "Tracking Multiple Humans in Crowded Environment" by Zhao, Tao and Nevatia, Ram
 account for the fact that objects closer to the camera use up more pixels
 BEWARE DIVISION BY ZERO. 
 *
 * \param bmap 
 * \return 
 */
double BodyModel::Likelyhood(BodyMap& bmap) {
	double PBG = 0, PFG = 0;
	
	double allpixels = doc->cameramodel.w*doc->cameramodel.h;

	cvConvert(fg, temp32);
	cvMul(temp32, distance_mask, temp32, 1.0/doc->cameramodel.m_minBodyDistance);
	double blobpixels = cvSum(temp32).val[0];
	if (blobpixels > 0) {
		cvThreshold(body_zbuffer, blob_mask, 0, 1, CV_THRESH_BINARY_INV);
		cvConvert(blob_mask, FGMapDouble);
		cvMul(temp32, FGMapDouble, ibg);	
		PBG = cvSum(ibg).val[0]/blobpixels;
	}
	
	cvConvert(bg, temp32);
	cvMul(temp32, distance_mask, temp32, 1.0/doc->cameramodel.m_minBodyDistance);
	double bodypixels = cvSum(temp32).val[0];
	if (bodypixels > 0) {
		cvThreshold(body_zbuffer, body_mask, 0, 1, CV_THRESH_BINARY);
		cvConvert(body_mask, FGMapDouble);
		cvMul(temp32, FGMapDouble, ifg);	
		PFG = cvSum(ifg).val[0]/bodypixels;
	}

	pbg = PBG;
	pfg = PFG;
	//doc->Message(String::Concat("FG = ", PFG, " BG = ", PBG));
	return exp(-(p_background*PBG+p_foreground*PFG));
}
// --------------------------------------------------------------------------
void BodyModel::RasterizeFrame(IplImage* frame, CvScalar color) {
	for (POSITION pos = body.GetStartPosition();pos;) {
		int i; Body *b;
		body.GetNextAssoc(pos, i, b);
		b->RasterizeFrame(frame, color);
	}
}
// --------------------------------------------------------------------------
void BodyModel::RasterizeNonDormant(IplImage* frame, CvScalar color) {
	for (POSITION pos = body.GetStartPosition();pos;) {
		int i; Body *b;
		body.GetNextAssoc(pos, i, b);
		if (b->dormantFor < m_dormantFor)
			b->RasterizeFrame(frame, color);
	}
}
// --------------------------------------------------------------------------
void BodyModel::RasterizeFloor(IplImage* frame, CvScalar color) {
	for (POSITION pos = body.GetStartPosition();pos;) {
		int i; Body *b;
		body.GetNextAssoc(pos, i, b);
		b->RasterizeFloor(frame, color);
	}
}
// --------------------------------------------------------------------------
void BodyModel::DrawBrowsingMarks( IplImage* frame, CvScalar color )
{
	
	for (POSITION pos = body.GetStartPosition();pos;) {
		int i; Body *b;
		body.GetNextAssoc(pos, i, b);
		if (d(b->bot, b->bot_prev) < m_maxPosChange && b->m_dAbsAvgFlow.x > MIN_X_FLOW*b->GetImageAxes().width ) 
			b->DrawDwellMark(frame, color);
	}
}
// --------------------------------------------------------------------------
void BodyModel::DrawFrame(IplImage* frame, CvScalar color) {
	for (POSITION pos = body.GetStartPosition();pos;) {
		int i; Body *b;
		body.GetNextAssoc(pos, i, b);
		// set the color to RED for bodies outside the interest region
		CvPoint2D32f foot = b->GetImageFoot();
		if (!doc->doormodel.IsInsideInterestRegion(foot))
			b->DrawFrame(frame, CV_RGB(255, 0, 0));
		else 
			b->DrawFrame(frame, color);
	}
}
// --------------------------------------------------------------------------
void BodyModel::DrawFrame( CDC* pDC , int ox, int oy, CPen* pen )
{
	CPen *penRed = new CPen(PS_SOLID, 1, RGB(255,0,0));
	for (POSITION pos = body.GetStartPosition();pos;) {
		int i; Body *b;
		body.GetNextAssoc(pos, i, b);
		// set the color to RED for bodies outside the interest region
		CvPoint2D32f foot = b->GetImageFoot();
		if (!doc->doormodel.IsInsideInterestRegion(foot))
			b->DrawFrame(pDC, ox, oy, penRed);
		else 
			b->DrawFrame(pDC, ox, oy, pen);
	}
	delete penRed;
}
// --------------------------------------------------------------------------
void BodyModel::DrawColorizingConfidence(IplImage* frame, CvScalar colorYes, CvScalar colorNo) {
	double CONFIDENCE_SHIFT = 0.9;
	double CONFIDENCE_STRETCH = 1.0/(1-CONFIDENCE_SHIFT);
	for (POSITION pos = body.GetStartPosition();pos;) {
		int i; Body *b;
		body.GetNextAssoc(pos, i, b);
		// set the color to colorNo for bodies with confidence=0
		BodyPath *bp;
		if (doc->m_TrackingData.data.Lookup(b->id, bp)) {
			double confidence = bp->isManual ? bp->manualConfidence : bp->autoConfidence;
			double c = (confidence-CONFIDENCE_SHIFT)*CONFIDENCE_STRETCH;
			CvScalar color, colorFG = CV_RGB(255,0,0), colorBG = CV_RGB(255,255,255);
			if (confidence == 0) {
				color = CV_RGB(0,0,255);
				if (bp->isManual) {
					colorFG = CV_RGB(255,0,0); colorBG = CV_RGB(64,0,0);
				}

			}
			else
				color = CV_RGB(colorYes.val[2]*c+colorNo.val[2]*(1-c), 
									colorYes.val[1]*c+colorNo.val[1]*(1-c), 
									colorYes.val[0]*c+colorNo.val[0]*(1-c));
			b->DrawFrameLabel(frame, colorFG, colorBG);
		}
	}
}
// --------------------------------------------------------------------------
void BodyModel::DrawFloor(IplImage* frame, CvScalar color) {
	for (POSITION pos = body.GetStartPosition();pos;) {
		int i; Body *b;
		body.GetNextAssoc(pos, i, b);
		b->DrawFloor(frame, color);
	}
}
// --------------------------------------------------------------------------
void BodyModel::DrawHeight(IplImage* frame, CvScalar color) {
	for (POSITION pos = body.GetStartPosition();pos;) {
		int i; Body *b;
		body.GetNextAssoc(pos, i, b);
		b->DrawHeight(frame, color);
	}
}
// --------------------------------------------------------------------------
void BodyModel::DrawInfo(IplImage* frame, CvScalar color) {
	for (POSITION pos = body_deleted.GetStartPosition();pos;) {
		int i; Body *b;
		body.GetNextAssoc(pos, i, b);
		b->DrawInfo(frame, CV_RGB(255,0,0));
	}
	for (POSITION pos = body.GetStartPosition();pos;) {
		int i; Body *b;
		body.GetNextAssoc(pos, i, b);
		b->DrawInfo(frame, color);
	}
}
// --------------------------------------------------------------------------
void BodyModel::DrawGroups(IplImage* frame, CvScalar color) {
	
	for (POSITION pos = body.GetStartPosition();pos;) {
		int bid; Body *b;
		body.GetNextAssoc(pos, bid, b);
		for (POSITION posG = doc->m_ActivityData.sActivity.GetStartPosition(); posG;) {
			int aid; SwarmActivity *ac; double d;
			doc->m_ActivityData.sActivity.GetNextAssoc(posG, aid, ac);
			// if this body is in the group draw label with a different color
			if (ac->actors.Lookup(bid, d)) {
				b->DrawFrameLabel(frame, CV_RGB(0,0,0), colorFromID(aid), 1.2f);
				break;
			}
		}
	}
}
// --------------------------------------------------------------------------
void BodyModel::DrawWeightMasks(IplImage* frame) {
	for (POSITION pos = body.GetStartPosition();pos;) {
		int i; Body *b;
		body.GetNextAssoc(pos, i, b);
		b->DrawWeightMask(frame);
	}
}
// --------------------------------------------------------------------------
void BodyModel::DrawZBuffer(IplImage* frame) {
	double min_val, max_val;
	cvMinMaxLoc(body_zbuffer, &min_val, &max_val);
	cvScale(body_zbuffer, temp, 255.0/max_val);
	cvCvtColor(temp, frame, CV_GRAY2RGB);
	//double min_val, max_val;
	//cvMinMaxLoc(temp_bg, &min_val, &max_val);
	//cvScale(temp_bg, temp, 255.0/max_val);
	//cvCvtColor(temp, frame, CV_GRAY2RGB);
}
// --------------------------------------------------------------------------
void BodyModel::DrawHistograms(IplImage* frame) {
	int scale = 4;
	int i=0;
	for (POSITION pos = body.GetStartPosition();pos;) {
		int id; Body *b;
		body.GetNextAssoc(pos, id, b);
		int x = i*(LABHistogram2D::a_bins*scale+10) % (frame->width-LABHistogram2D::a_bins*scale);
		int y = i*(LABHistogram2D::a_bins*scale+10) / (frame->width-LABHistogram2D::a_bins*scale);
		b->DrawHistogram(frame, scale, cvPoint(x, y));
		i++;
	}
	//double min_val, max_val;
	//cvMinMaxLoc(ifg, &min_val, &max_val);

	//cvScale(ifg, temp, 255.0/max_val);
	//cvCvtColor(temp, frame, CV_GRAY2RGB);
}

void BodyModel::DrawMotionOrientation( IplImage* srcFrame, IplImage* tgtFrame )
{
	for (POSITION pos = body.GetStartPosition();pos;) {
		int id; Body *b;
		body.GetNextAssoc(pos, id, b);
		//b->DrawHeadArea(tgtFrame, CV_RGB(255,0,0));
		//b->DrawHeadCenter(tgtFrame, CV_RGB(255,0,0));
		//b->DrawFaceCenter(tgtFrame, CV_RGB(0,255,100));
		b->DrawSectorHighlight(srcFrame, tgtFrame);
	}

}

// --------------------------------------------------------------------------
// --------------------------------------------------------------------------
// --------------------------------------------------------------------------
Body* BodyModel::FindBodyMatch(BodyMap& bmap, CvPoint3D32f bot, int R) {
	// return the first body within the radius R
	for (POSITION pos = bmap.GetStartPosition();pos;) {
		int i; Body *b;
		bmap.GetNextAssoc(pos, i, b);
		if (d(bot, b->bot) < R)
			return b;
	}
	return NULL;
}
// --------------------------------------------------------------------------
Body* BodyModel::FindBodyMatch(BodyMap& bmap, CvPoint2D32f p, int R) {
	// return the first body within the radius R
	CvPoint3D32f pBot = doc->cameramodel.coordsImage2Real(p, 0);		
	return FindBodyMatch(bmap, pBot, R);
}
// --------------------------------------------------------------------------
Body* BodyModel::FindBodyMatchByAgeAndDistance(BodyMap& bmap, Body* b, int d_age, double R) {
	// return the first body within the 'd_age' age difference and  radius 'R'
	for (POSITION pos = bmap.GetStartPosition();pos;) {
		int i; Body *b2;
		bmap.GetNextAssoc(pos, i, b2);
		if ( b->startFrame - b2->stopFrame < d_age
			 && d(b->bot, b2->bot) < R )
				return b2;
	}
	return NULL;
}
// --------------------------------------------------------------------------
Body* BodyModel::FindBodyMatchTop(BodyMap& bmap, CvPoint2D32f p, int R) {
	// return the first body within the radius R
	for (POSITION pos = bmap.GetStartPosition();pos;) {
		int i; Body *b;
		bmap.GetNextAssoc(pos, i, b);
		CvPoint3D32f pTop = doc->cameramodel.coordsImage2Real(p, b->top.z);
		if (d(pTop, b->top) < R)
			return b;
	}
	return NULL;
}
// --------------------------------------------------------------------------
int BodyModel::FindBodyIDTop(BodyMap& bmap, CvPoint2D32f p, int R) {
	// return the first body within the radius R
	for (POSITION pos = bmap.GetStartPosition();pos;) {
		int i; Body *b;
		bmap.GetNextAssoc(pos, i, b);
		CvPoint3D32f pTop = doc->cameramodel.coordsImage2Real(p, b->top.z);
		if (d(pTop, b->top) < R)
			return b->id;
	}
	return -1;
}
// --------------------------------------------------------------------------
int BodyModel::FindBodyIDTag(BodyMap& bmap, CvPoint2D32f p) {
	// return the first body match
	const OFFSET = 3;
	for (POSITION pos = bmap.GetStartPosition();pos;) {
		int i; Body *b;
		bmap.GetNextAssoc(pos, i, b);
		CvRect r; b->GetTagRect(r);
		if (cvPointInRect(cvPointFrom32f(p), r))
			return b->id;
	}
	return -1;
}

// --------------------------------------------------------------------------
void BodyModel::ReorderIDs(int last_id_before, BodyMap& bmap) {
	for (POSITION pos = bmap.GetStartPosition();pos;) {
		int i; Body *b;
		bmap.GetNextAssoc(pos, i, b);
		b->stopFrame = (int)doc->trackermodel.m_frameNumber;
		if (b->id >= last_id_before) {
			bmap.ChangeID(b->id, last_id_before);
			last_id_before++;
		}
	}
	last_id = last_id_before;
}

