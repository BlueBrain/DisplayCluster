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

#ifndef PINCHGESTURERECOGNIZER_H
#define PINCHGESTURERECOGNIZER_H

#include <QGestureRecognizer>

/**
 * Gesture recognizer for a pinch gesture. The implementation enhances the Qt
 * shipped PinchGestureRecognizer to setup a normalized center position which is
 * required in the event handling of this application.
 * @sa QPinchGestureRecognizer
 */
class PinchGestureRecognizer : public QGestureRecognizer
{
public:
    /** Construct a new pan gesture recognizer object. */
    PinchGestureRecognizer();

    /** @sa QGestureRecognizer::create */
    QGesture* create( QObject *target ) override;

    /** @sa QGestureRecognizer::recognize */
    QGestureRecognizer::Result recognize( QGesture* state,
                                          QObject* watched,
                                          QEvent* event ) override;

    /** @sa QGestureRecognizer::reset */
    void reset( QGesture* state ) override;

    /**
     * Installs the pinch recognizer in the current QApplication.
     * @sa QGestureRecognizer::registerRecognizer
     */
    static void install();

    /**
     * Uninstalls the pinch recognizer from the current QApplication.
     * @sa QGestureRecognizer::unregisterRecognizer
     */
    static void uninstall();

    /** @return the gesture type to be used for gesture handling
     * @sa QWidget::grabGesture
     * @sa QGestureEvent::gesture
     */
    static Qt::GestureType type();

private:
    static Qt::GestureType _type;
};

#endif
