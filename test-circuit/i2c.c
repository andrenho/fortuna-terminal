#include "i2c.h"

#include <string.h>

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/twi.h>

#include "buffer.h"

#define I2C_SLAVE_ADDRESS 0x3b

typedef enum { CS_SIZE, CS_CONTENT } CurrentlySending;
static CurrentlySending currently_sending = CS_SIZE;  // will invert in first invocation

static uint8_t size_idx = 0;

void i2c_init(void)
{
    TWAR = (I2C_SLAVE_ADDRESS << 1);           // set address
    TWSR &= ~(_BV(TWPS0) | ~_BV(TWPS1));       // set prescaler 1
    TWCR &= ~(_BV(TWSTA) | _BV(TWSTO));
    TWCR = _BV(TWEN) | _BV(TWIE) | _BV(TWEA);  // setup I²C
}

void i2c_printchar(uint8_t c)
{
    if (out_buffer_sz >= BUFFER_SZ - 1)
        return;
    if (out_buffer_sz > 0)
        memmove(&out_buffer[1], out_buffer, out_buffer_sz);
    out_buffer[0] = c;
    ++out_buffer_sz;
}

uint8_t i2c_getchar_nonblocking(void)
{
    if (in_buffer_sz == 0) {
        return 0;
    } else {
        uint8_t data = in_buffer[0];
        memmove(&in_buffer[1], in_buffer, --in_buffer_sz);
        return data;
    }
}

uint8_t i2c_getchar_blocking(void)
{
    while (1) {
        uint8_t data = i2c_getchar_nonblocking();
        if (data != 0)
            return data;
    }
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


#include "uart.h"
ISR(TWI_vect)
{
    switch (TWSR & 0xF8) {

        // slave receiver
        case TW_SR_SLA_ACK:   // 0x60: SLA+W received, ACK returned
            twcr_ack();
            break;

        case TW_SR_DATA_ACK:  // 0x80: data received, ACK returned
            in_buffer[in_buffer_sz++] = TWDR;
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
                if (size_idx == 0) {
                    TWDR = out_buffer_sz & 0xff;
                    ++size_idx;
                } else if (size_idx == 1) {
                    TWDR = (out_buffer_sz >> 8) & 0xff;  // end of size transmition
                    size_idx = 0;
                    if (out_buffer_sz)
                        currently_sending = CS_CONTENT;
                }
            } else {
                TWDR = out_buffer[out_buffer_sz-1];
                --out_buffer_sz;
                if (out_buffer_sz == 0) {
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
