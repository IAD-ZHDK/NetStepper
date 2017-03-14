#include "Utils.h"

String prefix(const char *str, const char *prefix) { return String(prefix) + String(str); }

String suffix(const char *str, const char *suffix) { return String(str) + String(suffix); }
