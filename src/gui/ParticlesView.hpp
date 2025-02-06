/// Copyright (C) 2025 Arlen Avakian
/// SPDX-License-Identifier: GPL-3.0-or-later


#pragma once

#include <QWidget>

#include "ImageView.hpp"


namespace gui
{
    class View final : public QWidget
    {
        Q_OBJECT

    public:
        View(QWidget* parent = nullptr);

    public slots:
        void setRadius(qreal radius);
        void draw(const QVector<QPointF>& points);

    protected:
        void paintEvent(QPaintEvent* event) override;

        qreal _radius;
        QVector<QPointF> _points;
    };


    class ParticlesView : public QWidget
    {
        Q_OBJECT

    signals:
        void radiusChanged(qreal radius);
        void pointsChanged(const QVector<QPointF>& points);

    public:
        ParticlesView();

    private:
        QWidget* _view;
    };
}
