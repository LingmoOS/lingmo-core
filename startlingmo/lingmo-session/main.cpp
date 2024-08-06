/*
    SPDX-FileCopyrightText: 2024 Elysia <c.elysia@foxmail.com>

    SPDX-License-Identifier: GPL-3.0-or-later
*/
#include "startup.hpp"

#include <QCoreApplication>

int main(int argc, char **argv) {
  QCoreApplication app(argc, argv);

  Startup::getInstance()->init(&app);
  
  app.exec();
}
