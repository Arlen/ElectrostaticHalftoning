/// Copyright (C) 2025 Arlen Avakian
/// SPDX-License-Identifier: GPL-3.0-or-later


#pragma once

#include <QWidget>


namespace gui
{
    class MainWindow : public QWidget
    {
        Q_OBJECT

    signals:
        void exportSvgTriggered(const QString& path, const QSize& size);

    public:
        MainWindow();
    };
}