#pragma once
class CNewVisionDoc; // forward declaration

class Model : public CObject
{
public:
	CNewVisionDoc* doc;

	Model(void)
	{
	}

	virtual ~Model(void)
	{
	}
};
