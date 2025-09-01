#pragma once

#include "spidev_lib++.h"

#include <cassert>
#include <vector>


class Max7219
{

public:

    Max7219(int numMatrices, SPI& spi) : numMatrices(numMatrices), spi(spi) {
        matrixBuffer.resize(numMatrices * 8, 0x00);
    }

    void init();

    void updateMatrices();

    /**
     * Sets the intensity level of the display, and marks it dirty to be updated on next updateMatrices() call.
     * @param level
     */
    void setIntensity(int level);

    void setPixel(int x, int y, bool on);
    void setPixelIfDifferent(int x, int y, bool on);


private:
    bool initialized{false};
    int numMatrices;
    int intensity{0x03}; // Default intensity

    bool dirtyRow[8] = {true, true, true, true, true, true, true, true}; // Track which rows need updating
    std::vector<uint8_t> matrixBuffer;

    SPI& spi;
};
