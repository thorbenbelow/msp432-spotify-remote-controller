#include <cstdio>
#include "posix_io.h"
#include "uart_msp432.h"
#include "gpio_msp432.h"
#include "task.h"
#include "yahal_String.h"
#include "uGUI.h"
#include "uGUI_colors.h"
#include "font_8x12.h"
#include "spi_msp432.h"
#include "st7735s_drv.h"
#include "adc14_msp432.h"

String tracks[] = {"Mikrocontroller", "Elektriker", "Hawaii Toast Song", "Hyper Hyper", "What's up?", "Creep", "Feel Good", "Infectious", "Die Welt ist elefantastisch", "Mask Off"};


enum Calls {
    Filter,
    PauseResume,
    Play,
    Previous,
    Next,
    VolumeUp,
    VolumeDown
};

int current = 4;
int playing = 5;
int max = 9;

String sized(String &s) {
    if(s.size() > 13) {
        return s.substr(0, 10) + "...";
    } else {
        return s;
    }
}

void move(uGUI &gui, int old) {
    if(old == playing) {
        gui.SetForecolor(C_BLUE);
    } else {
        gui.SetForecolor(C_ORANGE);
    }
    gui.PutString(4, 4 + old*12, sized(tracks[old]));
    gui.SetForecolor(C_GREEN);
    gui.PutString(4, 4 + current*12, sized(tracks[current]));
}

void movePlaying(uGUI &gui, int old) {
    gui.SetForecolor(C_BLUE);
    gui.PutString(4, 4 + playing*12, sized(tracks[playing]));
    gui.SetForecolor(C_ORANGE);
    gui.PutString(4, 4 + old*12, sized(tracks[old]));
}

int main(void)
{
    gpio_msp432_pin left_btn(PORT_PIN(1, 1));
    gpio_msp432_pin right_btn(PORT_PIN(1, 4));
    gpio_msp432_pin play_btn(PORT_PIN(5, 1));
    gpio_msp432_pin pause_btn(PORT_PIN(3, 5));

    adc14_msp432_channel joy_Y( 9);
    adc14_msp432_channel acc_Z(11);
    adc14_msp432_channel acc_Y(13);
    adc14_msp432_channel acc_X(14);
    adc14_msp432_channel joy_X(15);
    joy_X.adcMode(ADC::ADC_10_BIT);
    joy_Y.adcMode(ADC::ADC_10_BIT);

    adc14_msp432 & adc = adc14_msp432::inst;
    adc.adcSetupScan(ADC::ADC_10_BIT);
    adc.adcStartScan(9, 15);


    left_btn.gpioMode(GPIO::INPUT | GPIO::PULLUP);
    right_btn.gpioMode(GPIO::INPUT | GPIO::PULLUP);

    // Setup SPI interface
    gpio_msp432_pin lcd_cs (PORT_PIN(5, 0));
    spi_msp432  spi(EUSCI_B0_SPI, lcd_cs);
    spi.setSpeed(24000000);

    // Setup LCD driver
    gpio_msp432_pin lcd_rst(PORT_PIN(5, 7));
    gpio_msp432_pin lcd_dc (PORT_PIN(3, 7));
    st7735s_drv lcd(spi, lcd_rst, lcd_dc, st7735s_drv::Crystalfontz_128x128);

    // Setup uGUI
    uGUI gui(lcd);

    // The backchannel UART (default 115200 baud)
    uart_msp432 uart;
    // Register backchannel UART for printf etc.
    posix_io::inst.register_stdout( uart );

    // UART which is connected to the ESP8266
    uart_msp432 uart_esp(EUSCI_A3,115200);

    String msg;
        uart_esp.uartAttachIrq([&](char c) {
            if (c == '\n') {
                printf("%s\n", msg.c_str());
                msg.clear();
            } else {
                msg += c;
            }
        });

    // Reset the ESP8266
    gpio_msp432_pin esp_reset( PORT_PIN(10, 5) );
    esp_reset.gpioMode(GPIO::OUTPUT | GPIO::INIT_LOW);
    task::sleep(200);
    esp_reset.gpioWrite( HIGH );
    task::sleep(200);
    gui.FontSelect(&FONT_8X12);

    auto joy_x_base = joy_X.adcReadScan();
    auto joy_y_base = joy_Y.adcReadScan();

    lcd.clearScreen(0x0);

    for(int i=0;i<max; i++) {
        if(i== current){
            gui.SetForecolor(C_GREEN);
        } else if(i == playing){
            gui.SetForecolor(C_BLUE);
        } else {
            gui.SetForecolor(C_ORANGE);
        }
        gui.PutString(4, 4 + i*12, sized(tracks[i]));
    }

    while (true) {
        if(!left_btn.gpioRead()) {
            printf("MSP::VolumeDown\n");
            uart_esp.puts(String((char) VolumeDown).c_str());
            task::sleep(200);
        }

        if(!right_btn.gpioRead()) {
            printf("MSP::VolumeUp\n");
            uart_esp.puts(String((char) VolumeUp).c_str());
            task::sleep(200);
        }

        if(joy_X.adcReadScan() - joy_x_base > 20) {
            printf("MSP::Next\n");
            uart_esp.puts(String((char) Next).c_str());
            task::sleep(200);
            if(playing < max) {
                playing++;
                movePlaying(gui, playing - 1);
            }
        }

        if(joy_X.adcReadScan() - joy_x_base < -20) {
            printf("MSP::Previous\n");
            uart_esp.puts(String((char) Previous).c_str());
            task::sleep(200);
            if(playing > 0) {
                 playing--;
                 movePlaying(gui, playing + 1);
            }
        }

        if(joy_Y.adcReadScan() - joy_y_base > 20) {
            if(current > 0) {
                current--;
                move(gui, current+1);
            }

        }

        if(joy_Y.adcReadScan() - joy_y_base < -20) {
            if(current < max) {
                current++;
                move(gui, current-1);
            }
        }

        if(!play_btn.gpioRead()) {
            printf("MSP::Play\n");
            uart_esp.puts(String((char) Play).c_str());
            uart_esp.puts(String((char) current).c_str());
            auto temp = playing;
            playing = current;
            movePlaying(gui, temp);
            task::sleep(200);
        }

        if(!pause_btn.gpioRead()) {
            printf("MSP::PauseResume\n");
            uart_esp.puts(String((char) PauseResume).c_str());
            task::sleep(200);
        }


    }
    return 0;
}
