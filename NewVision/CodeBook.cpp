#include "StdAfx.h"
#include "CodeBook.h"

// --------------------------------------------------------------------------
CodeBook::CodeBook() {

}
// --------------------------------------------------------------------------
double CodeBook::MatchColorProbabilistic(Pixel* p, int frame, double t_cd, double alpha, double beta, int minAge) {
	// find the codeword that matches the pixel the closest and return the closeness to it
	double minDist = 1;
	for (int i=0;i<color.GetSize();i++) {
		if (color.GetAt(i).age > minAge) {
			double dist = color.GetAt(i).MatchProbabilistic(p, frame, t_cd, alpha, beta, minAge);
			if (dist < minDist)
				minDist = dist;
		}
	}
	return minDist;
}
// --------------------------------------------------------------------------
bool CodeBook::MatchColor(Pixel* p, int frame, double t_cd, double alpha, double beta, int minAge) {
	// find the codeword that matches the pixel and return true
	for (int i=0;i<color.GetSize();i++) {
		// minAge should be a new variable in BG dialog box !!!
		if (color.GetAt(i).age > minAge && color.GetAt(i).Match(p, frame, t_cd, alpha, beta, minAge)) {
			return true;
		}
	}
	// otherwise, return false
	return false;
}
// --------------------------------------------------------------------------
bool CodeBook::MatchThermalByFactor(int p, int frame, double alpha, double beta, int minAge) {
	// find the codeword that matches the pixel and return true
	for (int i=0;i<imgThermal.GetSize();i++) {
		if (imgThermal.GetAt(i).age > minAge && imgThermal.GetAt(i).MatchByFactor(p, frame, alpha, beta, minAge)) {
			return true;
		}
	}
	// otherwise, return false
	return false;
}
// --------------------------------------------------------------------------
bool CodeBook::MatchThermalByDeviation(int p, int frame, int m_ThermalMin, int m_ThermalMax, int minAge) {
	// find the codeword that matches the pixel and return true
	for (int i=0;i<imgThermal.GetSize();i++) {
		if (imgThermal.GetAt(i).age > minAge && imgThermal.GetAt(i).MatchByDeviation(p, frame, m_ThermalMin, m_ThermalMax, minAge)) {
			return true;
		}
	}
	// otherwise, return false
	return false;
}
// --------------------------------------------------------------------------
double CodeBook::MatchThermalByDeviationProbabilistic(int p, int frame, int m_ThermalMin, int m_ThermalMax, int minAge) {
	// find the codeword that matches the pixel and return true
	double minDist = DBL_MAX;
	for (int i=0;i<imgThermal.GetSize();i++) {
		if (imgThermal.GetAt(i).age > minAge) {
			double dist = imgThermal.GetAt(i).MatchProbabilistic(p, frame, m_ThermalMin, m_ThermalMax, minAge);
			if (dist < minDist)
				minDist = dist;
		}
	}
	// otherwise, return false
	return minDist;
}
// --------------------------------------------------------------------------
void CodeBook::UpdateColor(Pixel* p, int frame, double t_cd, double alpha, double beta) {
	// update the codeword that matches the pixel
	for (int i=0;i<color.GetSize();i++)
		if (color.GetAt(i).Match(p, frame, t_cd, alpha, beta)) {
			color.GetAt(i).Update(p, frame);
			return;
		}
		else {
			// update stale counter only when the region is being updated (i.e. not exluded by body mask)
			color.GetAt(i).stale++;
		}
	// otherwise, create a new codeword
	CodeWordColor c = CodeWordColor(p, frame);
	color.Add(c);
}
// --------------------------------------------------------------------------
void CodeBook::UpdateThermal(int p, int frame, int m_ThermalMin, int m_ThermalMax) {
	// update the codeword that matches the pixel
	for (int i=0;i<imgThermal.GetSize();i++) {
		if (imgThermal.GetAt(i).MatchByDeviation(p, frame, m_ThermalMin, m_ThermalMax)) {
			imgThermal.GetAt(i).Update(p, frame);
			return;
		}
		else {
			// update stale counter only when the region is being updated (i.e. not exluded by body mask)
			imgThermal.GetAt(i).stale++;
		}
	}
	// otherwise, create a new codeword
	CodeWordThermal c = CodeWordThermal(p, frame);
	imgThermal.Add(c);
}
// --------------------------------------------------------------------------
int CodeBook::CountOlderThan(int a) {
	int mature=0;
	for (int i=0;i<color.GetSize();i++) {
		if (color.GetAt(i).age > a) 
			mature++;
	}
	return mature;

}
// --------------------------------------------------------------------------
void CodeBook::RemoveStale(int cutoff) {
	for (int i=0;i<color.GetSize();i++)
		if (color.GetAt(i).stale > cutoff)
			color.RemoveAt(i);
}
// --------------------------------------------------------------------------
void CodeBook::GetRGB(Pixel* p) {
	p->R = 0; p->G = 0; p->B = 0;
	double age = 0, r = 0, g = 0, b = 0;
	for (int i=0;i<color.GetSize();i++) {
		CodeWordColor c = (CodeWordColor)color.GetAt(i);
		r += c.R*c.age;
		g += c.G*c.age;
		b += c.B*c.age;
		age += c.age;
	}
	if (age > 0) {
		p->R = (byte)(r/age); p->G = (byte)(g/age); p->B = (byte)(b/age);
	}
}
// --------------------------------------------------------------------------
template <> void AFXAPI SerializeElements <CodeBook> ( CArchive& ar, CodeBook* p, INT_PTR nCount )
{
	for ( int i = 0; i < nCount; i++, p++ ) {
        p->color.Serialize(ar);
		p->imgThermal.Serialize(ar);
	}
}
// --------------------------------------------------------------------------
