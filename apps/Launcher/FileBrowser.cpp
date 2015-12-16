/*********************************************************************/
/* Copyright (c) 2013, EPFL/Blue Brain Project                       */
/*                     Ahmet Bilgili <ahmet.bilgili@epfl.ch>         */
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

#include "FileBrowser.h"
#include "ImageProvider.h"

#include <dc/core/ContentFactory.h>

#include <deflect/Stream.h>
#include <deflect/Command.h>
#include <deflect/qt/QmlStreamer.h>

#include <QtQuick/QQuickItem>

const QUrl rootFolder( "file:///nfs4/bbp.epfl.ch/media/DisplayWall" );
const QUrl qml( "qrc:/FileBrowser/main.qml" );
const QString rootFolderOption = "rootfolder";
const QString imageProviderName = "folderimages";
const QString filterProperty = "filters";
QStringList filters = ContentFactory::getSupportedFilesFilter();

struct FileBrowser::Impl
{
    Impl( FileBrowser& fileBrowser,
          const QCommandLineParser& parser,
          deflect::qt::QmlStreamer& streamer )
        : streamer_( streamer )
    {

        QQuickItem* rootObject = streamer_.getRootItem();
        rootObject->setProperty( rootFolderOption.toStdString().c_str(),
                                 parser.value( rootFolderOption ));

        filters.append( "dcx" );

        rootObject->setProperty( filterProperty.toStdString().c_str(),
                                 filters );

        QObject::connect( rootObject, SIGNAL(itemSelected(QString)),
                          &fileBrowser, SLOT(_onItemSelected(QString)));

        QQmlEngine* engine = streamer_.getQmlEngine();
        engine->addImageProvider( imageProviderName, new ImageProvider );
    }

    void onItemSelected( QString filename )
    {
        deflect::Stream* stream = streamer_.getStream();
        if( stream->isConnected( ))
        {
            deflect::Command command( deflect::COMMAND_TYPE_FILE, filename );
            stream->sendCommand( command.getCommand( ).toStdString( ));
        }
    }

    ~Impl()
    {
        streamer_.getQmlEngine()->removeImageProvider( imageProviderName );
    }

    deflect::qt::QmlStreamer& streamer_;
};

FileBrowser::FileBrowser( const QCommandLineParser& parser,
                          deflect::qt::QmlStreamer& streamer )
    : impl_( new FileBrowser::Impl( *this,
                                    parser,
                                    streamer ))
{}

void FileBrowser::setParserOptions( QCommandLineParser& parser )
{
    const QCommandLineOption option( rootFolderOption,
                                     "Root folder name ",
                                     "folder",
                                     rootFolder.toString());
    parser.addOption( option );
}

void FileBrowser::_onItemSelected( QString filename )
{
    impl_->onItemSelected( filename );
}

FileBrowser::~FileBrowser()
{}
