#include <adc_driver.h>
#include <timer_driver.h>
#include <math.h>
#include <tc74.h>

#define CALIBRATION_MODE

// Calibration resistance at atmospheric CO2 level
#define RZERO 76.63
// The load resistance on the board
#define RLOAD 20.0
// Parameters to model temperature and humidity dependence
#define CORA 0.00035
#define CORB 0.02718
#define CORC 1.39538
#define CORD 0.0018
#define CORE -.003333333
#define CORF -.001923077
#define CORG 1.130128205

// Parameters for calculating ppm of CO2 from sensor resistance
#define PARA 116.6020682
#define PARB 2.769034857
// Atmospheric CO2 level for calibration purposes
#define ATMOCO2 414.72

void mq135_init(void);
void set_mq135_ready(void);
float get_co2_ppm_value(void);