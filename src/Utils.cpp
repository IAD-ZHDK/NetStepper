#include "Utils.h"

String prefix(const char *prefix, const char *str) { return String(prefix) + String(str); }

String suffix(const char *str, const char *suffix) { return String(str) + String(suffix); }
