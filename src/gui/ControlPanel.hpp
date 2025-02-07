/// Copyright (C) 2025 Arlen Avakian
/// SPDX-License-Identifier: GPL-3.0-or-later


#pragma once

#include <QWidget>


namespace gui
{
    class ControlPanel final : public QWidget
    {
        Q_OBJECT

    signals:
        void particleSizeChanged(qreal size);
        void particleCountChanged(int count);
        void iterationCountChanged(int count);

    public:
        ControlPanel(QWidget* parent = nullptr);
    };
}