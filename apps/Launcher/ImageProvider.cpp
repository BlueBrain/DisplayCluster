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

#include "ImageProvider.h"

#include <dc/core/thumbnail/ThumbnailGeneratorFactory.h>
#include <dc/core/thumbnail/ThumbnailGenerator.h>

#include <QFileInfo>
#include <QImageReader>

ImageProvider::ImageProvider( const QSize defaultSize )
    : QQuickImageProvider( QQuickImageProvider::Image,
                           QQuickImageProvider::ForceAsynchronousImageLoading )
    , defaultSize_( defaultSize )
{}

QImage ImageProvider::requestImage( const QString& filename,
                                    QSize* size,
                                    const QSize& requestedSize)
{

    std::cout << "Filename: "  << filename.toStdString() << std::endl;

    const QSize newSize( requestedSize.height() > 0 ? requestedSize.height() : defaultSize_.height(),
                         requestedSize.width() > 0 ? requestedSize.width() : defaultSize_.width());

    std::cout << "Size: "  << newSize.height() << " " << newSize.width() << std::endl;

    if( size )
        *size = newSize;

    const QImage image = ThumbnailGeneratorFactory::getGenerator( filename, newSize )->generate( filename );
    if( image.isNull( ))
    {
        QFileInfo fileInfo( filename );

        if( fileInfo.isFile( ))
        {
            static QImage im = QImageReader( "qrc:/staticimages/unknownfile.png" ).read();
            if( im.isNull())
                throw std::runtime_error( "qrc:/staticimages/unknownfile.png not found" );

            return im;
        }
        else if( fileInfo.isDir( ))
        {
            static QImage im = QImageReader( "qrc:/staticimages/folder.png" ).read();
            if( im.isNull())
                throw std::runtime_error( "qrc:/staticimages/folder.png not found" );

            return im;
        }
    }


    return image;
}



