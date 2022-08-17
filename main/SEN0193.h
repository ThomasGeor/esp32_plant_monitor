#include <adc_driver.h>

#define SEN0193_V_AIR 520
#define SEN0193_V_WATER 260
#define SEN0193_V_RANGE (SEN0193_V_AIR - SEN0193_V_WATER)

void sen0193_init(void);
uint8_t get_soil_humidity(void);