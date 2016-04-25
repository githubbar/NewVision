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
// $Id: OpenGLBase.cpp,v 1.8 2005/09/08 21:21:43 matt Exp $
//=============================================================================

//=============================================================================
// System Includes
//=============================================================================
#include "stdafx.h"

//=============================================================================
// PGR Includes
//=============================================================================

//=============================================================================
// Project Includes
//=============================================================================
#include "OpenGLBase.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


IMPLEMENT_DYNCREATE( COpenGLBase, CView )


COpenGLBase::COpenGLBase()
{
   m_pDC = NULL;
   m_hRC = NULL;
   
   m_bSizeInitialized = false;
}

COpenGLBase::~COpenGLBase()
{
   if( m_pDC != NULL )
   {
      delete m_pDC;
      m_pDC = NULL;
   }

   if( m_hRC != NULL )
   {
      ::wglDeleteContext( m_hRC );
      m_hRC = NULL;
   }
}


BOOL
COpenGLBase::initializeGL()
{
   if( m_pDC == NULL )
   {
      ASSERT( FALSE );
      return FALSE;
   }
   
   //
   // Fill in the Pixel Format Descriptor
   //
   PIXELFORMATDESCRIPTOR pfd;
   memset( &pfd, 0x0, sizeof( PIXELFORMATDESCRIPTOR ) );
   
   pfd.nSize      = sizeof( PIXELFORMATDESCRIPTOR );
   pfd.nVersion   = 1;
   pfd.dwFlags    =	
      PFD_DOUBLEBUFFER   |
      PFD_SUPPORT_OPENGL |
      PFD_DRAW_TO_WINDOW;
   pfd.iPixelType = PFD_TYPE_RGBA;
   pfd.cColorBits = 24;
   pfd.cAlphaBits = 0;
   pfd.cDepthBits = 0;

   int nPixelFormat = ::ChoosePixelFormat( m_pDC->m_hDC, &pfd );
   if( nPixelFormat == 0 )
   {
      ASSERT( FALSE );
      return FALSE;
   }
   
   if( !::SetPixelFormat( m_pDC->m_hDC, nPixelFormat, &pfd ) )
   {
      ASSERT( FALSE );
      return FALSE;
   }
   
   //
   // Create a rendering context.
   //
   m_hRC = ::wglCreateContext( m_pDC->m_hDC );
   if( m_hRC == NULL )
   {
      ASSERT( FALSE );
      return FALSE;
   }

   return TRUE;
}


void 
COpenGLBase::outputGLError( char* pszLabel )
{
#ifdef _DEBUG
   GLenum error = ::glGetError();
   
   if ( error != GL_NO_ERROR )
   {
      TRACE( 
	 "%s had error: #(%d) %s\r\n", 
	 pszLabel, 
	 error, 
	 ::gluErrorString( error ) );

      ASSERT( FALSE );
   }
#endif
}


void 
COpenGLBase::bindGL( CDC* pDC ) const
{
   if( !::wglMakeCurrent( pDC->m_hDC, m_hRC ) )
   {
      TRACE( 
         "COpenGLBase::bindGL(): wglMakeCurrent() Failed (%d)\n", 
         GetLastError() );
      ASSERT( FALSE );
   }
}


void 
COpenGLBase::unBindGL( CDC* pDC ) const
{
   if( !::wglMakeCurrent( pDC->m_hDC, NULL ) )
   {
      TRACE( 
         "COpenGLBase::unBindGL(): wglMakeCurrent() Failed (%d)\n", 
         GetLastError() );
      ASSERT( FALSE );
   }
}


void 
COpenGLBase::newSize()
{
   m_bSizeInitialized = false;
}


LadybugError 
COpenGLBase::initializeForSize()
{
   ASSERT( FALSE );
   return LADYBUG_FAILED;
}


void 
COpenGLBase::OnDraw( CDC* /* pDC */ )
{
   ASSERT( FALSE );
}

void 
COpenGLBase::drawDefault()
{
   glClearColor( (float)0.2, (float)0.2, (float)0.2, 0.0);
   glClear( GL_COLOR_BUFFER_BIT );
   glFlush();
}

void
COpenGLBase::bringWindowToTop( )
{
   GetParentFrame()->BringWindowToTop();
}


// eof.
