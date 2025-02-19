#include <Arduino.h>
#include <NTPClient.h>
#include <WiFi.h>
#include <WiFiUdp.h>

const char *ssid = "your_SSID";
const char *password = "your_PASSWORD";

// Software clock variables
uint32_t lastSecondUpdate = 0;
uint8_t hours = 0, minutes = 0, seconds = 0;

// Timer for correction time
uint32_t lastNTPUpdate = 0;
const uint32_t ntpInterval = 3600000; // 1 hour correction interval

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "europe.pool.ntp.org", 10800, ntpInterval); // Time zone +3, NTP update 1 hour

void parseTimeFromNTP()
{
    timeClient.update();

    String formattedTime = timeClient.getFormattedTime();

    int firstColon = formattedTime.indexOf(':');
    int secondColon = formattedTime.lastIndexOf(':');

    if (firstColon != -1 && secondColon != -1)
    {
        hours = formattedTime.substring(0, firstColon).toInt();
        minutes = formattedTime.substring(firstColon + 1, secondColon).toInt();
        seconds = formattedTime.substring(secondColon + 1).toInt();

        Serial.print("NTP CORRECTION: ");
        Serial.printf("%02d:%02d:%02d\n", hours, minutes, seconds);
    }
}

void setup()
{
    Serial.begin(115200);

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }

    Serial.println("\nWiFi connected");

    timeClient.begin();
    timeClient.forceUpdate();

    parseTimeFromNTP(); // Get NTP correction
}

void loop()
{
    // Get NTP correction every 1 hour
    if (millis() - lastNTPUpdate >= ntpInterval)
    {
        parseTimeFromNTP();

        lastNTPUpdate = millis();
        lastSecondUpdate = millis() + 1000;
    }

    // Update software clock every second
    if (millis() - lastSecondUpdate >= 1000)
    {
        lastSecondUpdate = millis();

        Serial.printf("Internal clock: %02d:%02d:%02d\n", hours, minutes, seconds);

        if (++seconds >= 60)
        {
            seconds = 0;

            if (++minutes >= 60)
            {
                minutes = 0;

                if (++hours >= 24)
                    hours = 0;
            }
        }
    }
}

// parse data---------------------------------------------------------------------------

// uint8_t hour;
// uint8_t minute;
// uint8_t firstValue;
// uint8_t secondValue;

// String s = "12:15";
//  tme = tme.substring(0, tme.length() - 3);

// int separatorIndexNtp = tme.indexOf(':'); // ищем индекс разделителя ':'

// if (separatorIndexNtp != -1)
// {
//     hour = tme.substring(0, separatorIndexNtp).toInt(); // Преобразуем в целое число
//     minute = tme.substring(separatorIndexNtp + 1).toInt();

// Serial.print(firstValue);
// Serial.print(":");
// Serial.println(secondValue);
// }

// int separatorIndexIncoming = s.indexOf(':'); // ищем индекс разделителя ':'

// if (separatorIndexIncoming != -1)
// {
//     firstValue = s.substring(0, separatorIndexIncoming).toInt(); // Преобразуем в целое число
//     secondValue = s.substring(separatorIndexIncoming + 1).toInt();

//     Serial.print(firstValue);
//     Serial.print(":");
//     Serial.println(secondValue);
// }

// delay(1000);
//-------------------------------------------------------------------------------------