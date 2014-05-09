/*********************************************************************/
/* Copyright (c) 2014, EPFL/Blue Brain Project                       */
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

#include "WallApplication.h"

#include "MPIChannel.h"
#include "configuration/WallConfiguration.h"
#include "Options.h"
#include "MainWindow.h"
#include "DisplayGroupManager.h"

WallApplication::WallApplication(int& argc_, char** argv_, MPIChannelPtr mpiChannel)
    : Application(argc_, argv_)
    , displayGroup_(new DisplayGroupManager)
{
    WallConfiguration* config = new WallConfiguration(getConfigFilename(),
                                                      mpiChannel->getRank());
    g_configuration = config;

    g_mainWindow = new MainWindow(config);

    connect(mpiChannel.get(), SIGNAL(received(DisplayGroupManagerPtr)),
            this, SLOT(updateDisplayGroup(DisplayGroupManagerPtr)));

    connect(mpiChannel.get(), SIGNAL(received(OptionsPtr)),
            this, SLOT(updateOptions(OptionsPtr)));

    // setup connection so renderFrame() will be called continuously.
    // Must be a queued connection to avoid infinite recursion.
    connect(this, SIGNAL(frameFinished()),
            this, SLOT(renderFrame()), Qt::QueuedConnection);
    renderFrame();
}

WallApplication::~WallApplication()
{
    // call finalize cleanup actions
    g_mainWindow->finalize();

    // destruct the main window
    delete g_mainWindow;
    g_mainWindow = 0;
}

void WallApplication::renderFrame()
{
    g_mpiChannel->receiveMessages(g_mainWindow->getPixelStreamFactory());

    // synchronize clock right after receiving messages to ensure we have an
    // accurate time for rendering, etc. below
    g_mpiChannel->synchronizeClock();

    // All processes swap windows sychronously
    g_mainWindow->updateGLWindows(displayGroup_);
    g_mpiChannel->globalBarrier();
    g_mainWindow->swapBuffers();

    displayGroup_->advanceContents();

    g_mainWindow->clearStaleFactoryObjects();

    ++g_frameCount;

    emit(frameFinished());
}

void WallApplication::updateDisplayGroup(DisplayGroupManagerPtr displayGroup)
{
    displayGroup_ = displayGroup;
}

void WallApplication::updateOptions(OptionsPtr options)
{
    g_configuration->setOptions(options);
}
