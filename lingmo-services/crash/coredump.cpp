// SPDX-FileCopyrightText: 2026 LingmoOS Contributors
// SPDX-License-Identifier: GPL-3.0-or-later

#include "coredump.h"

#include <cstdlib>
#include <cstring>
#include <ctime>
#include <cxxabi.h>
#include <format>
#include <regex>

#include "base/strings.h"

using namespace Lingmo::Base;
using namespace Lingmo::Base::strings;

static const QString s_scopePrefix = "app-lingmo-";
static const QString s_scopeSuffix = ".scope";

// The scope name without its prefix and suffix: <escaped id>-<pid>.
static QString scopeInstance(const QString &unit)
{
    if (!startsWith(unit, s_scopePrefix) || !endsWith(unit, s_scopeSuffix)
        || unit.size() < s_scopePrefix.size() + s_scopeSuffix.size())
        return {};
    return unit.substr(s_scopePrefix.size(), unit.size() - s_scopePrefix.size() - s_scopeSuffix.size());
}

// Reverses systemd's unit name escaping, \xNN for each byte of UTF-8.
static QString unescape(const QString &text)
{
    QString result;
    for (size_t i = 0; i < text.size(); ++i) {
        if (text[i] == '\\' && i + 3 < text.size() && text[i + 1] == 'x') {
            const auto byte = toNumber<int>(QStringView(text).substr(i + 2, 2), 16);
            if (byte) {
                result += char(*byte);
                i += 3;
                continue;
            }
        }
        result += text[i];
    }
    return result;
}

QString Coredump::applicationId() const
{
    const QString instance = scopeInstance(userUnit);
    const size_t dash = instance.rfind('-');
    if (dash == QString::npos || dash == 0)
        return {};
    return unescape(instance.substr(0, dash));
}

uint32_t Coredump::applicationPid() const
{
    const QString instance = scopeInstance(userUnit);
    const size_t dash = instance.rfind('-');
    if (dash == QString::npos || dash == 0)
        return 0;
    return toNumber<uint32_t>(QStringView(instance).substr(dash + 1)).value_or(0);
}

QString Coredump::signalDescription() const
{
    const QString description = signal > 0 ? QString(::strsignal(signal)) : QString();
    if (signalName.empty())
        return description;
    if (description.empty())
        return signalName;
    return std::format("{} ({})", description, signalName);
}

// The local time as ISO 8601 writes it without a zone: 2026-10-06T12:34:56.
static QString isoTime(int64_t milliseconds)
{
    const time_t seconds = time_t(milliseconds / 1000);
    struct tm local {};
    localtime_r(&seconds, &local);
    char text[64];
    std::strftime(text, sizeof(text), "%Y-%m-%dT%H:%M:%S", &local);
    return text;
}

QString Coredump::report(const QString &name) const
{
    QString text;
    text += std::format("{} quit unexpectedly\n", name);
    text += std::format("Executable: {}\n", executable);
    if (!commandLine.empty())
        text += std::format("Command line: {}\n", commandLine);
    text += std::format("Signal: {}\n", signalDescription());
    if (time > 0)
        text += std::format("Time: {}\n", isoTime(time));
    for (const CoredumpThread &thread : threads) {
        text += std::format("\nThread {}:\n", thread.id);
        for (size_t i = 0; i < thread.frames.size(); ++i) {
            const CoredumpFrame &frame = thread.frames[i];
            text += std::format("#{} {} ({})\n", i, frame.function.empty() ? QString("??") : frame.function,
                                frame.module);
        }
    }
    return text;
}

QList<CoredumpThread> Coredump::parseStackTrace(const QString &message)
{
    static const std::regex threadLine("^Stack trace of thread (\\d+):$");
    // #3  0x00007fe7198dbede name (module + 0xdbede); the module part is
    // missing for an address no mapping covers.
    static const std::regex frameLine("^#\\d+\\s+0x[0-9a-fA-F]+\\s+(\\S+)(?:\\s+\\((.*?)(?: \\+ 0x[0-9a-fA-F]+)?\\))?$");

    QList<CoredumpThread> threads;
    for (const QStringView rawLine : splitViews(message, '\n')) {
        const QString line(trimmed(rawLine));
        std::smatch match;
        if (std::regex_match(line, match, threadLine)) {
            threads.push_back({match[1].str(), {}});
            continue;
        }
        if (threads.empty())
            continue;
        if (!std::regex_match(line, match, frameLine))
            continue;
        const QString symbol = match[1].str();
        QString module = match[2].matched ? match[2].str() : QString();
        if (module == "n/a")
            module.clear();
        threads.back().frames.push_back({symbol == "n/a" ? QString() : demangle(symbol), module});
    }
    return threads;
}

QString Coredump::demangle(const QString &symbol)
{
    if (!startsWith(symbol, "_Z"))
        return symbol;
    int status = 0;
    char *name = abi::__cxa_demangle(symbol.c_str(), nullptr, nullptr, &status);
    if (status != 0 || !name)
        return symbol;
    QString result = name;
    std::free(name);
    return result;
}
