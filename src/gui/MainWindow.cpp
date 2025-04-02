/// Copyright (C) 2025 Arlen Avakian
/// SPDX-License-Identifier: GPL-3.0-or-later


#include "MainWindow.hpp"
#include "ImageView.hpp"
#include "ParticlesView.hpp"
#include "ControlPanel.hpp"

#include "core/Controller.hpp"

#include <QFileDialog>
#include <QLabel>
#include <QMenuBar>
#include <QSplitter>
#include <QStackedLayout>
#include <QTimer>
#include <QVBoxLayout>


using namespace gui;

MainWindow::MainWindow()
{
    auto* layout = new QVBoxLayout(this);
    layout->setSpacing(0);
    layout->setContentsMargins(4, 4, 4, 4);

    auto* mb = new QMenuBar(this);
    auto* fileMenu = mb->addMenu("File");
    auto* exportAction = fileMenu->addAction("Export SVG");
    exportAction->setDisabled(true);
    fileMenu->addSeparator();
    auto* quitAction = fileMenu->addAction("Quit");
    layout->addWidget(mb);

    connect(quitAction, &QAction::triggered, qApp, &QApplication::quit);

    auto* splitter = new QSplitter(this);
    splitter->setHandleWidth(8);
    splitter->setOrientation(Qt::Horizontal);
    layout->addWidget(splitter);

    auto* imageView     = new ImageView(splitter);
    auto* particlesView = new ParticlesView(splitter);
    splitter->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    splitter->setSizes({32, 32});

    auto* ctrlPanel = new ControlPanel(this);
    auto* label     = new QLabel("computing force field", this);
    layout->addWidget(ctrlPanel);
    layout->addWidget(label);
    ctrlPanel->hide();
    label->hide();

    auto* timer = new QTimer(this);
    timer->setInterval(125);


    /// notify ParticlesView to redraw whenever particles have changed.
    connect(core::controller(), &core::Controller::generated, particlesView, &ParticlesView::particlesChanged);

    /// notify UI to show/hide Force Field progress.
    /// ControlPanel is hidden when Force Field is being computed.
    connect(core::controller(), &core::Controller::forceFieldStarted, ctrlPanel, &QWidget::hide);
    connect(core::controller(), &core::Controller::forceFieldStarted, label, &QWidget::show);
    connect(core::controller(), &core::Controller::forceFieldStarted, timer, qOverload<>(&QTimer::start));

    connect(timer, &QTimer::timeout, [label] {
        auto str = label->text();
        str.append('.');
        if (str.endsWith("....")) {
            str.chop(4);
        }
        label->setText(str);
    });

    connect(core::controller(), &core::Controller::forceFieldGenerated, ctrlPanel, &QWidget::show);
    connect(core::controller(), &core::Controller::forceFieldGenerated, label, &QWidget::hide);
    connect(core::controller(), &core::Controller::forceFieldGenerated, timer, &QTimer::stop);


    /// notify controller whenever user provides an image as input.
    connect(imageView, &ImageView::opened, core::controller(), &core::Controller::consume);
    /// notify controller whenever the user changes particle count.
    connect(ctrlPanel, &ControlPanel::particleCountChanged, core::controller(), &core::Controller::setParticleCount);
    /// notify controller whenever the user changes particle radius.
    connect(ctrlPanel, &ControlPanel::particleRadiusChanged, core::controller(),  &core::Controller::setParticleRadius);
    /// notify controller whenever the user changes the iteration count.
    connect(ctrlPanel, &ControlPanel::iterationCountChanged, core::controller(), &core::Controller::setIterationCount);

    /// SVG export
    connect(core::controller(), &core::Controller::forceFieldStarted, [exportAction] {
        exportAction->setEnabled(false);
    });
    connect(core::controller(), &core::Controller::forceFieldGenerated, exportAction, [exportAction] {
        exportAction->setEnabled(true);
    });
    connect(exportAction, &QAction::triggered, [this, imageView] {
        auto path = QFileDialog::getSaveFileName(this, "Export SVG", QDir::homePath(), "Files (*.svg)");
        if (!path.isEmpty()) {
            emit exportSvgTriggered(path, imageView->image().size());
        }
    });

    connect(this, &MainWindow::exportSvgTriggered, particlesView, &ParticlesView::exportSvg);
}
