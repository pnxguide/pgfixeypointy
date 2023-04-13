#pragma once

// clang-format off
extern "C" {
#include "../../src/include/postgres.h"
}
#undef vsnprintf
#undef snprintf
#undef vsprintf
#undef sprintf
#undef vfprintf
#undef fprintf
#undef vprintf
#undef printf
#undef gettext
#undef dgettext
#undef ngettext
#undef dngettext

// clang-format on