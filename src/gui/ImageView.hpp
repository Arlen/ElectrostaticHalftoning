/// Copyright (C) 2025 Arlen Avakian
/// SPDX-License-Identifier: GPL-3.0-or-later


#pragma once

#include <QWidget>


namespace gui
{
    class ImageView : public QWidget
    {
        Q_OBJECT

    signals:
        void opened(QImage image);

    public:
        ImageView();

    protected:
        void dragEnterEvent(QDragEnterEvent* event) override;
        void dropEvent(QDropEvent* event) override;
        void paintEvent(QPaintEvent* event) override;

    private:
        QImage _image;
    };
}