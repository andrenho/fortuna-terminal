#include "i2c.h"

#include <util/twi.h>

#define SLAVE 0xE5

void i2c_init(void)
{
    // set SCL frequency
    TWSR = 0x00;
    TWBR = 0x0c;

    // enable TWI module
    TWCR = (1 << TWEN);
}

int i2c_start(void)
{
    // send start condition
    TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
    // wait for end of transmission
    while (!(TWCR & (1 << TWINT)));

    // check if the start condition was successfully transmitted
    if ((TWSR & 0xF8) != TW_START)
        return 1;

    // load slave address into data register
    TWDR = SLAVE;
    // start transmission of address
    TWCR = (1 << TWINT) | (1 << TWEN);
    // wait for end of transmission
    while (!(TWCR & (1 << TWINT)));

    // check if the device has acknowledged the READ / WRITE mode
    uint8_t twst = TW_STATUS & 0xF8;
    if ( (twst != TW_MT_SLA_ACK) && (twst != TW_MR_SLA_ACK) )
        return 1;

    return 0;
}

int i2c_send_byte(uint8_t c)
{
	// load data into data register
    TWDR = c;
    // start transmission of data
    TWCR = (1 << TWINT) | (1 << TWEN);
    // wait for end of transmission
    while (!(TWCR & (1 << TWINT)));

    // check if the device has acknowledged the READ / WRITE mode
    if ( (TWSR & 0xF8) != TW_MT_DATA_ACK )
        return 1;

    return 0;
}

void i2c_stop(void)
{
    // transmit STOP condition
    TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
}
