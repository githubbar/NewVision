#include "StdAfx.h"
#include "Filters.h"

// --------------------------------------------------------------------------------
bool addCamera(REFCLSID CLSIDcategory, WCHAR* filtername, IGraphBuilder *pGraph, IPin **outputPin) {
	// Create the System Device Enumerator.
	HRESULT hr;
	IBaseFilter* baseFilter = NULL;
	ICreateDevEnum *pSysDevEnum = NULL;
	hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER,
    IID_ICreateDevEnum, (void **)&pSysDevEnum);

	// Obtain a class enumerator for the specified category.
	IEnumMoniker *pEnumCat = NULL;
	hr = pSysDevEnum->CreateClassEnumerator(CLSIDcategory, &pEnumCat, 0);

	if (hr != S_OK)
		return 0;

	// Enumerate the monikers.
	IMoniker *pMoniker;
	ULONG cFetched;
	bool found= false;

	while(pEnumCat->Next(1, &pMoniker, &cFetched) == S_OK && !found)
	{
		IPropertyBag *pPropBag;
		pMoniker->BindToStorage(0, 0, IID_IPropertyBag, (void **)&pPropBag);

		// To retrieve the friendly name of the filter, do the following:
		VARIANT varName;
		VariantInit(&varName);
		CString str;
		hr = pPropBag->Read(L"FriendlyName", &varName, 0);
		if (SUCCEEDED(hr))
		{
			str= varName.bstrVal;

			SysFreeString(varName.bstrVal);
		}
		VariantClear(&varName);

	    // To create an instance of the filter, do the following:

		if (str == filtername) {

			found= true;
			hr= pMoniker->BindToObject(NULL, NULL, IID_IBaseFilter, (void**)&baseFilter);
		}

		// Clean up.
		pPropBag->Release();
		pMoniker->Release();
	}

	pEnumCat->Release();

	HRESULT h1 = pGraph->AddFilter( baseFilter, filtername);
	if(FAILED(h1) ) {
		TCHAR tmp[100];
		StringCchPrintf(tmp, 100, "Unable to connect %ls", filtername);
		::MessageBox( NULL, tmp, "Error", MB_OK | MB_ICONINFORMATION );
		return 0;
	}
	outputPin[0]= GetPin(baseFilter, PINDIR_OUTPUT, 1);
    return 1;
}
// --------------------------------------------------------------------------------
bool addFilter(IBaseFilter* baseFilter, WCHAR* filtername, IGraphBuilder *pGraph,
               IPin **outputPin, int numberOfOutput) {

      // This function assumes the filter is already created

      TCHAR tmp[100];
      // Obtain the input pin.

      IPin* inputPin= GetPin(baseFilter, PINDIR_INPUT);
      if (!inputPin) {

        StringCchPrintf(tmp, 100, "Unable to obtain %ls input pin", filtername);
        ::MessageBox( NULL, tmp, "Error", MB_OK | MB_ICONINFORMATION );
        return 0;
      }

      // Connect the filter to the ouput pin.
		HRESULT h1 = pGraph->AddFilter( baseFilter, filtername);
		HRESULT h2 = pGraph->Connect(*outputPin, inputPin);

      if(FAILED(h1) ||
         FAILED(h2) )           
      {

        StringCchPrintf(tmp, 100, "Unable to connect %ls filter", filtername);
        ::MessageBox( NULL, tmp, "Error", MB_OK | MB_ICONINFORMATION );
        return 0;
      }

      SafeRelease(inputPin);
      SafeRelease(*outputPin);

      // Obtain the scene pin(s).

      for (int i=0; i<numberOfOutput; i++) {

        outputPin[i]= 0;
        outputPin[i]= GetPin(baseFilter, PINDIR_OUTPUT, i+1);

        if (!outputPin[i]) {

          StringCchPrintf(tmp, 100, "Unable to obtain %s scene pin (%d)", filtername, i);
          ::MessageBox( NULL, tmp, "Error", MB_OK | MB_ICONINFORMATION );
          return 0;
        }
      }

      SafeRelease(baseFilter);

      return 1;
}
// --------------------------------------------------------------------------------
bool addFilter(REFCLSID filterCLSID, WCHAR* filtername, IGraphBuilder *pGraph,
               IPin **outputPin, int numberOfOutput) {

      // Create the filter.

      IBaseFilter* baseFilter = NULL;
      TCHAR tmp[100];

      if(FAILED(CoCreateInstance(
          filterCLSID, NULL, CLSCTX_INPROC_SERVER,
                IID_IBaseFilter, (void**)&baseFilter)) || !baseFilter)
      {
        StringCchPrintf(tmp, 100, "Unable to create %ls filter", filtername);
        ::MessageBox( NULL, tmp, "Error", MB_OK | MB_ICONINFORMATION );
        return 0;
      }

      // Obtain the input pin.

      IPin* inputPin= GetPin(baseFilter, PINDIR_INPUT);
      if (!inputPin) {
        StringCchPrintf(tmp, 100, "Unable to obtain %ls input pin", filtername);
        ::MessageBox( NULL, tmp, "Error", MB_OK | MB_ICONINFORMATION );
        return 0;
      }

      // Connect the filter to the ouput pin.
		HRESULT h1 = pGraph->AddFilter( baseFilter, filtername);
		HRESULT h2 = pGraph->Connect(*outputPin, inputPin);

      if(FAILED(h1) ||
         FAILED(h2) )           
      {

        StringCchPrintf(tmp, 100, "Unable to connect %ls filter", filtername);
        ::MessageBox( NULL, tmp, "Error", MB_OK | MB_ICONINFORMATION );
        return 0;
      }

      SafeRelease(inputPin);
      SafeRelease(*outputPin);

      // Obtain the scene pin(s).

      for (int i=0; i<numberOfOutput; i++) {

        outputPin[i]= 0;
        outputPin[i]= GetPin(baseFilter, PINDIR_OUTPUT, i+1);

        if (!outputPin[i]) {

          StringCchPrintf(tmp, 100, "Unable to obtain %s scene pin (%d)", filtername, i);
          ::MessageBox( NULL, tmp,  "Error", MB_OK | MB_ICONINFORMATION );
          return 0;
        }
      }

      SafeRelease(baseFilter);

      return 1;
}
// --------------------------------------------------------------------------------
bool renderPin(IGraphBuilder *pGraph, IPin **outputPin, int numberOfOutput) {

      if(FAILED(pGraph->Render(outputPin[numberOfOutput])))
      {
        ::MessageBox( NULL, "Unable to render stream", "Error", MB_OK | MB_ICONINFORMATION );
        return false;
      }
	  return true;
}
// --------------------------------------------------------------------------------
bool addRenderer(WCHAR* renderername, IGraphBuilder *pGraph, IPin **outputPin) {

      char tmp[100];
      IBaseFilter* pVideo = NULL;

      if(FAILED(CoCreateInstance(CLSID_VideoRenderer, NULL, CLSCTX_INPROC_SERVER,
                IID_IBaseFilter, (void**)&pVideo)) || !pVideo)
      {
        StringCchPrintf(tmp, 100, "Unable to create renderer %ls", renderername);
        ::MessageBox( NULL, tmp, "Error", MB_OK | MB_ICONINFORMATION );
        return 0;
      }


      IPin* pVideoIn= GetPin(pVideo, PINDIR_INPUT);
      if (!pVideoIn) {

        StringCchPrintf(tmp, 100, "Unable to obtain %ls input pin", renderername);
        ::MessageBox( NULL, tmp, "Error", MB_OK | MB_ICONINFORMATION );
        return 0;
      }
      
      if(FAILED(pGraph->AddFilter( pVideo, renderername)) ||
         FAILED(pGraph->Connect(*outputPin, pVideoIn)) )           
      {
        StringCchPrintf(tmp, 100, "Unable to connect %ls renderer", renderername);
        ::MessageBox( NULL, tmp, "Error", MB_OK | MB_ICONINFORMATION );
        return 0;
      }

      SafeRelease(pVideo);
      SafeRelease(pVideoIn);
      SafeRelease(*outputPin);

      return 1;
}
// --------------------------------------------------------------------------------
bool addSource(WCHAR* sourcename, IGraphBuilder *pGraph, IPin **outputPin) {
	USES_CONVERSION;
    char tmp[100];
    IBaseFilter*	pSource;

    if(FAILED(pGraph->AddSourceFilter(sourcename,A2W("Source"),&pSource)))
    {
    StringCchPrintf(tmp, 100, "Unable to connect to %ls source", sourcename);
    ::MessageBox( NULL, tmp, "Error", MB_OK | MB_ICONINFORMATION );
    return 0;
    }

    *outputPin= 0;
    *outputPin= GetPin(pSource, PINDIR_OUTPUT);

    if (! *outputPin) {

    StringCchPrintf(tmp, 100, "Unable to obtain source (%ls) input pin", sourcename);
    ::MessageBox( NULL, tmp, "Error", MB_OK | MB_ICONINFORMATION );
    return 0;
    }

    SafeRelease(pSource);

    return 1;
}
// --------------------------------------------------------------------------------
bool addFileWriter(WCHAR* filename, IGraphBuilder *pGraph, IPin **outputPin) {

      char tmp[100];
      IBaseFilter* pWriter = NULL;
      if(FAILED(CoCreateInstance(CLSID_FileWriter, NULL, CLSCTX_INPROC_SERVER,
                IID_IBaseFilter, (void**)&pWriter)) || !pWriter)
      {
        StringCchPrintf(tmp, 100, "Unable to create file writer (%ls)", filename);
        ::MessageBox( NULL, tmp, "Error", MB_OK | MB_ICONINFORMATION );
        return 0;
      }

      IFileSinkFilter2 *psink;
      pWriter->QueryInterface(IID_IFileSinkFilter2, (void **)&psink);

      psink->SetFileName(filename,NULL);
      psink->SetMode(AM_FILE_OVERWRITE);

      IPin* pWriterIn= GetPin(pWriter, PINDIR_INPUT);
      if (!pWriterIn) {

        StringCchPrintf(tmp, 100, "Unable to obtain writer input pin (%ls)", filename);
        ::MessageBox( NULL, tmp, "Error", MB_OK | MB_ICONINFORMATION );
        return 0;
      }

      if(FAILED(pGraph->AddFilter( pWriter, filename)) ||
         FAILED(pGraph->Connect(*outputPin, pWriterIn)) )           
      {
        ::MessageBox( NULL, "Unable to connect writer filter", "Error", MB_OK | MB_ICONINFORMATION );
        return 0;
      }

      SafeRelease(pWriter);
      SafeRelease(pWriterIn);
      SafeRelease(*outputPin);

      return 1;
}
// --------------------------------------------------------------------------------
// Returns the next available pin.
 
 IPin *GetPin(IBaseFilter *pFilter, PIN_DIRECTION PinDir)
{
    BOOL       bFound = FALSE;
    IEnumPins  *pEnum;
    IPin       *pPin;
    IPin       *pPin2;

    pFilter->EnumPins(&pEnum);
    while(pEnum->Next(1, &pPin, 0) == S_OK)
    {
        PIN_DIRECTION PinDirThis;
        pPin->QueryDirection(&PinDirThis);
        if (PinDir == PinDirThis) {

            pPin->ConnectedTo(&pPin2); // is this pin
                                       // connected to another pin ?
            if (pPin2 == 0) {
              bFound= TRUE;
              break;
            }
        }
        pPin->Release();
    }
    
    pEnum->Release();
    return (bFound ? pPin : 0);  
}
// --------------------------------------------------------------------------------
// Returns the n-th pin (the first one being number 1).
 
IPin *GetPin(IBaseFilter *pFilter, PIN_DIRECTION PinDir, int n)
{
    BOOL       bFound = FALSE;
    IEnumPins  *pEnum;
    IPin       *pPin;
    int i=1;

    pFilter->EnumPins(&pEnum);
    while(pEnum->Next(1, &pPin, 0) == S_OK)
    {
        PIN_DIRECTION PinDirThis;
        pPin->QueryDirection(&PinDirThis);
        if (PinDir == PinDirThis) {

            if (i == n) {

              bFound= TRUE;
              break;

            } else {

              i++;
            }
        }
        pPin->Release();
    }
    
    pEnum->Release();
    return (bFound ? pPin : 0);  
}
// --------------------------------------------------------------------------------
void enumFilters(IGraphBuilder *pGraph, CArray<CString> names) {

      IEnumFilters *pEnum = NULL;
      IBaseFilter *pFilter;
      ULONG cFetched;

      pGraph->EnumFilters(&pEnum);
      while(pEnum->Next(1, &pFilter, &cFetched) == S_OK)
      {
        FILTER_INFO FilterInfo;
        char szName[256];
        CString fname;

        pFilter->QueryFilterInfo(&FilterInfo);
        WideCharToMultiByte(CP_ACP, 0, FilterInfo.achName, -1, szName, 256, 0, 0);
        fname= szName;
        names.Add(fname);

        SafeRelease(FilterInfo.pGraph);
        SafeRelease(pFilter);
      }

      SafeRelease(pEnum);
}
// --------------------------------------------------------------------------------
IBaseFilter* getFilterByName(IGraphBuilder *pGraph, CString name) {

      IEnumFilters *pEnum = NULL;
      IBaseFilter *pFilter;
      ULONG cFetched;
	  bool found= false;

      pGraph->EnumFilters(&pEnum);
      while(!found && pEnum->Next(1, &pFilter, &cFetched) == S_OK)
      {
        FILTER_INFO FilterInfo;
        char szName[256];
        CString fname;

        pFilter->QueryFilterInfo(&FilterInfo);
        WideCharToMultiByte(CP_ACP, 0, FilterInfo.achName, -1, szName, 256, 0, 0);
        fname= szName;

        SafeRelease(FilterInfo.pGraph);

		if (fname == name) {

			found= true;

		} else {

			SafeRelease(pFilter);
		}
      }

      SafeRelease(pEnum);

	  if (found)
		  return pFilter;
	  else
          return 0;
}
// --------------------------------------------------------------------------------
bool removeAllFilters(IGraphBuilder *pGraph) {

      IEnumFilters *pEnum = NULL;
      IBaseFilter *pFilter;
      ULONG cFetched;
      CArray< IBaseFilter* > filters;
      bool done= TRUE;

      pGraph->EnumFilters(&pEnum);
      while(pEnum->Next(1, &pFilter, &cFetched) == S_OK) {

        filters.Add(pFilter);
      }

      SafeRelease(pEnum);

	  for (int i=0;i<filters.GetCount(); i++) {

        if (done && FAILED(pGraph->RemoveFilter(filters[i]))) {

         ::MessageBox( NULL, "Unable to remove filter", "Error", MB_OK | MB_ICONINFORMATION );
          done= FALSE;
          break;
        }

        SafeRelease(filters[i]);
      }

      return done;
}

// --------------------------------------------------------------------------------
void enumFilters(REFCLSID CLSIDcategory, CArray<CString>& names, CArray<CLSID>& clsidFilters) {

	// Create the System Device Enumerator.
	HRESULT hr;
	ICreateDevEnum *pSysDevEnum = NULL;
	hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER,
      IID_ICreateDevEnum, (void **)&pSysDevEnum);

	// Obtain a class enumerator for the specified category.
	IEnumMoniker *pEnumCat = NULL;
	hr = pSysDevEnum->CreateClassEnumerator(CLSIDcategory, &pEnumCat, 0);

	if (hr == S_OK)  {

		// Enumerate the monikers.
		IMoniker *pMoniker;
		ULONG cFetched;
		while(pEnumCat->Next(1, &pMoniker, &cFetched) == S_OK)
		{
			IPropertyBag *pPropBag;
			pMoniker->BindToStorage(0, 0, IID_IPropertyBag, (void **)&pPropBag);

			// To retrieve the friendly name of the filter, do the following:
			VARIANT varName;
			VariantInit(&varName);
			hr = pPropBag->Read(L"FriendlyName", &varName, 0);
			if (SUCCEEDED(hr))
			{
				CString str(varName.bstrVal);

                        names.Add(str);

				SysFreeString(varName.bstrVal);
			}
			VariantClear(&varName);

            VARIANT varFilterClsid;
            varFilterClsid.vt = VT_BSTR;

            // Read CLSID string from property bag
            hr = pPropBag->Read(L"CLSID", &varFilterClsid, 0);
            if(SUCCEEDED(hr))
			{
              CLSID clsidFilter;

              // Save filter CLSID
              if(CLSIDFromString(varFilterClsid.bstrVal, &clsidFilter) == S_OK)
			  {
                clsidFilters.Add(clsidFilter);
			  }

              SysFreeString(varFilterClsid.bstrVal);
			}

			// To create an instance of the filter, do the following:
			//   IBaseFilter *pFilter;
			//   pMoniker->BindToObject(NULL, NULL, IID_IBaseFilter, (void**)&pFilter);
			// Now add the filter to the graph. Remember to release pFilter later.
    
			// Clean up.
			pPropBag->Release();
			pMoniker->Release();
		}

		pEnumCat->Release();
	}

	pSysDevEnum->Release();
}

// --------------------------------------------------------------------------------
IBaseFilter* addFilterByEnum(REFCLSID CLSIDcategory, CString filtername, IGraphBuilder *pGraph,
               IPin **outputPin, int numberOfOutput, bool returnIt) {

	// Create the System Device Enumerator.
	HRESULT hr;
      char tmp[100];
	IBaseFilter* baseFilter = NULL;
	ICreateDevEnum *pSysDevEnum = NULL;
	hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER,
    IID_ICreateDevEnum, (void **)&pSysDevEnum);

	// Obtain a class enumerator for the specified category.
	IEnumMoniker *pEnumCat = NULL;
	hr = pSysDevEnum->CreateClassEnumerator(CLSIDcategory, &pEnumCat, 0);

	if (hr == S_OK)  {

		// Enumerate the monikers.
		IMoniker *pMoniker;
		ULONG cFetched;
		bool found= false;

		while(pEnumCat->Next(1, &pMoniker, &cFetched) == S_OK && !found)
		{
			IPropertyBag *pPropBag;
			pMoniker->BindToStorage(0, 0, IID_IPropertyBag, (void **)&pPropBag);

			// To retrieve the friendly name of the filter, do the following:
			VARIANT varName;
			VariantInit(&varName);
			CString str;
			hr = pPropBag->Read(L"FriendlyName", &varName, 0);
			if (SUCCEEDED(hr))
			{
				str= varName.bstrVal;

				SysFreeString(varName.bstrVal);
			}
			VariantClear(&varName);

	        // To create an instance of the filter, do the following:

			if (str == filtername) {

				found= true;
				hr= pMoniker->BindToObject(NULL, NULL, IID_IBaseFilter, (void**)&baseFilter);
			}
    
			// Clean up.
			pPropBag->Release();
			pMoniker->Release();
		}

		pEnumCat->Release();

		IPin* inputPin;
		if (*outputPin != NULL) {

          // Obtain the input pin.
          inputPin= GetPin(baseFilter, PINDIR_INPUT);
		  if (!inputPin) {

           StringCchPrintf(tmp, 100, "Unable to obtain %ls input pin", filtername);
           ::MessageBox( NULL, tmp, "Error", MB_OK | MB_ICONINFORMATION );
           return 0;
		  }
	    }

        // Add the filter to the graph.

        WCHAR *wfile= new WCHAR[filtername.GetLength()+1];
        MultiByteToWideChar(CP_ACP, 0, filtername, -1, wfile, 
				                filtername.GetLength()+1);
        if(FAILED(pGraph->AddFilter( baseFilter, wfile)))           
	    {
            StringCchPrintf(tmp, 100, "Unable to add %ls filter", filtername);
            ::MessageBox( NULL, tmp, "Error", MB_OK | MB_ICONINFORMATION );
            return 0;
	    }

        delete[] wfile;		 


	    if (*outputPin != NULL) {
          // Connect the filter to the ouput pin.

          if(FAILED(pGraph->Connect(*outputPin, inputPin)) )           
		  {

            StringCchPrintf(tmp, 100, "Unable to connect %ls filter", filtername);
            ::MessageBox( NULL, tmp, "Error", MB_OK | MB_ICONINFORMATION );
            return 0;
		  }

          // Release both pins.  We will set a new outputpin to be
          // returned in the next section of the function
          // Modified - Kent Feb 23 2003
          SafeRelease(*outputPin);
          SafeRelease(inputPin);
		}	 

        // Obtain the scene pin(s).
        // Change the scene pin passed in to be the sceneImg
        // pin of the newly added filter.
       for (int i=0; i<numberOfOutput; i++) {

			outputPin[i]= 0;
			outputPin[i]= GetPin(baseFilter, PINDIR_OUTPUT, i+1);

			if (!outputPin[i]) {

			StringCchPrintf(tmp, 100, "Unable to obtain %s scene pin (%d)", filtername, i);
			::MessageBox( NULL, tmp, "Error", MB_OK | MB_ICONINFORMATION );
			return 0;
			}
      }
	}

	pSysDevEnum->Release();

	if (returnIt) {

		return baseFilter;

	} else {

        SafeRelease(baseFilter);
		return 0;
	}
}
// --------------------------------------------------------------------------------
// Added - Kent Feb 23 2003, Updated Mar 10 2003
bool changeCompressorOptions(IPin **outputPin, int numberOfOutput, int& iKeyFrames, int& iPFrames, double& dQuality)
{
    // This function will check over each of the scene pins in the outputPin
    // array to see if any of them support the IAMVideoCompression interface
    // If the interface is supported on a pin, the properties of the compressor
    // will be found and displayed.  The given vaules for KeyFrames, PFrames, and
    // quality will be set if the compressor supports the options, and the given
    // values aren't -1.  If the value is -1 and the given option IS supported,
    // the default value will be used.  FALSE will be returned if there is a 
    // problem obtaining the interface, or a value for an option has been passed
    // but the compressor doesn't support that option.

    int i;
    HRESULT hr = E_FAIL;
    IPin *pPin = NULL;
    IAMVideoCompression *pCompress = NULL;

    long lCap;                     // Capability flags
    long lKeyFrameDef, lPFrameDef; // Default values
    double QualityDef;
    bool bResult = TRUE;        

    for (i=0; i < numberOfOutput; i++)
    {
        pPin = outputPin[i];
        hr = pPin->QueryInterface(IID_IAMVideoCompression, (void**)&pCompress);
        if (SUCCEEDED(hr))
            break;          // Interface found on pin, pCompress is valid.
    }

    if (SUCCEEDED(hr))
    {
        
        int cbVersion=0, cbDesc=0; // Size in bytes, not characters!
        hr = pCompress->GetInfo(0, &cbVersion, 0, &cbDesc, 0, 0, 0, 0);
        if (SUCCEEDED(hr))
        {
            // Fudge the values here since it seems that the operation of
            // IAMVideoCompression::GetInfo isn't quite as advertised.  The size
            // of the version and description are returned as zero, but using zero
            // length buffers with 0 size buffer length inputs results in an exception.
            // It seems safest to set both values to something reasonably large.

            WCHAR *pszVersion = new WCHAR[512];
            WCHAR *pszDesc = new WCHAR[512];
            cbVersion = 256;
            cbDesc = 256;
            // Get default values and capabilities.
            hr = pCompress->GetInfo(pszVersion, &cbVersion, pszDesc, &cbDesc, &lKeyFrameDef, &lPFrameDef,
                &QualityDef, &lCap);
            delete[] pszVersion;
            delete[] pszDesc;
        }
        else
            bResult = FALSE;

        if (SUCCEEDED(hr))
        {
            // For any values that aren't -1, attempt to set the values
            // If the compressor doesn't support the option, return false
            if (iKeyFrames != -1)
            {
                if (lCap & CompressionCaps_CanKeyFrame)
                {
                    hr = pCompress->put_KeyFrameRate(iKeyFrames);
                    if (FAILED(hr))
                        bResult = FALSE;
                }
                else
                    bResult = FALSE;

            } else {

				iKeyFrames= lKeyFrameDef;
			}
    
            if (iPFrames != -1)
            {
                if (lCap & CompressionCaps_CanBFrame)
                {
                    hr = pCompress->put_PFramesPerKeyFrame(iPFrames);
                    if (FAILED(hr))
                        bResult = FALSE;
                }
                else
                    bResult = FALSE;

            } else {

				iPFrames= lPFrameDef;
			}
    
            if (dQuality >= 0.0)
            {
                if (lCap & CompressionCaps_CanQuality)
                {
                    hr = pCompress->put_Quality(dQuality);
                    if (FAILED(hr))
                        bResult = FALSE;
                }
                else
                    bResult = FALSE;

            } else {

				dQuality= QualityDef;
			}
        
        }
        else
            bResult = FALSE;

        SafeRelease(pCompress);
        return bResult;
    }

    return false;
}
// --------------------------------------------------------------------------------
// Added by Kent - Mar 16 2003
bool changeVideoCaptureOptions(IBaseFilter* pVideoSource, long lParam, long lValue, long lFlags)
{
    // This function takes a pointer to the video capture filter and 
    // sets the specified property to the specified value, using the given
    // flags.  If the option cannot be set for some reason, FALSE is returned,
    // otherwise the function returns TRUE.  FALSE may be returned in the
    // case that the property is not available for this device, or
    // the given value is out of range.

    // lParam is the property from the VideoProcAmpProperty enumeration that
    // you would like to change.  lValue contains the new value to set for this
    // property.

    // The lFlags variable can have the value 0 or 1, with 1 indicating that the
    // specified parameter is controlled automatically, and 0 indicating otherwise.
    // This feature is not available for all parameters.  If the auto setting is 
    // used, the value specified it ignored.  If lFlags is not passed to the function,
    // 0, or non-automatic, is assumed.

    IAMVideoProcAmp* pVideoProcAmp = NULL;
    HRESULT hr = E_FAIL;

    hr = pVideoSource->QueryInterface(IID_IAMVideoProcAmp, (void**)&pVideoProcAmp);

    if (FAILED(hr))
        return FALSE;

    // Have interface to VideoProcAmp, attempt to change settings
    hr = pVideoProcAmp->Set(lParam, lValue, lFlags);

    if (FAILED(hr))
    {
        SafeRelease(pVideoProcAmp);
        return FALSE;
    }

    return TRUE;
}
// --------------------------------------------------------------------------------
bool changeVideoCaptureOptionsByName(WCHAR* wcVideoSource, IFilterGraph* pGraph, long lParam, long lValue, long lFlags)
{
    // This function is almost identical to changeVideoCaptureOptions, except
    // that it wraps the finding of the video capture device, given the name
    // of the device.

    bool bResult = FALSE;
    HRESULT hr = E_FAIL;

    IBaseFilter* pVideoSource = NULL;
    hr = pGraph->FindFilterByName(wcVideoSource, &pVideoSource);
    if (FAILED(hr))
        return FALSE;
    bResult= changeVideoCaptureOptions(pVideoSource, lParam, lValue, lFlags);
    SafeRelease(pVideoSource);
    return bResult;
}
