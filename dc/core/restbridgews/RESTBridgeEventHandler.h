/*********************************************************************/
/* Copyright (c) 2015, EPFL/Blue Brain Project                       */
/*                     Ahmet Bilgili <ahmet.bilgili@epfl.ch>       */
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

#ifndef RESTBRIDGEEVENTHANDLER_H
#define RESTBRIDGEEVENTHANDLER_H

#include <zeq/types.h>

#include <QObject>

/**
 * The event handler for RESTBridge events. The implementation guarantees
 * that zeq events are always processed in main event loop.
 */
class RESTBridgeEventHandler : public QObject
{
    Q_OBJECT

public:

    RESTBridgeEventHandler( const servus::uint128_t& eventId )
        : _eventId( eventId )
    {
        QObject::connect( this, &_onEventReceivedSignal,
                          this, &S_onEventReceivedSlot,
                          Qt::QueuedConnection );
    }

    ~RESTBridgeEventHandler()
    {}

    virtual onEvent( const zeq::Event& event ) = 0;

    const zeq::EventFunc& getEventFunc() const
        { return zeq::EventFunc( &RESTBridgeEventHandler::_onZeqEvent, this ); }

    const servus::uint128_t& getEventId() const
        { return _eventId; }

signals:

    void _onEventReceivedSignal( const zeq::Event& event );

private slots:

    void _onEventReceivedSlot( const zeq::Event& event )
    {
        onEvent( event );
    }

private:

    void _onZeqEvent( const zeq::Event& event )
    {
        emit _onEventReceivedSignal( event );
    }

    servus::uint128_t _eventId;
};

#endif // RESTBRIDGEEVENTHANDLER_H

