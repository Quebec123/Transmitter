//
// Created by wikto on 17/03/2026.
//

#ifndef SERVER_MANAGER_H
#define SERVER_MANAGER_H

#include <WiFi.h>
#include <WebServer.h>
#include <Preferences.h>

class ServerManager {
public:
    void startAP();
    void stopAP();
    bool loopAP();
    IPAddress getIP() const;
    void sendPresenceToHomeAssistant();
    void sendFallToHomeAssistant();
    String getCeilingHeight() const { return _ceiling_height; }
    void Reset();
private:
    WebServer _server{80};
    const char* _ap_ssid = "WS";
    const char* _ap_password = "12345678";
    Preferences _preferences; // For storing values persistently
    String _sta_ssid = "MERCUSYS";
    String _sta_password = "1234567890";
    String _ceiling_height;
    String _fall_url;
    String _presence_url;

    void connectToWiFi();
    void saveCredentials();
    void loadCredentials();
};
//eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiI5YWY2ZDgwMWUxOGE0NjQzYjdjYjNiMWNkOWY0OGY1YiIsImlhdCI6MTc3NTA1MDg5OSwiZXhwIjoyMDkwNDEwODk5fQ.cHMc0G7Ews-WqZ3VGvsBc74ACKP9Ge7QETPKAXzBvr8
#endif // SERVER_MANAGER_H