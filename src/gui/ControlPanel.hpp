/// Copyright (C) 2025 Arlen Avakian
/// SPDX-License-Identifier: GPL-3.0-or-later


#pragma once

#include <QWidget>


class QLineEdit;

namespace gui
{
    class ControlPanel final : public QWidget
    {
        Q_OBJECT

    signals:
        void particleRadiusChanged(qreal radius);
        void particleCountChanged(int count);
        void iterationCountChanged(int count);

    public:
        explicit ControlPanel(QWidget* parent = nullptr);
    };
}