/// Copyright (C) 2025 Arlen Avakian
/// SPDX-License-Identifier: GPL-3.0-or-later


#include "Controller.hpp"

#include <QImage>
#include <QTimer>


using namespace core;

Controller::Controller(QObject* parent)
    : QObject(parent)
{
    _eh = new ElectrostaticHalftoning(this);

    connect(_eh, &ElectrostaticHalftoning::iterationFinished, this, &Controller::generated);
}

void Controller::consume(const QImage& image)
{
    _eh->setValues(core::normalizedValues(image), image.width(), image.height());
    QTimer::singleShot(0, [this]{ iterate(); });
}

void Controller::setParticleCount(int count)
{
    _eh->setParticleCount(count);
    iterate();
}

void Controller::setParticleRadius(f32 radius)
{
    _eh->setParticleRadius(radius);
    iterate();
}

void Controller::setIterationCount(int i)
{
    _eh->setMaxIteration(i);
    iterate();
}

void Controller::iterate()
{
    _eh->nextIteration();
    if (_eh->currentIteration() < _eh->maxIterations()) {
        QTimer::singleShot(0, [this]{ iterate(); });
    }
}