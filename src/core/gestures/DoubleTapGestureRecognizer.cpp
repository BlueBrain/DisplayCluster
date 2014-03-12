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

#include "DoubleTapGestureRecognizer.h"
#include "DoubleTapGesture.h"

#include <QTouchEvent>
#include <QWidget>

const int MAX_DELTA = 40; // pixel
const int TAP_TIMEOUT = 500; // ms


Qt::GestureType DoubleTapGestureRecognizer::type_ = Qt::CustomGesture;

void DoubleTapGestureRecognizer::install()
{
    type_ = QGestureRecognizer::registerRecognizer( new DoubleTapGestureRecognizer );
}

void DoubleTapGestureRecognizer::uninstall()
{
    QGestureRecognizer::unregisterRecognizer( type_ );
}

Qt::GestureType DoubleTapGestureRecognizer::type()
{
    return type_;
}

DoubleTapGestureRecognizer::DoubleTapGestureRecognizer()
    : firstPoint_( -1, -1 )
{
}

QGesture* DoubleTapGestureRecognizer::create( QObject* target )
{
    if( target && target->isWidgetType( ))
        static_cast< QWidget* >( target )->setAttribute( Qt::WA_AcceptTouchEvents );
    return new DoubleTapGesture;
}

QGestureRecognizer::Result
DoubleTapGestureRecognizer::recognize( QGesture* state, QObject* /*watched*/,
                                       QEvent* event )
{
    if( !state || !event )
        return QGestureRecognizer::Ignore;

    const QTouchEvent* touchEvent = dynamic_cast< const QTouchEvent* >( event );
    if( !touchEvent )
        return QGestureRecognizer::Ignore;

    DoubleTapGesture* gesture = static_cast< DoubleTapGesture* >( state );
    if( touchEvent->touchPoints().size() != 1 )
        return cancel( gesture );

    const QTouchEvent::TouchPoint& touchPoint = touchEvent->touchPoints().at(0);

    switch( event->type( ))
    {
    case QEvent::TouchBegin:
    {
        if( firstPoint_ == QPointF( -1, -1 ))
        {
            firstPointTime_.restart();
            return QGestureRecognizer::MayBeGesture;
        }

        // validate time and distance to first touch point, otherwise start fresh
        const QPoint delta = touchPoint.pos().toPoint() - firstPoint_.toPoint();
        if( delta.manhattanLength() > MAX_DELTA || firstPointTime_.elapsed() > TAP_TIMEOUT )
        {
            firstPoint_ = QPointF( -1, -1 );
            firstPointTime_.restart();
        }

        return QGestureRecognizer::MayBeGesture;
    }

    case QEvent::TouchUpdate:
    {
        const QPoint delta = touchPoint.pos().toPoint() - firstPoint_.toPoint();
        if( delta.manhattanLength() > MAX_DELTA )
            return cancel( gesture );
        return QGestureRecognizer::MayBeGesture;
    }

    case QEvent::TouchEnd:
    {
        // validate time and distance to begin of touch
        QPoint delta = touchPoint.pos().toPoint() - touchPoint.startPos().toPoint();
        if( delta.manhattanLength() > MAX_DELTA || firstPointTime_.elapsed() > TAP_TIMEOUT )
            return cancel( gesture );

        // confirm first touch point
        if( firstPoint_ == QPointF( -1, -1 ))
        {
            firstPoint_ = touchPoint.pos();
            return QGestureRecognizer::MayBeGesture;
        }

        gesture->setPosition( touchPoint.startScreenPos());
        gesture->setHotSpot( gesture->position( ));
        firstPoint_ = QPointF( -1, -1 );
        return QGestureRecognizer::FinishGesture;
    }

    default:
        return QGestureRecognizer::Ignore;
    }
}

void DoubleTapGestureRecognizer::reset( QGesture* state )
{
    QGestureRecognizer::reset( state );
}

QGestureRecognizer::Result
DoubleTapGestureRecognizer::cancel( DoubleTapGesture* gesture )
{
    firstPoint_ = QPointF( -1, -1 );
    gesture->setPosition( firstPoint_ );
    gesture->setHotSpot( gesture->position( ));
    return QGestureRecognizer::CancelGesture;
}
