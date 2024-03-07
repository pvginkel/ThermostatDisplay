#include "includes.h"

#include "Application.h"
#include "ESP_Panel.h"

extern "C" {
void app_main(void) {
    ESP_Panel panel;

    auto display = panel.begin();

    Application application(panel);

    application.begin(display);

    while (1) {
        application.process();

        panel.process();
    }
}
}
