#pragma once

string format(const char* fmt, ...);

#ifdef NDEBUG
#define ESP_ERROR_ASSERT(x) \
    do {                    \
        (void)sizeof((x));  \
    } while (0)
#elif defined(CONFIG_COMPILER_OPTIMIZATION_ASSERTIONS_SILENT)
#define ESP_ERROR_ASSERT(x)   \
    do {                      \
        if (unlikely(!(x))) { \
            abort();          \
        }                     \
    } while (0)
#else
#define ESP_ERROR_ASSERT(x)                                                                                    \
    do {                                                                                                       \
        if (unlikely(!(x))) {                                                                                  \
            printf("ESP_ERROR_ASSERT failed");                                                                 \
            printf(" at %p\n", __builtin_return_address(0));                                                   \
            printf("file: \"%s\" line %d\nfunc: %s\nexpression: %s\n", __FILE__, __LINE__, __ASSERT_FUNC, #x); \
            abort();                                                                                           \
        }                                                                                                      \
    } while (0)
#endif

#define ESP_TIMER_MS(v) ((v) * 1000)
#define ESP_TIMER_SECONDS(v) ESP_TIMER_MS((v) * 1000)

#define ESP_ERROR_CHECK_JUMP(x, label)                                     \
    do {                                                                   \
        esp_err_t err_rc_ = (x);                                           \
        if (unlikely(err_rc_ != ESP_OK)) {                                 \
            ESP_LOGE(TAG, #x " failed with %s", esp_err_to_name(err_rc_)); \
            goto label;                                                    \
        }                                                                  \
    } while (0)

bool iequals(const string& a, const string& b);
int hextoi(char c);

void lv_label_get_text_size(lv_point_t* size_res, const lv_obj_t* obj, lv_coord_t letter_space, lv_coord_t line_space,
                            lv_coord_t max_width, lv_text_flag_t flag);

#ifdef LV_SIMULATOR

#define lv_obj_set_style_bg_img_src lv_obj_set_style_bg_image_src

lv_obj_t* lv_spinner_create(lv_obj_t* parent, uint32_t t, uint32_t angle);

#else

esp_err_t esp_http_download_string(const esp_http_client_config_t& config, string& target, size_t maxLength = 0);
esp_err_t esp_http_upload_string(const esp_http_client_config_t& config, const char* const data);

#endif
