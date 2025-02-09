/// Copyright (C) 2025 Arlen Avakian
/// SPDX-License-Identifier: GPL-3.0-or-later


#pragma once

#include <QWidget>


class QPushButton;

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
        void zoomIn();
        void zoomOut();

    private slots:
        void clearInfo();

    protected:
        void paintEvent(QPaintEvent* event) override;

        qreal _radius{1};
        qreal _scale{1};
        QString _info;
        QTimer* _timer;
        QVector<QPointF> _points;
    };


    class ParticlesView : public QWidget
    {
        Q_OBJECT

    signals:
        void zoomedIn();
        void zoomedOut();
        void radiusChanged(qreal radius);
        void particlesChanged(const QVector<QPointF>& points);

    public:
        ParticlesView(QWidget* parent = nullptr);

    protected:
        void resizeEvent(QResizeEvent* event) override;

    private:
        QWidget* _view{nullptr};
        QPushButton* _zoomIn{nullptr};
        QPushButton* _zoomOut{nullptr};
    };
}
