/// Copyright (C) 2025 Arlen Avakian
/// SPDX-License-Identifier: GPL-3.0-or-later


#pragma once

#include <QObject>
#include <QImage>

namespace core
{
    class Controller final : public QObject
    {
        Q_OBJECT

    signals:
        void generated(const QVector<QPointF>& points);

    public:
        Controller(QObject* parent = nullptr);

    public slots:
        void consume(QImage image);
        void setParticleCount(int count);
        void setIterationCount(int count);

    private:
        void generate();

        int _particleCount{256};
        QImage _image;
    };

    inline auto controller()
    {
        static Controller* controller = new Controller();

        return controller;
    }
}