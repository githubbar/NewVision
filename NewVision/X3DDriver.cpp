#include "StdAfx.h"
#include "X3DDriver.h"
#include "Globals.h"
#include <memory>   // std::auto_ptr
#using <mscorlib.dll>
#using <system.dll>
#using <system.configuration.dll>

using namespace System;
using namespace System::IO;
using namespace x3d;

// ----------------------------------------------------------------------------------
X3DDriver::X3DDriver() : x(new X3D(Scene(), x3dVersion::cxx_3_0, profileNames::Immersive))
{
	//try
	//{
	//	x = new X3D(Scene(), x3dVersion::cxx_3_0, profileNames::Immersive);
	//}
	//catch (const xml_schema::exception& e)
	//{
	//	AfxMessageBox("Failed to Create X3D Driver!");
	//	return;
	//}
}
// ----------------------------------------------------------------------------------
X3DDriver::~X3DDriver() {
	//delete x;
}
// ----------------------------------------------------------------------------------
void X3DDriver::LoadX3D(CString fileName) {
	// De-Serialize from file
	using namespace xercesc;
	XMLPlatformUtils::Initialize ();
	try
	{
		map["X3D"].name = "http://www.web3d.org/x3d/specifications";
		map["X3D"].schema = "http://www.web3d.org/x3d/specifications/x3d-3.0.xsd";
		// Deserialize from file
		std::auto_ptr<x3d::X3D> x_temp;
		x_temp = X3D_((LPCTSTR)(fileName));
		//x = x_auto.release();
	}
	catch (const xml_schema::exception& e)
	{
		AfxMessageBox("Failed to de-serialize from X3D file!");
		return;
	}
	XMLPlatformUtils::Terminate();
}
// ----------------------------------------------------------------------------------
void X3DDriver::SaveX3D(CString fileName) {
	// Serialize into file
	using namespace xercesc;
	XMLPlatformUtils::Initialize ();

	try
	{
		//map[""].name = "x3d";
		map["X3D"].name = "http://www.web3d.org/x3d/specifications";
		map["X3D"].schema = "http://www.web3d.org/x3d/specifications/x3d-3.0.xsd";
		XMLFormatTarget *ft;
		ft = new LocalFileFormatTarget ((LPCTSTR)(fileName));
		X3D_(*ft, *(x.get()), map);
		delete ft;
	}
	catch (const xml_schema::exception& e)
	{
		AfxMessageBox("Failed to serialize to X3D file!");
		return;
	}
	// append file content
	//String ^extraXML = File::ReadAllText(gcnew String(fileName+"_temp"));
	//File::AppendAllText(gcnew String(fileName), extraXML);
	XMLPlatformUtils::Terminate();
}
// ----------------------------------------------------------------------------------
void X3DDriver::AddObstacle(int id, CString center, CString size, CString rotation, CString label) {
	Scene::Transform::container &tc = x->Scene().Transform();
	Transform tr;
	// --- Add box
	CString sID; sID.Format("obstacle%d", id);
	tr.DEF() = (LPCTSTR)sID;
	tr.rotation() = SFRotation((LPCTSTR)rotation);
	tr.translation() = SFVec3f((LPCTSTR)center);
	Shape s;
	Box box;
	Appearance a;
	Material m;
	m.ambientIntensity() = SFFloat(0.2f); 
	m.shininess() = SFFloat(0.2f);
	m.transparency() = SFFloat(0.5f);
	m.diffuseColor() = SFVec3f("1 1 0");
	m.emissiveColor() = SFVec3f("0 0 0.3");
	m.specularColor() = SFVec3f("0 0 0");
	a.Material().push_back(m);
	s.Appearance(a);
	box.size() = SFVec3f((LPCTSTR)size);
	s.Box(box);
	tr.Shape().push_back(s);
	tc.push_back(tr);
}
// ----------------------------------------------------------------------------------
void X3DDriver::AddArrow(CString center, CString rotation, CString color, float bottomRadius, float height)
{
	Scene::Transform::container &tc = x->Scene().Transform();
	Transform tr;
	tr.rotation() = SFRotation((LPCTSTR)rotation);
	tr.translation() = SFVec3f((LPCTSTR)center);
	Material m;
	m.ambientIntensity() = SFFloat(0.2f); 
	m.shininess() = SFFloat(1.0f);
	m.transparency() = SFFloat(0.0f);
	m.diffuseColor() = SFVec3f((LPCTSTR)color);
	m.emissiveColor() = SFVec3f("0 0 0");
	m.specularColor() = SFVec3f("0 0 0");
	Appearance a;
	a.Material().push_back(m);
	Cone cone;
	cone.bottomRadius() = SFFloat(bottomRadius);
	cone.height() = SFFloat(height);
	Shape sh;
	sh.Cone(cone);
	sh.Appearance(a);
	tr.Shape().push_back(sh);
	tc.push_back(tr);
}
// ----------------------------------------------------------------------------------
void X3DDriver::AddObstacle(int id, CString coords, CString label) {
	Scene::Shape::container &sc = x->Scene().Shape();
	// --- Add box
	Shape s;
	CString sID; sID.Format("obstacle%d", id);
	s.DEF() = (LPCTSTR)sID;
	IndexedFaceSet ifs;
	// 0 1 2 3 -1 7 6 5 4 -1 1 7 4 2 -1 0 3 5 6 -1 3 2 4 5 -1 6 7 1 0 -1
	ifs.coordIndex() = MFInt32("0 1 2 3 -1 5 4 7 6 -1 1 5 6 2 -1 0 3 7 4 -1 3 2 6 7 -1 4 5 1 0 -1");
	Coordinate co;
	co.point() = MFVec3f((LPCTSTR)coords);
	ifs.Coordinate() = co;
	ifs.solid() = false;
	//ifs.normalIndex() = MFInt32("0 1 2 3 -1 5 4 7 6 -1 1 5 6 2 -1 0 3 7 4 -1 3 2 6 7 -1 4 5 1 0 -1");
	//Normal no;
	//no.vector() = MFVec3f((LPCTSTR)normals);
	//ifs.Normal() = no;

	Appearance a;
	Material m;
	m.ambientIntensity() = SFFloat(0.2f); 
	m.shininess() = SFFloat(0.2f);
	m.transparency() = SFFloat(0.7f);
	m.diffuseColor() = SFVec3f("1 1 0");
	m.emissiveColor() = SFVec3f("0 0 0.3");
	m.specularColor() = SFVec3f("0 0 0");
	a.Material().push_back(m);
	s.Appearance(a);
	s.IndexedFaceSet(ifs);
	sc.push_back(s);
}
// ----------------------------------------------------------------------------------
void X3DDriver::AddTrackAvatar( int id, CString urlAvatar, CString coords, CString times, CString orients, CString otimes, CString tcoords, CString ttimes)
{
	double height = 170, radius = 30;
	Scene::Transform::container &tc = x->Scene().Transform();
	Transform tr;
	CString sID; sID.Format("id%d", id);
	tr.DEF() = (LPCTSTR)sID;
	tr.scale() = SFVec3f("70 70 70");
	tr.rotation() = SFRotation("0 0 1 0");
	tr.center() = SFVec3f("0 0 0");	

	// --- Add avatar transform
	Transform sub1;
	Inline c;
	MFString mfurl;
	mfurl.push_back((LPCTSTR)urlAvatar);
	c.url() = mfurl;
	sub1.Inline().push_back(c);
	sub1.rotation() = SFRotation("1 0 0 1.7");
	Group gr;
	gr.Transform().push_back(sub1);


	// --- Add id text
	Transform sub2;
	sub2.translation() = SFVec3f("-0.2 0.3 0.2");
	sub2.rotation() = SFRotation("1 0 0 1.7");
	x3d::Text txt;
	Shape s;
	MFString idMFS;
	CString textID; textID.Format("%d", id);
	idMFS.push_back((LPCTSTR)textID);
	txt.string(idMFS);
	txt.solid() = false;
	FontStyle fs; //fs.size() = 2;
	txt.FontStyle(fs);
	s.Text(txt);
	Appearance a;
	Material m;
	m.ambientIntensity() = SFFloat(1); 
	m.shininess() = SFFloat(1);
	m.transparency() = SFFloat(0.0);
	m.diffuseColor() = SFVec3f("0 1 0");
	m.emissiveColor() = SFVec3f("0 0 0.3");
	m.specularColor() = SFVec3f("0 0 0");
	a.Material().push_back(m);
	s.Appearance(a);
	sub2.Shape().push_back(s);
	gr.Transform().push_back(sub2);
	tr.Group().push_back(gr);


	/*<Transform DEF='MYTRANS' translation="-0.2 0 0">
	<Shape>
	<Appearance>
	<Material ambientIntensity='1' diffuseColor='0 1 0'/>
	</Appearance>
	<Text DEF='MYTEXT' string='3'>
	<FontStyle DEF='MYFONT' style='BOLD'/>
	</Text>
	</Shape>
	</Transform>*/

	// --- Add position interpolator
	PositionInterpolator pi;
	pi.DEF() = (LPCTSTR)(sID+"_pi");
	pi.key() = MFFloat((LPCTSTR)times);
	pi.keyValue() = MFVec3f((LPCTSTR)coords);
	tr.PositionInterpolator().push_back(pi);

	// --- Add orientation interpolator
	OrientationInterpolator oi;
	oi.DEF() = (LPCTSTR)(sID+"_oi");
	oi.key() = MFFloat((LPCTSTR)times);
	oi.keyValue() = MFVec3f((LPCTSTR)orients);
	tr.OrientationInterpolator().push_back(oi);

	// --- Add visibility sequences
	IntegerSequencer viz;
	viz.DEF() = (LPCTSTR)(sID+"_viz");
	viz.key() = MFFloat((LPCTSTR)ttimes);
	viz.keyValue() = MFVec3f((LPCTSTR)tcoords);
	tr.IntegerSequencer().push_back(viz);

	// --- Route events
	ROUTE r1((LPCTSTR)("myTimeSensor"), "fraction_changed", (LPCTSTR)(sID+"_pi"), "set_fraction");
	ROUTE r2((LPCTSTR)(sID+"_pi"), "value_changed", (LPCTSTR)sID, "set_translation");
	ROUTE o1((LPCTSTR)("myTimeSensor"), "fraction_changed", (LPCTSTR)(sID+"_oi"), "set_fraction");
	ROUTE o2((LPCTSTR)(sID+"_oi"), "value_changed", (LPCTSTR)sID, "set_rotation");
	ROUTE v1((LPCTSTR)("myTimeSensor"), "fraction_changed", (LPCTSTR)(sID+"_viz"), "set_fraction");
	ROUTE v2((LPCTSTR)(sID+"_viz"), "value_changed", (LPCTSTR)(sID+"_switch"), "set_whichChoice");

	tr.ROUTE().push_back(r1);
	tr.ROUTE().push_back(r2);
	tr.ROUTE().push_back(o1);
	tr.ROUTE().push_back(o2);
	tr.ROUTE().push_back(v1);
	tr.ROUTE().push_back(v2);

	// Add visibility switch
	Transform tr2;
	Switch sw2;
	sw2.DEF() = (LPCTSTR)(sID+"_switch");
	sw2.Transform().push_back(tr);
	tr2.Switch().push_back(sw2);
	tr2.translation() = SFVec3f("0 0 120");
	tc.push_back(tr2);
}
// ----------------------------------------------------------------------------------
void X3DDriver::AddTrackCurve(int id, bool female, CString coords) {
	Scene::Shape::container &shapes = x->Scene().Shape();
	// Create extrusion shape
	Shape sh;
	//CString sID; sID.Format("id%d", id);
	//sh.DEF() = (LPCTSTR)sID;
	Extrusion extrusion;
	extrusion.spine() = MFVec3f((LPCTSTR)coords);
	// circlular cross-section
	extrusion.crossSection() = MFVec2f("1.00 0.00, 0.92 -0.38, 0.71 -0.71, 0.38 -0.92, 0.00 -1.00, -0.38 -0.92, -0.71 -0.71, -0.92 -0.38, -1.00 -0.00, -0.92 0.38, -0.71 0.71, -0.38 0.92, 0.00 1.00, 0.38 0.92, 0.71 0.71, 0.92 0.38, 1.00 0.00");
	extrusion.creaseAngle() = SFFloat(PI);
	sh.Extrusion() = extrusion;
	Appearance a;
	Material m;
	srand(id);
	//double RED = 1.0*rand()/RAND_MAX, GREEN = 1.0*rand()/RAND_MAX, BLUE = 1.0*rand()/RAND_MAX;
	//char s[20];
	//sprintf(s, "%.2f %.2f %.2f,", RED, GREEN, BLUE);
	if (female)
		m.diffuseColor() = SFVec3f("1 0 0");
	else
		m.diffuseColor() = SFVec3f("0 0 1");
	m.ambientIntensity() = SFFloat(1); 
	m.shininess() = SFFloat(1);
	a.Material().push_back(m);
	sh.Appearance(a);
	shapes.push_back(sh);
}
// ----------------------------------------------------------------------------------
// This one uses NurbsCurve not yet supported by any viewers 
//void X3DDriver::AddTrackCurve(int id, CString coords, CString tcoords) {
//	Scene::Transform::container &tc = x->Scene().Transform();
//	Transform tr;
//	CString sID; sID.Format("id%d", id);
//	tr.DEF() = (LPCTSTR)sID;
//	tr.rotation() = SFRotation("0 0 1 0");
//	tr.center() = SFVec3f("0 0 0");	
//
//	//geometry NurbsCurve {
//	//	knot [0, 0, 0, .2, .4, .6, 1, 1, 1] 
//	//	tessellation 50
//	//		controlPoint [0 0 0, 1 1 0, 0 2 0, 2 3 0, -1 4 0, 5 5 0]
//	//	}
//	Shape sh;
//	NurbsCurve curve;
//	Coordinate ccs;
//	ccs.point() = MFVec3f((LPCTSTR)coords);
//	curve.Coordinate(ccs);
//	Appearance a;
//	Material m;
//	m.ambientIntensity() = SFFloat(1); 
//	m.shininess() = SFFloat(1);
//	m.transparency() = SFFloat(0.0);
//	m.diffuseColor() = SFVec3f("0 1 0");
//	m.emissiveColor() = SFVec3f("0 0 0.3");
//	m.specularColor() = SFVec3f("0 0 0");
//	a.Material().push_back(m);
//	sh.Appearance(a);
//	sh.NurbsCurve() = curve;
//	tr.Shape().push_back(sh);
//	tc.push_back(tr);
//}
// ----------------------------------------------------------------------------------
void X3DDriver::AddTimeSensor(CString name, CString duration) { 
	// --- Add time sensor
	TimeSensor ts;
	ts.DEF() = (LPCTSTR)(name);
	ts.loop() = false;
	ts.cycleInterval() = SFTime((LPCTSTR)duration);
	x->Scene().TimeSensor().push_back(ts);
	// add touch sensor
	TouchSensor touch;
	touch.DEF() = (LPCTSTR)("touch");
	x->Scene().TouchSensor().push_back(touch);
	ROUTE r1("touch", "touchTime", (LPCTSTR)name, "set_startTime");
	x->Scene().ROUTE().push_back(r1);
}
// ----------------------------------------------------------------------------------
void X3DDriver::ClearScene() {
	// --- Clear the scene
	x->Scene().Background().clear();
	x->Scene().Transform().clear();
	x->Scene().Shape().clear();
	x->Scene().Viewpoint().clear();
}
// ----------------------------------------------------------------------------------
void X3DDriver::SetViewPoint() {
	//// --- Set Viewpoint
	Scene::Viewpoint::container &vc = x->Scene().Viewpoint();
	Viewpoint v;
	v.position() = SFVec3f("0 0 2300");
	vc.push_back(v);
	//<Viewpoint description="The World" orientation="0 1 0 1.57" position="10 0 0"/> 

	//// --- Set Navigation Info to EXAMINE
	Scene::NavigationInfo::container &nc = x->Scene().NavigationInfo();
	NavigationInfo n;
	MFString ntype; ntype.push_back("EXAMINE");
	n.type() = MFString(ntype);
	n.speed() = 10;
	nc.push_back(n);
	//<NavigationInfo type='"EXAMINE"' transitionType='"ANIMATE"' transitionTime='1.0' transitionComplete=''/> 
}
// ----------------------------------------------------------------------------------
void X3DDriver::AddFloorImage(CString imageFile, CString minmaxExt) {
	// --- Add Floor Plane
	Shape sh;
	Appearance a;
	IndexedFaceSet ifs;
	ImageTexture it;
	ifs.coordIndex() = MFInt32("0 1 2 3");
	ifs.texCoordIndex() = MFInt32("0 1 2 3");
	Coordinate co;
	co.point() = MFVec3f((LPCTSTR)minmaxExt);
	ifs.Coordinate() = co;
	TextureCoordinate tco;
	tco.point() = MFVec2f("0 1, 0 0, 1 0, 1 1");
	ifs.TextureCoordinate() = tco;
	MFString bgMFS; bgMFS.push_back((LPCTSTR)imageFile);
	it.url() = bgMFS;
	a.ImageTexture().push_back(it);
	Material m;
	m.transparency() = SFFloat(0.5f);
	a.Material().push_back(m);
	sh.Appearance(a);
	sh.IndexedFaceSet() = ifs;
	x->Scene().Shape().push_back(sh);
}
// ----------------------------------------------------------------------------------
void X3DDriver::AddSound( CString fileName )
{
	//<Sound intensity='1' minBack='5' minFront='5'>
	//	<AudioClip description='emitter1' loop='true' pitch='1.189' url='Axel F.mp3'/>
	//	</Sound>
}
// ----------------------------------------------------------------------------------
void X3DDriver::AddNurbsPatchSurface(CString coords, int udims, int vdims )
{
	// --- Add Floor Plane
	Shape sh;
	Appearance a;
	NurbsPatchSurface nps;
	Coordinate co;
	co.point() = MFVec3f((LPCTSTR)coords);
	co.containerField() = "controlPoint";
	nps.Coordinate().push_back(co);
	nps.uOrder() = 4;
	nps.vOrder() = 4;
	nps.uTessellation() = 0;
	nps.vTessellation() = 0;
	nps.solid() = false;
	nps.uDimension() = udims;
	nps.vDimension() = vdims;
	
	Material m;
	m.transparency() = SFFloat(0.5f);
	a.Material().push_back(m);
	sh.Appearance(a);
	sh.NurbsPatchSurface() = nps;
	x->Scene().Shape().push_back(sh);

	//<NurbsPatchSurface DEF='NS' containerField='geometry' solid='false' uDimension='5' uOrder='4' uTessellation='30' vDimension='5' vOrder='4' vTessellation='30' uClosed='false' vClosed='false'>
	//	<Coordinate containerField='controlPoint' point='-10 -10 0 -10 -5 0 -10 0 0 -10 5 0 -10 10 0 -5 -10 0 -5 -5 2.5 -5 0 5 -5 5 2.5 -5 10 0 0 -10 0 0 -5 2.5 0 0 5 0 5 2.5 0 10 0 5 -10 0 5 -5 2.5 5 0 15 5 5 2.5 5 10 0 10 -10 0 10 -5 0 10 0 0 10 5 0 10 10 0'/>
	//	</NurbsPatchSurface>
	//	<Appearance>
	//	<ImageTexture
	//	url=' "PearlHarborLowResolution.jpg" "https://savage.nps.edu/Savage/Locations/Hawaii/PearlHarborLowResolution.jpg" '/>
	//	</Appearance>

}
