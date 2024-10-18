
#include "swspi.h"

//void swspi_initial(swspi_t *d);
void swspi_hal_delay_us(uint32_t us) { (void)us; }
void swspi_hal_delay_ms(uint32_t ms) { (void)ms; }
void HAL_Delay(uint32_t mS) { swspi_hal_delay_ms(mS); }

void swspi_hal_gpio_out(swgpio_t *d, uint8_t val) {
    (void)d; (void)val;
}
uint8_t swspi_hal_gpio_in(swgpio_t *d) {
    (void)d; return 0;
}
void swspi_hal_gpio_mode(swgpio_t *d, uint8_t val) {
    (void)d; (void)val;
}

int swspi_hal_transmit(void *hWND, uint8_t *pWrite, uint16_t datasize) {
	//return HAL_SPI_Transmit(hWND, pWrite, datasize, HAL_MAX_DELAY);
    (void)hWND; (void)pWrite; (void)datasize; return 0;
}

int swspi_hal_receive(void *hWND, uint8_t *pRead, uint16_t datasize) {
	//return HAL_SPI_Receive(hWND, pRead, datasize, HAL_MAX_DELAY);
    (void)hWND; (void)pRead; (void)datasize; return 0;
}

int swspi_hal_transmit_receive(void *hWND, uint8_t *pRead, uint8_t *pWrite, uint16_t datasize) {
	//return HAL_SPI_TransmitReceive(hWND, pWrite, pRead, datasize, HAL_MAX_DELAY);
    (void)hWND; (void)pRead; (void)pWrite; (void)datasize; return 0;
}

int swspi_hal_init(swspi_t *d, swgpio_t *clk, swgpio_t *mosi, swgpio_t *miso) {
    //d->hal_io_ctl = swspi_port_io_ctl;
    if(d->Delay_Time == 0) { // h/w spi
    	d->CLK.port = (void*)clk; d->CLK.pin = 65535;
    } else { // s/w spi
    	if(clk) {
    		d->CLK.port = clk->port; d->CLK.pin = clk->pin;
    		swspi_hal_gpio_mode(&d->CLK, 1);
    	}
    	if(mosi) {
    		d->MOSI.port = mosi->port; d->MOSI.pin = mosi->pin;
    		swspi_hal_gpio_mode(&d->MOSI, 1);
    	}
    	if(miso) {
    		d->MISO.port = miso->port; d->MISO.pin = miso->pin;
    		swspi_hal_gpio_mode(&d->MISO, 0);
    	}
    }
    return 0;
}

int swspi_hal_setmode(swspi_t *d, uint8_t val) { (void)d; (void)val; return 0; }
int swspi_hal_setspeed(swspi_t *d, uint32_t val) { (void)d; (void)val; return 0; }
void swspi_hal_spiclose(swspi_t *d) { (void)d; return; }
int swspi_hal_setbits(swspi_t *d, uint8_t val) { (void)d; (void)val; return 0; }
