#include <adc_driver.h>

#define R_S_LDR 10000
#define LDR_VIN 3300 // mV
#define LDR_R_POW_FACTOR -1.4059
#define LDR_CONST 12500000

#define MAX_V_VAL 3150
#define MIN_V_VAL 342
#define LI_RANGE (MAX_V_VAL - MIN_V_VAL)

void ldr_init(void);
int get_light_intensity(void);
