#include "includes.h"

#include "support.h"

string format(const char* fmt, ...) {
    va_list ap;

    va_start(ap, fmt);
    auto length = vsnprintf(nullptr, 0, fmt, ap);
    va_end(ap);

    if (length < 0) {
        abort();
    }

    auto buffer = (char*)malloc(length + 1);
    vsprintf(buffer, fmt, ap);

    auto result = string(buffer, length);

    free(buffer);

    return result;
}