#pragma once
/*******************************************************************************
 * @file    [MainPageAP].[h]
 * @brief   [Strona konfiguracyjna czunika upadku]
 * * @project [Czujnik updaku oraz alarm]
 * @author  [Wiktor Sobczyński]
 * @date    [2026-06-14]
 * @version V1.0
 * *******************************************************************************/
#include <Arduino.h>

const char CONFIG_PAGE[] PROGMEM = R"raw(
<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <style>
        body { font-family: -apple-system, sans-serif; background: #1a1a1a; color: white; display: flex; justify-content: center; align-items: center; height: 100vh; margin: 0; }
        .card { background: #2d2d2d; padding: 2rem; border-radius: 12px; box-shadow: 0 10px 25px rgba(0,0,0,0.5); width: 90%; max-width: 400px; }
        input { width: 100%; padding: 12px; margin: 8px 0; border-radius: 6px; border: 1px solid #444; background: #1a1a1a; color: white; box-sizing: border-box; }
        button { width: 100%; padding: 12px; background: #007bff; border: none; color: white; border-radius: 6px; font-weight: bold; cursor: pointer; margin-top: 10px; }
        h2 { margin-top: 0; text-align: center; }
    </style>
</head>
<body>
    <div class="card">
        <h2>Device Setup</h2>
        <form action="/save" method="POST">
            <input type="text" name="ssid" placeholder="WiFi SSID" required>
            <input type="password" name="password" placeholder="Password">
            <input type="number" name="ceiling_height" placeholder="Ceiling Height (cm)" required>
            <input type="text" name="fall_url" placeholder="Fall URL is required" required>
            <input type="text" name="presence_url" placeholder="Presence URL is required)" required>
            <button type="submit">Connect Device</button>
        </form>
    </div>
</body>
</html>
)raw";