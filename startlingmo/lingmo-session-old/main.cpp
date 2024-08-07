/*
    SPDX-FileCopyrightText: 2024 Elysia <c.elysia@foxmail.com>

    SPDX-License-Identifier: GPL-3.0-or-later
*/
#include "startup.hpp"

#include <QCoreApplication>
#include <memory>

std::shared_ptr<Startup> startup_ptr;

int main(int argc, char **argv) {
  QCoreApplication app(argc, argv);

  startup_ptr = std::make_shared<Startup>(&app);

  app.exec();
}
