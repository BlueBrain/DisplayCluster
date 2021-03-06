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

#ifndef WEBKITHTMLSELECTREPLACER_H
#define WEBKITHTMLSELECTREPLACER_H

#include <QWebView>

/**
 * Substitutes all \<select\> elements on a webpage by equivalent HTML lists.
 *
 * This class is most useful when QWebKit renders without a window and the
 * system list elements are not working.
 *
 * It works by modifying the Html documents using Javascript after the page has
 * finished loading.
 */
class WebkitHtmlSelectReplacer : public QObject
{
    Q_OBJECT

public:
    /**
     * Constructor.
     * @param webView The QWebView on which to operate.
     */
    WebkitHtmlSelectReplacer( QWebView& webView );

    /**
     * Replace all \<select\> elements by Html equivalents.
     *
     * You only need to call this if new \<select\> elements have been
     * dynamically added after the page was loaded.
     */
    void replaceAllSelectElements();

private slots:
    void pageLoaded( bool success );

private:
    Q_DISABLE_COPY( WebkitHtmlSelectReplacer )

    QWebView& _webView;

    void _loadScripts();

    bool _hasJQuery();
    bool _hasJQueryUi();

    void _loadJavascript( const QString& jsFile );
    void _loadCssUsingJQuery( const QString& cssFile );
};

#endif // WEBKITHTMLSELECTREPLACER_H
