#pragma once

// sdkconfig.h defines CONFIG_IDF_TARGET_*. Included directly so this header
// picks the right map no matter where it lands in a translation unit's
// include order — keyer.cpp and fsk.cpp reach pins.h before Arduino.h.
#include <sdkconfig.h>

// ============================================================
//  VUKEYER — GPIO map
//
//  Two boards, one header. The classic ESP32 devkit (esp32dev,
//  ESP32-D0WD-V3, CP2102) is the board in service; the ESP32-S3
//  N16R8 is the port target — its pin numbers are entirely
//  different, so the map is selected by chip at compile time and
//  neither half is allowed to drift into the other.
//
//  Common to both: pot must be on ADC1 (WiFi owns ADC2), and the
//  paddle jack convention is tip = dit, ring = dah
//  (software-swappable).
// ============================================================

#if defined(CONFIG_IDF_TARGET_ESP32S3)

// ------------------------------------------------------------
//  ESP32-S3 N16R8 (WROOM-1-N16R8, YD-ESP32-S3 style board)
//
//  Verified on the bench 2026-09-23 with esptool: 16 MB quad
//  flash, 8 MB octal PSRAM (AP_3v3), chip rev v0.2. That memory
//  configuration is what rules most of the low GPIOs out:
//
//    GPIO 26-32   quad SPI flash          unusable
//    GPIO 33-37   octal PSRAM (R8)        unusable
//    GPIO 19, 20  USB D-/D+               native USB — flashing
//                                         and the CDC host link
//    GPIO 0,3,45,46  strapping            0 is also the BOOT
//                                         button; 46 is input-only
//    GPIO 48      onboard WS2812 RGB      needs neopixelWrite(),
//                                         digitalWrite does nothing
//    GPIO 43, 44  U0TXD/U0RXD             wired to the on-board USB-UART
//                                         bridge behind the second USB-C
//                                         (header pins marked TX/RX)
//
//  ADC1 is GPIO 1-10 on this chip, so the pot has to live down
//  there — it is not GPIO 34 as on the classic part, where 34 was
//  input-only. Every S3 GPIO except 46 is a full input/output, so
//  the "input-only spares" note below does not apply here.
// ------------------------------------------------------------

#define PIN_PADDLE_DIT    5   // input, internal pullup, paddle closes to GND
#define PIN_PADDLE_DAH    6   // input, internal pullup, paddle closes to GND
#define PIN_KEY_OUT       7   // active high → NPN/optocoupler to rig KEY
#define PIN_PTT_OUT      15   // active high → NPN/optocoupler to rig PTT
#define PIN_SIDETONE     16   // LEDC PWM → passive piezo
#define PIN_SPEED_POT     4   // ADC1_CH3; 10 k pot wiper, 3V3–GND
#define PIN_STATUS_LED   17   // EXTERNAL LED + series resistor. The board's
                              // own LED is the WS2812 on GPIO 48, which the
                              // digitalWrite() in keyer.cpp cannot drive.

// I²C — SSD1306 OLED. 8/9 are the Arduino core's default SDA/SCL for the
// S3 devkit variants, the same reasoning as 21/22 on the classic board.
#define PIN_I2C_SDA       8
#define PIN_I2C_SCL       9

#define PIN_KEY_OUT2     18   // radio 2 KEY — same drive as radio 1
#define PIN_PTT_OUT2     21   // radio 2 PTT (19/20 are USB, hence the jump)
#define PIN_FSK_OUT      14   // RTTY FSK keying line; mark = idle (invertible)

// Free: 1, 2, 10, 11, 12, 13, 38, 47, and 39-42 (JTAG pads — fine as GPIO
// unless a debug probe is wired). All of those are on the headers.
//
// 43/44 are broken out as TX/RX but are NOT free: they run to the board's
// USB-UART bridge, which is the only console left once native USB is given
// to the logger as the WinKeyer port. 35/36/37 are on the header too and
// look inviting — they are the octal PSRAM and will crash the board.

#else   // classic ESP32 (esp32dev, ESP32-D0WD-V3) — the board in service

// ------------------------------------------------------------
//  Chosen to avoid strapping pins (0,2,5,12,15) and boot-glitch
//  outputs. Pot on ADC1 (GPIO32-39).
// ------------------------------------------------------------

#define PIN_PADDLE_DIT   25   // input, internal pullup, paddle closes to GND
#define PIN_PADDLE_DAH   26   // input, internal pullup, paddle closes to GND
#define PIN_KEY_OUT      33   // active high → NPN/optocoupler to rig KEY
#define PIN_PTT_OUT      32   // active high → NPN/optocoupler to rig PTT
#define PIN_SIDETONE      4   // LEDC PWM → passive piezo
#define PIN_SPEED_POT    34   // ADC1_CH6, input-only; 10 k pot wiper, 3V3–GND
#define PIN_STATUS_LED    2   // onboard LED — lit while the key is down

// I²C — SSD1306 OLED. These are the ESP32's default I²C pins and every
// OLED library assumes them, which is why the OTRSP block was moved off
// them (2026-09-10) rather than the display being asked to move.
#define PIN_I2C_SDA      21
#define PIN_I2C_SCL      22

#define PIN_KEY_OUT2     18   // radio 2 KEY — same drive as radio 1
#define PIN_PTT_OUT2     19   // radio 2 PTT
#define PIN_FSK_OUT      27   // RTTY FSK keying line; mark = idle (invertible)

// The OTRSP/SO2R reservation that used to hold 16, 17, 27, 14, 13 and 5 is
// GONE (2026-09-11): there is no SO2R plan for this box, and holding six
// pins for work that was never going to start had begun to squeeze real
// features onto a resistor ladder. SO2R lives in ~/projects/SO2R box.
//
// Free: 13, 14, 16, 17, 23. Input-only spares: 35, 36, 39 (no
// internal pull-ups on those — a button there needs an external one).
//
// Avoid GPIO 5, 12 and 15: strapping pins, sampled at boot and pulsed on
// reset, so they make unreliable outputs and can stop the board booting.
// GPIO 6-11 are the SPI flash and are unusable. On a WROVER module 16/17
// are the PSRAM lines — fine on this WROOM-based D0WD-V3 devkit, but check
// before reusing them on a different board.

#endif
