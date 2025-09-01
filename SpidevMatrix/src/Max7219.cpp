#include "SpidevMatrix/Max7219.h"

struct SendReceiveContext
{
    uint8_t *tx;
    uint8_t *rx;
    int index;

    SendReceiveContext(uint8_t *tx, uint8_t *rx) : tx(tx), rx(rx), index(0)
    {}

    void write(uint8_t reg, uint8_t value)
    {
        tx[index++] = reg;
        tx[index++] = value;
    }

    void send(SPI& spi)
    {
        spi.xfer(tx, index, rx, index);
        index = 0; // Reset index after write
    }
};

#define CreateSendReceiveContext(name) \
    int p_size = numMatrices * 2;      \
    uint8_t p_tx[p_size];              \
    uint8_t p_rx[p_size];              \
    SendReceiveContext name(p_tx, p_rx);

void Max7219::init()
{
    if (initialized) return;

    auto sendAll = [&](uint8_t reg, uint8_t data) {
        CreateSendReceiveContext(ctx)
        for (int m = 0; m < numMatrices; ++m) {
            ctx.write(reg, data);
        }
        ctx.send(spi);
    };

    sendAll(0x09, 0x00); // Decode mode: none
    sendAll(0x0A, intensity); // Intensity (0x00–0x0F)
    sendAll(0x0B, 0x07); // Scan limit: all 8 digits
    sendAll(0x0C, 0x01); // Shutdown: normal operation
    sendAll(0x0F, 0x00); // Display test: off

    initialized = true;
}

void Max7219::updateMatrices()
{
    // stack allocate tx and rx to avoid heap fragmentation
    CreateSendReceiveContext(ctx)
    for (int row = 0; row < 8; ++row) {
        if(!dirtyRow[row]) continue;
        dirtyRow[row] = false;
        for (int m = numMatrices - 1; m >= 0; --m) {
            ctx.write(row + 1, matrixBuffer[m * 8 + row]);
        }
        ctx.send(spi);
    }
}

void Max7219::setIntensity(int level)
{
    assert(level >= 0 && level <= 15);
    CreateSendReceiveContext(ctx)
    for (int m = 0; m < numMatrices; ++m) {
        ctx.write(0x0A, level);
    }
    ctx.send(spi);
}

void Max7219::setPixel(int x, int y, bool on)
{
    assert(x >= 0 && x < numMatrices * 8);
    assert(y >= 0 && y < 8);

    int matrix = x / 8;
    int col = x % 8;
    if (on)
        matrixBuffer[matrix * 8 + y] |= (1 << col);
    else
        matrixBuffer[matrix * 8 + y] &= ~(1 << col);

    dirtyRow[y] = true;
}


void Max7219::setPixelIfDifferent(int x, int y, bool on)
{
    assert(x >= 0 && x < numMatrices * 8);
    assert(y >= 0 && y < 8);

    int matrix = x / 8;
    int col = x % 8;

    int index = matrix * 8 + y;

    bool currentState = (matrixBuffer[index] >> col) & 0x01;
    if (currentState == on) return; // No change needed

    if (on)
        matrixBuffer[index] |= (1 << col);
    else
        matrixBuffer[index] &= ~(1 << col);

    dirtyRow[y] = true;
}
