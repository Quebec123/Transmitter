//
// Created by wikto on 17/03/2026.
//

#include "ServerManager.h"
#include "webpage/MainPageAP.h"
#include <WiFi.h>
#include <HTTPClient.h>

void ServerManager::saveCredentials() {
    _preferences.begin("server", false);
    _preferences.putString("ssid", _sta_ssid);
    _preferences.putString("password", _sta_password);
    _preferences.putString("ceiling_height", _ceiling_height);
    _preferences.putString("fall_url", _fall_url);
    _preferences.putString("presence_url", _presence_url);

    _preferences.end();
}

void ServerManager::loadCredentials() {
    _preferences.begin("server", true);
    _sta_ssid = _preferences.getString("ssid", "");
    _sta_password = _preferences.getString("password", "");
    _ceiling_height = _preferences.getString("ceiling_height", "");
    _fall_url = _preferences.getString("fall_url", "");
    _presence_url = _preferences.getString("presence_url", "");`
    _preferences.end();
}

void ServerManager::connectToWiFi() {
    WiFi.begin(_sta_ssid.c_str(), _sta_password.c_str());
    Serial.println(_sta_password.c_str());
    Serial.println(_sta_ssid.c_str());
    delay(1000);
    Serial.print("Connecting to WiFi");
    int attempts = 0;

    while (WiFi.status() != WL_CONNECTED && attempts < 20) { // Retry for 20 attempts
        delay(500);
        Serial.print(".");
        attempts++;
    }

    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\nConnected to WiFi!");
        Serial.print("IP Address: ");
        Serial.println(WiFi.localIP());
        delay(2000); // Short delay before sending data
    } else {
        Serial.println("\nFailed to connect to WiFi.");
        delay(5000);
    }
}

void ServerManager::startAP() {
    loadCredentials(); // Load saved credentials
    if (!_sta_ssid.isEmpty() && !_sta_password.isEmpty()) {
        Serial.println("Credentials found. Attempting to connect to WiFi...");
        connectToWiFi(); // Attempt to connect to WiFi
        if (WiFi.status() == WL_CONNECTED) {
            Serial.println("WiFi connected. Skipping AP mode.");
            return; // Skip starting the AP if WiFi is connected
        }
    }
    WiFi.softAP(_ap_ssid, _ap_password);

    Serial.println("Access Point started");
    Serial.print("IP Address: ");
    Serial.println(getIP());

    _server.on("/", [this]() {
        _server.send(200, "text/html", CONFIG_PAGE);
    });
    _server.on("/save", HTTP_POST, [this]() {
        if (_server.hasArg("ssid") && _server.hasArg("password") && _server.hasArg("ceiling_height")
            && _server.hasArg("fall_url") && _server.hasArg("presence_url")) {
            _sta_ssid = _server.arg("ssid").c_str();
            _sta_password = _server.arg("password").c_str();
            _ceiling_height = _server.arg("ceiling_height").c_str();
            _fall_url = _server.arg("fall_url").c_str();
            _presence_url = _server.arg("presence_url").c_str();

            saveCredentials(); // Save credentials persistently
            _server.send(200, "text/plain", "Credentials and ceiling height saved.");

            connectToWiFi(); // Attempt to connect to WiFi
        } else {
            _server.send(400, "text/plain", "Invalid data");
        }
    });
    _server.begin();
}

void ServerManager::stopAP() {
    _server.stop();
    WiFi.softAPdisconnect(true);
}

bool ServerManager::loopAP() {
    _server.handleClient();
    if(WiFi.status() == WL_CONNECTED) {
        stopAP(); // Stop AP mode if connected to WiFi
        return false; // Exit loopAP
    }
    return true;
}

IPAddress ServerManager::getIP() const {
    return WiFi.softAPIP();
}
void ServerManager::sendPresenceToHomeAssistant() {
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("WiFi disconnected. Reconnecting...");
        connectToWiFi();
        if (WiFi.status() != WL_CONNECTED) {
            Serial.println("Failed to reconnect to WiFi. Aborting data send.");
            return;
        }
    }

    HTTPClient http;
    const String homeAssistantUrl = "http://192.168.1.101:8123/api/webhook/" + _presence_url;
    http.begin(homeAssistantUrl);
    String payload = "{}";

    int httpResponseCode = http.POST(payload);

    if (httpResponseCode > 0) {
        Serial.printf("Presence data sent to Home Assistant. HTTP Response code: %d\n", httpResponseCode);
    } else {
        Serial.printf("Failed to send presence data. Error: %s\n", http.errorToString(httpResponseCode).c_str());
    }

    http.end();
}
void ServerManager::sendFallToHomeAssistant() {
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("WiFi disconnected. Reconnecting...");
        connectToWiFi();
        if (WiFi.status() != WL_CONNECTED) {
            Serial.println("Failed to reconnect to WiFi. Aborting data send.");
            return;
        }
    }

    HTTPClient http;
    const String homeAssistantUrl = "http://192.168.1.101:8123/api/webhook/" + _fall_url;
    http.begin(homeAssistantUrl);
    String payload = "{}";

    int httpResponseCode = http.POST(payload);

    if (httpResponseCode > 0) {
        Serial.printf("Fall data sent to Home Assistant. HTTP Response code: %d\n", httpResponseCode);
    } else {
        Serial.printf("Failed to send fall data. Error: %s\n", http.errorToString(httpResponseCode).c_str());
    }

    http.end();
}
void ServerManager::Reset() {
    _sta_ssid = "";
    _sta_password = "";
    _ceiling_height = "";
    _fall_url = "";
    _presence_url = "";
    saveCredentials(); // Clear saved credentials
}