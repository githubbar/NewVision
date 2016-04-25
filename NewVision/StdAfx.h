// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__51432DE2_F12F_4668_9AD3_CB67CD2F7235__INCLUDED_)
#define AFX_STDAFX_H__51432DE2_F12F_4668_9AD3_CB67CD2F7235__INCLUDED_

#ifndef WINVER				// Allow use of features specific to Windows 95 and Windows NT 4 or later.
#define WINVER 0x0400		// Change this to the appropriate value to target Windows 98 and Windows 2000 or later.
#endif

#ifndef _WIN32_WINNT		// Allow use of features specific to Windows NT 4 or later.
#define _WIN32_WINNT 0x0500		
#endif						

#ifndef _WIN32_WINDOWS		// Allow use of features specific to Windows 98 or later.
#define _WIN32_WINDOWS 0x0410 // Change this to the appropriate value to target Windows Me or later.
#endif

#ifndef _WIN32_IE			// Allow use of features specific to IE 4.0 or later.
#define _WIN32_IE 0x0400	// Change this to the appropriate value to target IE 5.0 or later.
#endif

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define _CRT_SECURE_NO_DEPRECATE 1
#define _CRT_NON_CONFORMING_SWPRINTFS 1
#define _CRT_NONSTDC_NO_DEPRECATE 1
#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#include <afxtempl.h>
#include <strsafe.h>
#include <dshow.h>
#include <d3d9.h>
#include <vmr9.h>
#include <streams.h>
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#pragma warning(disable : 4995)
#pragma warning(disable : 4430)
#pragma warning(disable : 4793)
#pragma warning(disable : 4996)
// this is needed to overcome the crazyness of managed extensions
struct _IMAGELIST {}; 
struct _TREEITEM { };

#define COUNTOF(x) ( sizeof(x) / sizeof(*x) )
#define DEFAULT_STRING_SIZE 256
#define USE_CORELIBRARY 0
#define USE_PGRLIBRARY 1
#define USE_X3DLIBRARY 0
#define DEBUG_LEVEL 1
#define DEBUG_DIRECTX 0



//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__51432DE2_F12F_4668_9AD3_CB67CD2F7235__INCLUDED_)
