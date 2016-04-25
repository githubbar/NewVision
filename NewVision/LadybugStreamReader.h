//=============================================================================
// Copyright © 2004 Point Grey Research, Inc. All Rights Reserved.
// 
// This software is the confidential and proprietary information of Point
// Grey Research, Inc. ("Confidential Information").  You shall not
// disclose such Confidential Information and shall use it only in
// accordance with the terms of the license agreement you entered into
// with Point Grey Research, Inc. (PGR).
// 
// PGR MAKES NO REPRESENTATIONS OR WARRANTIES ABOUT THE SUITABILITY OF THE
// SOFTWARE, EITHER EXPRESSED OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
// PURPOSE, OR NON-INFRINGEMENT. PGR SHALL NOT BE LIABLE FOR ANY DAMAGES
// SUFFERED BY LICENSEE AS A RESULT OF USING, MODIFYING OR DISTRIBUTING
// THIS SOFTWARE OR ITS DERIVATIVES.
//=============================================================================
//=============================================================================
// $Id: LadybugStreamReader.h,v 1.4 2005/07/23 18:12:40 matt Exp $
//=============================================================================
#ifndef __LADYBUGSTREAMREADER_H__
#define __LADYBUGSTREAMREADER_H__

//=============================================================================
// System Includes
//=============================================================================
#include <cstdio>

//=============================================================================
// PGR Includes
//=============================================================================
#include <ladybug.h>

//=============================================================================
// Project Includes
//=============================================================================
#include "compressorheaderinfo.h"



/**
 * Stream reader helper class.
 */
class LadybugStreamReader
{
public:
   LadybugStreamReader();
   virtual ~LadybugStreamReader();

   LadybugError open( const char* pszBasePath );
   LadybugError close();

   /** Read next frame. */
   LadybugError readFrame( CompressorHeaderInfo* pinfo, 
      LadybugImage* pLadybugImage );

   /** Read frame at indicated index.. */
   LadybugError readFrame( CompressorHeaderInfo* pinfo, int iIndex, LadybugImage* pLadybugImage);

   /** Fast forward to the beginning of the indicated image index. */
   LadybugError fastForward( int iImageNum );

   double getMBRead() const;

   /** Read stream head information */
   LadybugError readStreamHeader( LadybugStreamHeadInfo* pStreamHeaderInfo );

   /** Returns he number of images in the stream.
    *  It scans all the stream file and get the total number of images */
   LadybugError getStreamNumImages( int* pimages );
   
   //** Move forward to the next frame */
   LadybugError nextFrame( );

   //** Get the total length of the image data in this frame */
   LadybugError getImageDataLength( const unsigned char* pData,
      long *totalSize);

   LadybugError getConfigFile( const char* pszConfigFileName );
   
   LadybugError getConfigData( char* pszConfigData );

protected:

   LadybugError openNextSplitFile();

   LadybugError readJPEGHeader( CompressorHeaderInfo* pinfo );

   FILE* m_pfile;

   __int64 m_bytesRead;
   int m_iSplitFile;

   char m_pszFilename[ _MAX_PATH ];
   char m_pszSrcPath[ _MAX_PATH ];

   unsigned char* m_pTempBuf;

   /** Stream file head structure, defined in ladybug.h */
   LadybugStreamHeadInfo streamHead;

};



#endif // #ifndef __LADYBUGSTREAMREADER_H__
