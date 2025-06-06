/// Copyright (C) 2025 Arlen Avakian
/// SPDX-License-Identifier: GPL-3.0-or-later


#include "gui/MainWindow.hpp"
#include <QApplication>

#include <print>


int main(int argc, char *argv[])
{
    using std::println;

    println("Electrostatic Halftoning");
    println("Copyright (C) 2025 Arlen Avakian");

    QApplication app(argc, argv);
    auto* mw = new gui::MainWindow;
    mw->resize(800, 600);
    mw->show();

    return QApplication::exec();
}
