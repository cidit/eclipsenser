#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <hp_BH1750.h>
#include <TaskScheduler.h>

hp_BH1750 PHOTO_SENSOR;
const int ONEWIRE_BUS = 36;
OneWire ONEWIRE(ONEWIRE_BUS);
DallasTemperature TEMPERATURE_SENSOR(&ONEWIRE);

Scheduler runner;

float lux = NULL;
Task measure_light(
    10 * TASK_MILLISECOND,
    TASK_FOREVER,
    []()
    {
      if (PHOTO_SENSOR.hasValue())
      {
        lux = PHOTO_SENSOR.getLux();
        PHOTO_SENSOR.start();
      }
      else if (lux == NULL)
      {
        PHOTO_SENSOR.start();
      }
    });

float temps = NULL;
Task measure_temperature(
    10 * TASK_MILLISECOND,
    TASK_FOREVER,
    []()
    {
      TEMPERATURE_SENSOR.requestTemperatures();
      float tempC = TEMPERATURE_SENSOR.getTempCByIndex(0);
      if (tempC != DEVICE_DISCONNECTED_C)
      {
        temps = tempC;
      }
      else
      {
        Serial.println("couldnt read temp data");
      }
    });

Task report_to_serial(
    0.5 * TASK_SECOND,
    TASK_FOREVER,
    []()
    {
      Serial.println("temps:" + String(temps) + " lux:" + String(lux));
    });

void setup()
{
  Serial.begin(115200);
  while (!Serial)
  {
  }

  TEMPERATURE_SENSOR.begin();
  auto photo_available = PHOTO_SENSOR.begin(BH1750_TO_GROUND);
  if (!photo_available)
  {
    Serial.println("No photo sensor found!");
  }
  {
    runner.init();
    runner.addTask(measure_light);
    measure_light.enable();
    runner.addTask(measure_temperature);
    measure_temperature.enable();
    runner.addTask(report_to_serial);
    report_to_serial.enable();
  }
}

void loop()
{
  // runner.execute();
  float lux;
  if (PHOTO_SENSOR.hasValue())
  {
    lux = PHOTO_SENSOR.getLux();
    PHOTO_SENSOR.start();
  }
  else if (lux == NULL)
  {
    PHOTO_SENSOR.start();
  }
  float temps;
  TEMPERATURE_SENSOR.requestTemperatures();
  float tempC = TEMPERATURE_SENSOR.getTempCByIndex(0);
  if (tempC != DEVICE_DISCONNECTED_C)
  {
    temps = tempC;
  }
  else
  {
    Serial.println("couldnt read temp data");
  }
  Serial.println("temps:" + String(temps) + " lux:" + String(lux));
}
