/*
  Copyright (C) 2015 Coriolis Technologies Pvt Ltd, info@coriolis.co.in
  Author: Ganesh Varadarajan

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:
  
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the <organization> nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.
  
  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  ARE DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
  THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "webview.h"
#include "phantom.h"

#include <QWebFrame>
#include <QWebPage>
#include <QPainter>

WebView::WebView(QWidget* parent)
    : QWidget(parent), renderHints(QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform), m_webpage(0), m_qwebpage(0), readonly(false)
{
    setAttribute(Qt::WA_InputMethodEnabled);
    setAcceptDrops(true);
    setMouseTracking(true);
    setFocusPolicy(Qt::WheelFocus);
}

WebView::~WebView()
{
    setPage(0, 0);
}

WebPage* WebView::page()
{
    return m_webpage;
}

void WebView::setPage(WebPage* wp, QWebPage* qwp, bool ro)
{
    if (m_webpage) {
        m_webpage->mainFrame()->setScrollBarPolicy(Qt::Horizontal, Qt::ScrollBarAlwaysOff);
        m_webpage->mainFrame()->setScrollBarPolicy(Qt::Vertical, Qt::ScrollBarAlwaysOff);
        disconnect(m_webpage, 0, this, 0);
        disconnect(m_qwebpage, 0, this, 0);
    }

    m_webpage = wp;
    m_qwebpage = qwp;
    readonly = ro;

    if (!m_webpage) {
        return;
    }

    m_qwebpage->setViewportSize(size());
    m_webpage->mainFrame()->setScrollBarPolicy(Qt::Horizontal, Qt::ScrollBarAsNeeded);
    m_webpage->mainFrame()->setScrollBarPolicy(Qt::Vertical, Qt::ScrollBarAsNeeded);
    connect(m_webpage, SIGNAL(repaintRequested(const int, const int, const int, const int)), this, SLOT(handleRepaintRequested(const int, const int, const int, const int)));
    connect(m_qwebpage, SIGNAL(scrollRequested(int, int, const QRect&)), this, SLOT(handleScrollRequested(int, int, const QRect&)));
    connect(m_webpage, SIGNAL(urlChanged(const QUrl&)), this, SLOT(handleUrlChanged(const QUrl&)));
}

void WebView::paintEvent(QPaintEvent *ev)
{
    if (m_webpage) {
        QWebFrame* frame = m_webpage->mainFrame();
        QPainter p(this);
        p.setRenderHints(renderHints);
        frame->render(&p, ev->region());
    }
}

void WebView::handleRepaintRequested(const int x, const int y, const int width, const int height)
{
    if (m_webpage) {
        update(x, y, width, height);
    }
}

void WebView::handleScrollRequested(int x, int y, const QRect& scrollViewRect)
{
    update();
}

void WebView::mouseMoveEvent(QMouseEvent* ev)
{
/*
 *  Disabling mouse move events for now. They cause drag events to be
 *  inferred, and the drag handling code in webkit crashes given our 
 *  "no view" setup.

    if (m_webpage) {
        const bool accepted = ev->isAccepted();
        m_qwebpage->event(ev);
        ev->setAccepted(accepted);
    }
*/
}

void WebView::mousePressEvent(QMouseEvent* ev)
{
    if (m_webpage) {
        const bool accepted = ev->isAccepted();
        m_qwebpage->event(ev);
        ev->setAccepted(accepted);
    }
}

void WebView::mouseDoubleClickEvent(QMouseEvent* ev)
{
    if (m_webpage) {
        const bool accepted = ev->isAccepted();
        m_qwebpage->event(ev);
        ev->setAccepted(accepted);
    }
}

void WebView::mouseReleaseEvent(QMouseEvent* ev)
{
    if (m_webpage) {
        const bool accepted = ev->isAccepted();
        m_qwebpage->event(ev);
        ev->setAccepted(accepted);
    }
}

void WebView::wheelEvent(QWheelEvent* ev)
{
    if (m_webpage) {
        const bool accepted = ev->isAccepted();
        m_qwebpage->event(ev);
        ev->setAccepted(accepted);
    }
}

void WebView::keyPressEvent(QKeyEvent* ev)
{
    if (m_webpage) {
        m_qwebpage->event(ev);
    }
    if (!ev->isAccepted()) {
        QWidget::keyPressEvent(ev);
    }
}

void WebView::keyReleaseEvent(QKeyEvent* ev)
{
    if (m_webpage) {
        m_qwebpage->event(ev);
    }
    if (!ev->isAccepted()) {
        QWidget::keyReleaseEvent(ev);
    }
}

void WebView::stop()
{
    if (m_webpage) {
        m_webpage->stop();
    }
}

void WebView::back()
{
    if (m_webpage) {
        m_webpage->goBack();
    }
}

void WebView::forward()
{
    if (m_webpage) {
        m_webpage->goForward();
    }
}

void WebView::reload()
{
    if (m_webpage) {
        m_webpage->reload();
    }
}

void WebView::handleUrlChanged(const QUrl& url)
{
    emit urlChanged(url);
}

void WebView::handleUrlEntered(const QString& url)
{
    if (m_webpage) {
        QVariant op("get");
        m_webpage->openUrl(url, op, Phantom::instance()->defaultPageSettings());
    }
}
