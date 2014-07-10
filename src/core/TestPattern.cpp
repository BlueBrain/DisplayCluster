/*********************************************************************/
/* Copyright (c) 2014, EPFL/Blue Brain Project                       */
/*                     Raphael Dumusc <raphael.dumusc@epfl.ch>       */
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

#include "TestPattern.h"

#include "configuration/WallConfiguration.h"

#include <QString>
#include <QFont>
#include <QGLWidget>

#define FONT_SIZE   24
#define LINE_WIDTH  10
#define TEXT_POS_X  50

TestPattern::TestPattern(QGLWidget* widget,
                         const WallConfiguration* configuration,
                         const int rank,
                         const int tileIndex)
    : glWindow_(widget)
{
    const QPoint globalScreenIndex = configuration->getGlobalScreenIndex(tileIndex);
    const QString fullsceenMode = configuration->getFullscreen() ? "True" : "False";

    labels_.push_back(QString("Rank: %1").arg(rank));
    labels_.push_back(QString("Host: %1").arg(configuration->getHost()));
    labels_.push_back(QString("Display: %1").arg(configuration->getDisplay()));
    labels_.push_back(QString("Tile coordinates: (%1,%2)").arg(globalScreenIndex.x()).arg(globalScreenIndex.y()));
    labels_.push_back(QString("Resolution: %1 x %2").arg(configuration->getScreenWidth()).arg(configuration->getScreenHeight()));
    labels_.push_back(QString("Fullscreen mode: %1").arg(fullsceenMode));
}

void TestPattern::render()
{
    glPushAttrib(GL_CURRENT_BIT | GL_LINE_BIT);
    glPushMatrix();

    renderCrossPattern();

    // screen information in front of cross pattern
    glTranslatef(0., 0., 0.1);

    renderLabels();

    glPopMatrix();
    glPopAttrib();
}

void TestPattern::renderCrossPattern()
{
    glLineWidth(LINE_WIDTH);

    glBegin(GL_LINES);

    for(double y_coord=-1.; y_coord<=2.; y_coord+=0.1)
    {
        QColor color = QColor::fromHsvF((y_coord + 1.)/3., 1., 1.);
        glColor3f(color.redF(), color.greenF(), color.blueF());

        glVertex2d(0., y_coord);
        glVertex2d(1., y_coord+1.);

        glVertex2d(0., y_coord);
        glVertex2d(1., y_coord-1.);
    }

    glEnd();
}

void TestPattern::renderLabels()
{
    QFont textFont;
    textFont.setPixelSize(FONT_SIZE);

    glColor3f(1.,1.,1.);

    unsigned int pos = 0;
    foreach (QString label, labels_)
        glWindow_->renderText(TEXT_POS_X, ++pos * FONT_SIZE, label, textFont);
}
