#include "StdAfx.h"
#include "BodyModel.h"
#include "NewVisionDoc.h"
#include "Globals.h"
#include "BodyActivityModel.h"
#include "BodyPathCluster.h"
#include "Body.h"
#include "Blob.h"
#using <mscorlib.dll>
#using <System.dll>
using namespace System;
using namespace System::Diagnostics;

// --------------------------------------------------------------------------
// Change a random body parameter
bool BodyModel::MutateDiffuse(BodyMap& proposed) {
	// fail if there are no bodies
	if (proposed.GetCount() == 0)
		return false;

	Body *b = proposed.GetRandomValue(); // choose one body randomly
	// choose one parameter
	if ((cvRandInt(&rnd_seed)) % 2 == 0) {
		b->SetHeight(b->GetHeight() + (2*cvRandReal(&rnd_seed)-1)*m_heightS);		
		b->SetHeight(min(b->GetHeight(), m_maxHeight));
		b->SetHeight(max(b->GetHeight(), m_minHeight));
	}
	else {
		//echo(String::Concat(S"shake w: "));
		b->SetWidth(b->GetWidth() + (2*cvRandReal(&rnd_seed)-1)*m_widthS);
		b->SetWidth(min(b->GetWidth(), m_maxWidth));
		b->SetWidth(max(b->GetWidth(), m_minWidth));
	}
	//ech(String::Concat(S"DIFFUSE: ", __box(proposed[i]->id)));
	return true;
}
// --------------------------------------------------------------------------
// Change position of a random body
bool BodyModel::MutateMove(BodyMap& proposed) {
	// fail if there are no bodies
	if (proposed.GetCount() == 0)
		return false;

	// update position of a random body
	Body *b = proposed.GetRandomValue(); // choose one body randomly
	CvPoint3D32f pos = b->GetFoot();
	int choice = cvRandInt(&rnd_seed) % 100;
	if (choice >= 0 && choice < 0) {
		// 1 - shift in the direction of gradient descent (P = 4/5)
		b->MoveTo(b->MeanShift(imgFrame, fg));
		//CvPoint3D32f center = doc->cameramodel.coordsImage2Real(b->MeanShiftImage(imgFrame, fg), (b->top.z+b->bot.z)/2);
		//b->MoveTo(cvPoint3D32f(center.x, center.y, b->bot.z));
		//doc->Message(String::Format("MOVE: {0} dx={1}, dy={2}", b->id, pos.x - b->bot.x, pos.y - b->bot.y));
	}
	else if (choice < 66) {
		// 2 - move a random displacement
		const OFFSET = 10;
		CvPoint3D32f pos_new = cvPoint3D32f(pos.x + (float)(2*m_distanceConfidence*cvRandReal(&rnd_seed)-m_distanceConfidence), 
											pos.y + (float)(2*m_distanceConfidence*cvRandReal(&rnd_seed)-m_distanceConfidence),
											pos.z);
		b->MoveTo(pos_new);
	}
	else {
		// 3 - move to the closest top candidate (P = 1/5)
		double min_dist = 1e10;
		BlobRay* min_ray = NULL;
		for (int blobi=0;blobi<doc->blobmodel.blob.GetCount();blobi++) {
			CvSeq* heads = doc->blobmodel.blob[blobi]->heads;
			for (int headi=0;headi<heads->total;headi++) {
				BlobRay* br = (BlobRay*)cvGetSeqElem(heads, headi);
				double dist = d(cvPointTo32f(br->p1), b->GetImageHead());
				if (dist < min_dist) {
					min_dist = dist;
					min_ray = br;
				}
			}
		}		
		if (!min_ray)
			return false;
		b->MoveTo(doc->cameramodel.coordsImage2Real(cvPointTo32f(min_ray->p2), 0));
	}
	return true;
}
// --------------------------------------------------------------------------
// Exchange two body positions
bool BodyModel::MutateExchange(BodyMap& proposed) {
	// fail if there are less than two bodies
	if (proposed.GetCount() < 2)
		return false;
	
	Body *b1 = proposed.GetRandomValue(), *b2;
	do {
		b2 = proposed.GetRandomValue();
	} while (b1->id == b2->id);

	CvPoint3D32f _top, _bot;
	_top = b1->top;
	_bot = b1->bot;
	b1->top = b2->top;
	b1->bot = b2->bot;
	b2->top = _top;
	b2->bot = _bot;
	return true;
}

// --------------------------------------------------------------------------
// Delete a random body
bool BodyModel::MutateDelete(BodyMap& proposed) {
	// fail if there are no bodies
	if (proposed.GetCount() == 0)
		return false;
	Body *b = proposed.GetRandomValue();
	if (doc->doormodel.GetDistanceFromClosestDoor(b->bot) > m_maxDoorDistance)
		return false;

	proposed.RemoveKey(b->id);
	return true;
}
// --------------------------------------------------------------------------
// Create a new body from a random top candidate of a random blob
bool BodyModel::MutateNew(BodyMap& proposed) {
	// fail if there are no blobs
	if (doc->blobmodel.blob.GetCount() == 0)
		return false;
	
	// get a list of head candidates
	int i = cvRandInt(&rnd_seed) % doc->blobmodel.blob.GetCount();
	CvSeq* heads = doc->blobmodel.blob[i]->heads;
	
	// fail if there are no head candidates
	if (heads->total == 0)
		return false;

	// pick a random head-foot candidate and compute its real coordinates
	CvPoint3D32f h3, f3;
	int j = cvRandInt(&rnd_seed) % heads->total;
	BlobRay* br = (BlobRay*)cvGetSeqElem(heads, j);
	doc->cameramodel.coordsImage2RealSameXY_Feet2Floor(cvPointTo32f(br->p1), cvPointTo32f(br->p2), &h3, &f3);
	double height = h3.z-f3.z;
	CvPoint center = cvPoint((br->p1.x+br->p2.x)/2, (br->p1.y+br->p2.y)/2);

	// if initial candidate is longer that two bodies => use either upper or lower part
	/*
	if (height > m_maxHeight) {
		double random = 2*cvRandReal(&rnd_seed)-1;
		double height_new = m_heightM+m_heightS*random;
		double dx = (br->p1.x-br->p2.x)*(height_new/height);
		double dy = (br->p1.y-br->p2.y)*(height_new/height);
		if ((cvRandInt(&rnd_seed)) % 2 == 0) {
			br->p1.x = (int)(br->p2.x + dx);
			br->p1.y = (int)(br->p2.y + dy);
		}
		else {
			br->p2.x = (int)(br->p1.x - dx);
			br->p2.y = (int)(br->p1.y - dy);
		}
	}
	*/

	// if initial candidate is to short, but not tiny :-) => expand the body around it's center
	if (height < m_minHeight && height > m_minHeight/2) { 
		double random = 2*cvRandReal(&rnd_seed)-1;
		double height_new = m_heightM+m_heightS*random;
		br->p1.x = (int)(center.x - (center.x-br->p1.x)*(height_new/height));
		br->p1.y = (int)(center.y - (center.y-br->p1.y)*(height_new/height));
		br->p2.x = (int)(center.x - (center.x-br->p2.x)*(height_new/height));
		br->p2.y = (int)(center.y - (center.y-br->p2.y)*(height_new/height));
	}

	// reject very small blob candidates
	if (height < m_minHeight/2)
		return false;

	doc->cameramodel.coordsImage2RealSameXY_Feet2Floor(cvPointTo32f(br->p1), cvPointTo32f(br->p2), &h3, &f3);

	// fail if body is too far from the doors
	if (doc->doormodel.GetDistanceFromClosestDoor(f3) > m_maxDoorDistance)
		return false;

	// add new body(s) to the proposed body model
	Body *b = new Body(br, doc, last_id, (int)doc->trackermodel.m_frameNumber);
	// TEMP: use the same width and height for all
	//Body *b = new Body(cvPoint2D32f(f3.x, f3.y), m_heightM, m_widthM, doc, last_id, (int)doc->trackermodel.m_frameNumber);

	//Body* match = FindBodyMatchByAgeAndDistance(body_deleted, b, m_maxDeletedAge, m_maxDeletedDist); // find a match in recently deleted array
	//if (match) {
	//	//doc->Message(String::Format("Recovered {0}", match->id));
	//	b->id = match->id;
	//	body_deleted.RemoveKey(match->id);
	//}
	proposed[b->id] = b;
	last_id++;

	CvPoint2D32f imfoot = doc->cameramodel.coordsReal2Image(b->GetFoot());
	CvPoint2D32f imhead = doc->cameramodel.coordsReal2Image(b->GetHead());
	CvPoint2D32f imcenter = doc->cameramodel.coordsReal2Image(b->GetCenter());
	return true;
}
// --------------------------------------------------------------------------
// Removes any bodies that were without a blob for a long time
void BodyModel::RemoveEmpty(BodyMap& accepted) {
	
	for (POSITION pos = accepted.GetStartPosition();pos;) {
		// check for dormant every 5 frames for speed
		if (doc->trackermodel.m_frameNumber % m_histUpdateFrequency)
			break;
		
		int i; Body *b;
		accepted.GetNextAssoc(pos, i, b);

		// update histograms and dHist before removing empty
		for (POSITION pos = accepted.GetStartPosition();pos;) {
			int i; Body *b_now, *b_old;
			accepted.GetNextAssoc(pos, i, b_now);
			LABHistogram2D color_now, color_old;
			b_now->HistogramCompute(imgFrame, color_now.h, 0);
			b_now->HistogramCompute(imgPreviousFrame, color_old.h, 0);
			b_now->dHist = color_now.Compare(&color_old);
			//b_now->dHist = cvCompareHist(color_now.h, color_old.h, CV_COMP_BHATTACHARYYA );
		}

		if ((1-b->dHist) < m_maxHistChange && d(b->bot, b->predicted) < m_maxPosChange)
			b->dormantFor++;
		else
			b->dormantFor = 0;

	}
	
	for (POSITION pos = accepted.GetStartPosition();pos;) {
		int i; Body *b;
		accepted.GetNextAssoc(pos, i, b);
		if (b->invisibleFor > m_invisibleFor || b->dormantFor > m_dormantFor) {
			cvZero(temp_bg);
			b->RasterizeFrame(temp_bg, cvScalar(1));
			cvAnd(fg, temp_bg, temp_bg);
			double pixFG = cvSum(temp).val[0];
			if (pixFG == 0) 
				b->invisibleFor++;
			else 
				b->invisibleFor = 0;

			// update corresponding BG region
			doc->bgmodel.Update(-1, temp_bg);
			// remove a dormant/ghost body 
			accepted.RemoveKey(i);
		}

	}
}
// --------------------------------------------------------------------------
// Removes any bodies that are outside of the interest region
void BodyModel::RemoveOutsiders(BodyMap& accepted) {
	for (POSITION pos = accepted.GetStartPosition();pos;) {
		int i; Body *b;
		accepted.GetNextAssoc(pos, i, b);
		CvPoint2D32f foot = b->GetImageFoot();
		if (foot.y >= doc->cameramodel.h || foot.x >= doc->cameramodel.w) {
			b->outsiderFor++;
			continue;
		}
		if (!doc->doormodel.IsInsideInterestRegion(foot))
			b->outsiderFor++;
		else
			b->outsiderFor = 0;
		
		if (b->outsiderFor > m_outsiderFor)
			accepted.RemoveKey(i);
	}
}
// --------------------------------------------------------------------------
// Add a body to each completely unoccupied blob and reflect changes in Z-buffer
void BodyModel::NewForEachBlob(BodyMap& accepted) {
	for (int i=0;i<doc->blobmodel.blob.GetCount();i++) {
		cvZero(fg);
		doc->blobmodel.blob[i]->Rasterize(fg, cvScalar(1));
		cvThreshold(body_zbuffer, temp, 0, 1, CV_THRESH_BINARY);
		cvAnd(fg, temp, temp);
		if (cvSum(temp).val[0] == 0) {
			CvSeq* heads = doc->blobmodel.blob[i]->heads;
			if (heads->total > 0) {
				// get the best (tallest) head candidate
				BlobRay* br = (BlobRay*)cvGetSeqElem(heads, 0);

				// check if inside the door interest region
				CvScalar p = cvGet2D(door_mask, (int)br->p2.y, (int)br->p2.x);
				if (p.val[0] == 0)
					continue;
				
				double height = doc->cameramodel.computeHeight(cvPointTo32f(br->p1), cvPointTo32f(br->p2));
				
				// reject very small blobs
				if (height < m_minHeight/2)
					continue;

				if (height < m_minHeight) { 
					CvPoint center = cvPoint((br->p1.x+br->p2.x)/2, (br->p1.y+br->p2.y)/2);
					double random = 2*cvRandReal(&rnd_seed)-1;
					double height_new = m_heightM+m_heightS*random;
					br->p1.x = (int)(center.x - (center.x-br->p1.x)*(height_new/height));
					br->p1.y = (int)(center.y - (center.y-br->p1.y)*(height_new/height));
					br->p2.x = (int)(center.x - (center.x-br->p2.x)*(height_new/height));
					br->p2.y = (int)(center.y - (center.y-br->p2.y)*(height_new/height));
				}
				accepted[last_id] = new Body(br, doc, ++last_id, (int)doc->trackermodel.m_frameNumber);
				UpdateZBuffer(body_zbuffer, accepted, doc->obstaclemodel.obstacle, BODIES_MINUS_OBSTACLES);
			}
		}
	}
}
// --------------------------------------------------------------------------
void BodyModel::UpdateRecentlyDeleted(BodyMap& arr_after, BodyMap& arr_before, BodyMap& arr_deleted, int frame) {
	
	// clean up arr_deleted from old bodies
	for (POSITION pos = arr_deleted.GetStartPosition();pos;) {
		int i; Body *b;
		arr_deleted.GetNextAssoc(pos, i, b);
		if (b->stopFrame < frame-m_maxDeletedAge)
			arr_deleted.RemoveKey(i);
	}
	
	// fill up arr_deleted with the newly deleted bodies
	for (POSITION pos = arr_before.GetStartPosition();pos;) {
		int i; Body *b;
		arr_before.GetNextAssoc(pos, i, b);
		Body* b_after;
		if (!arr_after.Lookup(b->id, b_after)) { // if body is deleted in a new array
			b->stopFrame = frame;
			arr_deleted[b->id] = new Body(*b); // add to recently deleted array
		}
	}
}
// --------------------------------------------------------------------------
void BodyModel::ReplaceNewWithRecentlyDeleted(BodyMap& arr_after, BodyMap& arr_before, BodyMap& arr_deleted, int delta_age, double delta_distance) {
	for (POSITION pos = arr_after.GetStartPosition();pos;) {
		int i; Body *b_after;
		arr_after.GetNextAssoc(pos, i, b_after);
		Body* b_before;
		if (!arr_before.Lookup(b_after->id, b_before)) { // if body is new
			Body* match = FindBodyMatchByAgeAndDistance(arr_deleted, b_after, delta_age, delta_distance); // find a match in recently deleted array
			if (match) {
				//doc->Message(String::Format("Recovered {0}", match->id));
				Body *restored = new Body(*b_after);
				restored->id = match->id;
				arr_after.RemoveKey(b_after->id);
				arr_after[restored->id] = restored;
				// merge histograms
				arr_deleted.RemoveKey(match->id);
			}
		}
	}	
}
// --------------------------------------------------------------------------
// --------------------------------------------------------------------------
// --------------------------------------------------------------------------
// --------------------------------------------------------------------------