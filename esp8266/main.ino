#include "Arduino.h"
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include "certs.h"
#include "tokens.h"


#ifndef STASSID
#define STASSID ""
#define STAPSK  ""
#endif

enum Calls {
    Filter,
    PauseResume,
    Play,
    Previous,
    Next,
    VolumeUp,
    VolumeDown
};

String tracks[] = {"1T0ulbDZBakrbTUjImduXG", "0BnRoiQbiSYb4K4y3aQWbg", "4Ka43W7pxLv41B2c5R1UAy", "417MeJ40upxxf3aNlr5Xbi", "0jWgAnTrNZmOGmqgvHhZEm", "6b2oQwSGFkzsMtQruIWm2p", "0dkV8DuMfWAVhfi3iMnIX3", "2Bz6TombrRCYCka9skP8fP", "2Bz6TombrRCYCka9skP8fP", "2Bz6TombrRCYCka9skP8fP"};

const char* ssid = STASSID;
const char* password = STAPSK;

X509List cert(cert_DigiCert_Global_Root_CA);
WiFiClientSecure client;


void setup() {
  Serial.begin(115200);

  // connect to wifi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

  // Set time via NTP, as required for x.509 validation
  configTime(3 * 3600, 0, "pool.ntp.org", "time.nist.gov");

  // wait for ntp sync
  time_t now = time(nullptr);
  while (now < 8 * 3600 * 2) {
    delay(500);
    now = time(nullptr);
  }
  struct tm timeinfo;
  gmtime_r(&now, &timeinfo);

  // Use WiFiClientSecure class to create TLS connection with generated cert
  client.setTrustAnchors(&cert);

}

void callSpotifyApi(String method, String url, String body = String("")) {
    if (!client.connect(spotify_host, spotify_port)) {
      Serial.println("Connection failed\n");
      return;
    }


    client.print(
            String(method) + " /v1" + url + " HTTP/1.1\r\n"
            + "Host: " + spotify_host + "\r\n"
            + "User-Agent: BuildFailureDetectorESP8266\r\n"
            + "Authorization: Bearer " + access_token  +"\r\n"
            + "Content-Type: application/json\r\n"
            + "Content-Length: " + String(body.length())+ "\r\n"
            + "Connection: close\r\n\r\n"
            + body
    );

//    Serial.println("Request sent");
//    while (client.connected()) {
//      String line = client.readStringUntil('\n');
//      if (line == "\r") {
//        Serial.println("Headers received");
//        break;
//      }
//    }
//    String line = client.readString();
//    Serial.println(line);
}

String url;
String method;
int volume = 50;
bool waitForId = false;
bool paused = false;
String body;
void loop() {
    if (Serial.available()) {

        body = "";
        auto called = Serial.read();
        if(waitForId) {
            body = String("{\"context_uri\": \"spotify:playlist:4EzRmRgHwrLzfyZ0G8IrGB\", \"offset\": {\"position\": ") + String(called) +"}, \"position_ms\": 0}\r\n\r\n";
            Serial.println(body);
            waitForId = false;
        } else if(called == (char) Previous){
            Serial.println("ESP::Previous");
            method = "POST";
            url = "/me/player/previous";
        } else if(called == (char) Next) {
            Serial.println("ESP::Next");
            method = "POST";
            url = "/me/player/next";
        } else if(called == (char) VolumeDown) {
            Serial.println("ESP::VolumeDown");
            if(volume >= 10) {
                volume -= 10;
            }
            method = "PUT";
            url = "/me/player/volume?volume_percent=" + String(volume);
        } else if(called == (char) VolumeUp) {
            Serial.println("ESP::VolumeUp");
            if(volume <= 90) {
                volume += 10;
            }
            method = "PUT";
            url = "/me/player/volume?volume_percent=" + String(volume);
        } else if(called == (char) PauseResume) {
            Serial.println("ESP::PauseResume");
            method = "PUT";
            if(paused) {
                url = "/me/player/play";
            } else {
                url = "/me/player/pause";
            }
            paused = !paused;
        } else if(called == (char) Play) {
            Serial.println("ESP::Play");
            method = "PUT";
            url = "/me/player/play";
            waitForId = true;
        }
        callSpotifyApi(method, url, body);
    }
}

