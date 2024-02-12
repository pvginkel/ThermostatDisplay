#include "includes.h"

#include "Application.h"

Application application;

extern "C" {
void app_main() { application.run(); }
}
