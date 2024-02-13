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
    if (!buffer) {
        abort();
    }

    va_start(ap, fmt);
    vsprintf(buffer, fmt, ap);
    va_end(ap);

    auto result = string(buffer, length);

    free(buffer);

    return result;
}

static bool ichar_equals(char a, char b) {
    return std::tolower(static_cast<unsigned char>(a)) == std::tolower(static_cast<unsigned char>(b));
}

bool iequals(const string& a, const string& b) {
    return a.size() == b.size() && std::equal(a.begin(), a.end(), b.begin(), ichar_equals);
}
