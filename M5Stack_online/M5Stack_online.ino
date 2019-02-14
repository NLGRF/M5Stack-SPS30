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
#include <WiFiClientSecure.h>

#define SSID                  "max"
#define PASSWORD              "max12345"

#define FIREBASE_HOST         "https://test-27303.firebaseio.com/"
#define FIREBASE_AUTH         "COdWyoTiScJyKkiRvyz3ITdiHOOTW90Xjk22DCPs"

String  ESP32_Get_Firebase(String path );
int     ESP32_Set_Firebase(String path, String value, bool push = false );
int     ESP32_Push_Firebase(String path, String value);


void setup() {
  Serial.begin(115200);
  Serial.println();
  WiFi.begin(SSID, PASSWORD);
  while (WiFi.status() != WL_CONNECTED ) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.println(WiFi.localIP());

  ESP32_Set_Firebase("Name", "\"SPS30_ESP32\"");
  Serial.println( ESP32_Get_Firebase("Name"));
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

  while (true) {
    delay(1000);
    ret = sps30_read_measurement(&measurement);

    if (ret < 0) {
      Serial.println("read measurement failed");
    } else {
      if (SPS_IS_ERR_STATE(ret)) {
        Serial.println("Measurements may not be accurate");
      } else {
        /*
        Serial.print("pm1.0 :"); Serial.println(measurement.mc_1p0);
        Serial.print("pm2.5 :"); Serial.println(measurement.mc_2p5);
        Serial.print("pm4.0 :"); Serial.println(measurement.mc_4p0);
        Serial.print("pm10.0 :"); Serial.println(measurement.mc_10p0);
        Serial.print("nc0.5 :"); Serial.println(measurement.nc_0p5);
        Serial.print("nc1.0 :"); Serial.println(measurement.nc_1p0);
        Serial.print("nc2.5 :"); Serial.println(measurement.nc_2p5);
        Serial.print("nc4.0 :"); Serial.println(measurement.nc_4p0);
        Serial.print("nc10.0 :"); Serial.println(measurement.nc_10p0);
        Serial.print("typical particle size :"); Serial.println(measurement.typical_particle_size);
*/
        Serial.println("Start send data => Firebase!");

        // ESP32_Set_Firebase("Sensor/MC1p0", String(measurement.mc_1p0));
        ESP32_Set_Firebase("Sensor/MC2p5", String(measurement.mc_2p5));
        // ESP32_Set_Firebase("Sensor/MC4p0", String(measurement.mc_4p0));
        ESP32_Set_Firebase("Sensor/MC10p0", String(measurement.mc_10p0));
        // ESP32_Set_Firebase("Sensor/NC1p0", String(measurement.nc_1p0));
        // ESP32_Set_Firebase("Sensor/NC2p5", String(measurement.nc_2p5));
        // ESP32_Set_Firebase("Sensor/NC4p0", String(measurement.nc_4p0));
        // ESP32_Set_Firebase("Sensor/NC10p0", String(measurement.nc_10p0));
        ESP32_Set_Firebase("Sensor/TPS", String(measurement.typical_particle_size));

        ESP32_Push_Firebase("Sensor_av/MC2p5", String(measurement.mc_2p5));
        ESP32_Push_Firebase("Sensor_av/MC10p0", String(measurement.mc_10p0));
        ESP32_Push_Firebase("Sensor_av/TPS", String(measurement.typical_particle_size));

        

        Serial.println("Send Successful!");
      }
    }
  }

  sps30_stop_measurement();
  sensirion_uart_close();
}


// Root CA : https://www.firebaseio.com
// 01/08/2018 17:21:49 GMT to 27/03/2019 00:00:00 GMT
const char* FIREBASE_ROOT_CA = \
                               "-----BEGIN CERTIFICATE-----\n" \
                               "MIIEXDCCA0SgAwIBAgINAeOpMBz8cgY4P5pTHTANBgkqhkiG9w0BAQsFADBMMSAw\n" \
                               "HgYDVQQLExdHbG9iYWxTaWduIFJvb3QgQ0EgLSBSMjETMBEGA1UEChMKR2xvYmFs\n" \
                               "U2lnbjETMBEGA1UEAxMKR2xvYmFsU2lnbjAeFw0xNzA2MTUwMDAwNDJaFw0yMTEy\n" \
                               "MTUwMDAwNDJaMFQxCzAJBgNVBAYTAlVTMR4wHAYDVQQKExVHb29nbGUgVHJ1c3Qg\n" \
                               "U2VydmljZXMxJTAjBgNVBAMTHEdvb2dsZSBJbnRlcm5ldCBBdXRob3JpdHkgRzMw\n" \
                               "ggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQDKUkvqHv/OJGuo2nIYaNVW\n" \
                               "XQ5IWi01CXZaz6TIHLGp/lOJ+600/4hbn7vn6AAB3DVzdQOts7G5pH0rJnnOFUAK\n" \
                               "71G4nzKMfHCGUksW/mona+Y2emJQ2N+aicwJKetPKRSIgAuPOB6Aahh8Hb2XO3h9\n" \
                               "RUk2T0HNouB2VzxoMXlkyW7XUR5mw6JkLHnA52XDVoRTWkNty5oCINLvGmnRsJ1z\n" \
                               "ouAqYGVQMc/7sy+/EYhALrVJEA8KbtyX+r8snwU5C1hUrwaW6MWOARa8qBpNQcWT\n" \
                               "kaIeoYvy/sGIJEmjR0vFEwHdp1cSaWIr6/4g72n7OqXwfinu7ZYW97EfoOSQJeAz\n" \
                               "AgMBAAGjggEzMIIBLzAOBgNVHQ8BAf8EBAMCAYYwHQYDVR0lBBYwFAYIKwYBBQUH\n" \
                               "AwEGCCsGAQUFBwMCMBIGA1UdEwEB/wQIMAYBAf8CAQAwHQYDVR0OBBYEFHfCuFCa\n" \
                               "Z3Z2sS3ChtCDoH6mfrpLMB8GA1UdIwQYMBaAFJviB1dnHB7AagbeWbSaLd/cGYYu\n" \
                               "MDUGCCsGAQUFBwEBBCkwJzAlBggrBgEFBQcwAYYZaHR0cDovL29jc3AucGtpLmdv\n" \
                               "b2cvZ3NyMjAyBgNVHR8EKzApMCegJaAjhiFodHRwOi8vY3JsLnBraS5nb29nL2dz\n" \
                               "cjIvZ3NyMi5jcmwwPwYDVR0gBDgwNjA0BgZngQwBAgIwKjAoBggrBgEFBQcCARYc\n" \
                               "aHR0cHM6Ly9wa2kuZ29vZy9yZXBvc2l0b3J5LzANBgkqhkiG9w0BAQsFAAOCAQEA\n" \
                               "HLeJluRT7bvs26gyAZ8so81trUISd7O45skDUmAge1cnxhG1P2cNmSxbWsoiCt2e\n" \
                               "ux9LSD+PAj2LIYRFHW31/6xoic1k4tbWXkDCjir37xTTNqRAMPUyFRWSdvt+nlPq\n" \
                               "wnb8Oa2I/maSJukcxDjNSfpDh/Bd1lZNgdd/8cLdsE3+wypufJ9uXO1iQpnh9zbu\n" \
                               "FIwsIONGl1p3A8CgxkqI/UAih3JaGOqcpcdaCIzkBaR9uYQ1X4k2Vg5APRLouzVy\n" \
                               "7a8IVk6wuy6pm+T7HT4LY8ibS5FEZlfAFLSW8NwsVz9SBK2Vqn1N0PIMn5xA6NZV\n" \
                               "c7o835DLAFshEWfC7TIe3g==\n" \
                               "-----END CERTIFICATE-----\n";

int ESP32_Set_Firebase(String path, String value, bool push ) {
  WiFiClientSecure ssl_client;
  String host = String(FIREBASE_HOST); host.replace("https://", "");
  if (host[host.length() - 1] == '/' ) host = host.substring(0, host.length() - 1);
  String resp = "";
  int httpCode = 404; // Not Found

  String firebase_method = (push) ? "POST " : "PUT ";
  ssl_client.setCACert(FIREBASE_ROOT_CA);
  if ( ssl_client.connect( host.c_str(), 443)) {
    String uri = ((path[0] != '/') ? String("/") : String("")) + path + String(".json?auth=") + String(FIREBASE_AUTH);
    String request = "";
    request +=  firebase_method + uri + " HTTP/1.1\r\n";
    request += "Host: " + host + "\r\n";
    request += "User-Agent: TD_ESP32\r\n";
    request += "Connection: close\r\n";
    request += "Accept-Encoding: identity;q=1,chunked;q=0.1,*;q=0\r\n";
    request += "Content-Length: " + String( value.length()) + "\r\n\r\n";
    request += value;

    ssl_client.print(request);
    while ( ssl_client.connected() && !ssl_client.available()) delay(10);
    if ( ssl_client.connected() && ssl_client.available() ) {
      resp      = ssl_client.readStringUntil('\n');
      httpCode  = resp.substring(resp.indexOf(" ") + 1, resp.indexOf(" ", resp.indexOf(" ") + 1)).toInt();
    }
    ssl_client.stop();
  }
  else {
    Serial.println("[Firebase] can't connect to Firebase Host");
  }
  return httpCode;
}

int ESP32_Push_Firebase(String path, String value) {
  return ESP32_Set_Firebase(path, value, true);
}

String ESP32_Get_Firebase(String path ) {
  WiFiClientSecure ssl_client;
  String host = String(FIREBASE_HOST); host.replace("https://", "");
  if (host[host.length() - 1] == '/' ) host = host.substring(0, host.length() - 1);
  String resp = "";
  String value = "";
  ssl_client.setCACert(FIREBASE_ROOT_CA);
  if ( ssl_client.connect( host.c_str(), 443)) {
    String uri = ((path[0] != '/') ? String("/") : String("")) + path + String(".json?auth=") + String(FIREBASE_AUTH);
    String request = "";
    request += "GET " + uri + " HTTP/1.1\r\n";
    request += "Host: " + host + "\r\n";
    request += "User-Agent: TD_ESP32\r\n";
    request += "Connection: close\r\n";
    request += "Accept-Encoding: identity;q=1,chunked;q=0.1,*;q=0\r\n\r\n";

    ssl_client.print( request);
    while ( ssl_client.connected() && !ssl_client.available()) delay(10);
    if ( ssl_client.connected() && ssl_client.available() ) {
      while ( ssl_client.available()) resp += (char)ssl_client.read();
      value = resp.substring( resp.lastIndexOf('\n') + 1, resp.length() - 1);
    }
    ssl_client.stop();
  } else {
    Serial.println("[Firebase] can't connect to Firebase Host");
  }
  return value;
}
