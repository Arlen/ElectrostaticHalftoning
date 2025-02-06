/// Copyright (C) 2025 Arlen Avakian
/// SPDX-License-Identifier: GPL-3.0-or-later


#include "ParticlesView.hpp"

#include <QPainter>
#include <QVBoxLayout>


using namespace gui;

/// View
View::View(QWidget* parent)
    : QWidget(parent)
{
    _radius = 1.0;
}

void View::setRadius(qreal radius)
{
    _radius = radius;
    update();
}

void View::draw(const QVector<QPointF>& points)
{
    _points = points;
    update();
}

void View::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    painter.fillRect(rect(), Qt::white);

    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::black);

    for (const auto& p : _points) {
        painter.drawEllipse(p, _radius, _radius);
    }
}


ParticlesView::ParticlesView()
{
    auto* layout = new QVBoxLayout(this);
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);

    _view = new View(this);
    layout->addWidget(_view);

    QObject::connect(this, SIGNAL(radiusChanged(qreal)), _view, SLOT(setRadius(qreal)));
    QObject::connect(this, SIGNAL(pointsChanged(const QVector<QPointF>&)),
        _view, SLOT(draw(const QVector<QPointF>&)));
}