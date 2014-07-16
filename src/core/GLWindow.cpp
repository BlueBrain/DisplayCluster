/*********************************************************************/
/* Copyright (c) 2011 - 2012, The University of Texas at Austin.     */
/* All rights reserved.                                              */
/*                                                                   */
/* Redistribution and use in source and binary forms, with or        */
/* without modification, are permitted provided that the following   */
/* conditions are met:                                               */
/*                                                                   */
/*   1. Redistributions of source code must retain the above         */
/*      copyright notice, this list of conditions and the following  */
/*      disclaimer.                                                  */
/*                                                                   */
/*   2. Redistributions in binary form must reproduce the above      */
/*      copyright notice, this list of conditions and the following  */
/*      disclaimer in the documentation and/or other materials       */
/*      provided with the distribution.                              */
/*                                                                   */
/*    THIS  SOFTWARE IS PROVIDED  BY THE  UNIVERSITY OF  TEXAS AT    */
/*    AUSTIN  ``AS IS''  AND ANY  EXPRESS OR  IMPLIED WARRANTIES,    */
/*    INCLUDING, BUT  NOT LIMITED  TO, THE IMPLIED  WARRANTIES OF    */
/*    MERCHANTABILITY  AND FITNESS FOR  A PARTICULAR  PURPOSE ARE    */
/*    DISCLAIMED.  IN  NO EVENT SHALL THE UNIVERSITY  OF TEXAS AT    */
/*    AUSTIN OR CONTRIBUTORS BE  LIABLE FOR ANY DIRECT, INDIRECT,    */
/*    INCIDENTAL,  SPECIAL, EXEMPLARY,  OR  CONSEQUENTIAL DAMAGES    */
/*    (INCLUDING, BUT  NOT LIMITED TO,  PROCUREMENT OF SUBSTITUTE    */
/*    GOODS  OR  SERVICES; LOSS  OF  USE,  DATA,  OR PROFITS;  OR    */
/*    BUSINESS INTERRUPTION) HOWEVER CAUSED  AND ON ANY THEORY OF    */
/*    LIABILITY, WHETHER  IN CONTRACT, STRICT  LIABILITY, OR TORT    */
/*    (INCLUDING NEGLIGENCE OR OTHERWISE)  ARISING IN ANY WAY OUT    */
/*    OF  THE  USE OF  THIS  SOFTWARE,  EVEN  IF ADVISED  OF  THE    */
/*    POSSIBILITY OF SUCH DAMAGE.                                    */
/*                                                                   */
/* The views and conclusions contained in the software and           */
/* documentation are those of the authors and should not be          */
/* interpreted as representing official policies, either expressed   */
/* or implied, of The University of Texas at Austin.                 */
/*********************************************************************/

#include "GLWindow.h"

#include "log.h"
#include "globals.h"
#include "configuration/WallConfiguration.h"
#include "Options.h"
#include "Renderable.h"

#include <QtOpenGL>
#include <boost/shared_ptr.hpp>

#ifdef __APPLE__
    #include <OpenGL/glu.h>
#else
    #include <GL/glu.h>
#endif

GLWindow::GLWindow(const int tileIndex, QRect windowRect, QGLWidget* shareWidget)
  : QGLWidget(0, shareWidget)
  , configuration_(static_cast<WallConfiguration*>(g_configuration))
  , tileIndex_(tileIndex)
  , left_(0)
  , right_(0)
  , bottom_(0)
  , top_(0)
{
    setGeometry(windowRect);
    setCursor(Qt::BlankCursor);

    if(shareWidget && !isSharing())
    {
        put_flog(LOG_FATAL, "failed to share OpenGL context");
        exit(-1);
    }

    setAutoBufferSwap(false);
}

GLWindow::~GLWindow()
{
}

int GLWindow::getTileIndex() const
{
    return tileIndex_;
}

void GLWindow::addRenderable(RenderablePtr renderable)
{
    renderables_.append(renderable);
}

void GLWindow::setTestPattern(RenderablePtr testPattern)
{
    testPattern_ = testPattern;
}

void GLWindow::initializeGL()
{
    // enable depth testing; disable lighting
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
}

void GLWindow::paintGL()
{
    OptionsPtr options = configuration_->getOptions();

    clear(options->getBackgroundColor());
    setOrthographicView();

    if(options->getShowTestPattern())
    {
        testPattern_->render();
        return;
    }

    foreach (RenderablePtr renderable, renderables_)
        renderable->render();

    if (options->getShowStreamingStatistics())
        drawFps();
}

void GLWindow::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    update();
}

void GLWindow::clear(const QColor& clearColor)
{
    glClearColor(clearColor.redF(), clearColor.greenF(), clearColor.blueF(), clearColor.alpha());
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void GLWindow::setOrthographicView()
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    // tiled display parameters
    double tileI = (double)configuration_->getGlobalScreenIndex(tileIndex_).x();
    double screenWidth = (double)configuration_->getScreenWidth();
    double mullionWidth = (double)configuration_->getMullionWidth();
    double totalWidth = (double)configuration_->getTotalWidth();

    double tileJ = (double)configuration_->getGlobalScreenIndex(tileIndex_).y();
    double screenHeight = (double)configuration_->getScreenHeight();
    double mullionHeight = (double)configuration_->getMullionHeight();
    double totalHeight = (double)configuration_->getTotalHeight();

    // border calculations
    left_ = tileI * (screenWidth + mullionWidth);
    right_ = left_ + screenWidth;
    top_ = tileJ * (screenHeight + mullionHeight);
    bottom_ = top_ + screenHeight;

    // normalize to 0->1
    left_ /= totalWidth;
    right_ /= totalWidth;
    bottom_ /= totalHeight;
    top_ /= totalHeight;

    gluOrtho2D(left_, right_, bottom_, top_);
    glPushMatrix();

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

bool GLWindow::isRegionVisible(const QRectF& region) const
{
    const QRectF screenRect(left_, top_, right_-left_, bottom_-top_);

    return screenRect.intersects(region);
}

void GLWindow::drawFps()
{
    fpsCounter_.tick();

    const int fontSize = 32;
    QFont textFont;
    textFont.setPixelSize(fontSize);

    glPushAttrib(GL_DEPTH_BUFFER_BIT | GL_CURRENT_BIT);

    glDisable(GL_DEPTH_TEST);
    glColor4f(0.,0.,1.,1.);

    renderText(10, fontSize, fpsCounter_.toString(), textFont);

    glPopAttrib();
}

QRectF GLWindow::getProjectedPixelRect(const bool clampToViewportBorders)
{
    // get four corners in object space (recall we're in normalized 0->1 dimensions)
    const double corners[4][3] =
    {
        {0.,0.,0.},
        {1.,0.,0.},
        {1.,1.,0.},
        {0.,1.,0.}
    };

    // get four corners in screen space
    GLdouble modelview[16];
    glGetDoublev(GL_MODELVIEW_MATRIX, modelview);

    GLdouble projection[16];
    glGetDoublev(GL_PROJECTION_MATRIX, projection);

    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);

    GLdouble xWin[4][3];

    for(size_t i=0; i<4; i++)
    {
        gluProject(corners[i][0], corners[i][1], corners[i][2], modelview, projection, viewport, &xWin[i][0], &xWin[i][1], &xWin[i][2]);

        const GLdouble viewportWidth = (GLdouble)viewport[2];
        const GLdouble viewportHeight = (GLdouble)viewport[3];

        // The GL coordinates system origin is at the bottom-left corner with
        // the y-axis pointing upwards. For the QRect, we want the origin at
        // the top of the viewport with the y-axis pointing downwards.
        xWin[i][1] = viewportHeight - xWin[i][1];

        if( clampToViewportBorders )
        {
            xWin[i][0] = std::min( std::max( xWin[i][0], 0. ), viewportWidth );
            xWin[i][1] = std::min( std::max( xWin[i][1], 0. ), viewportHeight );
        }
    }

    const QPointF topleft( xWin[0][0], xWin[0][1] );
    const QPointF bottomright( xWin[2][0], xWin[2][1] );
    return QRectF( topleft, bottomright );
}
