/// Copyright (C) 2025 Arlen Avakian
/// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QWidget>


class QLabel;
class QSlider;

namespace gui
{
    class Slider final : public QWidget
    {
        Q_OBJECT

    signals:
        void valueChanged(QVariant value);

    public:
        Slider(QString name, QVector<QVariant> values, QWidget* parent = nullptr);

    private slots:
        void setValue(int i);

    private:
        QLabel* _valueLabel{nullptr};
        QVector<QVariant> _values;
    };
}