/// Copyright (C) 2025 Arlen Avakian
/// SPDX-License-Identifier: GPL-3.0-or-later


#include "ImageView.hpp"

#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QPainter>


using namespace gui;

ImageView::ImageView()
{
    setAcceptDrops(true);
}

void ImageView::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasUrls()) {
        auto url = event->mimeData()->urls().first();
        if (url.isLocalFile()) {
            event->acceptProposedAction();
            return;
        }
    }

    QWidget::dragEnterEvent(event);
}

void ImageView::dropEvent(QDropEvent *event)
{
    if (auto url = event->mimeData()->urls().first(); url.isLocalFile()) {
        if (QImage image(url.toLocalFile()); !image.isNull()) {
            _image = image;
            update();
            emit opened(image);
        }
        event->acceptProposedAction();
        return;
    }

    QWidget::dropEvent(event);
}

void ImageView::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.fillRect(rect(), Qt::Dense5Pattern);

    if (_image.isNull()) {
        QString text("Drag & Drop an image here");
        QFontMetrics fmt(painter.font());
        auto bb = fmt.boundingRect(text);
        bb.moveCenter(rect().center());

        painter.setRenderHint(QPainter::Antialiasing);
        painter.setPen(Qt::black);
        painter.setBrush(Qt::darkGray);
        painter.drawRoundedRect(bb.adjusted(-8, -2, 8, 2), 2, 2);
        painter.setPen(QColor(32, 32, 32, 255));
        painter.drawText(bb, Qt::AlignCenter, text);
        return;
    }

    painter.drawImage(QPoint(0, 0), _image);
}
