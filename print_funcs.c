// Misc print and output functions for ap.c

#include "c_types.h"
#include "osapi.h"
#include "user_interface.h"
#include "print_funcs.h"

// New:  Read the system voltage.  Note that in order for this to work you HAVE to
// set the 127th (last) byte of esp_init_data_default.bin to 0xFF (255) and
// use esptool.py write_flash to write the new esp_init_data_default.bin to
// 0xFC000.  Otherwise you will get 65535 or some other garbage.  See the Non-OS
// SDK API Reference for details.
// Well ... it turns out that os_printf doesn't support float/double!  So ...
// you have to use a fancy hack to convert the float into a whole number and 
// its 2 digit decimal component and then display both with 2 %d format specs:
//   sys_volts/1024 will result in a truncate and will be the whole number
//   ((sys_volts%1024)*100)/1024 will result in another truncate and will be the
//   "decimal" portion (precision 2) of our number.  Try it by hand:
//   sys_volts = 2824
//   sys_volts/1024 = 2 (use bc and it will do the truncation automatically)
//   sys_volts%1024 = 776
//   776 * 100 = 77600
//   77600/1024 = 75 (again use bc or you will have to truncate it yourself)
//   Whole = 2 and decimal = 75 OR 2.75 :)
  
void ICACHE_FLASH_ATTR print_welcome(void) {
  uint16 sys_volts = system_get_vdd33();
  os_printf("\n\n\nWelcome to ESP8266\n");
  os_printf("Debug output on GPIO2 is active\n");
  os_printf("SDK Version: %s\n", system_get_sdk_version());
  os_printf("Chip ID: %d\n", system_get_chip_id());
  os_printf("System Voltage: %d.%d\n", sys_volts/1024, ((sys_volts%1024)*100)/1024);
}

void ICACHE_FLASH_ATTR print_config(struct softap_config *ptrconfig) {
 os_printf("AP Configuration:\n");
 os_printf("SSID: %s\n", ptrconfig->ssid);
 os_printf("Password:  %s\n", ptrconfig->password);
 os_printf("Auth Mode: %d\n", ptrconfig->authmode);
}
