// SPDX-FileCopyrightText: 2026 LingmoOS Contributors
// SPDX-License-Identifier: GPL-3.0-or-later

#include "application.h"

#include "base/i18n.h"
#include "ui/application.h"

int main(int argc, char *argv[])
{
    Lingmo::UI::Application app(argc, argv);
    app.setApplicationId("lingmo-services");
    Lingmo::I18n::setDomain("lingmo-core");
    Application services;
    return app.run();
}
