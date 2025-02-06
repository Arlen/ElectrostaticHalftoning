/// Copyright (C) 2025 Arlen Avakian
/// SPDX-License-Identifier: GPL-3.0-or-later


#include "MainWindow.hpp"
#include "ImageView.hpp"
#include "ParticlesView.hpp"

#include <QVBoxLayout>
#include <QSplitter>


namespace gui
{
    MainWindow::MainWindow()
    {
        auto* layout = new QVBoxLayout(this);
        layout->setSpacing(2);
        layout->setContentsMargins(0, 0, 0, 0);

        auto splitter = new QSplitter(this);
        splitter->setHandleWidth(8);
        layout->addWidget(splitter);

        splitter->setOrientation(Qt::Horizontal);
        splitter->addWidget(new ImageView);
        splitter->addWidget(new ParticlesView);
    }
}