/*********************************************************************/
/* Copyright (c) 2014, EPFL/Blue Brain Project                       */
/*                     Daniel Nachbaur <daniel.nachbaur@epfl.ch>     */
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

#ifndef MOCKTOUCHEVENTS_H
#define MOCKTOUCHEVENTS_H

#include <QTouchEvent>

const QSize widgetSize( 400, 400 );
QMap< int, QTouchEvent::TouchPoint > touchPointMap;

QEvent* createTouchEvent( const int id, const QEvent::Type eventType,
                          const Qt::TouchPointState state,
                          const QPointF& normalizedPosition )
{
    const QPoint position( widgetSize.width()  * normalizedPosition.x(),
                           widgetSize.height() * normalizedPosition.y( ));

    const Qt::TouchPointStates touchPointStates = Qt::TouchPointPrimary | state;
    QTouchEvent::TouchPoint touchPoint( id );
    touchPoint.setPressure( 1.0 );
    touchPoint.setNormalizedPos( normalizedPosition );
    touchPoint.setPos( position );
    touchPoint.setScenePos( normalizedPosition );
    touchPoint.setScreenPos( position );

    switch( eventType )
    {
    case QEvent::TouchBegin:
        touchPoint.setStartNormalizedPos( normalizedPosition );
        touchPoint.setStartPos( touchPoint.pos( ));
        touchPoint.setStartScreenPos( position );
        touchPoint.setStartScenePos( touchPoint.scenePos( ));

        touchPoint.setLastNormalizedPos( normalizedPosition );
        touchPoint.setLastPos( touchPoint.pos( ));
        touchPoint.setLastScreenPos( position );
        touchPoint.setLastScenePos( touchPoint.scenePos( ));
        break;

    case QEvent::TouchUpdate:
    case QEvent::TouchEnd:
    {
        const QTouchEvent::TouchPoint& prevPoint = touchPointMap.value( id );
        touchPoint.setStartNormalizedPos( prevPoint.startNormalizedPos( ));
        touchPoint.setStartPos( prevPoint.startPos( ));
        touchPoint.setStartScreenPos( prevPoint.startScreenPos( ));
        touchPoint.setStartScenePos( prevPoint.startScenePos( ));

        touchPoint.setLastNormalizedPos( prevPoint.normalizedPos( ));
        touchPoint.setLastPos( prevPoint.pos( ));
        touchPoint.setLastScreenPos( prevPoint.screenPos( ));
        touchPoint.setLastScenePos( prevPoint.scenePos( ));
        break;
    }
    default:
        ;
    }

    touchPointMap.insert( id, touchPoint );

    QEvent* event = new QTouchEvent( eventType, QTouchEvent::TouchScreen,
                                     Qt::NoModifier, touchPointStates,
                                     touchPointMap.values( ));

    if( eventType == QEvent::TouchEnd )
        touchPointMap.remove( id );

    return event;
}

#endif
