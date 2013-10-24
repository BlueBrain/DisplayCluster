/*********************************************************************/
/* Copyright (c) 2013, EPFL/Blue Brain Project                       */
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

#define BOOST_TEST_MODULE Configuration
#include <boost/test/unit_test.hpp>
namespace ut = boost::unit_test;

#include <QCoreApplication>

#include "configuration/MasterConfiguration.h"
#include "Options.h"
#include "configuration/WallConfiguration.h"

#define CONFIG_TEST_FILENAME "./configuration.xml"


// We need a global fixture because a bug in QApplication prevents
// deleting then recreating a QApplication in the same process.
// https://bugreports.qt-project.org/browse/QTBUG-7104
struct GlobalQtApp
{
    GlobalQtApp()
    {
        // need QApplication to instantiate WebkitPixelStreamer
        ut::master_test_suite_t& testSuite = ut::framework::master_test_suite();
        app = new QCoreApplication( testSuite.argc, testSuite.argv );
    }
    ~GlobalQtApp()
    {
        delete app;
    }

    QCoreApplication* app;
};

BOOST_GLOBAL_FIXTURE( GlobalQtApp );

void testBaseParameters(const Configuration& config, OptionsPtr options)
{
    BOOST_CHECK( config.getBackgroundColor() == QColor("#242424") );
    BOOST_CHECK_EQUAL( config.getBackgroundUri().toStdString(), QString("/nfs4/bbp.epfl.ch/visualization/DisplayWall/media/background.png").toStdString() );

    BOOST_CHECK_EQUAL( config.getFullscreen(), true );

    options->setEnableMullionCompensation(false);
    BOOST_CHECK_EQUAL( config.getMullionHeight(), 0 );
    BOOST_CHECK_EQUAL( config.getMullionWidth(), 0 );
    options->setEnableMullionCompensation(true);
    BOOST_CHECK_EQUAL( config.getMullionHeight(), 12 );
    BOOST_CHECK_EQUAL( config.getMullionWidth(), 14 );

    BOOST_CHECK_EQUAL( config.getScreenHeight(), 1080 );
    BOOST_CHECK_EQUAL( config.getScreenWidth(), 3840 );

    options->setEnableMullionCompensation(false);
    BOOST_CHECK_EQUAL( config.getTotalHeight(), 3240 );
    BOOST_CHECK_EQUAL( config.getTotalWidth(), 7680 );
    options->setEnableMullionCompensation(true);
    BOOST_CHECK_EQUAL( config.getTotalHeight(), 3264 );
    BOOST_CHECK_EQUAL( config.getTotalWidth(), 7694 );

    BOOST_CHECK_EQUAL( config.getTotalScreenCountX(), 2 );
    BOOST_CHECK_EQUAL( config.getTotalScreenCountY(), 3 );
}

BOOST_AUTO_TEST_CASE( test_configuration )
{
    OptionsPtr options(new Options());

    Configuration config(CONFIG_TEST_FILENAME, options);

    testBaseParameters(config, options);
}


BOOST_AUTO_TEST_CASE( test_wall_configuration )
{
    OptionsPtr options(new Options());

    WallConfiguration config(CONFIG_TEST_FILENAME, options, 1);

    BOOST_CHECK_EQUAL( config.getDisplay().toStdString(), QString(":0.2").toStdString() );
    BOOST_CHECK( config.getGlobalScreenIndex(0) == QPoint(0,0) );
    BOOST_CHECK_EQUAL( config.getHost().toStdString(), QString("bbplxviz03i").toStdString() );

    BOOST_CHECK_EQUAL( config.getScreenCount(), 1 );
}


BOOST_AUTO_TEST_CASE( test_master_configuration )
{
    OptionsPtr options(new Options());

    MasterConfiguration config(CONFIG_TEST_FILENAME, options);

    BOOST_CHECK_EQUAL( config.getDockStartDir().toStdString(), QString("/nfs4/bbp.epfl.ch/visualization/DisplayWall/media").toStdString() );
}

BOOST_AUTO_TEST_CASE( test_save_configuration )
{
    OptionsPtr options(new Options());
    {
        Configuration config(CONFIG_TEST_FILENAME, options);
        config.setBackgroundColor(QColor("#123456"));
        BOOST_CHECK( config.save("configuration_modified.xml") );
    }

    // Check reloading
    Configuration config("configuration_modified.xml", options);
    BOOST_CHECK( config.getBackgroundColor() == QColor("#123456") );
}