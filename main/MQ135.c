#include <MQ135.h>

static adc_unit_t unit = ADC_UNIT_1;
static adc_atten_t atten = ADC_ATTEN_DB_11;
static adc_channel_t channel = ADC1_CHANNEL_6;
static esp_adc_cal_characteristics_t adc_chars;

void mq135_init(void)
{
  adc_init(unit, atten, channel, &adc_chars);
  xTaskCreate(timer_int_task, "timer_int_task", 2048, NULL, 10, NULL);
}

// Read R0 after burning in
static float getRZero(float Rs)
{
  return (Rs * pow((ATMOCO2 / PARA), (1. / PARB)));
}

/*
@brief  Get the correction factor to correct for temperature and humidity
@param[in] t  The ambient air temperature
@param[in] h  The relative humidity
@return The calculated correction factor
*/

static float getCorrectionFactor(float t, float h)
{
  if (t < 20)
  {
    return CORA * t * t - CORB * t + CORC - (h - 33.) * CORD;
  }
  else
  {
    return CORE * t + CORF * h + CORG;
  }
}

/*
@brief  Get the resistance of the sensor, ie. the measurement value corrected
        for temp/hum
@param[in] t  The ambient air temperature
@param[in] h  The relative humidity
@return The corrected sensor resistance kOhm
*/
static float getCorrectedResistance(float t, float h, float Rs)
{
  return Rs / getCorrectionFactor(t, h);
}

/*
@brief  Get the ppm of CO2 sensed (assuming only CO2 in the air), corrected
        for temp/hum
@param[in] t  The ambient air temperature
@param[in] h  The relative humidity
@return The ppm of CO2 in the air
*/
static float getCorrectedPPM(float t, float h, float Rs)
{
  return PARA * pow((getCorrectedResistance(t, h, Rs) / RZERO), -PARB);
}

float get_co2_ppm_value(void)
{

  if (is_mq135_ready() != 0)
  {
    // also convert Vout from mV to V
    float air_temperature = (float)temperature_reading();
    float humidity = 50.0; // fow now
    float Vout = ((float)get_adc_reading(channel, &adc_chars)) / 1000;
    float Rs = ((5.0 * MQ135_RLOAD) - (MQ135_RLOAD * Vout)) / Vout;
    float co2_ppm;
#ifdef CALIBRATION_MODE
    co2_ppm = getRZero(Rs);
#else
    co2_ppm = getCorrectedPPM(air_temperature, humidity, Rs);
#endif
    return co2_ppm;
  }
  return 0;
}
