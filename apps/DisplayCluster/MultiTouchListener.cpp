/*********************************************************************/
/* Copyright (c) 2013, EPFL/Blue Brain Project                       */
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

#include "MultiTouchListener.h"
#include "DisplayGroupView.h"
#include "log.h"

#include <QCoreApplication>
#include <QScreen>
#include <QWindow>

MultiTouchListener::MultiTouchListener( DisplayGroupView* targetWindow,
                                        const QSize& wallSize )
    : TUIO::TuioListener()
    , _targetWindow( targetWindow )
    , _device()
    , _wallSize( wallSize )
{
    _device.setType( QTouchDevice::TouchScreen );

    assert( _targetWindow );

    _client.addTuioListener( this );
    _client.connect();
}

MultiTouchListener::~MultiTouchListener()
{
    _client.removeTuioListener( this );
    _client.disconnect();
}

void MultiTouchListener::addTuioObject( TUIO::TuioObject* )
{
}

void MultiTouchListener::updateTuioObject( TUIO::TuioObject* )
{
}

void MultiTouchListener::removeTuioObject( TUIO::TuioObject* )
{
}

void MultiTouchListener::addTuioCursor( TUIO::TuioCursor* tcur )
{
    _handleEvent( tcur, QEvent::TouchBegin );
    emit touchPointAdded( tcur->getCursorID(), _getWallPos( tcur ));
}

void MultiTouchListener::updateTuioCursor( TUIO::TuioCursor* tcur )
{
    _handleEvent( tcur, QEvent::TouchUpdate );
    emit touchPointUpdated( tcur->getCursorID(), _getWallPos( tcur ));
}

void MultiTouchListener::removeTuioCursor( TUIO::TuioCursor* tcur )
{
    _handleEvent( tcur, QEvent::TouchEnd );
    emit touchPointRemoved( tcur->getCursorID( ));
}

void MultiTouchListener::refresh( TUIO::TuioTime )
{
}

QPointF MultiTouchListener::_getScreenPos( TUIO::TuioCursor* tcur ) const
{
    return _targetWindow->normalizedToScreenPos(
                QPointF( tcur->getX(), tcur->getY( )));
}

QPointF MultiTouchListener::_getWallPos( TUIO::TuioCursor* tcur ) const
{
    return QPointF( tcur->getX() * _wallSize.width(),
                    tcur->getY() * _wallSize.height( ));
}

void MultiTouchListener::_fillBegin( QTouchEvent::TouchPoint& touchPoint ) const
{
    touchPoint.setStartPos( touchPoint.pos( ));
    touchPoint.setStartScreenPos( touchPoint.screenPos( ));
    touchPoint.setStartNormalizedPos( touchPoint.normalizedPos( ));

    touchPoint.setLastPos( touchPoint.pos( ));
    touchPoint.setLastScreenPos( touchPoint.screenPos( ));
    touchPoint.setLastNormalizedPos( touchPoint.normalizedPos( ));
}

void MultiTouchListener::_fill( QTouchEvent::TouchPoint& touchPoint,
                               const QTouchEvent::TouchPoint& prevPoint) const
{
    touchPoint.setStartPos( prevPoint.startPos( ));
    touchPoint.setStartScreenPos( prevPoint.startScreenPos( ));
    touchPoint.setStartNormalizedPos( prevPoint.startNormalizedPos( ));

    touchPoint.setLastPos( prevPoint.pos( ));
    touchPoint.setLastScreenPos( prevPoint.screenPos( ));
    touchPoint.setLastNormalizedPos( prevPoint.normalizedPos( ));
}

void MultiTouchListener::_handleEvent( TUIO::TuioCursor* tcur,
                                      const QEvent::Type eventType )
{
    const QPointF screenPos = _getScreenPos( tcur );
    const QPointF normalizedPos( tcur->getX(), tcur->getY( ));

    QTouchEvent::TouchPoint touchPoint( tcur->getCursorID( ));
    touchPoint.setPressure( 1.0 );
    touchPoint.setPos( screenPos );
    touchPoint.setScreenPos( screenPos ); // used for QGesture::hotspot & itemAt

    touchPoint.setNormalizedPos( normalizedPos );

    Qt::TouchPointStates touchPointStates = 0;

    switch( eventType )
    {
    case QEvent::TouchBegin:
        touchPointStates = Qt::TouchPointPressed;
        _fillBegin( touchPoint );
        break;

    case QEvent::TouchUpdate:
    case QEvent::TouchEnd:
    {
        if( eventType == QEvent::TouchUpdate )
            touchPointStates = tcur->isMoving() ? Qt::TouchPointMoved
                                                : Qt::TouchPointStationary;
        else
            touchPointStates = Qt::TouchPointReleased;

        const int id = tcur->getCursorID();
        const QTouchEvent::TouchPoint& prevPoint = _touchPointMap.value( id );
        _fill( touchPoint, prevPoint );
        break;
    }

    default:
        put_flog( LOG_ERROR, "Got wrong touch event type %i", eventType );
        return;
    }

    touchPoint.setState( touchPointStates );
    _touchPointMap.insert( tcur->getCursorID(), touchPoint );

    QEvent* touchEvent = new QTouchEvent( eventType, &_device,
                                          Qt::NoModifier, touchPointStates,
                                          _touchPointMap.values( ));
    QCoreApplication::postEvent( _targetWindow, touchEvent );

    if( eventType == QEvent::TouchEnd )
        _touchPointMap.remove( tcur->getCursorID( ));
}
