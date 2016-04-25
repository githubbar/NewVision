#pragma once
#include "Pixel.h"
#include "cxcore.h"
int min(int a, int b);
int max(int a, int b);
// NOTE: by default OpenCV images are BGR, not RGB
namespace OpenCVExtensions
{
	// redo it with templates , for now represents RGB, 3 channel unsigned char image
	class Image : public CObject
	{
	public:
		Image(void);
		virtual ~Image(void);
		static int getSize(IplImage* im) {
			return im->imageSize;
		};
		
		static int getSizeInPixels(IplImage* im) {
			return im->width*im->height;
		};

/*
		static void getPixel(IplImage* im, Pixel* p, int i) {
			// BUG: widthStep != width .... alignment problem
			p->R = im->imageData[i*3];
			p->G = im->imageData[i*3+1];
			p->B = im->imageData[i*3+2];
		};
*/

		static void getPixel(IplImage* im, Pixel* p, int x, int y) {
			p->B = ((uchar*)(im->imageData + im->widthStep*y))[x*3];
			p->G = ((uchar*)(im->imageData + im->widthStep*y))[x*3+1];
			p->R = ((uchar*)(im->imageData + im->widthStep*y))[x*3+2];
		};
/*
		static void setPixel(IplImage* im, Pixel* p, int i) {
			// BUG: widthStep != width .... alignment problem
			im->imageData[i*3]   = p->R;
			im->imageData[i*3+1] = p->G;
			im->imageData[i*3+2]  = p->B;
		};
*/

		static void setPixel(IplImage* im, Pixel* p, int x, int y) {
			((uchar*)(im->imageData + im->widthStep*y))[x*3] = p->B;
			((uchar*)(im->imageData + im->widthStep*y))[x*3+1] = p->G;
			((uchar*)(im->imageData + im->widthStep*y))[x*3+2] = p->R;
		};
		
		static void Highlight(IplImage* srcFrame, IplImage* tgtFrame, IplImage* mask, Pixel* high) {
			Pixel *p = new Pixel();
			for (int x=0; x<mask->width; x++)
			for (int y=0; y<mask->height; y++)
			{
				if (((uchar*)(mask->imageData + mask->widthStep*y))[x] == 1) {
					getPixel(srcFrame, p, x, y);
					p->R = min(255, p->R + high->R);
					p->G = min(255, p->G + high->G);
					p->B = min(255, p->B + high->B);
					setPixel(tgtFrame, p, x, y);
				}
			} 
			delete p;
		}
		
		static void Overlay(IplImage* srcFrame1, IplImage* srcFrame2, IplImage* tgtFrame, IplImage* mask) {
			Pixel *p1 = new Pixel();
			Pixel *p2 = new Pixel();
			for (int x=0; x<mask->width; x++)
				for (int y=0; y<mask->height; y++)
				{
					if (((uchar*)(mask->imageData + mask->widthStep*y))[x] > 0) {
						getPixel(srcFrame1, p1, x, y);
						getPixel(srcFrame2, p2, x, y);
						p1->R = (p1->R + p2->R)/2;
						p1->G = (p1->G + p2->G)/2;
						p1->B = (p1->B + p2->B)/2;
						setPixel(tgtFrame, p1, x, y);
					}
				} 
			delete p1; delete p2;
		}

		static void Serialize(IplImage* im, CArchive& ar) {
			if (ar.IsLoading()) {
				int old_size = im->imageSize;
				ar >> im->nSize >> im->ID >> im->nChannels >> im->depth >> im->dataOrder >> im->origin >> im->width >> im->height >> im->imageSize;
				im->roi = NULL;
				im->maskROI = NULL;
				if(im->imageSize != old_size) {
					cvFree((void**)&(im->imageData));
					im->imageData = (char*)cvAlloc(im->imageSize);
				}
				ar.Read( im->imageData, im->imageSize);
				ar >> im->widthStep;
				im->imageDataOrigin = im->imageData;
			}
			else {
				ar << im->nSize << im->ID << im->nChannels << im->depth << im->dataOrder << im->origin << im->width << im->height << im->imageSize;
				ar.Write( im->imageData, im->imageSize);
				ar << im->widthStep;
			}
		}
	};

}// end namespace OpenCVExtensions