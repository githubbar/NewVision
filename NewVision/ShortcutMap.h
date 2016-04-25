#ifndef SHORTCUT_MAP_H
#define SHORTCUT_MAP_H

#pragma once

using namespace System;
using namespace System::Configuration;
using namespace System::Collections;
using namespace System::Collections::Specialized;

#using <system.dll>
#using <system.configuration.dll>

#include "NewVisionDoc.h"

class ShortCutMap : public CMapStringToString {
	CString modifier;
public:		
	void ReadProfile() {
		ATLASSERT(!GetCount()); // are you mental?

		// provide a large buffer; the normal keys are ~250, so this overdesign should be ok!
		ACCEL buf[1024];
		DWORD dwCount = sizeof(buf);
		System::Configuration::KeyValueConfigurationCollection ^settings
			= ConfigurationManager::OpenExeConfiguration(ConfigurationUserLevel::None)->AppSettings->Settings;
		KeyValueConfigurationElement ^myElem = settings["tags"];
		if (!myElem)
			return;
		CommaDelimitedStringCollectionConverter ^cv = gcnew CommaDelimitedStringCollectionConverter();
		CommaDelimitedStringCollection ^rowColl = (CommaDelimitedStringCollection ^)cv->ConvertFromString(myElem->Value);
		// Read "tags"
		for (int i=0;i<rowColl->Count;i++) {
			// Read "shortcuts"
			myElem = settings[rowColl[i]+" #shortcut"];
			if (myElem)
				SetAt(CString(rowColl[i]), CString(myElem->Value));
		}
	}

	void ResetModifier(MSG* pMsg) {
		modifier = "";
	}

	void ExecuteTagShortcut(CNewVisionDoc* doc, MSG* pMsg) {
		CString input = NameFromVKey(pMsg->wParam).MakeUpper();
		if (input.Compare("CTRL") == 0 ||
			input.Compare("ALT") == 0 ||
			input.Compare("SHIFT") == 0) {
			modifier = input.MakeUpper();	
			return;
		}
			
		if (modifier.Compare("") != 0)
			input = modifier+"+"+input;
		CString key, value;
		for (POSITION pos = GetStartPosition(); pos;) {
			GetNextAssoc(pos, key, value);
			if (value.MakeUpper().Compare(input) == 0) {
				//AfxMessageBox(input);
				doc->SetTagValue(key);
				return;
			}
		}
		
	}

	// this helper from codeproject.com
	// Copyright 2004 Jörgen Sigvardsson <jorgen@profitab.com>
	CString NameFromVKey(UINT nVK)
	{
		UINT nScanCode = MapVirtualKeyEx(nVK, 0, GetKeyboardLayout(0));
		switch(nVK) {
			// Keys which are "extended" (except for Return which is Numeric Enter as extended)
			case VK_INSERT:
			case VK_DELETE:
			case VK_HOME:
			case VK_END:
			case VK_NEXT:  // Page down
			case VK_PRIOR: // Page up
			case VK_LEFT:
			case VK_RIGHT:
			case VK_UP:
			case VK_DOWN:
				nScanCode |= 0x100; // Add extended bit
		}	

		// GetKeyNameText() expects the scan code to be on the same format as WM_KEYDOWN
		// Hence the left shift
		CString str;
		LPTSTR prb = str.GetBuffer(80);
		BOOL bResult = GetKeyNameText(nScanCode << 16, prb, 79);

		// these key names are capitalized and look a bit daft
		int len = lstrlen(prb);
		if(len > 1) {
			LPTSTR p2 = CharNext(prb);
			CharLowerBuff(p2, len - (p2 - prb) );
		}

		str.ReleaseBuffer();
		ATLASSERT(str.GetLength());
		return str.MakeUpper(); // internationalization ready, sweet!
	}
};

#endif SHORTCUT_MAP_H