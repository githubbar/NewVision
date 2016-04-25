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
// $Id: OpenGLBase.h,v 1.7 2005/09/08 21:21:43 matt Exp $
//=============================================================================
#if !defined(AFX_OPENGLBASE_H__52A5D461_F1D2_4148_BF7D_9F2D270741E1__INCLUDED_)
#define AFX_OPENGLBASE_H__52A5D461_F1D2_4148_BF7D_9F2D270741E1__INCLUDED_

#pragma once

//=============================================================================
// System Includes
//=============================================================================
#include "stdafx.h"

//=============================================================================
// PGR Includes
//=============================================================================
#include <ladybug.h>

//=============================================================================
// Project Includes
//=============================================================================

//=============================================================================
// OpenGL Includes
//=============================================================================
#include <GL/gl.h>
#include <GL/glu.h>
#include "NewVisionView.h"


class COpenGLBase : public CNewVisionView
{
public:
   COpenGLBase();
   DECLARE_DYNCREATE(COpenGLBase)

   virtual ~COpenGLBase();

   /** Set a flag to reinitialize for new texture size. */
   void newSize();

   void bringWindowToTop( );


protected:

   /** OpenGL rendering context. */
   HGLRC m_hRC;

   /** Device context for drawing. */
   CDC*	 m_pDC;

   bool m_bSizeInitialized;

   
   LadybugError initializeForSize();

   virtual void OnDraw( CDC* pDC );

   BOOL initializeGL();

   void bindGL( CDC* pDC ) const;
   void unBindGL( CDC* pDC ) const;
   void outputGLError( char* pszLabel );
   /** Draw a default color to current rendering context */
   void drawDefault();
 
};



#endif // !defined(AFX_OPENGLBASE_H__52A5D461_F1D2_4148_BF7D_9F2D270741E1__INCLUDED_)
