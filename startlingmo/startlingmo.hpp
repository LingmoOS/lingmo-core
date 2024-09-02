/*
    SPDX-FileCopyrightText: 2024 Elysia <c.elysia@foxmail.com>
    SPDX-FileCopyrightText: 2019 Aleix Pol Gonzalez <aleixpol@kde.org>

    SPDX-License-Identifier: GPL-3.0-or-later
*/
#ifndef STARTLINGMO_HPP
#define STARTLINGMO_HPP

#include <QLoggingCategory>
#include <QProcess>
#include <QStringList>
#include <optional>

void sigtermHandler(int signalNumber);

QStringList allServices(const QLatin1String &prefix);

void gentleTermination(QProcess *process);

int runSync(const QString &program, const QStringList &args,
            const QStringList &env = {});

void createConfigDirectory();

void setupCursor(bool wayland);

void runEnvironmentScripts();

void sourceFiles(const QStringList &files);

bool isShellVariable(const QByteArray &name);

void setEnvironmentVariable(const QByteArray &name, const QByteArray &value);

void setupLingmoEnvironment();

std::optional<QProcessEnvironment> getSystemdEnvironment();

bool syncDBusEnvironment();

void initLanguage();

void importSystemdEnvrionment();

bool startLingmoSession(bool wayland);

struct KillBeforeDeleter {
  void operator()(QProcess *pointer) {
    if (pointer) {
      gentleTermination(pointer);
    }
    delete pointer;
  }
};

bool isSessionVariable(const QByteArray &name);

void initScreenScaleFactors();

void messageBox(const QString &text);

void cleanupPlasmaEnvironment(
    const std::optional<QProcessEnvironment> &oldSystemdEnvironment);

#endif // STARTLINGMO_HPP