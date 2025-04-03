/// Copyright (C) 2025 Arlen Avakian
/// SPDX-License-Identifier: GPL-3.0-or-later


#pragma once

#include "eh.hpp"

#include <QApplication>
#include <QObject>
#include <QImage>
#include <QThread>


namespace core
{
    class Controller final : public QObject
    {
        Q_OBJECT

    signals:
        void generated(const QVector<QPointF>& points, int iter, int iterMax);
        void forceFieldStarted();
        void forceFieldGenerated();

    public:
        explicit Controller(QObject* parent = nullptr);

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
        static Controller* controller = nullptr;

        if (controller == nullptr) {
            auto* thread = new QThread;
            controller   = new Controller;
            controller->moveToThread(thread);

            QObject::connect(qApp, &QApplication::aboutToQuit, thread, &QThread::quit);
            QObject::connect(thread, &QThread::finished, controller, &QObject::deleteLater);
            QObject::connect(thread, &QThread::finished, thread, &QObject::deleteLater);

            thread->start();
        }

        return controller;
    }
}