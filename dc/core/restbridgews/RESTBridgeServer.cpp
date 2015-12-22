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

#include "RESTBridgeServer.h"

#include <restbridge/RestBridge.h>
#include <zeq/event.h>
#include <zeq/publisher.h>
#include <zeq/subscriber.h>

#include <thread>
#include <functional>
#include <mutex>

struct RESTBridgeServer::Impl
{
    Impl( RESTBridgeServer& server, int argc, const char** argv )
        : _server( server )
        , _restBridge( restbridge::RestBridge::parse( argc, argv ))
        , _subscriber( _restBridge->getSubscriberURI( ))
        , _running( true )
        , _receiveThread( this, &RESTBridgeServer::Impl::receive )
    {
        if( !_restBridge )
            throw std::runtime_error( "RESTBridge initialization failed" );

        if( !_restBridge.isRunning( ))
            throw std::runtime_error( "RESTBridge is not running" );

    }

    void receive()
    {
        while( _running )
        {
            std::lock_guard<std::mutex> lock( _lock );
            _subscriber.receive( 100 );
        }
    }

    ~Impl()
    {
        _running = false;
        _receiveThread.join();
    }

    void registerHandler( const RESTBridgeEventHandler& handler )
    {
        std::lock_guard<std::mutex> lock( _lock );
        _subscriber.registerHandler( handler->getEventId(),
                                     handler->getEventFunc( ));
    }

    void deregisterHandler( const RESTBridgeEventHandler& handler )
    {
        std::lock_guard<std::mutex> lock( _lock );
        _subscriber.deregisterHandler( handler->getEventId( ));
    }

    std::unique_ptr< restbridge::RestBridge > _restBridge;
    zeq::Subscriber _subscriber;
    bool _running;
    std::thread _receiveThread;
    std::mutex _lock;

};

RESTBridgeServer::RESTBridgeServer( int argc, const char** argv )
    : _impl( new RESTBridgeServer::Impl( *this, argc, argv ))
{}

RESTBridgeServer::~RESTBridgeServer() {}


void RESTBridgeServer::registerHandler( const RESTBridgeEventHandler& handler )
{
    _restBridge.registerHandler( handler );
}

void RESTBridgeServer::deregisterHandler( const RESTBridgeEventHandler& handler )
{
    _impl->deregisterHandler( handler );
}



