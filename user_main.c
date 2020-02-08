// Configure ESP8266 as AP
// 
// NOTE: EXAMPLE ONLY!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// This program does not register a system init done callback and it does
// not use WiFi callback events or functions. Don't use this code as a guide on
// how to properly configure the WiFi as an AP on ESP8266! 
// Why? Because the ESP8266 NONOS SDK does not contain an operating system..
// Therefore, there is no task scheduler. Also, user_init (unlike a regular C
// program main function) is asynchronous; it returns control to the SoC immediately
// after it is called. Therefore, you have no guarantee the the SoC has completed
// all its setup and configuration activities before you start to configure the WiFi.
// This example is just a PoC to evaluate the WiFi related APIs. Investigate the following
// items for more information:  system_init_done_cb and wifi_set_event_handler_cb.

// Compile, link and then convert to a bin using the Makefile:  
//      make clean 
//      make
// Then flash to device using the Makefile.  Device must be in program mode
// which means you have to press and hold RST, press and hold PROGRAM, release
// RST and finally release PROGRAM.  PROGRAM is nothing more than GPIO0 connected
// to ground via a switch and resistor.
//      make flash

// Connect GPIO2 to RX of FTDI232 and start screen (115200) to see the printf debug
// messages.  

// Includes:
//	- c_types.h gets us our type defs including attributes like ICACHE_FLASH_ATTR
//	- osapi.h gets us lots of os stuff like timers, printf, etc
//	- user_interface.h gets us the flash maps and all kinds of system and wifi structs
//	  enums and functions
//	- uart.h gets us the uart functions and enums ... note that it has to be copied into
//        the "include" directory because it lives in a non-standard location ... note that it
//	  also requires you to add -ldriver to the linker in Makefile to link uart.c
//      - print_funcs.h contains our user defined print and output functions
#include "credentials.h"
#include "c_types.h"
#include "osapi.h"
#include "user_interface.h"
#include "uart.h"
#include "print_funcs.h"

// RF Pre-Init function ... according to SDK API reference this needs to be
// in user_main.c even though we aren't using it.  It can be used to set RF 
// options. We aren't setting any options so just leave it empty.
void ICACHE_FLASH_ATTR user_rf_pre_init(void)
{
}

// RF calibrate sector function ... again ... SDK API says to add this to
// user_main.c.  It is used to set the RF calibration sector.  Note that the
// SDK API says that you have to add it to user_main.c but you don't ever have
// to call it; it will be called by the SDK itself. 
uint32 ICACHE_FLASH_ATTR user_rf_cal_sector_set(void)
{
    enum flash_size_map size_map = system_get_flash_size_map();
    uint32 rf_cal_sec = 0;

    switch (size_map) {
        case FLASH_SIZE_4M_MAP_256_256:
            rf_cal_sec = 128 - 5;
            break;

        case FLASH_SIZE_8M_MAP_512_512:
            rf_cal_sec = 256 - 5;
            break;

        case FLASH_SIZE_16M_MAP_512_512:
        case FLASH_SIZE_16M_MAP_1024_1024:
            rf_cal_sec = 512 - 5;
            break;

        case FLASH_SIZE_32M_MAP_512_512:
        case FLASH_SIZE_32M_MAP_1024_1024:
            rf_cal_sec = 1024 - 5;
            break;

        case FLASH_SIZE_64M_MAP_1024_1024:
            rf_cal_sec = 2048 - 5;
            break;
        case FLASH_SIZE_128M_MAP_1024_1024:
            rf_cal_sec = 4096 - 5;
            break;
        default:
            rf_cal_sec = 0;
            break;
    }

    return rf_cal_sec;
}

// Entry function ... execution starts here.  Note the use of attribute
// ICACHE_FLASH_ATTR which directs the ESP to store the user code in flash
// instead of RAM.
void ICACHE_FLASH_ATTR user_init (void) {
  
  char const *SSID = WIFI_SSID;
  char const *PASSWORD = WIFI_PASSWORD;
  
  // Use this function so that you can actually read the os_printf output ...
  // otherwise you get the standard baud rate don't match gibberish instead of
  // useful os_printf output ... you'll still see a little bit of gibberish at
  // boot or reset because the ESP starts to output boot messages at 74880 and then
  // switches to 115200
  uart_div_modify(0, UART_CLK_FREQ / 115200);

  // Print welcome messages ... these are in my custom module print_funcs.c
  print_welcome();
  
  // Get the current AP configuration
  struct softap_config config; 
  if (wifi_softap_get_config(&config)) {; 
    print_config(&config);
  } else {
    os_printf("Error getting existing AP Configuration\n");
  }
  
  // If the parameters are not correct for SSID, password, or authmode
  // (this can happen when you reflash esp_init_data_default.bin or blank.bin)
  // then load the correct parameters and redisplay the configuration.
  // Don't forget that config.ssid needs to be cast to a pointer because SSID is
  // itself a pointer (look above where you defined it). Also notice how we have to
  // null the SSID and password pointers or you will have junk left over from the 
  // previous run.  We use os_bzero to null these pointers (aka char arrays).
  //
  // Hmmm ... for some reason this isn't working ... evdidently SSD and config.ssid are NOT
  // exactly the same ... if you use os_printf wth %x you can see the difference.  putchar()
  // causes an error in compile ... wonder how we can find the acutal contents of config.ssid???
  // How about the xtensa_lx106_elf_gdb???
  if ((char *)config.ssid != SSID || (char *)config.password != PASSWORD || config.authmode != AUTH_WPA2_PSK) {
    config.ssid_len = 8;
    os_bzero(&config.ssid, 32);
    os_memcpy(&config.ssid, SSID, 8);
    os_bzero(&config.password, 64);
    os_memcpy(&config.password, PASSWORD, 10);
    config.authmode = AUTH_WPA2_PSK;
    if (wifi_softap_set_config(&config)) {
      print_config(&config);
    } else {
      os_printf("Error setting AP Configuration\n");
    }
  }
}
