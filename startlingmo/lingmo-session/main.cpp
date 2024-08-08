/*
 * Copyright (C) 2023-2024 LingmoOS Team.
 *
 * Author:     revenmartin <revenmartin@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "application.h"
#include <QCoreApplication>
#include <QQuickWindow>
#include <QCommandLineOption>
#include <qcommandlineparser.h>


int main(int argc, char *argv[])
{
    qputenv("QT_QPA_PLATFORM", QByteArrayLiteral("wayland"));
    qputenv("XDG_SESSION_TYPE", QByteArrayLiteral("wayland"));
    QQuickWindow::setDefaultAlphaBuffer(true);
    QCoreApplication::setAttribute(Qt::AA_DisableHighDpiScaling);

    QCoreApplication app(argc, argv);

    QCommandLineParser parser;
    parser.setApplicationDescription(QStringLiteral("Lingmo Session"));
    parser.addHelpOption();

    QCommandLineOption waylandOption(QStringList() << "w" << "wayland" << "Wayland Mode");
    parser.addOption(waylandOption);
    parser.process(app);

    new Application(parser, &app);

    return app.exec();
}
