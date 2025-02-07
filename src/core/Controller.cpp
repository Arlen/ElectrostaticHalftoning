/// Copyright (C) 2025 Arlen Avakian
/// SPDX-License-Identifier: GPL-3.0-or-later

#include "Controller.hpp"

#include <QImage>

#include <random>


using namespace core;


Controller::Controller(QObject* parent)
    : QObject(parent)
{


}

void Controller::consume(QImage image)
{
    _image = image;
    generate();
}

void Controller::setParticleCount(int count)
{
    _particleCount = count;
    generate();
}

void Controller::setIterationCount(int count)
{

}

void Controller::generate()
{
    if (_image.isNull()) {
        return;
    }

    std::minstd_rand rng(time(0));
    std::uniform_real_distribution<double> urdX(0, _image.width() - 1);
    std::uniform_real_distribution<double> urdY(0, _image.height() - 1);

    QVector<QPointF> particles;
    for (int i = 0; i < _particleCount; i++) {
        auto x = urdX(rng);
        auto y = urdY(rng);
        particles.append(QPointF(x, y));
    }

    emit generated(particles);
}