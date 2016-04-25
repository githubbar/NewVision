/*!
 * 
 * Copyright : Alex Leykin 2007
 *
 *	Email : alexleykin@gmail.com
 *
 *	This code may be used in compiled form in any way you desire. This
 *	file may be redistributed unmodified by any means PROVIDING it is 
 *	not sold for profit without the authors written consent, and 
 *	providing that this notice and the authors name is included.
 *
 *	Description  
 *  ----------- 
*/

#include "StdAfx.h"
#include "UPDialog.h"
#include "PersistantFormats.h"
#include "NewVisionDoc.h"
#include "BodyPath.h"
#include "BodyPathCluster.h"
#include "Body.h"
#include "BodyPath.h"
#include "FloorRegion.h"
#include "Globals.h"
#include "SwarmActivity.h"
#include "SwarmEvent.h"
#if USE_X3DLIBRARY
	#include "X3DDriver.h"
#endif
#include "CVML.h"
#include "TrackExportSettings.h"
#include "PedestrianTags.h"
#include "TextEntryDlg.h"
#include "Cuboid.h"
#include "Vexel.h"
#include "highgui.h"  
#include <iostream>
#include <sstream>
#include <fstream>

struct CvVideoWriter{};

#using <mscorlib.dll>
#using <system.dll>
#using <system.configuration.dll>

using namespace System;
using namespace System::Diagnostics;
using namespace System::IO;
using namespace System::Globalization;
using namespace System::Configuration;
using namespace System::Collections;
using namespace System::Collections::Specialized;
using namespace System::Text::RegularExpressions;
#ifdef _DEBUG
#undef new 
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


// --------------------------------------------------------------------------
BodyPathMap::BodyPathMap( ) {
}
// --------------------------------------------------------------------------
BodyPathMap::BodyPathMap( const BodyPathMap &that ) {
	POSITION pos = that.GetStartPosition();
	while (pos) {
		int i; BodyPath *b;
		that.GetNextAssoc(pos, i, b);
		this->SetAt(i, new BodyPath(*b));
	}
}
// --------------------------------------------------------------------------
BodyPathMap& BodyPathMap::operator=( const BodyPathMap &that )  { 
	RemoveAll();
	POSITION pos = that.GetStartPosition();
	while (pos) {
		int i; BodyPath *b;
		that.GetNextAssoc(pos, i, b);
		this->SetAt(i, new BodyPath(*b));
	}
	return *this;
}
// --------------------------------------------------------------------------
BodyPath* BodyPathMap::GetRandomValue() {
	int i_random = cvRandInt(&rnd_seed) % GetCount(); 
	POSITION pos = GetStartPosition();
	int i=0;
	while (pos) {
		int id; BodyPath *b;
		GetNextAssoc(pos, id, b);
		if (i == i_random)
			return b;
		i++;
	}
	return NULL;
}
// --------------------------------------------------------------------------
BodyPath*& BodyPathMap::operator[](int key) {
	BodyPath *b;
	if (Lookup(key, b))
		delete b;
	return CMap<int, int, BodyPath*, BodyPath*>::operator[](key);
}
// --------------------------------------------------------------------------
void BodyPathMap::SetAt(int key, BodyPath* value) {
	BodyPath *b;
	if (Lookup(key, b))
		delete b;
	CMap<int, int, BodyPath*, BodyPath*>::SetAt(key, value);
}
// --------------------------------------------------------------------------
void BodyPathMap::RemoveKey(int id) {
	BodyPath *b;
	if (Lookup(id, b)) {
		delete b;
		CMap<int, int, BodyPath*, BodyPath*>::RemoveKey(id);
	}
}
// --------------------------------------------------------------------------
void BodyPathMap::RemoveAll() {
	POSITION pos = GetStartPosition();
	while (pos) {
		int id; BodyPath *b;
		GetNextAssoc(pos, id, b);
		delete b;
	}
	CMap<int, int, BodyPath*, BodyPath*>::RemoveAll();
}
// --------------------------------------------------------------------------
int BodyPathMap::GetMaxID() {
	POSITION pos = GetStartPosition();
	int maxID = -1;
	while (pos) {
		int id; BodyPath *b;
		GetNextAssoc(pos, id, b);
		if (id > maxID)
			maxID = id;
	}
	return maxID;
}
// --------------------------------------------------------------------------
void BodyPathMap::ChangeID(int id_old, int id_new) {
	if (id_old == id_new)
		return;

	BodyPath *b_old, *b_new;
	if (Lookup(id_old, b_old)) {
		if (Lookup(id_new, b_new))
			RemoveKey(id_new);
		b_new = new BodyPath(*b_old);
		b_new->id = id_new;
		SetAt(id_new, b_new);
		RemoveKey(id_old);
	}
}
// --------------------------------------------------------------------------
bool BodyPathMap::SwapID(int id1, int id2) {
	if (id1 == id2)
		return true;

	BodyPath *b1, *b2;
	if (!Lookup(id1, b1) || !Lookup(id2, b2))
		return false;
	BodyPath *b1copy = new BodyPath(*b1);
	BodyPath *b2copy = new BodyPath(*b2);
	RemoveKey(id1);
	RemoveKey(id2);
	b1copy->id = id2;
	b2copy->id = id1;
	SetAt(id1, b2copy);
	SetAt(id2, b1copy);
	return true;
}

// ==========================================================================
// --------------------------------------------------------------------------
TrackingData::TrackingData() {
	m_initialized = false;
}
// --------------------------------------------------------------------------
TrackingData::~TrackingData() {
	POSITION pos = data.GetStartPosition();
	int i; BodyPath *p;
	while (pos) {
		data.GetNextAssoc(pos, i, p);
		delete p;
	}
}
// --------------------------------------------------------------------------
void TrackingData::Initialize(CNewVisionDoc* doc) {
	m_initialized = true;
	this->doc = doc;
	m_sourcename = doc->trackermodel.m_sourcename;
	m_startTime = doc->trackermodel.m_startTime;
}
// --------------------------------------------------------------------------
void TrackingData::DeInitialize() {
	if (!m_initialized)
		return;
}
// --------------------------------------------------------------------------
void TrackingData::Clear()
{
	data.RemoveAll();
	doc->bodymodel.last_id = 0;
}
// --------------------------------------------------------------------------
void TrackingData::Put(int nFrame) {
	// how put deletes steps in bodypath?
	POSITION pos = doc->bodymodel.body.GetStartPosition();
	while (pos) {
		int id; Body *b;
		doc->bodymodel.body.GetNextAssoc(pos, id, b);
		BodyPath *bp, *newbp = new BodyPath(b->id, cvPoint2D32f(b->bot.x, b->bot.y), nFrame, b->GetHeight(), b->GetWidth(), b->orientation,  b->confidence, true);
		if (data.Lookup(id, bp)) {
				bp->Merge(newbp);
		}
		else {
			data[id] = newbp;
		}
	}
}
// --------------------------------------------------------------------------
void TrackingData::Get(int nFrame) {

	// make it without removing all
	POSITION pos = data.GetStartPosition();
	while (pos) {
		int id; BodyPath *p;
		data.GetNextAssoc(pos, id, p);
		if (nFrame >= p->startFrame && nFrame < p->startFrame + p->GetCount()) {		// paths occurs at this frame
			Step s = p->GetAt(nFrame-p->startFrame);
			if (s.visible) {
				Body *b_new;
				if (doc->bodymodel.body.Lookup(id, b_new)) {
					b_new->bot = cvPoint3D32f(s.p.x, s.p.y, 0);
					b_new->top = cvPoint3D32f(s.p.x, s.p.y, s.height);
					b_new->predicted = b_new->bot;
					b_new->widthRatio = s.width/s.height;
				}
				else {
					b_new = new Body(s.p, s.height, s.width, doc, id, p->startFrame);
					doc->bodymodel.body.SetAt(id, b_new);
					b_new->orientation = s.orientation;
					b_new->stopFrame = p->startFrame+p->GetUpperBound();
				}
			
				// update id for newly created bodies
				if (id > doc->bodymodel.last_id)
					doc->bodymodel.last_id = id;
			}
		}
		else {
			doc->bodymodel.body.RemoveKey(id);
		}
	}
	
}
// --------------------------------------------------------------------------
int TrackingData::GetLastRecordedFrame() {
	POSITION pos = data.GetStartPosition();
	int lastFrame = -1;
	while (pos) {
		int id; BodyPath *p;
		data.GetNextAssoc(pos, id, p);
		lastFrame = max(lastFrame, p->startFrame + p->GetCount() - 1);
	}
	return lastFrame;
}
// --------------------------------------------------------------------------
int TrackingData::GetLastRecordedID() {
	POSITION pos = data.GetStartPosition();
	int lastID = 0;
	while (pos) {
		int id; BodyPath *p;
		data.GetNextAssoc(pos, id, p);
		lastID = max(id, lastID);
	}
	return lastID;
}
// --------------------------------------------------------------------------
bool TrackingData::ChangeID(int id_old, int id_new) {
	BodyPath *bp_old;
	if (data.Lookup(id_old, bp_old)) {
		// merge two BodyPath's
		BodyPath *bp_new;
		if (data.Lookup(id_new, bp_new))
			data.RemoveKey(id_new);
		bp_new = new BodyPath(*bp_old);
		bp_new->id = id_new;
		data[id_new] = bp_new;
		data.RemoveKey(id_old);
		return true;
	}
	else 
		return false;
}

void TrackingData::ChangeID( int id )
{

}
// --------------------------------------------------------------------------
bool TrackingData::AppendID(int id_master, int id_slave) {
	BodyPath *bp_master;
	if (data.Lookup(id_master, bp_master)) {
		// merge two BodyPath's
		BodyPath *bp_slave;
		if (data.Lookup(id_slave, bp_slave))
			bp_master->Append(bp_slave);

		data.RemoveKey(id_slave);
		return true;
	}
	else
		return false;
}
// --------------------------------------------------------------------------
bool TrackingData::MergeID(int id_master, int id_slave) {
	if (id_master == id_slave)
		return false;
	BodyPath *bp_master;
	if (data.Lookup(id_master, bp_master)) {
		// merge two BodyPath's
		BodyPath *bp_slave;
		if (data.Lookup(id_slave, bp_slave))
			bp_master->Merge(bp_slave);

		data.RemoveKey(id_slave);
		
		// replace id1's in current tracking
		doc->bodymodel.body.ChangeID(id_slave, id_master);
		doc->bodymodel.body_deleted.ChangeID(id_slave, id_master);

		// if ID was the last one, then update last_id
		if (id_slave == doc->bodymodel.last_id && id_master < doc->bodymodel.last_id)
			doc->bodymodel.last_id--;
		if (id_master > doc->bodymodel.last_id)
			doc->bodymodel.last_id = id_master;
		return true;
	}
	else
		return false;
}

// --------------------------------------------------------------------------
bool TrackingData::SwapID(int id1, int id2) {
	BodyPath *bp1, *bp2;
	if (!data.Lookup(id1, bp1) || !data.Lookup(id2, bp2))
		return false;
	BodyPath *bp1copy = new BodyPath(*bp1);
	BodyPath *bp2copy = new BodyPath(*bp2);
	data.RemoveKey(id1);
	data.RemoveKey(id2);
	bp1copy->id = id2;
	bp2copy->id = id1;
	data.SetAt(id1, bp2copy);
	data.SetAt(id2, bp1copy);
	return true;
}
// --------------------------------------------------------------------------
int TrackingData::CountVisibleAt(int nFrame, int zoneID) {
	POSITION pos = data.GetStartPosition();
	int count = 0;
	while (pos) {
		int id; BodyPath *b;
		data.GetNextAssoc(pos, id, b);
		if (nFrame < b->startFrame || nFrame > b->startFrame + b->GetUpperBound())
			continue;
		if (!b->GetAt(nFrame - b->startFrame).visible)
			continue;
		// If zone is defined, count only inside the zone
		if (zoneID != -1) {
			FloorRegion *z;
			if (doc->zonemodel.zone.Lookup(zoneID, z)){
				if (z->IsRealPointIn(b->GetAt(nFrame - b->startFrame).p))
					count++;
			}
		}
		else
			count++;

	}
	return count;
}
// --------------------------------------------------------------------------
int TrackingData::CountPresentAt(int nFrame) {
	POSITION pos = data.GetStartPosition();
	int i=0;
	while (pos) {
		int id; BodyPath *b;
		data.GetNextAssoc(pos, id, b);
		int first = 0, last = 0;
		b->GetFirstAndLastVisible(first, last);
		if (first <= nFrame &&  last >= nFrame)
			i++;
	}
	return i;
}
// --------------------------------------------------------------------------
void TrackingData::Serialize(CArchive& ar) {
	if (ar.IsStoring()) {
		ar << m_sourcename << doc->trackermodel.m_startTime;
		data.Serialize(ar);
	}
	else {
		//this->Clear();
		//if (data.GetCount() == 0) {
			ar >> m_sourcename >> doc->trackermodel.m_startTime;
			data.Serialize(ar);
		//}
		//else {
		//	// BEGIN TEMP
		//	CString dum;
		//	CTime dumdum;
		//	ar >> dum >> dumdum;

		//	int maxID = data.GetMaxID();
		//	BodyPathMap that;
		//	that.Serialize(ar);
		//	POSITION pos = that.GetStartPosition();
		//	while (pos) {
		//		int i; BodyPath *b;
		//		that.GetNextAssoc(pos, i, b);
		//		b->id = b->id + maxID;
		//		data.SetAt(b->id + maxID, new BodyPath(*b));
		//	}
		//	that.RemoveAll();
		//	// END TEMP
		//}

		// TEMP !!!!!!!!!!
		//doc->trackermodel.m_frameLastProcessed = GetLastRecordedFrame();
		//doc->bodymodel.last_id = GetLastRecordedID();
		// END TEMP !!!!!!!!!!

		//BodyPathMap dataTmp;
		//dataTmp.Serialize(ar);
		//int FAKE_ID = 65535;
		//POSITION pos = dataTmp.GetStartPosition();
		//SetProgressDialogRange(1, dataTmp.GetCount());
		//while (pos) {
		//	StepProgressDialog();
		//	int id; BodyPath *pLoad;
		//	dataTmp.GetNextAssoc(pos, id, pLoad);
		//	// update last_id in BodyModel
		//	if (id >= doc->bodymodel.last_id) {
		//		doc->bodymodel.last_id = id+1;
		//	}
		//	BodyPath *pThere;
		//	if (data.Lookup(id, pThere)) {
		//		AfxMessageBox("Some of the data you are loading is already in the system! Please press Stop/Reset button and load again!");
		//		pThere->Append(pLoad);  // merge
		//	}
		//	else {
		//		data[id] = pLoad; // move
		//	}
		//}
	}


}
// --------------------------------------------------------------------------
void TrackingData::ImportFromCVML(CString fname) {
#if USE_CORELIBRARY
	if (doc->cameramodel.m_cameraType != PROJECTION) {
		AfxMessageBox("XML file format for Spherical Camera has not been created yet!", MB_ICONINFORMATION | MB_OK);
		return;
	}
	SetProgressDialogRange(0, 100);
	JString filename = fname;
    JString xml = JFile::readAFileASCII(filename);
    if (xml.length() == 0) {
        AfxMessageBox("Could not read sequence file!", MB_ICONINFORMATION | MB_OK);
        return;
    }
	SetProgressDialogPos(20);
	XMLParser parser;
    if (!parser.parseXML(xml)) {
        AfxMessageBox("Error parsing sequence file!", MB_ICONINFORMATION | MB_OK);
        return;
    }
	SetProgressDialogPos(50);
    CVMLDataSet* newDataSet = new CVMLDataSet();
    if (!newDataSet->fromXML(parser.getRootNode())) {
		AfxMessageBox("Could not understand sequence file!", MB_ICONINFORMATION | MB_OK);
        return;
    }

	SetProgressDialogRange(0, 2*newDataSet->getCount());
	for (int n=0; n < newDataSet->getCount(); n++) { // for each frame
		SetProgressDialogPos(n+newDataSet->getCount());
		CVMLFrame *frame = (CVMLFrame*)newDataSet->frames.get(n);
		for (int i=0;i<frame->objects.getCount();i++) { // for each body
			//printf("frame ##: %s\n", (char*) (frame->id));
			CVMLObject* obj = (CVMLObject*)frame->objects.get(i);
			CvPoint2D32f center = cvPoint2D32f(obj->getBox().getCMX(), obj->getBox().getCMY());
			CvSize2D32f size = cvSize2D32f(obj->getBox().getWidth(), obj->getBox().getHeight());
			// fit ellipse into bounding box
			double angle = deg2rad(obj->getBox().orientation);
			double co = cos(angle);
			double si = sin(angle);
			BlobRay br; 
			double K = 0.7;
			br.p1.x = (int)(center.x+co*K*size.height); br.p1.y = (int)(center.y-si*K*size.height);
			br.p2.x = (int)(center.x-co*K*size.height); br.p2.y = (int)(center.y+si*K*size.height);
			Body *b = new Body(&br, doc, obj->getID().toInt(), n);
			BodyPath *bp;
			if (data.Lookup(b->id, bp)) {
				bp->Add(cvPoint2D32f(b->bot.x, b->bot.y), b->GetHeight(), b->GetWidth(), b->orientation, b->confidence, obj->getAppearance().equals("visible"));
			}
			else {
				bp = new BodyPath(b->id, cvPoint2D32f(b->bot.x, b->bot.y), frame->id.toInt(), b->GetHeight(), b->GetWidth(), b->orientation, b->confidence, obj->getAppearance().equals("visible"));
				data[b->id] = bp;
			}

			delete b;
		}
    }
	delete newDataSet;
#endif
}
// --------------------------------------------------------------------------
void TrackingData::ExportToCVML(CString fname) {
#if USE_CORELIBRARY
	if (doc->cameramodel.m_cameraType != PROJECTION) {
		AfxMessageBox("CVML file format for Spherical Camera has not been created yet!", MB_ICONINFORMATION | MB_OK);
		return;
	}

	// add all body paths to a single cluster
	BodyPathCluster *all_bodies = new BodyPathCluster();
	POSITION pos = data.GetStartPosition();
	int i; BodyPath *p;
	while (pos) {
		data.GetNextAssoc(pos, i, p);
		all_bodies->Add(p);
	}

	CVMLDataSet* newDataSet = new CVMLDataSet();
	int n_frames = all_bodies->last-all_bodies->first;
	SetProgressDialogRange(0, 2*n_frames);
	for (int n=all_bodies->first; n <= all_bodies->last; n++) { // for each frame
		SetProgressDialogPos(n);
		CVMLFrame *frame = new CVMLFrame();
		frame->id = JString(n);
		for (int i=0;i<all_bodies->path.GetCount();i++) { // for each body in the cluster
			//printf("frame ##: %s\n", (char*) (frame->id));
			// skip bodies not existing at this frame
			BodyPath* p = all_bodies->path[i];
			if ( (p->startFrame > n) || (p->startFrame+p->GetCount()-1 < n))
				continue;
			// add body object to the frame object
			else  { 
				Body b(p, doc, n);
				CvPoint2D32f head = b.GetImageHead();
				CvPoint2D32f foot = b.GetImageFoot();
				CvRect rect = b.GetBoundingRect();
				CVMLObject *obj = new CVMLObject();
				obj->id = b.id;
				obj->box = ::Box(rect.x, rect.y, rect.width, rect.height);
				obj->box.orientation = rad2deg(atan2(foot.y-head.y, head.x-foot.x));
				frame->addObject(obj);
			}
		}
		newDataSet->addFrame(frame);
    }
	SetProgressDialogRange(0, 100);
	SetProgressDialogPos(50);
	JString xml = newDataSet->toXML();
    if (!xml.length()) {
		AfxMessageBox("Could not convert to XML!", MB_ICONINFORMATION | MB_OK);
        return;
    }
	SetProgressDialogPos(90);
	JString filename = fname;
	if (!JFile::writeAFileASCII(filename, xml)) {
        AfxMessageBox("Could not write to sequence file!", MB_ICONINFORMATION | MB_OK);
        return;
    }
	SetProgressDialogPos(100);
	delete newDataSet;
	delete all_bodies;
#endif
}
// --------------------------------------------------------------------------
void TrackingData::ExportToXML(CString fname) {
	if (doc->cameramodel.m_cameraType == SPHERICAL) {
		AfxMessageBox("XML file format for Spherical Camera has not been created yet!", MB_ICONINFORMATION | MB_OK);
		return;
	}
	UtilXml::Init();
	std::ofstream os;
	os.open(fname);
	XmlOutputHandler	out(os);
	xmlTextWriterPtr  writer = xmlNewTextWriter(out.xmlOutputBuffer());

	xmlTextWriterSetIndent(writer, 1);	
	xmlTextWriterStartDocument(writer, NULL, "UTF-8", NULL);

	xmlTextWriterStartElement(writer, BAD_CAST"Header");
		xmlTextWriterStartElement(writer, BAD_CAST"Camera");
		xmlTextWriterWriteFormatAttribute(writer, BAD_CAST"id",   "%s", doc->cameramodel.name().c_str());
		xmlTextWriterEndElement(writer);
		xmlTextWriterStartElement(writer, BAD_CAST"recording");
		xmlTextWriterWriteFormatAttribute(writer, BAD_CAST"id",   "%s", doc->cameramodel.name().c_str());
		xmlTextWriterEndElement(writer);
		xmlTextWriterStartElement(writer, BAD_CAST"software");
		xmlTextWriterWriteFormatAttribute(writer, BAD_CAST"name",   "%s", "NewVision Pro");
		xmlTextWriterWriteFormatAttribute(writer, BAD_CAST"version",   "%s", "1.1");
		xmlTextWriterWriteFormatAttribute(writer, BAD_CAST"platform",   "%s", "Win32");
		xmlTextWriterEndElement(writer);

	// add all body paths to a single cluster

	SetProgressDialogRange(0, doc->trackermodel.m_frameTotal);
	for (int n=0; n <= doc->trackermodel.m_frameTotal; n++) { // for each frame
		xmlTextWriterStartElement(writer, BAD_CAST"Frame");
		xmlTextWriterWriteFormatAttribute(writer, BAD_CAST"id", "%d", n);
		SetProgressDialogPos(n);
		POSITION pos = data.GetStartPosition(); int i; BodyPath *p;
		while (pos) {
			data.GetNextAssoc(pos, i, p);
			// skip bodies not existing at this frame
			if ( (p->startFrame > n) || (p->startFrame+p->GetUpperBound() < n))
				continue;
			// add body object to the frame object
			else  { 
				Body b(p, doc, n);
				CvPoint2D32f head = b.GetImageHead();
				CvPoint2D32f foot = b.GetImageFoot();
				CvRect rect = b.GetBoundingRect();

				
				//obj->box.orientation = rad2deg(atan2(foot.y-head.y, head.x-foot.x));
				xmlTextWriterStartElement(writer, BAD_CAST"Person");
					xmlTextWriterWriteFormatAttribute(writer, BAD_CAST"id", "%d", b.id);
					xmlTextWriterStartElement(writer, BAD_CAST"BoundingBox");
						xmlTextWriterWriteFormatAttribute(writer, BAD_CAST"top", "%d", rect.y);
						xmlTextWriterWriteFormatAttribute(writer, BAD_CAST"left", "%d", rect.x);
						xmlTextWriterWriteFormatAttribute(writer, BAD_CAST"bottom", "%d", rect.y + rect.height);
						xmlTextWriterWriteFormatAttribute(writer, BAD_CAST"right", "%d", rect.x + rect.width);
						xmlTextWriterStartElement(writer, BAD_CAST"Camera");
							xmlTextWriterWriteFormatAttribute(writer, BAD_CAST"id",   "%s", doc->cameramodel.name().c_str());
						xmlTextWriterEndElement(writer);
					xmlTextWriterEndElement(writer);

					xmlTextWriterStartElement(writer, BAD_CAST"GroundPlane");
						xmlTextWriterWriteFormatAttribute(writer, BAD_CAST"obj_x", "%lf", b.GetFoot().x);
						xmlTextWriterWriteFormatAttribute(writer, BAD_CAST"obj_y", "%lf", b.GetFoot().y);
						xmlTextWriterWriteFormatAttribute(writer, BAD_CAST"obj_w", "%lf", b.GetWidth());
						xmlTextWriterWriteFormatAttribute(writer, BAD_CAST"obj_h", "%lf", b.GetHeight());
					xmlTextWriterEndElement(writer);
				xmlTextWriterEndElement(writer);
			}
		}
		xmlTextWriterEndElement(writer); 
	}

	xmlTextWriterEndElement(writer);
	xmlTextWriterEndDocument(writer);
	xmlFreeTextWriter(writer);
	UtilXml::Cleanup();
}
// --------------------------------------------------------------------------
void TrackingData::ExportToX3D(CString fname) {
#if USE_X3DLIBRARY
	X3DDriver xd;
	xd.ClearScene();
	xd.SetViewPoint();
	//double totalFrames = doc->trackermodel.m_frameStop;
	double totalFrames = 6000;
	CString duration; duration.Format("%.2f", totalFrames/doc->trackermodel.m_frameRate);
	xd.AddTimeSensor("myTimeSensor", duration);
	// --------- Add bodies
	int count = 0;
	SetProgressDialogRange(0, data.GetCount());
	for (POSITION pos = data.GetStartPosition();pos;) {
		count++;
		StepProgressDialog((CString)String::Format("Processing path {0} out of {1}", count, data.GetCount()));
		int id; BodyPath *b;
		data.GetNextAssoc(pos, id, b);
		if (b->startFrame > totalFrames)
			continue;
		CString s, coords="", ctimes="", orients="", otimes="", tcoords="", ttimes="";
		boolean visible = false;
		int SIGMA = 10;
		// set initial invisibility
		ttimes += "0 ";
		tcoords += "-1 ";
		int SAMPLING_FREQUENCY = 30;
		for (int i=0;i<b->GetCount();i+=SAMPLING_FREQUENCY) {
			// x <-> y to convert from left-coordinate system to right coordinate system
			CvPoint2D32f point = b->Smooth(b->startFrame+i, SIGMA);
			s.Format("%.2f %.2f 0", point.y, point.x);
			if (i<b->GetCount()-SAMPLING_FREQUENCY)
				s+=", ";
			coords += s;
			double a = b->GetMotionAngle(b->startFrame+i, SIGMA);
			if (a != -10) {
				s.Format("0 0 1 %.2f", PI-a);
				if (i<b->GetCount()-SAMPLING_FREQUENCY)
					s+=", ";
				orients += s;
				s.Format("%.5f ", ((double)(b->startFrame+i))/totalFrames);
				otimes += s;
			}
			s.Format("%.5f ", ((double)(b->startFrame+i))/totalFrames);
			ctimes += s;
			//change to visible
			if (b->GetAt(i).visible && !visible) {
				ttimes += s;
				tcoords += "0 ";
				visible = true;
			}
			//change to invisible
			if (!b->GetAt(i).visible && visible) {
				ttimes += s;
				tcoords += "-1 ";
				visible = false;
			}
		}

		CString avatar;
		CString gender="";
		b->tags.Lookup("Gender", gender);
		if (gender == "Male")
			avatar = "http://www.vrinternal.com/lib/avatars/male/male05.wrl";
		else if (gender == "Female")
			avatar = "http://www.vrinternal.com/lib/avatars/female/female09.wrl";
		else 
			avatar = "http://www.vrinternal.com/lib/avatars/male/male07.wrl";

		xd.AddTrackAvatar(id, avatar, coords, ctimes, orients, otimes, tcoords, ttimes);
		
		//if (cc>5)
		//	break;
		//cc++;
	}
	xd.SaveX3D(fname);

	// --------- Add floormap
	doc->floormodel.ExportToX3D(xd);
	// --------- Add obstacles
	doc->obstaclemodel.ExportToX3D(xd);
#endif
}
// --------------------------------------------------------------------------
void TrackingData::ExportToX3DStatic(CString fname) {
#if USE_X3DLIBRARY
	X3DDriver xd;
	xd.ClearScene();
	xd.SetViewPoint();

	double totalFrames = doc->trackermodel.m_frameStop;
	//double totalFrames = 6000;
	CString duration; duration.Format("%.2f", totalFrames/doc->trackermodel.m_frameRate);

	// --------- Add bodies
	SetProgressDialogRange(0, data.GetCount());
	int count = 0;
	for (POSITION pos = data.GetStartPosition();pos;) {
		count++;
		StepProgressDialog((CString)String::Format("Processing path {0} out of {1}", count, data.GetCount()));
		int id; BodyPath *b;
		data.GetNextAssoc(pos, id, b);

		// exclude tracks after target time interval
		if (b->startFrame > totalFrames)
			continue;

		// exclude employees
		CString employee="";
		b->tags.Lookup("Employee", employee);
		if (employee == "Yes")
			continue;

		CString s, coords="", ctimes="", orients="", otimes="", tcoords="", ttimes="";
		boolean visible = false;
		int SIGMA = 15;
		// set initial invisibility
		ttimes += "0 ";
		tcoords += "-1 ";
		int SAMPLING_FREQUENCY = 10;
		CArray<IntPair, IntPair&> list;
		b->ListVisibleSegments(list);

		// for each visible segment 
		for (int n=0; n < list.GetCount(); n++) {
			bool dwelling = false;
			CArray<CvPoint2D32f> locations;
			for (int i = list[n].id1; i < list[n].id2; i++) {
				// accumulate visible steps
				if ((i-list[n].id1) % SAMPLING_FREQUENCY == 0) {
					// x <-> y to convert from left-coordinate system to right coordinate system
					CvPoint2D32f point = b->Smooth(i, SIGMA);
					CvPoint2D32f pointPlus = b->Smooth(i+SIGMA, SIGMA);
					CvPoint2D32f pointMinus = b->Smooth(i-SIGMA, SIGMA);
					double velocity = (d(point, pointPlus) + d(point, pointMinus))/2;
					//if (pointMinus.x == -1 && pointMinus.y == -1)
					//	velocity = d(point, pointPlus);
					//if (pointPlus.x == -1 && pointPlus.y == -1)
					//	velocity = d(point, pointMinus);
					//if (pointMinus.x == -1 && pointMinus.y == -1 && pointPlus.x == -1 && pointPlus.y == -1)
					//	continue;
					//velocity *= 3;
					velocity = min(MAX_VELOCITY*SIGMA, velocity);
					s.Format("%.2f %.2f %.2f", point.y, point.x, velocity);
					if (i < list[n].id2-SAMPLING_FREQUENCY)
						s+=", ";
					coords += s;
				}
			}
			// write visible tracklet (color code gender)
			CString gender="";
			b->tags.Lookup("Gender", gender);
			bool female = false;
			if (gender == "Female")
				female = true;
			xd.AddTrackCurve(id, female, coords);
			coords = "";
		}
	}
	// --------- Add floormap
	doc->floormodel.ExportToX3D(xd);
	// --------- Add obstacles
	doc->obstaclemodel.ExportToX3D(xd);

	xd.SaveX3D(fname);
#endif
}

// --------------------------------------------------------------------------
void TrackingData::ExportToX3DClusteredTraffic(CString fname)
{
#if USE_X3DLIBRARY
	// --------------------- Export BodyPaths --> Vexels
	CArray<INode*> vexels;
	int MIN_LENGTH = 5;
	SetProgressDialogRange(0, data.GetCount());
	int count=1;
	for (POSITION pos = data.GetStartPosition();pos;) {
		StepProgressDialog((CString)String::Format("Processing path {0} out of {1}", count, data.GetCount()));
		count++;
		int id; BodyPath *bp;
		data.GetNextAssoc(pos, id, bp);
		// exclude employees
		CString employee="";
		bp->tags.Lookup("Employee", employee);
		if (employee == "Yes")
			continue;
		bp->ConvertToVexels(vexels, 15, 120.0/doc->trackermodel.m_frameRate, 15);
		//if (count > 10) 
		//	break;
	}
	HideProgressDialog();

	// --------------------- Cluster vexels
	IFuzzyCluster::last_id = 0;
	CArray<IFuzzyCluster*> clusters;
	ClusteringParams param;
	// Initialize clustering parameters
	param.maxIterations				= 10;	// maximum number of iterations
	param.maxClusters				= 5;	// initial number of clusters
	param.maxObjectiveFunctionDelta = 1;	// used as a stop criteria (convergence
	param.eta0						= 0.01;	// multiplier: controls the speed of clustering
	param.minCardinality			= 0.05;	// minimum cardinality to keep cluster alive
	param.tau						= 10;	// exponent: controls the speed of clustering
	param.nIteration0				= 5;	// after this iteration clustering will slow down
	param.cTuningMin				= 4;	// a tuning constant
	param.cTuning0					= 12;	// a tuning constant
	param.cTuningDelta				= 0.1;	// a tuning constant
	param.silent					= true;	// true=do not display messages

	//ClusteringMachine cm(param);
	//cm.RunFuzzy(vexels, clusters);

	// ----------------- Cluster each cell (x by x meters) of vexels
	// determine sampling cell size
	int XSTEP = 50, YSTEP = 50, cellcount = 0;
	ShowProgressDialog("Clustering vexels", XSTEP*YSTEP);
	for (int x=doc->floormodel.floormin.x;x<= doc->floormodel.floormax.x;x+=XSTEP)
		for (int y=doc->floormodel.floormin.y;y<= doc->floormodel.floormax.y;y+=YSTEP) {
			// find all vexels within a cell and add to temporary array
			CArray<INode*> vCell;
			CArray<IFuzzyCluster*> cCell;
			for (int i=0;i<vexels.GetCount();i++) {
				Vexel* v = (Vexel*)vexels[i];
				if (v->p.x >= x && v->p.x < x+XSTEP	&& v->p.y >= y && v->p.y < y+YSTEP)
					vCell.Add(vexels[i]);
			}
			// cluster vexels within a cell
			if (vCell.GetCount() > 0) {
				ClusteringMachine cm(param);
				cm.RunFuzzy(vCell, cCell);
			}

			// aggregate cell clusters 
			clusters.Append(cCell);
			StepProgressDialog((CString)String::Format("Floor cell {0} out of {1}", cellcount, XSTEP*YSTEP));
			cellcount++;
		}
	HideProgressDialog();

	// ----------------- Convert clusters to tracklets
	//CArray<Tracklet*> new_tracklets;
	//for (int i=0;i<clusters.GetCount();i++) {
	//	double pAll = 0;
	//	new
	//	for (POSITION pos = clusters[i]->rec.GetStartPosition(); pos != NULL;) {
	//		int nid; Record* r;
	//		clusters[i]->rec.GetNextAssoc(pos, nid, r);
	//		Tracklet *tr = (Tracklet*)(r->node);
	//		new_tr
	//	}

	// ----------- Find maximum cluster magnitude
	ShowProgressDialog("Normalizing the data", clusters.GetCount());
	double maxVcount = 0;
	for (int i=0;i<clusters.GetCount();i++) {
		StepProgressDialog((CString)String::Format("Processing cluster {0} out of {1}", i, clusters.GetCount()));
		Vexel *v = new Vexel();
		v->p.x = 0; v->p.y = 0;
		v->v.x = 0; v->v.y = 0;
		int vcount = 0;
		for (POSITION pos = clusters[i]->rec.GetStartPosition(); pos != NULL; ) {
			int nid; Record* r;
			clusters[i]->rec.GetNextAssoc(pos, nid, r);	
			if (r->crispIn)
				vcount++;
		}
		if (vcount > maxVcount)
			maxVcount = vcount;
	}
	HideProgressDialog();

	// --------------------- Export to X3D 
	X3DDriver xd;
	// if file existed, load it
	/*xd.LoadX3D(fname);*/
	xd.ClearScene();
	xd.SetViewPoint();
	double totalFrames = doc->trackermodel.m_frameStop;
	CString duration; duration.Format("%.2f", totalFrames/doc->trackermodel.m_frameRate);

	//// --------- Add vexels to X3D file 
	//ShowProgressDialog("Saving vexels to X3D", vexels.GetCount());
	//for (int i=0;i<vexels.GetCount();i++) {
	//	StepProgressDialog((CString)String::Format("Processing vexel {0} out of {1}", i, vexels.GetCount()));
	//	CString center, rotation;
	//	Vexel *v = (Vexel*)vexels[i];
	//	center.Format("%.2f %.2f 50", v->p.y, v->p.x);
	//	CvPoint2D32f vConverted;
	//	vConverted.x = v->v.y;
	//	vConverted.y = v->v.x;
	//	float rot = PI/2+angle(vConverted);
	//	rotation.Format("0 0 1 %.2f", rot);
	//	float height = d(vConverted);
	//	double R = 5, hScale = 5;
	//	if (height > 1 && height < 10) {
	//		if (rot > PI/2)
	//			xd.AddArrow( center, rotation, "1 0 0", R, hScale*height); // traffic out 
	//		else
	//			xd.AddArrow( center, rotation, "0 1 0", R, hScale*height); // traffic in
	//	}
	//}


	// --------- Add clusters of vexels to X3D file 
	ShowProgressDialog("Saving clusters of vexels to X3D", clusters.GetCount());
	for (int i=0;i<clusters.GetCount();i++) {
		StepProgressDialog((CString)String::Format("Processing cluster {0} out of {1}", i, clusters.GetCount()));
		Vexel *v = new Vexel();
		v->p.x = 0; v->p.y = 0;
		v->v.x = 0; v->v.y = 0;
		int vcount = 0;
		for (POSITION pos = clusters[i]->rec.GetStartPosition(); pos != NULL; ) {
			int nid; Record* r;
			clusters[i]->rec.GetNextAssoc(pos, nid, r);	
			if (r->crispIn) {
				Vexel *v0 = (Vexel*)r->node;
				v->p.x += v0->p.x;
				v->p.y += v0->p.y;
				v->v.x += v0->v.x;
				v->v.y += v0->v.y;
				vcount++;
			}
		}
		v->p.x /= vcount;
		v->p.y /= vcount;
		v->v.x /= vcount;
		v->v.y /= vcount;

		CString center, rotation;
		center.Format("%.2f %.2f 50", v->p.y, v->p.x);
		CvPoint2D32f vConverted;
		vConverted.x = v->v.y;
		vConverted.y = v->v.x;
		float rot = PI/2+angle(vConverted);
		rotation.Format("0 0 1 %.2f", rot);
		
		double CLUSTER_MAGNITUDE_CUTOFF = 0.05;

		if (vcount > CLUSTER_MAGNITUDE_CUTOFF*maxVcount) {
			float height = d(vConverted);
			double R = 0.1*vcount, hScale = 200;
			if (rot > PI/2)
				xd.AddArrow( center, rotation, "1 0 0", R, hScale*height); // traffic out 
			else
				xd.AddArrow( center, rotation, "0 1 0", R, hScale*height); // traffic in
		}
		delete v;

	}
	// --------- Add floormap
	doc->floormodel.ExportToX3D(xd);
	// --------- Add obstacles
	doc->obstaclemodel.ExportToX3D(xd);
	xd.SaveX3D(fname);
	HideProgressDialog();
#endif
}
// --------------------------------------------------------------------------
void TrackingData::ImportFromCSV(CString fname) {
	AfxMessageBox("Importing tracking data from CSV is not supported!", MB_ICONINFORMATION | MB_OK);
}
// --------------------------------------------------------------------------
void TrackingData::ExportToCSV(CString fname) {
	int SAMPLING_FREQUENCY = 16;
	int SIGMA = 8;
	int TOTAL_FRAMES = 60000000;

	// Create CSV file
	String ^fnametracks = gcnew String(fname);
	StreamWriter^ srtracks = File::CreateText(fnametracks->Replace(".csv", ".tracks.csv"));
//	srtracks->Write("TrackID,StartTime,X,Y"); 
	

	// --------- Add bodies
	int count = 0;
	SetProgressDialogRange(0, data.GetCount());
	for (POSITION pos = data.GetStartPosition();pos;) {
		count++;
		StepProgressDialog((CString)String::Format("Processing path {0} out of {1}", count, data.GetCount()));
		int id; BodyPath *b;
		data.GetNextAssoc(pos, id, b);
		if (b->startFrame > TOTAL_FRAMES)
			continue;

		srtracks->Write("{0}, {1}, {2}, ", id, b->startFrame, b->startFrame+b->GetUpperBound());
		for (int i=0;i<b->GetCount();i+=SAMPLING_FREQUENCY) {
			CvPoint2D32f point = b->Smooth(b->startFrame+i, SIGMA);
			srtracks->Write("{0}, {1:F2}, {2:F2}, ",  b->startFrame+i, point.y, point.x);
		}
		srtracks->WriteLine();
	}
	HideProgressDialog();
	srtracks->Close();
}
// --------------------------------------------------------------------------
void TrackingData::ExportToCSVStatic(CString fname) {
	gcroot<NewVision::TrackExportSettings^> tSettings;
	tSettings = gcnew NewVision::TrackExportSettings();
	if (tSettings->ShowDialog() != ::DialogResult::OK)
		return;
	String ^fnametracks = gcnew String(fname);
	String ^fnameevents  = gcnew String(fname);
	String ^fnamezones  = gcnew String(fname);
	StreamWriter^ srtracks = File::CreateText(fnametracks->Replace(".csv", ".tracks.csv"));
	StreamWriter^ srevents = File::CreateText(fnameevents->Replace(".csv", ".events.csv"));
	StreamWriter^ srtimeevents = File::CreateText(fnameevents->Replace(".csv", ".timeevents.csv"));
	StreamWriter^ srzones = File::CreateText(fnamezones->Replace(".csv", ".zones.csv"));

	SetProgressDialogRange(0, data.GetCount());
	// Tracks header
	srtracks->Write("TrackID,Height,Width,Confidence,GroupID,GroupSize,StartTime,Duration,PercentOfTimeVisible,PeopleOnBegin,PeopleOnEnd,DistanceCovered(ft.),AreaCovered(sq.ft.),Sinuosity(deg.),ObjectsZonesShopped,TrackedToExit,"); 
	// WRITE: UserDefinedTag1, UserDefinedTag2 names
	CommaDelimitedStringCollection ^values2names = GetUserSettingsCommaList("tags");
	for (int i=0;i<values2names->Count;i++) {
		String^ printableName = Regex::Replace(values2names[i], " ", "_"); // replace all spaces with an underscore
		printableName = Regex::Replace(printableName, "[^\\w]", ""); // strip all non-alphanumeric characters
		srtracks->Write(printableName);
		if (i< values2names->Count-1)
			srtracks->Write(",");
	}
	srtracks->WriteLine();
	// Time events header
	srtimeevents->WriteLine("TrackID,Time Event Type, StartTime, StopTime, StartZoneID, StopZoneID, ClosestMeanZoneID, TimeFromEntry, DistanceFromEntry");

	// Events header
	srevents->Write("TrackID,ObjectZoneID,StartTime,Duration,PercentOfTimeVisible,Sinuosity(deg.),Direction,PeopleOnBegin,PeopleOnEnd,");
	if (tSettings->crowdingBefore)
		srevents->Write("People30SecBefore,People60SecBefore,People90SecBefore,People120SecBefore,");
	srevents->WriteLine("DistanceCovered, AreaCovered"); 	
	// Zones header
	srzones->WriteLine("ObjectZoneID,ObjectZoneName"); 	

	for (POSITION pos = data.GetStartPosition();pos;) {
		// ======================== WRITE TRACKS ===============================
		int i; BodyPath *b;
		data.GetNextAssoc(pos, i, b);
		// WRITE: TrackID, Height, Width
		srtracks->Write("{0},{1:F2},{2:F2},", i, meters2feet(b->GetHeight()/100), meters2feet(b->GetWidth()/100));

		// WRITE: Confidence
		srtracks->Write("{0:F2},", b->autoConfidence*100);
		
		// WRITE: GroupID, GroupSize
		if (tSettings->groupData) {
			// find first matching grouping activity
			int aid = -1;  SwarmActivity* activity; bool found = false;
			for (POSITION pos = doc->m_ActivityData.sActivity.GetStartPosition(); pos != NULL; ) {
				
				doc->m_ActivityData.sActivity.GetNextAssoc(pos, aid, activity);	
				// found "grouping" activity, write and break
				double actorD;
				if (activity->type == "GROUPING" && activity->actors.Lookup(i, actorD)) {
					srtracks->Write("{0},{1},", aid, activity->actors.GetCount());
					found = true;
					break;
				}
			}
			// none found, write blank
			if (!found)
				srtracks->Write(",,");
		}
		else 
			srtracks->Write(",,");
		
		// WRITE: StartTime, Duration, PercentOfTimeVisible
		int first = 0, last = 0;
		b->GetFirstAndLastVisible(first, last);
		String ^start = doc->trackermodel.GetDateTime(first).ToString("dd-MMM-yy HH:mm:ss", DateTimeFormatInfo::InvariantInfo);
		String ^stop  = doc->trackermodel.GetTimeSpan(last-first).ToString(); 
		srtracks->Write("{0},{1},{2:F2},", start, stop, 100.0*b->CountVisibleFrames()/(last-first+1));

		// WRITE: PeopleOnBegin, PeopleOnEnd
		if (tSettings->crowdingData) {
			srtracks->Write("{0},{1},", CountPresentAt(first)-1, CountPresentAt(last)-1); // exclude itself
		}
		else 
			srtracks->Write(",,");

		// WRITE: DistanceCovered, AreaCovered, Sinuosity
		srtracks->Write("{0:F2},{1:F2},{2:F2},", meters2feet(b->GetDistanceCovered()/100), metersSQ2feetSQ(b->GetAreaCovered()/10000), b->GetSinuosity());


		// ======================== WRITE EVENTS ===============================
		int eventCount = 0;
		if (tSettings->zoneStatistics) {
			POSITION posZ = doc->zonemodel.zone.GetStartPosition();
			while (posZ) { // for each zone
				int iz; FloorRegion *z;
				doc->zonemodel.zone.GetNextAssoc(posZ, iz, z);
				CArray<IntPair, IntPair&> list;
				b->ListZoneEvents(z, list, Int32::Parse(tSettings->allowOutsideFor->Text));
				for (int ie = 0; ie < list.GetCount(); ie++) { // for each zone event
					int zoneFirst = list[ie].id1, zoneLast = list[ie].id2;
					// WRITE: TrackID, ObjectZoneID
					srevents->Write("{0},{1},", i, z->id);
				
					// WRITE: StartTime, Duration, PercentOfTimeVisible
					start = doc->trackermodel.GetDateTime(zoneFirst).ToString("dd-MMM-yy HH:mm:ss", DateTimeFormatInfo::InvariantInfo);
					stop  = doc->trackermodel.GetTimeSpan(zoneLast-zoneFirst).ToString(); 
					srevents->Write("{0},{1},{2:F2},", start, stop, 100.0*b->CountVisibleFrames(zoneFirst, zoneLast)/(zoneLast-zoneFirst+1));
					// WRITE: sinuosity with the zone
					srevents->Write("{0:F2},", b->GetSinuosity(zoneFirst, zoneLast));

					// WRITE: Direction
					srevents->Write(",");
					// TODO: get direction of 'b' through 'obst'

					// WRITE: PeopleOnBegin, PeopleOnEnd
					if (tSettings->crowdingData) {
						srevents->Write("{0},{1},", CountVisibleAt(zoneFirst, iz)-1, CountVisibleAt(zoneLast, iz)); // on entrance, exclude itself; on exit the track itself is already out of the zone
					}
					else 
						srevents->Write(",,");

					// WRITE: 30, 60, 90, 120 "sec before" columns
					if (tSettings->crowdingBefore) {
						srevents->Write("{0},{1},{2},{3},", 
							CountVisibleAt(zoneFirst-30*doc->trackermodel.m_frameRate, iz), 
							CountVisibleAt(zoneFirst-60*doc->trackermodel.m_frameRate, iz), 
							CountVisibleAt(zoneFirst-90*doc->trackermodel.m_frameRate, iz), 
							CountVisibleAt(zoneFirst-120*doc->trackermodel.m_frameRate, iz) );
					}

					// WRITE: DistanceCovered, AreaCovered
					srevents->Write("{0:F2},{1:F2}", meters2feet(b->GetDistanceCovered(zoneFirst, zoneLast)/100), metersSQ2feetSQ(b->GetAreaCovered(zoneFirst, zoneLast)/10000));
					srevents->WriteLine(); 
					eventCount++;
				}
			}
		}

		// ======================== WRITE TIME EVENTS ===============================
		// WRITE: UserDefinedTags with time stamps
		System::Configuration::KeyValueConfigurationCollection ^settings
			= ConfigurationManager::OpenExeConfiguration(ConfigurationUserLevel::None)->AppSettings->Settings;
		for (int iTag = 0;iTag<values2names->Count;iTag++) {
			CString value;
			KeyValueConfigurationElement ^tagType = settings[values2names[iTag]+" values"];
			if (tagType && tagType->Value =="#time#time") {
				if (b->tags.Lookup(CString(values2names[iTag]), value)) {
					CommaDelimitedStringCollectionConverter ^cv = gcnew CommaDelimitedStringCollectionConverter();
					CommaDelimitedStringCollection ^startstop = (CommaDelimitedStringCollection ^)cv->ConvertFromString(gcnew String(value));
					for (int i=0;i<startstop->Count-1;i+=2) {
						int eventStartFrame = Int32::Parse(startstop[i]);
						int eventStopFrame = Int32::Parse(startstop[i+1]);
						String ^start = doc->trackermodel.GetDateTime(eventStartFrame).ToString("dd-MMM-yy HH:mm:ss", DateTimeFormatInfo::InvariantInfo);
						String ^stop = doc->trackermodel.GetDateTime(eventStopFrame).ToString("dd-MMM-yy HH:mm:ss", DateTimeFormatInfo::InvariantInfo);
						srtimeevents->Write("{0}, {1}, {2}, {3},", b->id, values2names[iTag], gcnew String(start), gcnew String(stop));

						// Catch invalid time tags
						if (eventStartFrame < b->startFrame || eventStopFrame > b->startFrame + b->GetUpperBound() ||
							eventStartFrame > eventStopFrame 
							/*|| !b->GetAt(eventStartFrame-b->startFrame).visible || !b->GetAt(eventStopFrame-b->startFrame).visible*/) {
							srtimeevents->Write("Error! Event time is less/greater than start/stop time for this track!,,");
						}
						else {
							// Write ZoneID
							Step stepStart = b->GetAt(eventStartFrame-b->startFrame);
							Step stepStop = b->GetAt(eventStopFrame-b->startFrame);
							CvPoint2D32f meanP = b->Smooth((eventStartFrame+eventStopFrame)/2, 10);
							srtimeevents->Write("{0},", doc->zonemodel.GetZone(stepStart.p));
							srtimeevents->Write("{0},", doc->zonemodel.GetZone(stepStop.p));
							srtimeevents->Write("{0},", doc->zonemodel.GetClosestZone(meanP));
							

							// Write TimeFromEntry
							String ^timeFromEntry = doc->trackermodel.GetTimeSpan(eventStartFrame-b->startFrame).ToString();
							srtimeevents->Write("{0},", gcnew String(timeFromEntry));
							// Write DistanceFromEntry
							srtimeevents->Write("{0}", meters2feet(b->GetDistanceCovered(-1, eventStartFrame)/100));

						}
						srtimeevents->WriteLine();
					}
				}
			}
		}

		// ======================= CONTINUE WRITE TRACKS ===============================
		// WRITE: TODO: ObjectsZonesShopped, TrackedToExit
		srtracks->Write("{0},{1},", eventCount, " ");
		
		// WRITE: UserDefinedTag1, UserDefinedTag2
		for (int i=0;i<values2names->Count;i++) {
			CString value;
			if (b->tags.Lookup(CString(values2names[i]), value)) {
				if (value == "Select...")
					value = "";
				srtracks->Write("{0}", gcnew String(value));
			}
			if (i< values2names->Count-1)
				srtracks->Write(",");

		}
		srtracks->WriteLine(); 


		StepProgressDialog();
	}
	
	// ============================ WRITE ZONES =================================
	for (POSITION posZ = doc->zonemodel.zone.GetStartPosition();posZ;) {
		int id; FloorRegion *z;
		doc->zonemodel.zone.GetNextAssoc(posZ, id, z);
		srzones->WriteLine("{0},{1}", z->id, gcnew String(z->label)); 
	}

	srtracks->Close();
	srevents->Close();
	srtimeevents->Close();
	srzones->Close();
}
// --------------------------------------------------------------------------
void TrackingData::DrawFloor(IplImage* frame, CvScalar color) {
	POSITION pos = data.GetStartPosition();
	while (pos) {
		int i; BodyPath *bp;
		data.GetNextAssoc(pos, i, bp);
		bp->DrawFloor(frame, color, doc, 300);
	}
}

// --------------------------------------------------------------------------
// --------------------------------------------------------------------------
struct GENHEATDATA{TrackingData *td; REFERENCE_TIME start; REFERENCE_TIME stop; int segments; CString filename; CString ext;};

bool RenderHeatmap(const CUPDUPDATA* pParam) {
	GENHEATDATA* para = (GENHEATDATA*) (pParam->GetAppData());
	BodyPathMap data = para->td->data;
	CNewVisionDoc* doc = para->td->doc;
	REFERENCE_TIME start = para->start;
	REFERENCE_TIME stop = para->stop;
	int segments = para->segments;
	CString filename = para->filename;
	CString ext = para->ext;

	// for each body path, draw a circle on the floor where it was; and add it to the heatmap image
	// NOTE: due to scaling, circle becomes an ellipse
	POSITION pos = data.GetStartPosition();
	int count = 1;
	int SAMPLING_FREQUENCY = 6;
	int SIGMA = 3;
	int OVERLAP = 3;
	float BLEND_STEP = 0.05;

	CvSize sz = cvSize(doc->floormodel.w, doc->floormodel.h);
	IplImage *img = cvCreateImage(sz, IPL_DEPTH_32F, 1);	

	if (ext == "bmp" || ext == "png") {
		//double min_val = 0, max_val = 0;
		//double maxAll = 0;
		//for (int i=0;i<NHOURS;i++) {
		//	//load data
		//	char s[25]; sprintf_s(s, "AE Nov 25, 2005 - %2d", i);
		//	CFile file(filename+s+".heatmap", CFile::modeRead);
		//	CArchive ar(&file, CArchive::load);
		//	doc->m_HeatmapData.Serialize(ar);
		//	ar.Close();
		//	file.Close();
		//	cvCopy(doc->m_HeatmapData.map, img[i]);
		//	//cvFlip(img[i], doc->m_HeatmapData.map);

		//	// find min/max
		//	cvMinMaxLoc(img[i], &min_val, &max_val);
		//	if (max_val > maxAll)
		//		maxAll = max_val;
		//}
		//for (int i=0;i<NHOURS;i++) {
		//	char s[25]; sprintf_s(s, "AE Nov 25, 2005 - %2d", i);
		//	////save data
		//	//cvCopy(img[i], doc->m_HeatmapData.map);
		//	//CFile file(filename+s+".heatmap", CFile::modeCreate | CFile::modeWrite);
		//	//CArchive ar(&file, CArchive::store);
		//	//doc->m_HeatmapData.Serialize(ar);
		//	//ar.Close();
		//	//file.Close();
		//	//save image
		//	cvCopy(img[i], doc->m_HeatmapData.map);
		//	doc->m_HeatmapData.DrawHeatMap(floorImg, false, false, 0.2*maxAll);
		//	doc->obstaclemodel.DrawFloor(floorImg, CV_RGB(255,128,0));
		//	cvSaveImage(filename+s+".png", floorImg);
		//}
	}
	if (ext == "avi") {
		double maxAll = 0;

		CvVideoWriter* aviWriter =  cvCreateVideoWriter(
			(LPCTSTR)(filename), -1, 15, cvSize(floorImg->width, floorImg->height) );
		
		segments *= OVERLAP;
		pParam->SetRange(1, segments);
		for (int i=0;i<segments && !pParam->ShouldTerminate();i++) {
			cvZero(img); cvZero(floorImg);
			pParam->SetProgress((CString)String::Format("Processing segment {0} out of {1}", i, segments), i);

			POSITION pos = data.GetStartPosition();
			// overlap segments
			int f1 = round(1.0*i/segments*(stop-start)+start);
			int f2 = round(1.0*(i+OVERLAP)/segments*(stop-start)+start);
			while (pos) {
				int id; BodyPath *bp;
				data.GetNextAssoc(pos, id, bp);

				if (bp->startFrame > f2 || bp->startFrame+bp->GetUpperBound() < f1)
					continue;
				bp->GenerateTrackHeatmap(doc, img, f1, f2, SAMPLING_FREQUENCY, SIGMA);
				count++;
			}

			double min_val = 0, max_val = 0;
			cvMinMaxLoc(img, &min_val, &max_val);
			if (max_val > maxAll)
				maxAll = max_val;
			//store data
			cvCopy(img, doc->m_HeatmapData.map);
			doc->m_HeatmapData.DrawHeatMap(floorImg, false, false, 500);
			doc->obstaclemodel.DrawFloor(floorImg, CV_RGB(255,128,0));
			
			CTimeSpan elapsed((int)(f1 / doc->trackermodel.m_frameRate));
			CTime time = doc->trackermodel.m_startTime + elapsed;
			drawTextInRectangle(
				time.Format( "%m/%d/%y, %I:%M%p"), 
				cvPoint(floorImg->width-350, floorImg->height-30), 
				floorImg, CV_RGB(255,255,255), CV_RGB(0,0,0), (float)1.5);


			// write blended intermediate frames 
			for (float w = 0; w < 1; w += BLEND_STEP) {
				cvAddWeighted( floormap, 1-w, floorImg, w, 0, motionmapscaled);
				cvWriteFrame(aviWriter, motionmapscaled);
			}

			// save 1st key frame
			cvCopy(floorImg, floormap);
		}
		cvReleaseVideoWriter(&aviWriter);
	}

	else {
		//for (int i=0;i<segments;i++) {
		//	cvZero(img); cvZero(floorImg);
		//	StepProgressDialog((CString)String::Format("Processing segment {0} out of {1}", i, segments));
		//	POSITION pos = data.GetStartPosition();
		//	while (pos) {
		//		int i; BodyPath *bp;
		//		data.GetNextAssoc(pos, i, bp);
		//		int f1 = round(i/segments*(stop-start)+start);
		//		int f2 = round((i+1)/segments*(stop-start)+start);
		//		bp->GenerateTrackHeatmap(doc, img, f1, f2);
		//		count++;
		//		if (count > 10)
		//			break;
		//	}

		//	double min_val = 0, max_val = 0;
		//	double maxAll = 0;
		//	//store data
		//	cvCopy(img, doc->m_HeatmapData.map);
		//	char s[25]; sprintf_s(s, "%2d", i);
		//	CFile file(filename+s+".heatmap", CFile::modeCreate | CFile::modeWrite);
		//	CArchive ar(&file, CArchive::store);
		//	doc->m_HeatmapData.Serialize(ar);
		//	ar.Close();
		//	file.Close();

		//	doc->m_HeatmapData.DrawHeatMap(floorImg, false, false);
		//	doc->obstaclemodel.DrawFloor(floorImg, CV_RGB(255,128,0));
		//	cvSaveImage(filename+s+".png", floorImg);
		//}
	}
	return true;
}
// --------------------------------------------------------------------------
void TrackingData::GenerateTrackHeatmap(REFERENCE_TIME start, REFERENCE_TIME stop, int segments) {
	// get file names
	char szFilters[] = "Heatmap Data Files (*.heatmap)|*.heatmap|Image Files(*.png, *.bmp)|*.png;*.bmp|AVI Files (*.avi)|*.avi|XML Files (*.xml)|*.xml|X3D Files (*.x3d)|*.x3d|All Files (*.*)|*.*||";
	CFileDialog dlg(FALSE, ".heatmap", "Heatmap Data 01", OFN_OVERWRITEPROMPT |OFN_PATHMUSTEXIST | OFN_HIDEREADONLY, szFilters, NULL);
	char title[]= {"Save Heatmap Data As..."};
	dlg.m_ofn.lpstrTitle = title;
	CString filename = "";
	if (dlg.DoModal() == IDOK)
		filename = dlg.GetPathName();
	else
		return;

	// start length operation with a progress bar
	GENHEATDATA para;
	para.td = this; para.start = start; para.stop = stop; para.segments = segments;
	para.filename = filename; para.ext = dlg.GetFileExt();
	CUPDialog	Dlg(NULL, RenderHeatmap, &para);
	INT_PTR nResult = Dlg.DoModal();
}
// --------------------------------------------------------------------------
void TrackingData::GenerateDwellHeatmap() {
	// for each body path, draw a circle on the floor where it was; and add it to the heatmap image
	// NOTE: due to scaling, circle becomes an ellipse
	cvZero(doc->m_HeatmapData.map);
	cvZero(doc->m_HeatmapData.count);
	ShowProgressDialog("Generating dwell heatmap", data.GetCount());
	POSITION pos = data.GetStartPosition();
	int count = 1;
	while (pos) {
		StepProgressDialog((CString)String::Format("Processing path {0} out of {1}", count, data.GetCount()));
		int i; BodyPath *bp;
		data.GetNextAssoc(pos, i, bp);

		// exclude employees
		CString employee="";
		bp->tags.Lookup("Employee", employee);
		if (employee == "Yes")
			continue;

		bp->GenerateDwellHeatmap(doc);
		count++;
	}
	HideProgressDialog();
}
// --------------------------------------------------------------------------
void TrackingData::DrawTracksFloor(IplImage* frame, CvScalar color) {
	POSITION pos = data.GetStartPosition();
	while (pos) {
		StepProgressDialog();
		int i; BodyPath *bp;
		data.GetNextAssoc(pos, i, bp);
		bp->DrawTrackFloor(frame, colorFromID(bp->id), doc, 1);
	}
}
// --------------------------------------------------------------------------
void TrackingData::DrawTimeTagsFloor(IplImage* frame, CvScalar color) {
	POSITION pos = data.GetStartPosition();
	while (pos) {
		StepProgressDialog();
		int i; BodyPath *bp;
		data.GetNextAssoc(pos, i, bp);
		bp->DrawTimeTagsFloor(frame, colorFromID(bp->id), doc, "Product Interaction");
	}
}
// -------------  Methods with pop-up prompts -------------------------------
// --------------------------------------------------------------------------
void TrackingData::EditInfoID( int id )
{
	BodyPath *bp;
	if (data.Lookup(id, bp)) {
		// load tag info from BodyPath
		NameValueCollection^ coll = gcnew NameValueCollection();
		POSITION pos;
		CString key, value;
		for( pos = bp->tags.GetStartPosition(); pos != NULL; )
		{
			bp->tags.GetNextAssoc(pos, key, value);
			String ^skey = gcnew String(key);
			String ^svalue = gcnew String(value);
			coll->Add(skey, svalue);
		}
		// Show Dialog
		NewVision::PedestrianTags ^infoDlg = gcnew NewVision::PedestrianTags(coll);
		infoDlg->Text = String::Format("Information for ID = {0}", id);
		if (infoDlg->ShowDialog() == ::DialogResult::OK) {
			// save tag info to BodyPath
			NameValueCollection^ coll = gcnew NameValueCollection();
			infoDlg->ExportTags(coll);
			bp->tags.RemoveAll();
			for (int i=0;i<coll->Count;i++) 
				bp->tags.SetAt(CString(coll->GetKey(i)), CString(coll->GetValues(i)[0]));
		}
		delete infoDlg;
		delete coll;
	}
}
// --------------------------------------------------------------------------
void TrackingData::MergeID( int id1 )
{
	String ^s = String::Format("Merge ID = {0} with: ", id1);
	CTextEntryDlg dlgTextEntry;
	int id2 = -1;	
	if (dlgTextEntry.Show(NULL, "Merge tracks", CString(s)) != IDOK)
		return;
	CString strResult = dlgTextEntry.GetText();
	id2 = _atoi64(strResult);

	// merge in tracking history
	if (!MergeID(id2, id1)) {
		AfxMessageBox("Track to merge with not found!", MB_ICONINFORMATION | MB_OK);
		return;
	}

	// replace id1's in current tracking
	doc->bodymodel.body.ChangeID(id1, id2);
	doc->bodymodel.body_deleted.ChangeID(id1, id2);

	// if ID was the last one, then update last_id
	if (id1 == doc->bodymodel.last_id && id2 < doc->bodymodel.last_id)
		doc->bodymodel.last_id--;
	if (id2 > doc->bodymodel.last_id)
		doc->bodymodel.last_id = id2;
}
// --------------------------------------------------------------------------
void TrackingData::AppendID( int id1 )
{
	String ^s = String::Format("Append ID = {0} as a new segment to: ", id1);
	CTextEntryDlg dlgTextEntry;
	if (dlgTextEntry.Show(NULL, "Append segment", CString(s)) != IDOK)
		return;

	CString strResult = dlgTextEntry.GetText();
	int id2 = _atoi64(strResult);

	// append in tracking history
	if (!AppendID(id2, id1)) {
		AfxMessageBox("Track to append to not found!", MB_ICONINFORMATION | MB_OK);
		return;
	}

	// replace id1's in current tracking
	doc->bodymodel.body.ChangeID(id1, id2);
	doc->bodymodel.body_deleted.ChangeID(id1, id2);

	// if ID was the last one, then update last_id
	if (id1 == doc->bodymodel.last_id && id2 < doc->bodymodel.last_id)
		doc->bodymodel.last_id--;
	if (id2 > doc->bodymodel.last_id)
		doc->bodymodel.last_id = id2;
}
// --------------------------------------------------------------------------
void TrackingData::DeleteID( int id )
{

}
// --------------------------------------------------------------------------
void TrackingData::ShiftTime(int frames) {
	for (POSITION pos1 = data.GetStartPosition();pos1;) {
		// get a track
		int id1; BodyPath *bp;
		data.GetNextAssoc(pos1, id1, bp);
		bp->ShiftTime(frames);
	}
}
// --------------------------------------------------------------------------
void TrackingData::ReNumberIDs()
{
	//while (POSITION pos = data.GetStartPosition();pos;) {
	//	int i; BodyPath *p;
	//	data.GetNextAssoc(pos, i, p);
	//	all_bodies->Add(p);
	//}
}
// --------------------------------------------------------------------------
void TrackingData::AutoMerge( int maxFrames, double maxDist, double minOtherDist )
{
	// Merges all tracks that ended and began near the same time and the same place
	CMap<int, int, int, int> keysToDelete;
	int dummy, nMerges = 0;
	for (POSITION pos1 = data.GetStartPosition();pos1;) {
		// get a track
		int id1; BodyPath *b1;
		data.GetNextAssoc(pos1, id1, b1);
		// if path is marked for deletion
		if (keysToDelete.Lookup(id1, dummy))
			continue;
		int first1, last1, first2, last2;
		b1->GetFirstAndLastVisible(first1, last1);
		
		// for all other tracks
		bool othersFound = false;
		BodyPath *bClosest1 = NULL, *bClosest2 = NULL;
		double minDistSoFar = DBL_MAX;
		for (POSITION pos2 = data.GetStartPosition();pos2;) {
			int id2; BodyPath *b2;
			data.GetNextAssoc(pos2, id2, b2);
			if (id2 == id1)
				continue;
			// if path is marked for deletion
			if (keysToDelete.Lookup(id2, dummy))
				continue;
			b2->GetFirstAndLastVisible(first2, last2);
			// find first and second closest to the tail of b1
			if (first2-last1 < maxFrames && first2-last1 > 0) {
				double dd = d(b2->GetAt(first2-b2->startFrame).p, b1->GetAt(last1-b1->startFrame).p);
				if (dd < maxDist) {
					if (!bClosest1)
						bClosest1 = b2;
					else if (dd < minDistSoFar) {
						bClosest1 = b2;
						minDistSoFar = dd;
					}
					else if (dd < minOtherDist) {
						othersFound = true;
						break;
					}
				}	
			}
		}
		// no close tracklets found or others close-by  => continue
		if (!bClosest1 || othersFound)
			continue;

		nMerges++;
		b1->Merge(bClosest1);
		keysToDelete[bClosest1->id] = 1;
		StepProgressDialog();
	}

	for (POSITION pos1 = keysToDelete.GetStartPosition();pos1;) {
		int i, dummy;
		keysToDelete.GetNextAssoc(pos1, i, dummy);
		data.RemoveKey(i);
	}
	AfxMessageBox(CString(Convert::ToString(nMerges, 10)));
}


//================================================================================
//================================================================================
// --------------------------------------------------------------------------
HeatmapData::~HeatmapData()
{

}
// --------------------------------------------------------------------------
HeatmapData::HeatmapData()
{
	m_initialized = false;
}
// --------------------------------------------------------------------------
void HeatmapData::Initialize(CNewVisionDoc* doc) {
	m_initialized = true;
	this->doc = doc;
	CvSize sz = cvSize(doc->floormodel.w, doc->floormodel.h);
	map   = cvCreateImage(sz, IPL_DEPTH_32F, 1);	
	count = cvCreateImage(sz, IPL_DEPTH_32F, 1);	
	maptemp   = cvCreateImage(sz, IPL_DEPTH_32F, 1);	
	counttemp = cvCreateImage(sz, IPL_DEPTH_32F, 1);	
	m_sourcename = doc->trackermodel.m_sourcename;
	m_startTime = doc->trackermodel.m_startTime;
}
// --------------------------------------------------------------------------
void HeatmapData::DeInitialize() {
	if (!m_initialized)
		return;
	cvReleaseImage(&map);
	cvReleaseImage(&count);
	cvReleaseImage(&maptemp);
	cvReleaseImage(&counttemp);
}
// --------------------------------------------------------------------------
void HeatmapData::Put(int nFrame) {
}
// --------------------------------------------------------------------------
void HeatmapData::Get(int nFrame) {
}
// --------------------------------------------------------------------------
void HeatmapData::Serialize(CArchive& ar) {
	if (ar.IsStoring()) {
		ar << m_sourcename << doc->trackermodel.m_startTime;
	}
	else {
		ar >> m_sourcename >> doc->trackermodel.m_startTime;
	}
	OpenCVExtensions::Image::Serialize(map, ar);
	OpenCVExtensions::Image::Serialize(count, ar);
}
// --------------------------------------------------------------------------
void HeatmapData::ImportFromCVML(CString fname) {
}
// --------------------------------------------------------------------------
void HeatmapData::ExportToCVML(CString fname) {
}
// --------------------------------------------------------------------------
void HeatmapData::ImportFromCSV(CString fname) {	
}
// --------------------------------------------------------------------------
void HeatmapData::ExportToCSV(CString fname) {
}
// --------------------------------------------------------------------------
void HeatmapData::ExportToX3DStatic(CString fname, double scale, bool useCount ) {
#if USE_X3DLIBRARY
	X3DDriver xd;
	// x <-> y to convert from left-coordinate system to right coordinate system
	xd.ClearScene();
	xd.SetViewPoint();
	
	CString coords="";
	double min_val = 0, max_val = 0;
	cvMinMaxLoc(count, &min_val, &max_val);
	int c = 1;
	SetProgressDialogRange(0, count->width*count->height);
	int STEP = 5;
	for (int y=0; y < map->height; y+=STEP) {
		StepProgressDialog((CString)String::Format("Point {0} out of {1}", c, count->width*count->height));
		for (int x=0; x < map->width; x+=STEP)
		{
			float value = ((float*)(count->imageData + count->widthStep*y))[x];
			CvPoint3D32f p = doc->floormodel.coordsFloor2Real(cvPoint(x, y));
			// index ranges from 0 to 1
			double z = scale*(value-min_val)/(max_val-min_val);
			CString s; s.Format("%.2f %.2f %.2f ", p.y, p.x, z);
			coords += s;
			c++;
		}	
	}
	xd.AddNurbsPatchSurface(coords, map->width/STEP+1, map->height/STEP+1);
	// --------- Add floormap
	doc->floormodel.ExportToX3D(xd);
	// --------- Add obstacles
	doc->obstaclemodel.ExportToX3D(xd);
	xd.SaveX3D(fname);
#endif
}
// --------------------------------------------------------------------------
void HeatmapData::ExportToX3D( CString fname, bool useCount ) {
#if USE_X3DLIBRARY
	//doc->m_HeatmapData.ExportToX3D(filename);
#endif
}
// --------------------------------------------------------------------------
void HeatmapData::DrawHeatMap(IplImage* frame, bool useCount, bool useColorTable, double maxAll) {
	if (map->width != frame->width || map->height != frame->height)
		return;

	// use colors from HEAT_COLOR array
	if (useColorTable) {
		double min_val = 0, max_val = 0;
		cvMinMaxLoc(map, &min_val, &max_val);
		//min_val = 0; max_val = 600;
		//min_val = 1000;
		//max_val = max_val;
		//cvConvertScale(map, motionmapscaled, 1./max_val);
		//cvConvertImage(motionmapscaled, frame);
		//cvZero(frame);
		for (int y=0; y<frame->height; y++)
			for (int x=0; x<frame->width; x++)
			{
				int a = sizeof(float);
				float value = ((float*)(map->imageData + map->widthStep*y))[x];
				int index = (int)(NTEMPERATURES*(value-min_val-1)/(max_val-min_val));
				// LOG scale
				//int index = (int)((NTEMPERATURES*log(value)-log(min_val))/log(max_val-log(min_val)));
				//index = max(0, min(index, NTEMPERATURES-1));
				//p->B = (unsigned char)(255*(value-min_val)/(max_val-min_val));
				//p->G = 255-p->B;
				if (index>0)
					OpenCVExtensions::Image::setPixel(frame, &HEAT_COLOR[index-1], x, y);
			}
		drawText(max_val, cvPoint(frame->width-100, 10), frame, CV_RGB(0,0,0), CV_RGB(235,255,235), (float)1.2);
		drawText(min_val, cvPoint(frame->width-100, 35), frame, CV_RGB(0,0,0), CV_RGB(235,255,235), (float)1.2);
	}
	else {
		cvCopy(map, maptemp);
		cvCopy(count, counttemp);
		if (useCount) {
			// replace 0 counts with 1s
			cvMaxS(counttemp, 1, counttemp);
			cvDiv(maptemp, counttemp, maptemp);
		}
		double min_val = 0, max_val = 0;
		if (maxAll == 0)
			cvMinMaxLoc(maptemp, &min_val, &max_val);
		else
			max_val = maxAll;
		for (int y=0; y<frame->height; y++)
			for (int x=0; x<frame->width; x++)
			{
				float value = ((float*)(maptemp->imageData + maptemp->widthStep*y))[x];
				// index ranges from 0 to 1
				double index = (value-min_val)/(max_val-min_val);
				Pixel color(0, 0, 0);
				if (index > 0.75) {
					color.R = 255;
					color.G = 255*(1-4*(index-0.75));
				}
				else if (index > 0.5) {
					color.R = 255*4*(index-0.5);
					color.G = 255;
				}
				else if (index > 0.25) {
					color.G = 255;
					color.B = 255*(1-4*(index-0.25));
				}
				else if (index > 0.05) {
					color.G = 255*4*(index);
					color.B = 255;
				}

				// include only values with signification samples
				float n = ((float*)(count->imageData + count->widthStep*y))[x];
				if (value > min_val && (!useCount || n > 5))
					OpenCVExtensions::Image::setPixel(frame, &color, x, y);
			}
	}
}
//================================================================================
//================================================================================
// --------------------------------------------------------------------------
void ActivityData::Initialize(CNewVisionDoc* doc) {
	this->doc = doc;
	m_initialized = true;
}
// --------------------------------------------------------------------------
void ActivityData::DeInitialize() {
	if (!m_initialized)
		return;
}
// --------------------------------------------------------------------------
void ActivityData::Put(int nFrame) {
}
// --------------------------------------------------------------------------
void ActivityData::Clear() {
	sActivity.RemoveAll();
	sEvent.RemoveAll();
	SwarmEvent::last_id = 0;
	SwarmActivity::last_id = 0;
}
// --------------------------------------------------------------------------
void ActivityData::Get(int nFrame) {
}
// --------------------------------------------------------------------------
void ActivityData::Serialize(CArchive& ar) {
	// ------------ Export Swarm Event Data ------------------
	ar.IsStoring() ?
		ShowProgressDialog("Exporting event data to .activity file...", sEvent.GetCount()) :
		ShowProgressDialog("Importing event data from .activity file...", sEvent.GetCount());
	sEvent.Serialize(ar);
	HideProgressDialog();

	// ------------ Export Swarm Activity Data ------------------
	ar.IsStoring() ?
		ShowProgressDialog("Exporting activity data to .activity file...", sActivity.GetCount()) :
		ShowProgressDialog("Importing activity data from .activity file...", sActivity.GetCount());
	sActivity.Serialize(ar);
	HideProgressDialog();
}
// --------------------------------------------------------------------------
void ActivityData::ImportFromCVML(CString fname) {
}
// --------------------------------------------------------------------------
void ActivityData::ExportToCVML(CString fname) {
}
// --------------------------------------------------------------------------
void ActivityData::ImportFromCSV(CString fname) {	
}
// --------------------------------------------------------------------------
void ActivityData::ExportToCSV(CString fname) {
	StreamWriter^ sr = File::CreateText(gcnew String(fname));

	//POSITION posE = sEvent.GetStartPosition();
	//while (posE) {
	//	int id; SwarmEvent *ev;
	//	sEvent.GetNextAssoc(posE, id, ev);
	//	POSITION pos = ev->actors.GetStartPosition();
	//	CvPoint2D32f floor = cvPoint2D32f(0, 0);
	//	int aCount = 0;
	//	System::String ^header
	//		= System::String::Format("{2}, {0}, {1}", ev->start, ev->stop, ev->id)
	//		+ System::String::Format(", {0:f3}", ev->p);

	//	while (pos) {
	//		int id; double d;
	//		ev->actors.GetNextAssoc(pos, id, d);
	//		BodyPath *bp;
	//		if (doc->m_TrackingData.data.Lookup(id, bp)) {
	//			for (int i = ev->start;i<=ev->stop;i++) {
	//				floor.x += bp->GetAt(i-bp->startFrame).p.x;
	//				floor.y += bp->GetAt(i-bp->startFrame).p.y;
	//			}
	//			//floor.x /= (ev->stop - ev->start + 1);
	//			//floor.y /= (ev->stop - ev->start + 1);
	//			//echo(String::Format("{0}", (ev->stop - ev->start + 1)));
	//			aCount++;
	//		}
	//	}
	//	floor.x /= aCount;
	//	floor.y /= aCount;
	//	String ^s = System::String::Format(", {0:f3}, {1:f3}", floor.x, floor.y);
	//	sr->WriteLine(header+s);
	//}

	//// ------------ Export Swarm Event Data ------------------
	//ShowProgressDialog("Exporting event data to CSV file...", sEvent.GetCount());
	//sr->WriteLine("EventName, EventID, Type, Start-Stop, Confidence, ID0, ID1, ID2, ..., ..."); 
	//POSITION posE = sEvent.GetStartPosition();
	//while (posE) {
	//	int id; SwarmEvent *ev;
	//	sEvent.GetNextAssoc(posE, id, ev);
	//	sr->WriteLine(ev->ToString());
	//	StepProgressDialog();
	//}
	//HideProgressDialog();

	// ------------ Export Swarm Activity Data ------------------
	ShowProgressDialog("Exporting activity data to CSV file...", sActivity.GetCount());
	sr->WriteLine("ActivityName, ActivityID, ActivityType, Start-Stop, Confidence, ID0, ID1, ID2, ..., ..."); 
	POSITION posA = sActivity.GetStartPosition();
	while (posA) {
		int id; SwarmActivity *ac;
		sActivity.GetNextAssoc(posA, id, ac);
		sr->WriteLine(ac->ToString());
		StepProgressDialog();
	}
	HideProgressDialog();
	sr->Close();
}
// --------------------------------------------------------------------------
int ActivityData::GetLastEventID() {
	POSITION pos = sEvent.GetStartPosition();
	int lastID = 0;
	while (pos) {
		int id; SwarmEvent *p;
		sEvent.GetNextAssoc(pos, id, p);
		lastID = max(id, lastID);
	}
	return lastID;
}
// --------------------------------------------------------------------------
int ActivityData::GetLastActivityID() {
	POSITION pos = sActivity.GetStartPosition();
	int lastID = 0;
	while (pos) {
		int id; SwarmActivity *p;
		sActivity.GetNextAssoc(pos, id, p);
		lastID = max(id, lastID);
	}
	return lastID;
}
