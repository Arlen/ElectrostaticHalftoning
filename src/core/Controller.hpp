/// Copyright (C) 2025 Arlen Avakian
/// SPDX-License-Identifier: GPL-3.0-or-later


#pragma once

#include "eh.hpp"

#include <QObject>
#include <QImage>


namespace core
{
    class Controller final : public QObject
    {
        Q_OBJECT

    signals:
        void generated(const QVector<QPointF>& points, int iter, int iterMax);

    public:
        Controller(QObject* parent = nullptr);

    public slots:
        void consume(const QImage& image);
        void setParticleCount(int count);
        void setParticleRadius(f32 radius);
        void setIterationCount(int count);
        void iterate();

    private:
        core::ElectrostaticHalftoning* _eh{nullptr};
        QTimer* _timer{nullptr};
    };


    inline auto controller()
    {
        static Controller* controller = new Controller();

        return controller;
    }
}