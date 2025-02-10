/// Copyright (C) 2025 Arlen Avakian
/// SPDX-License-Identifier: GPL-3.0-or-later


#include "MainWindow.hpp"
#include "ImageView.hpp"
#include "ParticlesView.hpp"
#include "ControlPanel.hpp"

#include "core/Controller.hpp"

#include <QVBoxLayout>
#include <QSplitter>


using namespace gui;

MainWindow::MainWindow()
{
    auto* layout = new QVBoxLayout(this);
    layout->setSpacing(2);
    layout->setContentsMargins(0, 0, 0, 0);

    auto* splitter = new QSplitter(this);
    splitter->setHandleWidth(8);
    splitter->setOrientation(Qt::Horizontal);
    layout->addWidget(splitter);

    auto* imageView = new ImageView(this);
    auto* particlesView = new ParticlesView(this);
    splitter->addWidget(imageView);
    splitter->addWidget(particlesView);
    splitter->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    splitter->setSizes({32, 32});

    auto* ctrlPanel = new ControlPanel(this);
    ctrlPanel->setMinimumHeight(64);
    layout->addWidget(ctrlPanel);

    /// notify ParticlesView to redraw whenever the user changes dot radius size.
    connect(ctrlPanel, &ControlPanel::particleSizeChanged, particlesView, &ParticlesView::radiusChanged);
    /// notify ParticlesView to redraw whenever particles have changed.
    connect(core::controller(), &core::Controller::generated, particlesView, &ParticlesView::particlesChanged);

    /// notify controller whenever user provides an image as input.
    connect(imageView, &ImageView::opened, core::controller(), &core::Controller::consume);
    /// notify controller whenever the user changes particle count.
    connect(ctrlPanel, &ControlPanel::particleCountChanged, core::controller(), &core::Controller::setParticleCount);
    /// notify controller whenever the user changes the iteration count.
    connect(ctrlPanel, &ControlPanel::iterationCountChanged, core::controller(), &core::Controller::setIterationCount);
}
