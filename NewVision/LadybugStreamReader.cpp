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
// $Id: LadybugStreamReader.cpp,v 1.9 2005/09/19 19:52:10 matt Exp $
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
#include "LadybugStreamReader.h"
#include "LadybugStreamWriter.h"


#define TEMP_BUF_SIZE ( 6 * 1024 * 1024 )

static __inline unsigned
swab( unsigned num )
{
   return (
      ( num>>24) |
      ( ( num & 0x00FF0000 ) >> 8 ) |
      ( ( num & 0x0000FF00 ) << 8 ) |
      (  num << 24 ) );
}


LadybugStreamReader::LadybugStreamReader()
{
   m_pfile = NULL;
   m_bytesRead = 0;
   m_iSplitFile = 0;
   
   strcpy( m_pszFilename, "" );
   strcpy( m_pszSrcPath, "" );

   m_pTempBuf = new unsigned char[ TEMP_BUF_SIZE ];
}


LadybugStreamReader::~LadybugStreamReader()
{
   close();

   if( m_pTempBuf != NULL )
   {
      delete [] m_pTempBuf;
      m_pTempBuf = NULL;
   }
}


LadybugError 
LadybugStreamReader::readJPEGHeader( CompressorHeaderInfo* pinfo )
{
   LadybugError error = LADYBUG_OK;
   LadybugStreamHeadInfo streamHeaderInfo;

   if( m_pfile == NULL )
   {
      assert( false );
      return LADYBUG_FAILED;
   }
   
   size_t count = fread( m_pTempBuf, CompressorHeaderInfo::SIZE, 1, m_pfile );
   if( count != 1 )
   {
      //
      // In this case the stream file reading pointer points to the end 
      // Try to open the next stream file if it is possible
      //
      
      if( ( error = openNextSplitFile() ) == LADYBUG_OK )
      {
         //
         // Read the head of the stream file and seek 
         // the position of the first iamge
         //
         if ( readStreamHeader( &streamHeaderInfo ) == LADYBUG_OK )
         {
            fseek( m_pfile, streamHeaderInfo.ulConfigrationDataSize, SEEK_CUR );
         }

         //
         // Read the master header of the JPEG images
         //
         size_t count = fread( 
            m_pTempBuf, CompressorHeaderInfo::SIZE, 1, m_pfile );
         if( count != 1 )
         {
            //output( "Corrupt stream file: %s\n", pszFilename );
            return LADYBUG_FAILED;
         }
      } 
      else if( error == LADYBUG_COULD_NOT_OPEN_FILE ) 
      {
         // There is no next split file.
         return LADYBUG_FAILED;
      }
   } 
   
   //
   // Get the each header info of all JPEG images in this frame including 
   // the total size of this frame data
   //
   if( ( error = pinfo->assembleJPEGHeaderInfo( m_pTempBuf ) )!= LADYBUG_OK )
   {
      assert( false );
      return error;
   }

   return error;
}

LadybugError 
LadybugStreamReader::open( const char* pszPath )
{
   close();

   m_bytesRead = 0;
   m_iSplitFile = 0;

   strncpy( m_pszSrcPath, pszPath, _MAX_PATH );
   return openNextSplitFile();
}


LadybugError 
LadybugStreamReader::close()
{
   if( m_pfile != NULL )
   {
      fclose( m_pfile );
      m_pfile = NULL;         
   }

   return LADYBUG_OK;
}


LadybugError 
LadybugStreamReader::readStreamHeader( LadybugStreamHeadInfo* pStreamHeaderInfo )
{
   if( m_pfile == NULL )
   {
      assert( false );
      return LADYBUG_FAILED;
   }

   fseek(m_pfile, SIZE_OF_PGR_STREAM_FILE_FLAG, SEEK_SET);
   size_t count = fread( pStreamHeaderInfo, 
      sizeof( LadybugStreamHeadInfo ), 1, m_pfile );
   if( count != 1 )
   {
      return LADYBUG_FAILED;
   }

   return LADYBUG_OK;
}


LadybugError 
LadybugStreamReader::openNextSplitFile()
{
   close();

   sprintf( m_pszFilename, "%s-%03d.pgr", m_pszSrcPath, m_iSplitFile);
   m_pfile = fopen( m_pszFilename, "rb" );
   if( m_pfile == NULL )
   {
      return LADYBUG_COULD_NOT_OPEN_FILE;
   }
   else
   {
      //
      // Verity Ladybug stream file flag
      //
      unsigned char flagString[SIZE_OF_PGR_STREAM_FILE_FLAG + 1];

      fseek(m_pfile, 0, SEEK_SET);
      size_t count = fread( 
         flagString, SIZE_OF_PGR_STREAM_FILE_FLAG, 1, m_pfile );
      if( count != 1 )
         return LADYBUG_FAILED;
      else
      {
         flagString[ SIZE_OF_PGR_STREAM_FILE_FLAG ] = '\0';
         CString flagStringCompare = flagString;
         //
         // Make sure it is a PGR stream file by comparing the flag
         //
         if ( flagStringCompare.Compare(PGR_STREAM_FILE_FLAG) != 0 )
            return LADYBUG_FAILED;
      }
      fseek(m_pfile, 0, SEEK_SET);
   }

   m_iSplitFile++;

   return LADYBUG_OK;
}


LadybugError 
LadybugStreamReader::getStreamNumImages( int* pimages )
{
   int iImages = 0;
   LadybugStreamHeadInfo streamHeaderInfo;

   m_iSplitFile =0;

   // 
   // Read all the stream file and count the number of images in each stream file 
   //
   while( openNextSplitFile() == LADYBUG_OK )
   {
      //
      // Read the head from the stream file
      //
      if ( readStreamHeader( &streamHeaderInfo ) == LADYBUG_OK )
      {
         iImages += streamHeaderInfo.ulNumberOfImages;
      }
   }

   //
   // Restore the default openning position to the beginning of the stream
   //
   close();
   open( m_pszSrcPath );
   m_iSplitFile =0;

   *pimages = iImages;

   return LADYBUG_OK;
}

LadybugError 
LadybugStreamReader::getImageDataLength( const unsigned char* pData,
                                        long *totalSize)
{
   unsigned offset = 0;;
   unsigned size = 0;
   long iTotalSize = 0;

   if ( pData == NULL )
      return LADYBUG_FAILED;

   iTotalSize = 0;
  
   //
   // Parse the master header of this frame 
   // Get the maximum offset of the 24 JPEG images
   // This maximum offset value is data length of this frame
   // 
   for( int i = 0; i < COMPRESSOR_HEADER_MAX_IMAGES; i++ )
   {
      offset = 
         swab( *(unsigned*)( pData + 1024 - ( 24 - i ) * 8 + 0 ) );
      size = 
         swab( *(unsigned*)( pData + 1024 - ( 24 - i ) * 8 + 4 ) );

      if( offset + size > (unsigned)iTotalSize )
         iTotalSize = offset + size;
   }
   
   *totalSize = iTotalSize;

   return LADYBUG_OK;
}

LadybugError 
LadybugStreamReader::nextFrame( )
{
   LadybugError error;
   long totalFrameDataSize = 0;
      
   //
   // Read the master header of this frame
   //
   size_t count = fread( 
      m_pTempBuf, CompressorHeaderInfo::SIZE, 1, m_pfile );
   if( count != 1 )
   {
      return LADYBUG_FAILED;
   }

   //
   // Get the data size of this frame
   //
   if( ( error = getImageDataLength( m_pTempBuf, &totalFrameDataSize ) ) != 
      LADYBUG_OK )
   {
      return error;
   }

   //
   // Skip this frame by 'totalFrameDataSize' bytes
   //
   int iRet = fseek( 
      m_pfile,
      totalFrameDataSize - CompressorHeaderInfo::SIZE, 
      SEEK_CUR );
   if( iRet != 0 )
   {
      assert( false );
      return LADYBUG_FAILED;
   }

   return LADYBUG_OK;
}

 
LadybugError 
LadybugStreamReader::fastForward( int iImageNum )
{

   LadybugStreamHeadInfo *streamHeaderInfo = new LadybugStreamHeadInfo;
   int iImageCounter = 0;
   bool bWantedStreamFound = false;
   LadybugError error = LADYBUG_FAILED;
   int iImagePositionNumber = 0;
   int iSkipImages = 0;

   m_iSplitFile =0;

   // 
   // Read each stream file and count the number of images
   // Check if this stream file contains the wanted image
   //
   while( openNextSplitFile() == LADYBUG_OK )
   {
      //
      // Read the head from the stream file
      //
      if ( (error = readStreamHeader( streamHeaderInfo )) == LADYBUG_OK )
      {
         iImageCounter += streamHeaderInfo->ulNumberOfImages;

         //
         // Check if it is the wanted stream file
         //
         if ( iImageCounter <= iImageNum )
            continue;
         else
         {
            //
            // Set iImageCounter to the total number of images
            // of the files skipped
            //
            iImageCounter -= streamHeaderInfo->ulNumberOfImages;
            bWantedStreamFound = true;
            break;
         }
      }
      else
         break;
   }

   if ( bWantedStreamFound ) 
   {
      long lSearchPositionOffset = 0;
      unsigned long offsetIndex = 0;
      int iRet;
      
      //
      // Calculate the position of the image
      //
      iImagePositionNumber =  iImageNum - iImageCounter;
      assert( iImagePositionNumber >= 0 );

      //
      // Calculate the the key index number of this image
      //
      offsetIndex = iImagePositionNumber / streamHeaderInfo->ulIncrement;

      //
      // Skip images from the first image of this offset
      //
      iSkipImages = iImagePositionNumber - 
	 offsetIndex * streamHeaderInfo->ulIncrement;
      assert( iSkipImages >= 0 );

      if ( offsetIndex > streamHeaderInfo->ulNumberOfKeyIndex ) 
         error = LADYBUG_FAILED;
      else
      {
         //
         // Get the beginning search position from the offsetset
         // Set the file reading position
         //
         lSearchPositionOffset = streamHeaderInfo->ulOffsetTable[ 
            MAX_NUMBER_OF_IMAGE_INDEXES - 1 - offsetIndex ];
         iRet = fseek( m_pfile, lSearchPositionOffset, SEEK_SET );
         if( iRet != 0 )
         {
            assert( false );
            error = LADYBUG_FAILED;
         }
         else
         {
            //
            // Move forward to the destination
            //
            for( int i = 0; i < iSkipImages; i++ )
            {
               if( ( error = nextFrame( ) ) != LADYBUG_OK )
                  break;
            }
         }
      }
   }

   delete streamHeaderInfo;
   return error;
}



LadybugError 
LadybugStreamReader::readFrame( CompressorHeaderInfo* pinfo, int iIndex, LadybugImage* pLadybugImage)
{
   close();
   open( m_pszSrcPath );

   fastForward( iIndex );

   return readFrame( pinfo, pLadybugImage );
}


double 
LadybugStreamReader::getMBRead() const
{
   return (double)m_bytesRead / (double)( 1024 * 1024 );
}



LadybugError 
LadybugStreamReader::readFrame( CompressorHeaderInfo* pinfo, 
                               LadybugImage* pLadybugImage )
{
   LadybugError error;

   if( pinfo == NULL )
   {
      return LADYBUG_INVALID_ARGUMENT;
   }

   //
   // Get the master HEADER of the JPEG frame
   //
   if( ( error = readJPEGHeader( pinfo ) ) != LADYBUG_OK )
      return error;
   //
   // Read the JPEG images following the master header
   //
   size_t count = fread( 
      m_pTempBuf + CompressorHeaderInfo::SIZE, 
      pinfo->totalSize() - CompressorHeaderInfo::SIZE, 
      1, 
      m_pfile );
   if( count != 1 )
      return LADYBUG_FAILED;

   if ( pLadybugImage != NULL )
   {
      pLadybugImage->pData = m_pTempBuf;
      pLadybugImage->uiDataSizeBytes = pinfo->totalSize();

      //
      // Parse the LadybugImageInfo from the stream data
      //
      const unsigned char* pInfoStruct = pLadybugImage->pData + 0x10;
      // initialize the image info 
      memset( &pLadybugImage->imageInfo, 0, sizeof( LadybugImageInfo ) );

      //
      // Copy the info quadlets from the stream data to the imageInfo header
      // converting endiannes (reversing the byte ordering within the quadlet)
      //
      unsigned long* pDest = (unsigned long*)&pLadybugImage->imageInfo;
      const unsigned long* pSrc  = (unsigned long*)pInfoStruct;

      assert( sizeof( LadybugImageInfo ) % 4 == 0 );
      const unsigned int uiQuadlets = sizeof( LadybugImageInfo ) / 4;

      for( unsigned int i = 0; i < uiQuadlets; i++ )
      {
         const unsigned char* pBytes = (unsigned char*)&pSrc[ i ];

         pDest[ i ] = (unsigned long)(  
	    pBytes[ 3 ] | 
	    pBytes[ 2 ] << 8 | 
	    pBytes[ 1 ] << 16 | 
	    pBytes[ 0 ] << 24 );
      }
      //
      // Fill in LadybugImge
      //
      pLadybugImage->timeStamp.ulSeconds = 
         pLadybugImage->imageInfo.ulTimeSeconds;
      pLadybugImage->timeStamp.ulMicroSeconds = 
         pLadybugImage->imageInfo.ulTimeMicroSeconds;
      pLadybugImage->uiSeqNum = pLadybugImage->imageInfo.ulSequenceId;

   }
   //
   // Count the bytes read
   //
   m_bytesRead += pinfo->totalSize();

   return LADYBUG_OK;
}


LadybugError 
LadybugStreamReader::getConfigFile( const char* pszConfigFileName )
{
   LadybugError error = LADYBUG_FAILED;
   LadybugStreamHeadInfo streamHeaderInfo;

   m_iSplitFile =0;

   // 
   // Read all the stream file and count the number of images in each stream file 
   //
   while( openNextSplitFile() == LADYBUG_OK )
   {
      //
      // Read the head from the stream file
      //
      if ( readStreamHeader( &streamHeaderInfo ) == LADYBUG_OK )
      {
         size_t count = fread( m_pTempBuf, 
            streamHeaderInfo.ulConfigrationDataSize, 1, m_pfile );
         if( count != 1 )
         {
            //
            // Try to read next one
            //
            error = LADYBUG_FAILED;
            continue;
         }
         else
         {
            //
            // Write it to the file
            //
            FILE *writingfile = fopen( pszConfigFileName, "wb" );
            if( writingfile != NULL )
            {
               long result = fwrite( m_pTempBuf, 
                  streamHeaderInfo.ulConfigrationDataSize, 1, writingfile );
               if ( result != 1 ) {
                  return error = LADYBUG_COULD_NOT_OPEN_FILE;
                  break;
               }
               else
                  error = LADYBUG_OK;
               fclose(writingfile);
            }
            else
               error = LADYBUG_COULD_NOT_OPEN_FILE;
            break;
         }
      }
   }

   //
   // Restore the default openning position to the beginning of the stream
   //
   close();
   open( m_pszSrcPath );
   m_iSplitFile =0;

   return error;
}

LadybugError 
LadybugStreamReader::getConfigData( char* pszConfigData )
{
   LadybugError error = LADYBUG_FAILED;
   LadybugStreamHeadInfo streamHeaderInfo;

   m_iSplitFile =0;

   // 
   // Read all the stream file and count the number of images in each stream file 
   //
   while( openNextSplitFile() == LADYBUG_OK )
   {
      //
      // Read the head from the stream file
      //
      if ( readStreamHeader( &streamHeaderInfo ) == LADYBUG_OK )
      {
         size_t count = fread( pszConfigData, 
            streamHeaderInfo.ulConfigrationDataSize, 1, m_pfile );
         if( count != 1 )
         {
            //
            // Try to read next one
            //
            error = LADYBUG_FAILED;
            continue;
         }
         else
            error = LADYBUG_OK;
            break;
      }
   }

   //
   // Restore the default openning position to the beginning of the stream
   //
   close();
   open( m_pszSrcPath );
   m_iSplitFile =0;

   return error;
}

#endif