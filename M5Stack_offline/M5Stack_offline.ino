/**
   For example sps30 & esp32
   Mod by Sonthaya Boonchan @HONEYLab
   05/02/2019

   Hardware test
   -> esp32 lite (mcu)
   -> sps30 (sensor)

   Communication uart
   rx -> 16
   tx -> 17

   Please edit file sensirion_uart.cpp for change pin or port communication
*/

#include "sensirion_uart.h"
#include "sps30.h"

#include <M5Stack.h>

// Stock font and GFXFF reference handle
#define GFXFF 1
#define FF18 &FreeSans12pt7b

#define CF_OL24 &Orbitron_Light_24
#define CF_OL32 &Orbitron_Light_32
#define CF_RT24 &Roboto_Thin_24
#define CF_S24  &Satisfy_24
#define CF_Y32  &Yellowtail_32


void setup() {
  Serial.begin(115200);
  M5.begin();
  sensirion_uart_open();

  while (sps30_probe() != 0) {
    Serial.println("probe failed");
    delay(1000);
  }
  // sps30_set_fan_auto_cleaning_interval(60*60);

  /* start measurement and wait for 10s to ensure the sensor has a
     stable flow and possible remaining particles are cleaned out */
  if (sps30_start_measurement() != 0) {
    Serial.println("error starting measurement");
  }
}

void loop() {
  struct sps30_measurement measurement;

  s16 ret;
  M5.Lcd.setFreeFont(FF18);                 // Select the font

  while (true) {
    delay(1000);
    ret = sps30_read_measurement(&measurement);

    if (ret < 0) {
      Serial.println("read measurement failed");
    } else {
      if (SPS_IS_ERR_STATE(ret)) {
        Serial.println("Measurements may not be accurate");
      } else {
        // Serial.print("pm2.5 :"); Serial.println(measurement.mc_2p5);

        /*  Serial.print("pm1.0 :");Serial.println(measurement.mc_1p0);
          Serial.print("pm2.5 :");Serial.println(measurement.mc_2p5);
          Serial.print("pm4.0 :");Serial.println(measurement.mc_4p0);
          Serial.print("pm10.0 :");Serial.println(measurement.mc_10p0);
          Serial.print("nc0.5 :");Serial.println(measurement.nc_0p5);
          Serial.print("nc1.0 :");Serial.println(measurement.nc_1p0);
          Serial.print("nc2.5 :");Serial.println(measurement.nc_2p5);
          Serial.print("nc4.0 :");Serial.println(measurement.nc_4p0);
          Serial.print("nc10.0 :");Serial.println(measurement.nc_10p0);
          Serial.print("typical particle size :");Serial.println(measurement.typical_particle_size);
        */
        
          if(measurement.mc_2p5 < 26) {
          M5.Lcd.fillScreen(TFT_BLUE);
          M5.Lcd.setTextColor(TFT_WHITE, TFT_BLUE);
          }
          else if(measurement.mc_2p5 < 38) {
          M5.Lcd.fillScreen(TFT_GREEN);
          M5.Lcd.setTextColor(TFT_WHITE, TFT_GREEN);
          }
          else if(measurement.mc_2p5 < 51) {
          M5.Lcd.fillScreen(TFT_YELLOW);
          M5.Lcd.setTextColor(TFT_WHITE, TFT_YELLOW);
          }
          else if(measurement.mc_2p5 < 91) {
          M5.Lcd.fillScreen(TFT_ORANGE);
          M5.Lcd.setTextColor(TFT_WHITE, TFT_ORANGE);
          }
          else {
          M5.Lcd.fillScreen(TFT_RED);
          M5.Lcd.setTextColor(TFT_WHITE, TFT_RED);
          }
        
        /*
          if(measurement.mc_2p5 > 40){
          M5.Lcd.fillScreen(TFT_RED);
          M5.Lcd.setTextColor(TFT_WHITE, TFT_RED);
          } else {
          M5.Lcd.fillScreen(TFT_GREEN);
          M5.Lcd.setTextColor(TFT_WHITE, TFT_GREEN);
          }
        */
        M5.Lcd.setTextSize(5);
        M5.Lcd.drawNumber(measurement.mc_2p5, 98, 65);
        M5.Lcd.setTextSize(0.5);
        M5.Lcd.drawString("------- PM 2.5 ------", 67, 180, GFXFF);
        M5.Lcd.setTextPadding(0);
        M5.update();

      }
    }
  }

  sps30_stop_measurement();
  sensirion_uart_close();
}
/*
  int AQI(float pm) {
  // I = (((Ij - Ii)/(Xj -Xi))*(X - Xi)) + Ii
  int I;
  if(){
   int I = (((25 - 0)/(25 - 0))*(pm - 0)) + 0;
  }else if(){
   int I = (((50 - 26)/(37 - 26))*(pm - 26)) + 26;
  }else if(){
   int I = (((100 - 51)/(50 - 38))*(pm - 38)) + 51;
  }else if(){
   int I = (((200 - 101)/(90 - 51))*(pm - 51)) + 101;
  }
  //  else{
  //   int I = (((? - 201)/(? - 91))*(pm - 91)) + 201;
  //  }
  }
*/
