/****************************************************************************
**
** Copyright (C) 2018 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "printhandler.h"
#include <QPrintDialog>
#include <QPainter>
#include <QPrintPreviewDialog>
#include <QWebEngineView>

PrintHandler::PrintHandler(QObject *parent)
    : QObject(parent)
{
    m_printer.setResolution(300);
}

void PrintHandler::setView(QWebEngineView *view)
{
    Q_ASSERT(!m_view);
    m_view = view;
    connect(view, &QWebEngineView::printRequested, this, &PrintHandler::printPreview);
    connect(view, &QWebEngineView::printFinished, this, &PrintHandler::printFinished);
}

void PrintHandler::print()
{
    QPrintDialog dialog(&m_printer, m_view);
    if (dialog.exec() != QDialog::Accepted)
        return;
    printDocument(&m_printer);
}

void PrintHandler::printDocument(QPrinter *printer)
{
    m_view->print(printer);
    m_waitForResult.exec();
}

void PrintHandler::printFinished(bool success)
{
    if (!success) {
        QPainter painter;
        if (painter.begin(&m_printer)) {
            QFont font = painter.font();
            font.setPixelSize(20);
            painter.setFont(font);
            painter.drawText(QPointF(10,25),
                             QStringLiteral("Could not generate print preview."));
            painter.end();
        }
    }
    m_waitForResult.quit();
}

void PrintHandler::printPreview()
{
    if (!m_view)
        return;
    if (m_inPrintPreview)
        return;
    m_inPrintPreview = true;
    QPrintPreviewDialog preview(&m_printer, m_view);
    connect(&preview, &QPrintPreviewDialog::paintRequested,
            this, &PrintHandler::printDocument);
    preview.exec();
    m_inPrintPreview = false;
}
