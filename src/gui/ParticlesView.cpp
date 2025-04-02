/// Copyright (C) 2025 Arlen Avakian
/// SPDX-License-Identifier: GPL-3.0-or-later


#include "ParticlesView.hpp"

#include <QPainter>
#include <QPushButton>
#include <QTimer>
#include <QVBoxLayout>
#include <QSvgGenerator>


using namespace gui;

/// View
View::View(QWidget* parent)
    : QWidget(parent)
{
    _timer = new QTimer(this);
    connect(_timer, &QTimer::timeout, this, &View::clearInfo);
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

void View::increaseDotSize()
{
    _dotRadius = qBound(0.25, _dotRadius + 0.25, 8.0);
    _info  = QString("Dot Radius= %1px").arg(_dotRadius);

    _timer->start(2000);
    update();
}

void View::decreaseDotSize()
{
    _dotRadius = qBound(0.25, _dotRadius - 0.25, 8.0);
    _info  = QString("Dot Radius= %1px").arg(_dotRadius);

    _timer->start(2000);
    update();
}

void View::exportSvg(const QString& path, const QSize& size)
{
    QSvgGenerator generator;
    generator.setFileName(path);
    const auto width  = size.width() * _scale + _dotRadius*2.0;
    const auto height = size.height() * _scale + _dotRadius*2.0;
    generator.setSize(QSize(width, height));
    generator.setViewBox(QRectF(-_dotRadius, -_dotRadius, width, height));

    QPainter painter(&generator);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::black);
    for (const auto& p : _points) {
        painter.drawEllipse(p * _scale, _dotRadius, _dotRadius);
    }
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
        painter.drawEllipse(p * _scale, _dotRadius, _dotRadius);
    }

    if (!_info.isEmpty()) {
        QFontMetrics fmt(font());
        auto bbox = fmt.boundingRect(_info);
        bbox.moveCenter(rect().center());
        bbox.moveBottom(rect().bottom() - 16);
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
    _increaseDotSize = new QPushButton("O", this);  _increaseDotSize->setFixedSize(24, 24);
    _decreaseDotSize = new QPushButton("o", this);  _decreaseDotSize->setFixedSize(24, 24);
    connect(this, &ParticlesView::particlesChanged, _view, &View::draw);
    connect(this, &ParticlesView::exportSvg, _view, &View::exportSvg);

    connect(_zoomIn, &QPushButton::clicked, this, &ParticlesView::zoomedIn);
    connect(_zoomOut, &QPushButton::clicked, this, &ParticlesView::zoomedOut);
    connect(_increaseDotSize, &QPushButton::clicked, this, &ParticlesView::increasedDotSize);
    connect(_decreaseDotSize, &QPushButton::clicked, this, &ParticlesView::decreasedDotSize);

    connect(this, SIGNAL(zoomedIn()), _view, SLOT(zoomIn()));
    connect(this, SIGNAL(zoomedOut()), _view, SLOT(zoomOut()));

    connect(this, SIGNAL(increasedDotSize()), _view, SLOT(increaseDotSize()));
    connect(this, SIGNAL(decreasedDotSize()), _view, SLOT(decreaseDotSize()));
}

void ParticlesView::resizeEvent(QResizeEvent* event)
{
    const auto margin = QPoint(8, 8);
    const auto space  = QPoint(0, 4);
    const auto rec    = rect();
    const auto br     = rec.bottomRight() - _zoomOut->rect().bottomRight() - margin;
    const auto tr     = rec.topLeft();

    _zoomOut->move(br);
    _zoomIn->move(_zoomOut->geometry().topLeft() - _zoomIn->rect().bottomLeft() - space);

    const auto y = height() - (_increaseDotSize->height()*2 + space.y() + margin.y());
    _increaseDotSize->move(tr + QPoint(margin.x(), y));
    _decreaseDotSize->move(_increaseDotSize->geometry().bottomLeft() + space);

    QWidget::resizeEvent(event);
}