#ifndef _SENDERBLYNK_h
#define _SENDERBLYNK_h

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

#include "master_i2c.h"
#include "Logging.h"

bool send_blynk(const Settings &sett, const float &value0, const float &value1, const float &voltage)
{
    bool ret = false;
    
    LOG_NOTICE( "WIF", "Starting Wifi" );
    IPAddress ip(sett.ip);
    IPAddress gw(sett.gw);
    IPAddress subnet(sett.subnet);
    WiFi.mode(WIFI_STA);
    WiFi.config( ip, gw, subnet );
    WiFi.begin();   //WifiManager уже записал ssid & pass в Wifi

    uint32_t now = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - now < ESP_CONNECT_TIMEOUT)  {

        LOG_NOTICE("WIF", "Wifi status: " << WiFi.status());
        delay(200);
    }

    if (WiFi.status() == WL_CONNECTED) {

        LOG_NOTICE( "WIF", "connected");

        Blynk.config(sett.key, sett.hostname, BLYNK_DEFAULT_PORT);
        if (Blynk.connect(SERVER_TIMEOUT)) {
            
            LOG_NOTICE( "BLK", "run");

            Blynk.virtualWrite(V0, value0);
            Blynk.virtualWrite(V1, value1);
            Blynk.virtualWrite(V2, voltage);

            if (sett.email) {
                LOG_NOTICE( "BLK", "send email");
                String msg;
                String v0(value0, 2);
                String v1(value1, 2);
                String v2(voltage, 3);
                msg = "{DEVICE_NAME}:\nВход 1: ";
                msg += v0;
                msg += "\nВход 2: ";
                msg += v1;
                msg += "\nНапряжение: ";
                msg += v2;
                msg += " В\nСМС для отправки:\nвода добавить: ";
                msg += v0;
                msg += " ";
                msg += v1;

                Blynk.email(sett.email, 
                    "{DEVICE_NAME} : Новые показания", 
                    msg);
                
                LOG_NOTICE( "BLK", "email was send");
            }

            ret = true;
        } else {

            LOG_ERROR("BLK", "connect error");
        } 
    }

    Blynk.disconnect();
    LOG_NOTICE("BLK", "disconnected");
    return ret;

    /*
    IPAddress myip = WiFi.localIP();
    BLYNK_LOG_IP("IP: ", myip);
    */
}		

#endif