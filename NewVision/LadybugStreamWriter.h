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
// $Id: LadybugStreamWriter.h,v 1.4 2005/09/15 17:34:30 matt Exp $
//=============================================================================
#ifndef __LADYBUGSTREAMWRITER_H__
#define __LADYBUGSTREAMWRITER_H__

//=============================================================================
// System Includes
//=============================================================================
#include <cstdio>

//=============================================================================
// PGR Includes
//=============================================================================
#include <ladybug.h>

#define SPLIT_SIZE ( (__int64)( 1024 * 1024 * 1024 ) )
#define MAX_NUMBER_OF_IMAGE_INDEXES 512
#define OFFSET_INCREMENT_CONSTANT 50
#define PGR_STREAM_FILE_FLAG "PGRLADYBUGSTREAM"
#define SIZE_OF_PGR_STREAM_FILE_FLAG 16
#define LADYBUG_STREAMFILE_VERISON 0x00000001

//=============================================================================
// Project Includes
//=============================================================================

/**
 * Stream writer helper class.
 * 
 */
class LadybugStreamWriter
{
public:
   LadybugStreamWriter();
   virtual ~LadybugStreamWriter();

   LadybugError open( const char* pszPath, 
      LadybugStreamHeadInfo *streamInfo,
      const char* pszConfigFilePath);

   LadybugError openWithConfigData( const char* pszPath, 
      LadybugStreamHeadInfo streamInfo,
      const char* pszConfigData );

   LadybugError close();

   LadybugError writeFrame( const LadybugImage* pimage );

   double getMBWritten() const;

   
protected:

   LadybugError openNextSplitFile();

   FILE* m_pfile;

   __int64 m_bytesWritten;
   int m_iSplitFile;

   char m_pszFilename[ _MAX_PATH ];
   char m_pszDestPath[ _MAX_PATH ];

   /**  Configuration file name */ 
   char m_pszConfigFilePath[ _MAX_PATH ];

   /**  Config data buffer, used to write it to each stream file */
   char* m_pConfigBuffer;

   /** Stream file head structure, defined in ladybug.h */
   LadybugStreamHeadInfo streamHead;

};



#endif // #ifndef __LADYBUGSTREAMWRITER_H__
