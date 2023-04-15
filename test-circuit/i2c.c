#include "i2c.h"

#include <string.h>

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/twi.h>

#include "fifo.h"

#define I2C_SLAVE_ADDRESS 0x3b

typedef enum { CS_SIZE, CS_CONTENT } CurrentlySending;
static CurrentlySending currently_sending = CS_SIZE;  // will invert in first invocation

static uint8_t size_idx = 0;

static int write_cb(const char* buf, size_t bufsz, void* data)
{
    for (size_t i = 0; i < bufsz; ++i)
        fifo_push(&out_fifo, buf[i]);
    return 0;
}

static int read_cb(char* buf, size_t bufsz, void* data)
{
    size_t i = 0;
    for (i = 0; i < bufsz; ++i) {
        uint8_t c;
        bool has = fifo_pop(&in_fifo, &c);
        if (has)
            buf[i] = c;
        else
            break;
    }
    return i;
}

void i2c_init(FTClient* ft)
{
    TWAR = (I2C_SLAVE_ADDRESS << 1);           // set address
    TWSR &= ~(_BV(TWPS0) | ~_BV(TWPS1));       // set prescaler 1
    TWCR &= ~(_BV(TWSTA) | _BV(TWSTO));
    TWCR = _BV(TWEN) | _BV(TWIE) | _BV(TWEA);  // setup I²C

	// initialize fifo queues
    fifo_init(&in_fifo);
    fifo_init(&out_fifo);

    // initialize ftclient
    ftclient_init(ft, write_cb, read_cb, NULL, NULL, 512);
}

#define twcr_ack() \
    do { \
        TWCR = (1<<TWEN)| \
               (1<<TWIE)|(1<<TWINT)| \
               (1<<TWEA)|(0<<TWSTA)|(0<<TWSTO)| \
               (0<<TWWC); \
    } while (0)

#define twcr_reset() \
	do { \
		TWCR = (1<<TWEN)| \
		       (1<<TWIE)|(1<<TWINT)| \
		       (1<<TWEA)|(0<<TWSTA)|(1<<TWSTO)| \
		       (0<<TWWC); \
	} while (0)


ISR(TWI_vect)
{
    switch (TWSR & 0xF8) {

        // slave receiver
        case TW_SR_SLA_ACK:   // 0x60: SLA+W received, ACK returned
            twcr_ack();
            break;

        case TW_SR_DATA_ACK:  // 0x80: data received, ACK returned
            fifo_push(&in_fifo, TWDR);
            twcr_ack();
            break;

        case TW_SR_DATA_NACK: // 0x88: data received, NACK returned
            twcr_reset();
            break;

        case TW_SR_STOP:      // 0xA0: stop or repeated start condition received while selected
            twcr_reset();
            break;

        // slave transmitter
        case TW_ST_SLA_ACK:   // 0xA8: SLA+R received, ACK returned
            // fallthrough!
        case TW_ST_DATA_ACK:  // 0xB8: data transmitted, ACK received
            if (currently_sending == CS_SIZE) {
                size_t out_fifo_sz = fifo_size(&out_fifo);
                if (size_idx == 0) {
                    TWDR = out_fifo_sz & 0xff;
                    ++size_idx;
                } else if (size_idx == 1) {
                    TWDR = (out_fifo_sz >> 8) & 0xff;  // end of size transmission
                    size_idx = 0;
                    if (out_fifo_sz != 0)
                        currently_sending = CS_CONTENT;
                }
            } else {
                uint8_t data;
                if (fifo_pop(&out_fifo, &data)) {
                    TWDR = data;
                } else {
                    currently_sending = CS_SIZE;
                    size_idx = 0;
                }
            }
            twcr_ack();
            break;

        case TW_ST_DATA_NACK: // 0xC0: data transmitted, NACK received - no more data is requested
            twcr_reset();
            break;

        case TW_ST_LAST_DATA: // 0xC8: last data byte in TWDR transmitted (TWEA = “0”), ACK received
            twcr_reset();
            break;

        default:
            twcr_reset();
            break;
    }
}

// https://github.com/keyboardio/avr_keyscanner/blob/master/firmware/twi-slave.c
// https://gitlab.com/marsin/avr-libc-mars-twi/-/blob/master/src/slave.c
