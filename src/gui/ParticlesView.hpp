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
        explicit View(QWidget* parent = nullptr);

    public slots:
        void draw(const QVector<QPointF>& points, int iter, int iterMax);
        void zoomIn();
        void zoomOut();
        void increaseDotSize();
        void decreaseDotSize();
        void exportSvg(const QString& path, const QSize& size);

    private slots:
        void clearInfo();

    protected:
        void paintEvent(QPaintEvent* event) override;

        qreal _dotRadius{1};
        qreal _scale{1};
        int _iter{0};
        int _iterMax{0};
        QString _info;
        QTimer* _timer;
        QVector<QPointF> _points;
    };


    class ParticlesView final : public QWidget
    {
        Q_OBJECT

    signals:
        void zoomedIn();
        void zoomedOut();
        void increasedDotSize();
        void decreasedDotSize();
        void particlesChanged(const QVector<QPointF>& points, int iter, int iterMax);
        void exportSvg(const QString& path, const QSize& size);

    public:
        explicit ParticlesView(QWidget* parent = nullptr);

    protected:
        void resizeEvent(QResizeEvent* event) override;

    private:
        View* _view{nullptr};
        QPushButton* _zoomIn{nullptr};
        QPushButton* _zoomOut{nullptr};
        QPushButton* _increaseDotSize{nullptr};
        QPushButton* _decreaseDotSize{nullptr};
    };
}
