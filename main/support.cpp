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

int hextoi(char c) {
    if (c >= '0' && c <= '9') {
        return c - '0';
    }
    if (c >= 'A' && c <= 'F') {
        return 10 + (c - 'A');
    }
    if (c >= 'a' && c <= 'f') {
        return 10 + (c - 'a');
    }
    return -1;
}

esp_err_t esp_http_download_string(const esp_http_client_config_t& config, string& target, size_t maxLength) {
    target.clear();

    constexpr size_t BUFFER_SIZE = 1024;
    const auto bufferSize = maxLength > 0 ? min(maxLength + 1, BUFFER_SIZE) : BUFFER_SIZE;

    auto buffer = new char[bufferSize];
    auto err = ESP_OK;
    int64_t length = 0;

    auto client = esp_http_client_init(&config);

    if ((err = esp_http_client_open(client, 0)) != ESP_OK) {
        goto end;
    }

    length = esp_http_client_fetch_headers(client);
    if (length < 0) {
        err = -length;
        goto end;
    }

    while (true) {
        auto read = esp_http_client_read(client, buffer, bufferSize);
        if (read < 0) {
            err = -read;
            goto end;
        }
        if (read == 0) {
            break;
        }

        if (maxLength > 0 && target.length() + read > maxLength) {
            err = ESP_ERR_INVALID_SIZE;
            goto end;
        }

        target.append(buffer, read);
    }

end:
    esp_http_client_close(client);
    esp_http_client_cleanup(client);

    delete[] buffer;

    return err;
}
