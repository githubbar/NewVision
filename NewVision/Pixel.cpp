#include "StdAfx.h"
#include "pixel.h"

using namespace OpenCVExtensions;

Pixel::Pixel(void)
{
}

Pixel::Pixel(Pixel &that) {
	this->R = that.R;
	this->G = that.G;
	this->B = that.B;
}

Pixel::Pixel(unsigned char r, unsigned char g, unsigned char b) {
	R = r;
	G = g;
	B = b;
}

Pixel::~Pixel(void)
{
}