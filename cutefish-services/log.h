// SPDX-FileCopyrightText: 2026 LingmoOS Contributors
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "base/logging.h"

namespace Lingmo::Services {

const Lingmo::Base::LogCategory &log();

} // namespace Lingmo::Services

#define LM_DEBUG(cat)    CF_DEBUG(cat)
#define LM_INFO(cat)     CF_INFO(cat)
#define LM_WARNING(cat)  CF_WARNING(cat)
#define LM_CRITICAL(cat) CF_CRITICAL(cat)
#define LM_FATAL(cat)    CF_FATAL(cat)
