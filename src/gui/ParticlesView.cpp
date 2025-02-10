/// Copyright (C) 2025 Arlen Avakian
/// SPDX-License-Identifier: GPL-3.0-or-later


#include "ParticlesView.hpp"

#include <QPainter>
#include <QPushButton>
#include <QTimer>
#include <QVBoxLayout>


using namespace gui;

/// View
View::View(QWidget* parent)
    : QWidget(parent)
{
    _timer = new QTimer(this);
    connect(_timer, &QTimer::timeout, this, &View::clearInfo);
}

void View::setRadius(qreal radius)
{
    _radius = radius;
    update();
}

void View::draw(const QVector<QPointF>& points, int iter, int iterMax)
{
    _points  = points;
    _iter    = iter;
    _iterMax = iterMax;
    update();
}

void View::zoomIn()
{
    _scale = qBound(0.25, _scale + 0.25, 4.0);
    _info  = QString("Zoom %1x").arg(_scale);

    _timer->start(2000);
    update();
}

void View::zoomOut()
{
    _scale = qBound(0.25, _scale - 0.25, 4.0);
    _info  = QString("Zoom %1x").arg(_scale);

    _timer->start(2000);
    update();
}

void View::clearInfo()
{
    _info.clear();
    _timer->stop();
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
        painter.drawEllipse(p * _scale, _radius, _radius);
    }

    if (!_info.isEmpty()) {
        QFontMetrics fmt(font());
        auto bbox = fmt.boundingRect(_info);
        bbox.moveTopLeft(QPoint(0, 0) + QPoint(16, 8));
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setPen(Qt::black);
        painter.setBrush(Qt::darkGray);
        painter.drawRoundedRect(bbox.adjusted(-8, -2, 8, 2), 2, 2);
        painter.setPen(QColor(32, 32, 32, 255));
        painter.drawText(bbox, Qt::AlignCenter, _info);
    }

    if (_iter < _iterMax && _iterMax > 0) {
        const auto bot = rect().bottom()-3; /// 4px thick
        const auto perc = width() * (qreal(_iter)/qreal(_iterMax));
        painter.fillRect(QRect(0, bot, width(), 4), QColor(196, 196, 196, 255));
        painter.fillRect(QRect(0, bot, perc, 4), QColor(32, 196, 64, 255));
    }
}


ParticlesView::ParticlesView(QWidget* parent)
    : QWidget(parent)
{
    auto* layout = new QVBoxLayout(this);
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);

    _view = new View(this);
    layout->addWidget(_view);

    _zoomIn  = new QPushButton("+", this);  _zoomIn->setFixedSize(24, 24);
    _zoomOut = new QPushButton("-", this);  _zoomOut->setFixedSize(24, 24);

    connect(this, SIGNAL(radiusChanged(qreal)), _view, SLOT(setRadius(qreal)));
    connect(this, SIGNAL(particlesChanged(const QVector<QPointF>&, int, int)),
        _view, SLOT(draw(const QVector<QPointF>&, int, int)));

    connect(_zoomIn, &QPushButton::clicked, this, &ParticlesView::zoomedIn);
    connect(_zoomOut, &QPushButton::clicked, this, &ParticlesView::zoomedOut);

    connect(this, SIGNAL(zoomedIn()), _view, SLOT(zoomIn()));
    connect(this, SIGNAL(zoomedOut()), _view, SLOT(zoomOut()));
}

void ParticlesView::resizeEvent(QResizeEvent* event)
{
    const auto margin = QPoint(0, 8);
    const auto space  = QPoint(0, 4);
    const auto rec    = rect();
    const auto br     = rec.bottomRight() - _zoomOut->rect().bottomRight() - margin;

    _zoomOut->move(br);
    _zoomIn->move(_zoomOut->geometry().topLeft() - _zoomIn->rect().bottomLeft() - space);

    QWidget::resizeEvent(event);
}