
#include <memory>
#include <vector>
n#pragma once

#include "base/signal.h"

class WaylandClipboard;

// Keeps the clipboard's contents once the application that copied them quits:
// every new selection is read and offered again from this process.
class Clipboard
{
public:
    Clipboard();
    ~Clipboard();

    Clipboard(const Clipboard &) = delete;
    Clipboard &operator=(const Clipboard &) = delete;

private:
    void onDataChanged();

    std::unique_ptr<WaylandClipboard> m_waylandClipboard;
    QList<Lingmo::Base::Subscription> m_subscriptions;
};

