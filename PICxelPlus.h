#ifndef PICxelPlus_H
#define PICxelPlus_H

#include <Arduino.h>
#include <stdint.h>
#include <DSPI.h>

#define CODEMASK 0b100100100100100100100100

template <uint32_t S> class PICxelPlus {

    private:

        uint8_t     _colorArray[S * 3];
        uint8_t     _dataArray[S * 9 + 50] __attribute__((aligned(4),coherent));
        DSPI        *_spi;

        inline static uint32_t colourToCode(uint8_t c) {
            uint32_t o = CODEMASK;

            o |= ( c & 0x01 ? 0b000000000000000000000010 : 0);
            o |= ( c & 0x02 ? 0b000000000000000000010000 : 0);
            o |= ( c & 0x04 ? 0b000000000000000010000000 : 0);
            o |= ( c & 0x08 ? 0b000000000000010000000000 : 0);
            o |= ( c & 0x10 ? 0b000000000010000000000000 : 0);
            o |= ( c & 0x20 ? 0b000000010000000000000000 : 0);
            o |= ( c & 0x40 ? 0b000010000000000000000000 : 0);
            o |= ( c & 0x80 ? 0b010000000000000000000000 : 0);
            return o;
        }

    public:
        PICxelPlus(DSPI &spi) : _spi(&spi) {}
        PICxelPlus(DSPI *spi) : _spi(spi) {}
        ~PICxelPlus() {}

        void begin() {
            _spi->begin();
            _spi->setSpeed(2500000);
        }

        void refreshLEDs() {
            memset(_dataArray, 0, S * 9 + 50);
            uint32_t ptr = 0;
            for (int i = 0; i < S; i++) {
                uint8_t g = _colorArray[i * 3 + 0];
                uint8_t r = _colorArray[i * 3 + 1];
                uint8_t b = _colorArray[i * 3 + 2];
                uint32_t gblock = colourToCode(g);
                _dataArray[i * 9 + 0 + 50] = gblock >> 16;
                _dataArray[i * 9 + 1 + 50] = gblock >> 8;
                _dataArray[i * 9 + 2 + 50] = gblock;
                uint32_t rblock = colourToCode(r);
                _dataArray[i * 9 + 3 + 50] = rblock >> 16;
                _dataArray[i * 9 + 4 + 50] = rblock >> 8;
                _dataArray[i * 9 + 5 + 50] = rblock;
                uint32_t bblock = colourToCode(b);
                _dataArray[i * 9 + 6 + 50] = bblock >> 16;
                _dataArray[i * 9 + 7 + 50] = bblock >> 8;
                _dataArray[i * 9 + 8 + 50] = bblock;
            }

            _spi->transfer(S * 9 + 50, _dataArray);
        }

        void GRBsetLEDColor(uint16_t pixelNumber, uint8_t green, uint8_t red, uint8_t blue) {
            if (pixelNumber < S) {
                _colorArray[pixelNumber * 3 + 0] = green;
                _colorArray[pixelNumber * 3 + 1] = red;
                _colorArray[pixelNumber * 3 + 2] = blue;
            }
        }

        void GRBsetLEDColor(uint16_t pixelNumber, uint32_t color) {
            if (pixelNumber < S) {
                _colorArray[pixelNumber * 3 + 0] = color >> 8;
                _colorArray[pixelNumber * 3 + 1] = color >> 16;
                _colorArray[pixelNumber * 3 + 2] = color >> 0;
            }
        }

        void HSVsetLEDColor(uint16_t pixelNumber, uint16_t hue, uint8_t sat, uint8_t val) {
            uint32_t hsv = (hue << 16) | (sat << 8) | (val);
            HSVsetLEDColor(pixelNumber, hsv);
        }

        void HSVsetLEDColor(uint16_t pixelNumber, uint32_t color) {
            uint32_t rgb = HSVToColor(color);
            GRBsetLEDColor(pixelNumber, (rgb >> 8) & 0xFF, (rgb >> 16) & 0xFF, rgb & 0xFF);
        }

        void clear() {
            memset(_colorArray, 0, S * 3);
        }

        void clear(uint8_t pixelNumber) {
            if (pixelNumber < S) {
                _colorArray[pixelNumber * 3 + 0] = 0;
                _colorArray[pixelNumber * 3 + 1] = 0;
                _colorArray[pixelNumber * 3 + 2] = 0;
            }
        }

        uint32_t HSVToColor(unsigned int HSV) {
            uint8_t val = HSV & 0xFF;
            uint8_t sat = (HSV >> 8) & 0xFF;
            uint16_t hue = (HSV >> 16) & 0xFFFF;

            uint32_t H_accent = hue / 256;
            uint32_t bottom = ((256 - sat) * val) >> 8;
            uint32_t top = val;
            uint32_t rising = ((top - bottom) * (hue % 256)) / 256 + bottom;
            uint32_t falling = ((top - bottom) * (256 - hue % 256)) / 256 + bottom;

            int r = 0;
            int g = 0;
            int b = 0;

            switch (H_accent) {
                case 0:
                    r = top;
                    g = rising;
                    b = bottom;
                    break;
                case 1:
                    r = falling;
                    g = top;
                    b = bottom;
                    break;
                case 2:
                    r = bottom;
                    g = top;
                    b = rising;
                    break;
                case 3:
                    r = bottom;
                    g = falling;
                    b = top;
                    break;
                case 4:
                    r = rising;
                    g = bottom;
                    b = top;
                    break;
                case 5:
                    r = top;
                    g = bottom;
                    b = falling;
                    break;
            }

            if (r > 255) r = 255;
            if (g > 255) g = 255;
            if (b > 255) b = 255;

            return ((r & 0xFF) << 16 | (g & 0xFF) << 8 | (b & 0xFF));

        }

        uint32_t getNumberOfLEDs() { 
            return S;
        }
};
#endif
