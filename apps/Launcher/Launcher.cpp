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

#include "Launcher.h"
#include "FileBrowser.h"

#include "localstreamer/CommandLineOptions.h"

#include <deflect/version.h>
#include <deflect/qt/QmlStreamer.h>

#include <QCommandLineParser>

const QUrl qmlFile( "qrc:/qml/qml/main.qml" );

struct Launcher::Impl
{
    Impl( Launcher& launcher )
        : launcher_( launcher )
    {

        QGuiApplication::setApplicationVersion(
                    QString::fromStdString( deflect::Version::getString( )));
        launcher_.setQuitOnLastWindowClosed( true );

        QCommandLineParser parser;

        // Set parser options ( Different modules can have different arguments )
        setParserOptions( parser );
        FileBrowser::setParserOptions( parser );
        parser.process( launcher_ );

        const QString streamHost = parser.value( "host" );
        const QString streamName = parser.value( "streamname" );

        qmlStreamer_.reset( new deflect::qt::QmlStreamer( qmlFile.toString(),
                                                          streamHost.toStdString(),
                                                          streamName.toStdString( )));



        fileBrowser_.reset( new FileBrowser( parser,
                                             *qmlStreamer_ ));

    }

    ~Impl()
    {}

    static void setParserOptions( QCommandLineParser& parser )
    {
        parser.setApplicationDescription( "Launcher" );
        parser.addHelpOption();
        parser.addVersionOption();

        const QCommandLineOption streamHostOption( "host", "Stream target hostname "
                                                     "(default: localhost)",
                                             "hostname", "localhost" );

        const QCommandLineOption streamNameOption( "streamname", "Stream name (default: "
                                                     "Qml's root objectName "
                                                     "property or 'QmlStreamer')",
                                             "name" );

        parser.addOption( streamNameOption );
        parser.addOption( streamHostOption );
    }

    Launcher& launcher_;
    QScopedPointer<FileBrowser> fileBrowser_;
    QScopedPointer<deflect::qt::QmlStreamer> qmlStreamer_;
};

Launcher::Launcher( int argc, char* argv[] )
    : QGuiApplication( argc, argv )
    , impl_( new Launcher::Impl( *this ))
{}

Launcher::~Launcher()
{}

