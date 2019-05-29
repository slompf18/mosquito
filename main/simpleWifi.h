#pragma once

typedef void (*SimpleWifiCallback)(void);

void init_simple_wifi(char* ssid, char* passwd, void* callback);
