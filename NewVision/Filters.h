#pragma once

#define SafeRelease(p) { if( (p) != 0 ) { (p)->Release(); (p)= 0; } }

IPin *GetPin(IBaseFilter *pFilter, PIN_DIRECTION PinDir);
IPin *GetPin(IBaseFilter *pFilter, PIN_DIRECTION PinDir, int n);
bool addRenderer(WCHAR* renderername, IGraphBuilder *pGraph, IPin **outputPin);
bool addSource(WCHAR* sourcename, IGraphBuilder *pGraph, IPin **outputPin);
bool addCamera(REFCLSID CLSIDcategory, WCHAR* filtername, IGraphBuilder *pGraph, IPin **outputPin);

bool addFileWriter(WCHAR* filename, IGraphBuilder *pGraph, IPin **outputPin);
bool addFilter(IBaseFilter* baseFilter, WCHAR* filtername, IGraphBuilder *pGraph,
               IPin **outputPin, int numberOfOutput=1);
bool addFilter(REFCLSID filterCLSID, WCHAR* filtername, IGraphBuilder *pgraph,
               IPin **outputPin, int numberOfOutput=1);
bool renderPin(IGraphBuilder *pGraph, IPin **outputPin, int numberOfOutput);
bool removeAllFilters(IGraphBuilder *pGraph);
void enumFilters(IGraphBuilder *pGraph, CArray<CString> names);
void enumFilters(REFCLSID CLSIDcategory, CArray<CString>& names, CArray<CLSID>& clsidFilters);
IBaseFilter* addFilterByEnum(REFCLSID CLSIDcategory, CString filtername, IGraphBuilder *pGraph,
               IPin **outputPin, int numberOfOutput=1, bool returnIt=false);
IBaseFilter* getFilterByName(IGraphBuilder *pGraph, CString name);
bool changeCompressorOptions(IPin **outputPin, int numberOfOutput, int& iKeyFrames, int& iPFrames, double& dQuality);
bool changeVideoCaptureOptions(IBaseFilter* pVideoSource, long lParam, long lValue, long lFlags=0);
bool changeVideoCaptureOptionsByName(WCHAR* wcVideoSource, IFilterGraph* pGraph, long lParam, long lValue, long lFlags=0);
