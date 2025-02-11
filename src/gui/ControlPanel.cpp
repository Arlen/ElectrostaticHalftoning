/// Copyright (C) 2025 Arlen Avakian
/// SPDX-License-Identifier: GPL-3.0-or-later

#include "ControlPanel.hpp"
#include "Slider.hpp"

#include <QDoubleValidator>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>

#include <ranges>


using namespace gui;

namespace
{
    QVector<QVariant> powerOfTwos(int stop, int offset = 0)
    {
        using namespace std::views;

        QVector<QVariant> counts;

        for (auto val : iota(0, stop)
            | transform([offset](auto x) { return std::pow(2, x+offset); })) {
            counts.emplace_back(val);
        }

        return counts;
    }

    auto radiusEdit(QWidget* parent = nullptr)
    {
        auto* le = new QLineEdit(parent);
        auto* validator = new QDoubleValidator(0.000001, 8.0, 7, le);
        validator->setNotation(QDoubleValidator::StandardNotation);
        validator->setLocale(QLocale::C);
        le->setValidator(validator);
        le->setText("1.0");

        return le;
    }
}

ControlPanel::ControlPanel(QWidget* parent)
    : QWidget(parent)
{
    /// layout
    auto* layout = new QGridLayout(this);
    layout->setHorizontalSpacing(4);
    layout->setVerticalSpacing(2);
    layout->setContentsMargins(0, 0, 0, 0);

    /// widgets
    auto* particles  = new Slider("Particles", powerOfTwos(16, 8), 4, this);
    auto* iterations = new Slider("Iterations", powerOfTwos(13), 4, this);

    auto* radiusLabel = new QLabel("Radius", this);
    _radiusEdit = radiusEdit(this);

    /// connections
    connect(particles, &Slider::valueChanged, [this](QVariant val) {
        if (val.canConvert<int>()) {
            emit particleCountChanged(val.value<int>());
        }
    });
    connect(iterations, &Slider::valueChanged, [this](QVariant val) {
        if (val.canConvert<int>()) {
            emit iterationCountChanged(val.value<int>());
        }
    });
    connect(_radiusEdit, &QLineEdit::textEdited, this, &ControlPanel::validateRadiusEdit);

    /// placement
    int row = 0;
    int col = 0;
    layout->addWidget(particles,  row, col++, 1, 1);

    col = 0;
    row++;
    layout->addWidget(iterations,  row, col++, 1, 1);
    layout->addWidget(radiusLabel,   row, col++, 1, 1);
    layout->addWidget(_radiusEdit,     row, col++, 1, 2);
}

void ControlPanel::validateRadiusEdit(QString text)
{
    if (_radiusEdit && _radiusEdit->hasAcceptableInput()) {
        bool ok{false};
        auto value = text.toDouble(&ok);
        if (ok) {
            emit particleRadiusChanged(value);
        }
    }
}
