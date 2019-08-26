#ifndef PICxelPlus_H
#define PICxelPlus_H

#include <Arduino.h>
#include <stdint.h>
#include <DSPI.h>

#define CODEMASK 0b100100100100100100100100

template <uint32_t S> class PICxelPlus {

    private:

#if defined(__HAS_DMA__)
        typedef struct {
            union {
                volatile __DCH0CONbits_t    dchCon;
                volatile p32_regset         sxCon;
            };
            union {
                volatile __DCH0ECONbits_t   dchEcon;
                volatile p32_regset         sxEcon;
            };
            union {
                volatile __DCH0INTbits_t    dchInt;
                volatile p32_regset         sxInt;
            };
            volatile p32_regset             sxSsa;
            volatile p32_regset             sxDsa;
            union {
                volatile __DCH0SSIZbits_t   dchSsiz;
                volatile p32_regset         sxSsiz;
            };
            union {
                volatile __DCH0DSIZbits_t   dchDsiz;
                volatile p32_regset         sxDsiz;
            };
            union {
                volatile __DCH0SPTRbits_t   dchSptr;
                volatile p32_regset         sxSptr;
            };
            union {
                volatile __DCH0DPTRbits_t   dchDptr;
                volatile p32_regset         sxDptr;
            };
            union {
                volatile __DCH0DSIZbits_t   dchCsiz;
                volatile p32_regset         sxCsiz;
            };
            union {
                volatile __DCH0CPTRbits_t   dchCptr;
                volatile p32_regset         sxCptr;
            };
            union {
                volatile __DCH0DATbits_t    dchDat;
                volatile p32_regset         sxDat;
            };
        } p32_dch;

        p32_dch     *_dma;
#endif // __HAS_DMA__

        // Casting a DSPI object to this type will give us access to any protected variables
        // that we need. In this case, the pspi registers address.
        class DSPIExtension : private DSPI {
            public:
                p32_spi *getSPIRegisters() { return pspi; }
        };

        uint8_t     _colorArray[S * 3];

        // The coherent attribute here seems to be being ignored. So a horrible fudge to make
        // a pointer to the data array and AND it with 0xA0000000 to push it into KSEG1.  It's
        // REALLY nasty - but it works.
        uint8_t     __attribute__((coherent)) _dataArray[S * 9 + 50];
        uint8_t     *_dataArrayNoCache;

        DSPI        *_spi;
        p32_spi     *_spiRegisters;
        int         _spiVector;

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
            _spiRegisters = ((DSPIExtension *)_spi)->getSPIRegisters();

            _dataArrayNoCache = (uint8_t *)(0xA0000000 | (uint32_t)_dataArray);

#if defined(__PIC32MZ__)
# if defined(_SPI1_TX_VECTOR)
#  define _SPI1_TX_IRQ _SPI1_TX_VECTOR
# endif
# if defined(_SPI2_TX_VECTOR)
#  define _SPI2_TX_IRQ _SPI2_TX_VECTOR
# endif
# if defined(_SPI3_TX_VECTOR)
#  define _SPI3_TX_IRQ _SPI3_TX_VECTOR
# endif
# if defined(_SPI4_TX_VECTOR)
#  define _SPI4_TX_IRQ _SPI4_TX_VECTOR
# endif
# if defined(_SPI5_TX_VECTOR)
#  define _SPI5_TX_IRQ _SPI5_TX_VECTOR
# endif
# if defined(_SPI6_TX_VECTOR)
#  define _SPI6_TX_IRQ _SPI6_TX_VECTOR
# endif
#endif

            _spiVector = -1;

#if defined(_SPI1_TX_IRQ) 
            if ((uint32_t)_spiRegisters == (uint32_t)&SPI1CON) {
                _spiVector = _SPI1_TX_IRQ;
            }
#endif

#if defined(_SPI1A_TX_IRQ) 
            if ((uint32_t)_spiRegisters == (uint32_t)&SPI1ACON) {
                _spiVector = _SPI1A_TX_IRQ;
            }
#endif

#if defined(_SPI2_TX_IRQ) 
            if ((uint32_t)_spiRegisters == (uint32_t)&SPI2CON) {
                _spiVector = _SPI2_TX_IRQ;
            }
#endif

#if defined(_SPI2A_TX_IRQ) 
            if ((uint32_t)_spiRegisters == (uint32_t)&SPI2ACON) {
                _spiVector = _SPI2A_TX_IRQ;
            }
#endif

#if defined(_SPI3_TX_IRQ) 
            if ((uint32_t)_spiRegisters == (uint32_t)&SPI3CON) {
                _spiVector = _SPI3_TX_IRQ;
            }
#endif

#if defined(_SPI3A_TX_IRQ) 
            if ((uint32_t)_spiRegisters == (uint32_t)&SPI3ACON) {
                _spiVector = _SPI3A_TX_IRQ;
            }
#endif

#if defined(_SPI4_TX_IRQ) 
            if ((uint32_t)_spiRegisters == (uint32_t)&SPI4CON) {
                _spiVector = _SPI4_TX_IRQ;
            }
#endif

#if defined(_SPI4A_TX_IRQ) 
            if ((uint32_t)_spiRegisters == (uint32_t)&SPI4ACON) {
                _spiVector = _SPI4A_TX_IRQ;
            }
#endif

#if defined(_SPI5_TX_IRQ) 
            if ((uint32_t)_spiRegisters == (uint32_t)&SPI5CON) {
                _spiVector = _SPI5_TX_IRQ;
            }
#endif

#if defined(_SPI5A_TX_IRQ) 
            if ((uint32_t)_spiRegisters == (uint32_t)&SPI5ACON) {
                _spiVector = _SPI5A_TX_IRQ;
            }
#endif

#if defined(_SPI6_TX_IRQ) 
            if ((uint32_t)_spiRegisters == (uint32_t)&SPI6CON) {
                _spiVector = _SPI6_TX_IRQ;
            }
#endif

#if defined(_SPI6A_TX_IRQ) 
            if ((uint32_t)_spiRegisters == (uint32_t)&SPI6ACON) {
                _spiVector = _SPI6A_TX_IRQ;
            }
#endif

#if defined(__HAS_DMA__)
            _dma = NULL;
#if defined(DCH7SSA)
            if ((DCH7CONbits.CHEN == 0) && (_dma == NULL)) {
                _dma = (p32_dch *)&DCH7CON;
            }
#endif
                
#if defined(DCH6SSA)
            if ((DCH6CONbits.CHEN == 0) && (_dma == NULL)) {
                _dma = (p32_dch *)&DCH6CON;
            }
#endif
                
#if defined(DCH5SSA)
            if ((DCH5CONbits.CHEN == 0) && (_dma == NULL)) {
                _dma = (p32_dch *)&DCH5CON;
            }
#endif
                
#if defined(DCH4SSA)
            if ((DCH4CONbits.CHEN == 0) && (_dma == NULL)) {
                _dma = (p32_dch *)&DCH4CON;
            }
#endif
                
#if defined(DCH3SSA)
            if ((DCH3CONbits.CHEN == 0) && (_dma == NULL)) {
                _dma = (p32_dch *)&DCH3CON;
            }
#endif
                
#if defined(DCH2SSA)
            if ((DCH2CONbits.CHEN == 0) && (_dma == NULL)) {
                _dma = (p32_dch *)&DCH2CON;
            }
#endif
                
#if defined(DCH1SSA)
            if ((DCH1CONbits.CHEN == 0) && (_dma == NULL)) {
                _dma = (p32_dch *)&DCH1CON;
            }
#endif
                
#if defined(DCH0SSA)
            if ((DCH0CONbits.CHEN == 0) && (_dma == NULL)) {
                _dma = (p32_dch *)&DCH0CON;
            }
#endif
                
            if (_dma != NULL) {
                _spiRegisters->spiCon.STXISEL = 0b11;
                _dma->sxSsa.reg = ((uint32_t)_dataArrayNoCache) & 0x1FFFFFFF; 
                _dma->sxDsa.reg = ((uint32_t)&(_spiRegisters->sxBuf)) & 0x1FFFFFFF;
                _dma->sxSsiz.reg = S * 9 + 50;
                _dma->sxDsiz.reg = 1;
                _dma->sxCsiz.reg = 1;
                _dma->dchEcon.SIRQEN = 1;
                _dma->dchEcon.CHSIRQ = _spiVector;
                _dma->dchCon.CHAEN = 0;
                _dma->dchCon.CHEN = 1;
                DMACONbits.ON = 1;
            }
#endif // __HAS_DMA__

        }

        void refreshLEDs() {
            memset(_dataArrayNoCache, 0, S * 9 + 50);
            uint32_t ptr = 0;
            for (int i = 0; i < S; i++) {
                uint8_t g = _colorArray[i * 3 + 0];
                uint8_t r = _colorArray[i * 3 + 1];
                uint8_t b = _colorArray[i * 3 + 2];
                uint32_t gblock = colourToCode(g);
                _dataArrayNoCache[i * 9 + 0 + 50] = gblock >> 16;
                _dataArrayNoCache[i * 9 + 1 + 50] = gblock >> 8;
                _dataArrayNoCache[i * 9 + 2 + 50] = gblock;
                uint32_t rblock = colourToCode(r);
                _dataArrayNoCache[i * 9 + 3 + 50] = rblock >> 16;
                _dataArrayNoCache[i * 9 + 4 + 50] = rblock >> 8;
                _dataArrayNoCache[i * 9 + 5 + 50] = rblock;
                uint32_t bblock = colourToCode(b);
                _dataArrayNoCache[i * 9 + 6 + 50] = bblock >> 16;
                _dataArrayNoCache[i * 9 + 7 + 50] = bblock >> 8;
                _dataArrayNoCache[i * 9 + 8 + 50] = bblock;
            }

#if defined(__HAS_DMA__)
            if (_dma != NULL) {
                // Wait for a previous update to finish
                while (_dma->dchCon.CHBUSY == 1) {
                    yield();
                }
                _dma->dchCon.CHEN = 1;
                _dma->dchEcon.CFORCE=1;
                return;
            } 
#endif // __HAS_DMA__
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
#endif // PICxelPlus_H
