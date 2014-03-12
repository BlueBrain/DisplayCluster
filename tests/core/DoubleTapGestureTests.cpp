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

#define BOOST_TEST_MODULE DoubleTapGestureTests
#include <boost/test/unit_test.hpp>
namespace ut = boost::unit_test;

#include "MockTouchEvents.h"
#include <gestures/DoubleTapGesture.h>
#include <gestures/DoubleTapGestureRecognizer.h>


BOOST_AUTO_TEST_CASE( testInvalidDoubleTap )
{
    DoubleTapGestureRecognizer recognizer;
    BOOST_CHECK_EQUAL( recognizer.recognize( 0, 0, 0 ),
                       QGestureRecognizer::Ignore );
}

BOOST_AUTO_TEST_CASE( testValidDoubleTap )
{
    DoubleTapGestureRecognizer recognizer;

    const QPointF position( .6, .6 );
    QEvent* press = createTouchEvent( 0, QEvent::TouchBegin,
                                      Qt::TouchPointPressed, position );
    QEvent* release = createTouchEvent( 0, QEvent::TouchEnd,
                                        Qt::TouchPointReleased, position );

    DoubleTapGesture result;
    BOOST_CHECK_EQUAL( recognizer.recognize( &result, 0, press ),
                       QGestureRecognizer::MayBeGesture );
    BOOST_CHECK_EQUAL( recognizer.recognize( &result, 0, release ),
                       QGestureRecognizer::MayBeGesture );
    BOOST_CHECK_EQUAL( recognizer.recognize( &result, 0, press ),
                       QGestureRecognizer::MayBeGesture );
    BOOST_CHECK_EQUAL( recognizer.recognize( &result, 0, release ),
                       QGestureRecognizer::FinishGesture );
    BOOST_CHECK_EQUAL( result.position().x(),
                       position.x() * widgetSize.width());
    BOOST_CHECK_EQUAL( result.position().y(),
                       position.y() * widgetSize.height() );
    delete press;
    delete release;
}

BOOST_AUTO_TEST_CASE( testMultipleDoubleTaps )
{
    DoubleTapGestureRecognizer recognizer;

    const QPointF position1( .5, .5 );
    const QPointF position2( .3, .9 );
    const QPointF position3( .0, .1 );
    QEvent* press1 = createTouchEvent( 0, QEvent::TouchBegin,
                                       Qt::TouchPointPressed, position1 );
    QEvent* release1 = createTouchEvent( 0, QEvent::TouchEnd,
                                         Qt::TouchPointReleased, position1 );
    QEvent* press2 = createTouchEvent( 0, QEvent::TouchBegin,
                                       Qt::TouchPointPressed, position2 );
    QEvent* release2 = createTouchEvent( 0, QEvent::TouchEnd,
                                         Qt::TouchPointReleased, position2 );
    QEvent* press3 = createTouchEvent( 0, QEvent::TouchBegin,
                                       Qt::TouchPointPressed, position3 );
    QEvent* release3 = createTouchEvent( 0, QEvent::TouchEnd,
                                         Qt::TouchPointReleased, position3 );

    DoubleTapGesture result;
    BOOST_CHECK_EQUAL( recognizer.recognize( &result, 0, press1 ),
                       QGestureRecognizer::MayBeGesture );
    BOOST_CHECK_EQUAL( recognizer.recognize( &result, 0, release1 ),
                       QGestureRecognizer::MayBeGesture );
    BOOST_CHECK_EQUAL( recognizer.recognize( &result, 0, press1 ),
                       QGestureRecognizer::MayBeGesture );
    BOOST_CHECK_EQUAL( recognizer.recognize( &result, 0, release1 ),
                       QGestureRecognizer::FinishGesture );
    BOOST_CHECK_EQUAL( result.position().x(),
                       position1.x() * widgetSize.width());
    BOOST_CHECK_EQUAL( result.position().y(),
                       position1.y() * widgetSize.height() );

    BOOST_CHECK_EQUAL( recognizer.recognize( &result, 0, press1 ),
                       QGestureRecognizer::MayBeGesture );
    BOOST_CHECK_EQUAL( recognizer.recognize( &result, 0, release1 ),
                       QGestureRecognizer::MayBeGesture );
    BOOST_CHECK_EQUAL( recognizer.recognize( &result, 0, press2 ),
                       QGestureRecognizer::MayBeGesture );
    BOOST_CHECK_EQUAL( recognizer.recognize( &result, 0, release2 ),
                       QGestureRecognizer::MayBeGesture );
    BOOST_CHECK_EQUAL( recognizer.recognize( &result, 0, press2 ),
                       QGestureRecognizer::MayBeGesture );
    BOOST_CHECK_EQUAL( recognizer.recognize( &result, 0, release2 ),
                       QGestureRecognizer::FinishGesture );
    BOOST_CHECK_EQUAL( result.position().x(),
                       position2.x() * widgetSize.width());
    BOOST_CHECK_EQUAL( result.position().y(),
                       position2.y() * widgetSize.height() );

    BOOST_CHECK_EQUAL( recognizer.recognize( &result, 0, press3 ),
                       QGestureRecognizer::MayBeGesture );
    BOOST_CHECK_EQUAL( recognizer.recognize( &result, 0, release3 ),
                       QGestureRecognizer::MayBeGesture );
    BOOST_CHECK_EQUAL( recognizer.recognize( &result, 0, press3 ),
                       QGestureRecognizer::MayBeGesture );
    BOOST_CHECK_EQUAL( recognizer.recognize( &result, 0, release3 ),
                       QGestureRecognizer::FinishGesture );
    BOOST_CHECK_EQUAL( result.position().x(),
                       position3.x() * widgetSize.width());
    BOOST_CHECK_EQUAL( result.position().y(),
                       position3.y() * widgetSize.height() );
    delete press1;
    delete release1;
    delete press2;
    delete release2;
    delete press3;
    delete release3;
}

BOOST_AUTO_TEST_CASE( testWaitTooOffForDoubleTap )
{
    DoubleTapGestureRecognizer recognizer;

    const QPointF position1( .5, .5 );
    const QPointF position2( .5, 1. );
    QEvent* press1 = createTouchEvent( 0, QEvent::TouchBegin,
                                       Qt::TouchPointPressed, position1 );
    QEvent* release1 = createTouchEvent( 0, QEvent::TouchEnd,
                                         Qt::TouchPointReleased, position1 );
    QEvent* press2 = createTouchEvent( 0, QEvent::TouchBegin,
                                        Qt::TouchPointPressed, position2 );
    QEvent* release2 = createTouchEvent( 0, QEvent::TouchEnd,
                                         Qt::TouchPointReleased, position2 );

    DoubleTapGesture result;
    BOOST_CHECK_EQUAL( recognizer.recognize( &result, 0, press1 ),
                       QGestureRecognizer::MayBeGesture );
    BOOST_CHECK_EQUAL( recognizer.recognize( &result, 0, release1 ),
                       QGestureRecognizer::MayBeGesture );
    BOOST_CHECK_EQUAL( recognizer.recognize( &result, 0, press2 ),
                       QGestureRecognizer::MayBeGesture );
    BOOST_CHECK_EQUAL( recognizer.recognize( &result, 0, release2 ),
                       QGestureRecognizer::MayBeGesture );
    BOOST_CHECK_EQUAL( result.position().x(), -1 );
    BOOST_CHECK_EQUAL( result.position().y(), -1 );

    delete press1;
    delete release1;
    delete press2;
    delete release2;
}

BOOST_AUTO_TEST_CASE( testTooSlowForDoubleTap )
{
    DoubleTapGestureRecognizer recognizer;

    const QPointF position( .5, .5 );
    QEvent* press = createTouchEvent( 0, QEvent::TouchBegin,
                                      Qt::TouchPointPressed, position );
    QEvent* release = createTouchEvent( 0, QEvent::TouchEnd,
                                        Qt::TouchPointReleased, position );

    DoubleTapGesture result;
    BOOST_CHECK_EQUAL( recognizer.recognize( &result, 0, press ),
                       QGestureRecognizer::MayBeGesture );
    BOOST_CHECK_EQUAL( recognizer.recognize( &result, 0, release ),
                       QGestureRecognizer::MayBeGesture );
    usleep( 760000 );
    BOOST_CHECK_EQUAL( recognizer.recognize( &result, 0, press ),
                       QGestureRecognizer::MayBeGesture );
    BOOST_CHECK_EQUAL( recognizer.recognize( &result, 0, release ),
                       QGestureRecognizer::MayBeGesture );
    BOOST_CHECK_EQUAL( result.position().x(), -1 );
    BOOST_CHECK_EQUAL( result.position().y(), -1 );

    delete press;
    delete release;
}
