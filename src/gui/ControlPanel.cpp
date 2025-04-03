/// Copyright (C) 2025 Arlen Avakian
/// SPDX-License-Identifier: GPL-3.0-or-later

#include "ControlPanel.hpp"
#include "Slider.hpp"

#include <QDoubleValidator>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>

#include <functional>
#include <ranges>


using namespace gui;

namespace
{
    QVector<QVariant> powerOfTwos(std::uint32_t start, uint32_t stop)
    {
        using namespace std::views;

        Q_ASSERT(start <= stop+1);

        QVector<QVariant> result;

        std::ranges::copy(iota(start, stop+1) | transform(std::bind_front(pow, 2)),
            std::back_inserter(result));

        return result;
    }

    auto createRadiusLineEdit(QWidget* parent = nullptr)
    {
        auto* lineEdit  = new QLineEdit(parent);
        auto* validator = new QDoubleValidator(0.000001, 8.0, 7, lineEdit);
        validator->setNotation(QDoubleValidator::StandardNotation);
        validator->setLocale(QLocale::C);
        lineEdit->setValidator(validator);
        lineEdit->setText("1.0");

        return lineEdit;
    }
}

ControlPanel::ControlPanel(QWidget* parent)
    : QWidget(parent)
{
    /// layout
    auto* layout = new QGridLayout(this);
    layout->setHorizontalSpacing(4);
    layout->setVerticalSpacing(4);
    layout->setContentsMargins(0, 0, 0, 0);

    /// widgets
    auto* particles   = new Slider("Particles", powerOfTwos(8, 21), 4, this);
    auto* iterations  = new Slider("Iterations", powerOfTwos(0, 12), 4, this);
    auto* radiusLabel = new QLabel("Radius", this);
    auto* radiusEdit  = createRadiusLineEdit(this);

    /// connections
    connect(particles, &Slider::valueChanged, [this](const QVariant &val) {
        if (val.canConvert<int>()) {
            emit particleCountChanged(val.value<int>());
        }
    });
    connect(iterations, &Slider::valueChanged, [this](const QVariant &val) {
        if (val.canConvert<int>()) {
            emit iterationCountChanged(val.value<int>());
        }
    });
    connect(radiusEdit, &QLineEdit::textEdited, [this, radiusEdit](const QString &text) {
        if (radiusEdit->hasAcceptableInput()) {
            auto ok = false;
            if (const auto value = text.toDouble(&ok); ok) {
                emit particleRadiusChanged(value);
            }
        }
    });

    /// placement
    auto row = 0;
    auto col = 0;
    layout->addWidget(particles,  row, col++, 1, 1);

    col = 0;
    row++;
    layout->addWidget(iterations,  row, col++, 1, 1);
    layout->addWidget(radiusLabel, row, col++, 1, 1);
    layout->addWidget(radiusEdit,  row, col++, 1, 2);
}
