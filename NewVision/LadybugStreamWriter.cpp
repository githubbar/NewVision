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
// $Id: LadybugStreamWriter.cpp,v 1.7 2005/09/15 17:34:30 matt Exp $
//=============================================================================
 
//=============================================================================
// System Includes
//=============================================================================
#include "stdafx.h"

#if USE_PGRLIBRARY

#include <cassert>
#include <cstdlib>
#include <cstring>

//=============================================================================
// ProjectIncludes
//=============================================================================
#include "LadybugStreamWriter.h"


LadybugStreamWriter::LadybugStreamWriter()
{
   m_pfile = NULL;
   m_bytesWritten = 0;
   m_iSplitFile = 0;
   
   strcpy( m_pszFilename, "" );
   strcpy( m_pszDestPath, "" );
   strcpy( m_pszConfigFilePath, "" );

   m_pConfigBuffer = NULL;

   streamHead.ulLadybugStreamVersion = 0x00000001;
   streamHead.ulConfigrationDataSize = 0;
   streamHead.ulNumberOfImages = 0;
   streamHead.ulNumberOfKeyIndex = 0;
   streamHead.ulIncrement = OFFSET_INCREMENT_CONSTANT;

}


LadybugStreamWriter::~LadybugStreamWriter()
{
   if ( m_pConfigBuffer != NULL ) 
   {
      free( m_pConfigBuffer );
      m_pConfigBuffer = NULL;
   }
   close();
}


LadybugError 
LadybugStreamWriter::open( const char* pszPath, LadybugStreamHeadInfo *streamInfo,
      const char* pszConfigFilePath )
{
   FILE *configSourceFile = NULL;
   long  result   = 0;

   close();

   m_bytesWritten = 0;
   m_iSplitFile = 0;

   strncpy( m_pszConfigFilePath, pszConfigFilePath, _MAX_PATH );
   strncpy( m_pszDestPath, pszPath, _MAX_PATH );

   //
   //Initialize the head info
   //
   streamHead.dataFormat = streamInfo->dataFormat;
   streamHead.resolution = streamInfo->resolution;
   streamHead.stippledFormat = streamInfo->stippledFormat;

   memset(&streamHead.ulOffsetTable[0], '\0', 
      sizeof(unsigned long) * MAX_NUMBER_OF_IMAGE_INDEXES );

   //
   // Write the config info to the config buffer
   //
   configSourceFile = fopen( m_pszConfigFilePath, "rb" );

   if ( configSourceFile == NULL )
      return LADYBUG_COULD_NOT_OPEN_FILE;
   else
   {
      //
      //Go to the end of the file
      //
      result = fseek( configSourceFile, 0, SEEK_END );
      if ( result ) 
         return LADYBUG_FAILED;
      else
      {
         //Get the size of the config file and alloc a reading buffer
         streamHead.ulConfigrationDataSize = ftell ( configSourceFile );
         if ( m_pConfigBuffer != NULL ) 
            free( m_pConfigBuffer );
         m_pConfigBuffer = (char *)malloc( streamHead.ulConfigrationDataSize );

         //
         // Read config data to the buffer
         //
         result = fseek( configSourceFile, 0, SEEK_SET );
         if ( result ) 
            return LADYBUG_FAILED;
         else
         {
            result = fread(m_pConfigBuffer, sizeof(char), 
               streamHead.ulConfigrationDataSize, configSourceFile );
            if( fclose( configSourceFile ) )
               return LADYBUG_FAILED;

            if ( (unsigned)result != streamHead.ulConfigrationDataSize )
               return LADYBUG_FAILED;
         }
      }

   }

   return openNextSplitFile();
}

LadybugError 
LadybugStreamWriter::openWithConfigData( const char* pszPath, 
                                        LadybugStreamHeadInfo streamInfo,
                                        const char* pszConfigData )
{
   close();

   m_bytesWritten = 0;
   m_iSplitFile = 0;

   strncpy( m_pszDestPath, pszPath, _MAX_PATH );

   //
   //Initialize the head info
   //
   streamHead = streamInfo;

   memset(&streamHead.ulOffsetTable[0], '\0', 
      sizeof(unsigned long) * MAX_NUMBER_OF_IMAGE_INDEXES );

   //
   // Write the config info to the config buffer
   //
   if ( m_pConfigBuffer != NULL ) 
      free( m_pConfigBuffer );
   m_pConfigBuffer = (char *)malloc( streamHead.ulConfigrationDataSize );
   memcpy(m_pConfigBuffer, pszConfigData, streamHead.ulConfigrationDataSize );

   return openNextSplitFile();
}


LadybugError 
LadybugStreamWriter::close()
{
   int result = 0;

   if( m_pfile != NULL )
   {

      //
      // Write the head for each stream file
      //
      result = fseek( m_pfile, SIZE_OF_PGR_STREAM_FILE_FLAG, SEEK_SET );
      if ( result ) 
         return LADYBUG_FAILED;
      //
      // Write the head info 
      //
      result = fwrite( &streamHead, sizeof( LadybugStreamHeadInfo ), 1, m_pfile );
      if ( result != 1 ) 
         return LADYBUG_FAILED;

      fclose( m_pfile );
      m_pfile = NULL;         
   }

   return LADYBUG_OK;
}


LadybugError 
LadybugStreamWriter::writeFrame( const LadybugImage* pimage )
{
   unsigned long startWritingPosition = 0;

   if( pimage == NULL )
   {
      return LADYBUG_INVALID_ARGUMENT;
   }

   if( m_pfile == NULL )
   {
      assert( false );
      return LADYBUG_FAILED;
   }

   startWritingPosition = ftell( m_pfile );
   size_t count = fwrite( pimage->pData, pimage->uiDataSizeBytes, 1, m_pfile );
   if( count != 1 )
   {
      assert( false );
      return LADYBUG_FAILED;
   }
   else
   {
      m_bytesWritten += pimage->uiDataSizeBytes;
   }
   

   //
   // Set the offset of images written
   // Only set this value for every MAX_NUMBER_OF_IMAGE_INDEXES of 
   // iamges
   //
   if ( ( streamHead.ulNumberOfImages % OFFSET_INCREMENT_CONSTANT ) == 0 )
   {
      streamHead.ulOffsetTable[ 
         MAX_NUMBER_OF_IMAGE_INDEXES - ++streamHead.ulNumberOfKeyIndex ] = 
         (unsigned long)startWritingPosition;
   }
   //
   // Count the number of images written
   //
   streamHead.ulNumberOfImages++;  
   //
   // Split files each GiB
   //
   if( m_bytesWritten >= 
         (__int64)( m_iSplitFile + 1 ) * (__int64)( SPLIT_SIZE ) )
   {
      m_iSplitFile++;
      
      LadybugError error;
      if( ( error = openNextSplitFile() ) != LADYBUG_OK )
      {
         assert( false );
         return error;
      }
   }

   return LADYBUG_OK;
}



LadybugError 
LadybugStreamWriter::openNextSplitFile()
{
   long  result = 0;

   close();

   //
   // Reset the counter for a new stream file
   //
   streamHead.ulNumberOfImages = 0;
   streamHead.ulNumberOfKeyIndex = 0;
   memset(&streamHead.ulOffsetTable[0], '\0', 
      sizeof(unsigned long) * MAX_NUMBER_OF_IMAGE_INDEXES );

   sprintf( m_pszFilename, "%s-%03d.pgr", m_pszDestPath, m_iSplitFile );
   m_pfile = fopen( m_pszFilename, "wb" );
   if( m_pfile == NULL )
   {
      assert( false );
      return LADYBUG_COULD_NOT_OPEN_FILE;
   }
   else
   {
      //
      // Write the head for each stream file
      //
      result = fwrite( PGR_STREAM_FILE_FLAG, 
         SIZE_OF_PGR_STREAM_FILE_FLAG, 1, m_pfile );
      if ( result != 1 ) 
         return LADYBUG_FAILED;
      //
      // Write the head info 
      //
      result = fwrite( &streamHead, 
         sizeof( LadybugStreamHeadInfo ), 1, m_pfile );
      if ( result != 1 ) 
         return LADYBUG_FAILED;

      //
      // Write config data to stream file
      //
      result = fwrite(m_pConfigBuffer, 
         streamHead.ulConfigrationDataSize, 1, m_pfile );
      if ( result != 1 ) 
         return LADYBUG_FAILED;
   }
   return LADYBUG_OK;
}

 
double 
LadybugStreamWriter::getMBWritten() const
{
   return (double)m_bytesWritten / (double)( 1024 * 1024 );
}



#endif