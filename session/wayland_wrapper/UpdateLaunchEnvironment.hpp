/*
    SPDX-FileCopyrightText: 2020 Kai Uwe Broulik <kde@broulik.de>
    SPDX-FileCopyrightText: 2021 David Edmundson <davidedmundson@kde.org>
    SPDX-FileCopyrightText: 2024 Elysia <c.elysia@foxmail.com>
 
    SPDX-License-Identifier: LGPL-2.0-or-later
*/
#ifndef UPDATELAUNCHENVIRONMENT_HPP
#define UPDATELAUNCHENVIRONMENT_HPP

#include <QProcessEnvironment>
 
#include <memory>
 
class QString;
class UpdateLaunchEnvironmentJobPrivate;
 
/**
 * @class UpdateLaunchEnvironmentJob updatelaunchenvironmentjob.h <UpdateLaunchEnvironmentJob>
 *
 * Job for updating the launch environment.
 *
 * This job adds or updates an environment variable in process environment that will be used
 * when a process is launched:
 * This includes:
 *  - DBus activation
 *  - Systemd units
 *  - lingmo-session
 *
 * Environment variables are sanitized before uploading.
 *
 * This object deletes itself after completion, similar to KJobs
 */
class  UpdateLaunchEnvironmentJob : public QObject
{
    Q_OBJECT
 
public:
    explicit UpdateLaunchEnvironmentJob(const QProcessEnvironment &environment);
    ~UpdateLaunchEnvironmentJob() override;
 
Q_SIGNALS:
    void finished();
 
private:
    void start();
 
private:
    std::unique_ptr<UpdateLaunchEnvironmentJobPrivate> const d;
};

#endif // UPDATELAUNCHENVIRONMENT_HPP