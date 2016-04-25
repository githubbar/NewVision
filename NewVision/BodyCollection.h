#pragma once

class Body;

using namespace System;
using namespace System::Collections::Generic;
using namespace System::Collections::ObjectModel;

ref class BodyCollection :
public KeyedCollection<int, Body^> {
	public:
		BodyCollection::BodyCollection(void){}

	protected:
		virtual int GetKeyForItem(Body^ body) override 
		{
			return body->id;
		}

};
