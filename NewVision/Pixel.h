#pragma once

namespace OpenCVExtensions
{
	class Pixel : public CObject
	{
	public:
		unsigned char R, G, B;

	public:
		Pixel(void);
		Pixel(Pixel&);
		Pixel(unsigned char, unsigned char, unsigned char);
		virtual ~Pixel(void);
	};

}// end namespace OpenCVExtensions

