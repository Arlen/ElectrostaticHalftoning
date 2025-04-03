/// Copyright (C) 2025 Arlen Avakian
/// SPDX-License-Identifier: GPL-3.0-or-later


#include "Slider.hpp"

#include <QHBoxLayout>
#include <QLabel>
#include <QLocale>
#include <QSlider>


using namespace gui;

Slider::Slider(const QString& name, const QVector<QVariant>& values, int initIndex, QWidget* parent)
    : QWidget(parent)
    , _values(values)
{
    setFocusPolicy(Qt::StrongFocus);

    Q_ASSERT(initIndex < values.size());

    auto* layout = new QHBoxLayout(this);
    layout->setSpacing(4);
    layout->setContentsMargins(0, 0, 0, 0);

    auto* nameLabel = new QLabel(name, this);
    nameLabel->setFixedWidth(64);
    nameLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    layout->addWidget(nameLabel);

    _valueLabel = new QLabel(values[initIndex].toString(), this);
    _valueLabel->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
    layout->addWidget(_valueLabel);

    auto* slider = new QSlider(Qt::Horizontal, this);
    slider->setMinimum(0);
    slider->setMaximum(values.size() - 1);
    slider->setPageStep(1);
    slider->setSingleStep(1);
    slider->setValue(initIndex);
    layout->addWidget(slider);

    connect(slider, &QSlider::valueChanged, this, &Slider::setValue);
}

void Slider::setValue(int index)
{
    if (index >= 0 && index < _values.size()) {
        const auto x = _values.at(index);

        if (x.canConvert<long int>()) {
            _valueLabel->setText(QLocale::system().toString(x.toLongLong()));
        } else if (x.canConvert<double>()) {
            _valueLabel->setText(QLocale::system().toString(x.toDouble()));
        } else {
            _valueLabel->setText(x.toString());
        }

        emit valueChanged(x);
    }
}