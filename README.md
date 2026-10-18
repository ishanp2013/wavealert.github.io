# WaveAlert

WaveAlert is a small earthquake-awareness project in two parts:

1. **A vibration alarm** built on an Arduino Uno. An SW-420 vibration sensor detects shaking. When it does, a red LED flashes and a buzzer sounds.
2. **A companion website** (`index.html`). It shows live earthquake data from the USGS, lets you watch places you care about, and includes QuakeGuard, an earthquake Q&A assistant.

The two parts run separately. The website does not read data from the Arduino.

> **WaveAlert is not an official warning system.** The SW-420 is a simple on/off vibration switch, not a seismometer, so it also reacts to taps, footsteps and nearby machines. In an emergency, follow your local authorities.

---

## Project structure

```
wavealert.github.io/
├── circut_firmware.ino   # Arduino Uno firmware
├── index.html            # Companion website (single self-contained file)
└── README.md
```

> The Arduino IDE needs each `.ino` file to be inside a folder with the same name. When you open `circut_firmware.ino`, the IDE will offer to create a `circut_firmware/` folder for it. Click OK.

---

## Part 1 — Hardware

### Components

| Qty | Part | Notes |
|-----|------|-------|
| 1 | Arduino Uno R3 | Any 5 V Uno-compatible board will work |
| 1 | SW-420 vibration sensor module | Digital output (DO); its potentiometer sets sensitivity |
| 1 | Active buzzer | Must be *active*, because the firmware switches it on and off with no tone signal |
| 1 | Red LED | Alarm indicator |
| 1 | Green LED | "System OK" indicator |
| 2 | 1 kΩ resistor | One in series with each LED |
| — | Breadboard and jumper wires | |

### Wiring

| From | To (Arduino) | Wire |
|------|--------------|------|
| SW-420 VCC | 5V | Power |
| SW-420 GND | GND | Ground |
| SW-420 DO | **D2** | Signal |
| Buzzer + | **D8** | Signal |
| Buzzer − | GND | Ground |
| Red LED + (long leg) via 1 kΩ | **D9** | Signal |
| Red LED − (short leg) | GND | Ground |
| Green LED + (long leg) via 1 kΩ | **D10** | Signal |
| Green LED − (short leg) | GND | Ground |

```
 SW-420            Arduino Uno              Outputs
 ┌──────┐         ┌───────────┐
 │ VCC  ├─────────┤ 5V     D8 ├──────────── Buzzer (+)
 │ GND  ├─────────┤ GND    D9 ├──[1kΩ]───── Red LED (+)
 │ DO   ├─────────┤ D2    D10 ├──[1kΩ]───── Green LED (+)
 └──────┘         │       GND ├──────────── all (−) legs
                  └───────────┘
```

### Firmware

Source: [`circut_firmware.ino`](circut_firmware.ino)

| Pin | Constant | Role |
|-----|----------|------|
| D2 | `sensorPin` | Input from SW-420 DO |
| D8 | `buzzerPin` | Active buzzer |
| D9 | `redLed` | Alarm LED |
| D10 | `greenLed` | System-OK LED |

**What it does**

1. **On startup:** the green LED turns on to show the system is running. Serial opens at **9600 baud**.
2. **Every loop:** the firmware reads D2. When the value changes, it prints `Sensor output: 0` or `Sensor output: 1` to the Serial Monitor.
3. **When vibration is detected** (the pin reads `DETECTED`):
   - the green LED turns off
   - the red LED and buzzer pulse together **5 times** (150 ms on, 150 ms off)
   - the firmware waits **1 second** so the buzzer's own vibration doesn't set the sensor off again
   - the green LED turns back on

One alarm cycle lasts about 2.5 seconds. If the shaking continues, the cycle repeats.

### Upload and calibrate

1. Open `circut_firmware.ino` in the Arduino IDE.
2. Select **Tools → Board → Arduino Uno** and the correct port, then click **Upload**.
3. Open **Tools → Serial Monitor** at **9600 baud**.
4. Leave the sensor still and note the value it prints. Then tap the sensor and note the new value.
5. If tapping prints `0`, change this line and upload again:
   ```cpp
   const int DETECTED = LOW;   // default is HIGH
   ```
6. Turn the potentiometer on the SW-420 to adjust sensitivity until light taps trigger it and the background doesn't.

---

## Part 2 — Companion website

`index.html` is a single file. It needs no build step, no server and no dependencies. Open it in Chrome, Edge, Safari or Firefox while online, or host it with GitHub Pages (**Settings → Pages → Deploy from branch `main`**).

### Features

| Section | What it does |
|---------|--------------|
| **Live pulse** | Shows the number of M2.5+ earthquakes worldwide in the last 24 hours, plus the strongest and the most recent. Refreshes every 2 minutes. An animated seismograph line gets busier when there are more earthquakes. |
| **Live Seismometer** | Add any city or country to a watch list. Each saved place gets a card with a magnitude gauge, a severity label (QUIET / LIGHT / MODERATE / STRONG / MAJOR / SEVERE) and its four strongest earthquakes from the last 30 days, linked to USGS. Cities match within 300 km; countries match by name. The list is saved in your browser. |
| **QuakeGuard** | An earthquake-only Q&A assistant. You can type or speak questions, and it can read answers aloud. Details are below. |
| **Stay safe** | Drop, Cover and Hold On instructions, plus tsunami advice for coastal areas. |

### QuakeGuard modes

QuakeGuard works without any setup and can optionally use Claude for fuller answers.

| Mode | When it's active | How it answers |
|------|------------------|----------------|
| **Built-in answers** (default) | No API key or proxy is set | A rule-based engine in the page (`QuakeEngine`). It answers live questions such as "strongest quake today", "earthquakes near Tokyo", "any tsunami alerts?" or "M5+ this week" directly from USGS feeds. It also answers common questions about safety, causes, magnitude, aftershocks and tsunamis from a built-in knowledge base. |
| **AI answers** | An API key or proxy is set | Sends your question to Claude together with fresh USGS data: all M2.5+ events in the last 24 hours, significant events from the last 7 days, and the built-in engine's computed answer. The system prompt keeps it on earthquake topics and tells it never to make up events. |

Other QuakeGuard features:
- **"Near me" questions** use your browser's location (you'll be asked for permission) and search within 500 km.
- **Voice input** uses the Web Speech API. In browsers that don't support it, use your keyboard's dictation instead.
- **Speak answers** reads replies aloud with speech synthesis.

### Enabling AI answers

You can enable AI answers in three ways, listed from safest to least safe:

1. **Proxy server (recommended for public hosting).** Run a small backend that forwards requests to `https://api.anthropic.com/v1/messages` and adds your key on the server. Then set:
   ```js
   window.WAVEALERT_CONFIG = { apiKey: "", model: "claude-sonnet-5-5", proxyUrl: "https://your-server.example/claude" };
   ```
2. **Per-browser key.** In QuakeGuard, open **AI settings** and paste an Anthropic API key (it starts with `sk-ant-`). The key is stored in that browser's `localStorage` only.
3. **Key in the file.** Set `apiKey` in `WAVEALERT_CONFIG` near the top of the HTML. ⚠️ Anyone who opens the page can see the key with View Source. Use this only for private, local testing, and set a spending limit on the key.

### Data sources

| Source | Used for |
|--------|----------|
| [USGS Earthquake Hazards Program](https://earthquake.usgs.gov/earthquakes/feed/v1.0/geojson.php) GeoJSON feeds | All earthquake data (`all_hour`, `2.5_day`, `2.5_week`, `2.5_month`, `4.5_month`, `significant_week`) |
| [Open-Meteo Geocoding API](https://open-meteo.com/en/docs/geocoding-api) | Converting typed place names to coordinates in the Seismometer |
| Anthropic Messages API (optional) | AI answers in QuakeGuard |

USGS figures are preliminary and can be delayed or revised.

### Data stored in the browser

| `localStorage` key | Contents |
|--------------------|----------|
| `wavealert.places.v1` | Seismometer watch list and view settings |
| `wavealert.quakeguard.prefs` | Speak-answers and auto-send preferences |
| `wavealert.quakeguard.key` | Your Anthropic API key, only if you saved one under AI settings |

---

## Known limitations

- **The hardware and website are separate.** The Arduino prints to Serial, but the website doesn't listen. A possible next step is connecting them with the Web Serial API.
- **The sensor reports only "shaking" or "not shaking."** The SW-420 doesn't measure magnitude and can't tell an earthquake from a door slam. An accelerometer such as the MPU-6050 or ADXL345 would give real readings.
- **The alarm loop blocks.** The firmware uses `delay()`, so it can't read the sensor during the 2.5-second alarm cycle.
- **Small earthquakes are missing.** The website shows M2.5 and above only (M4.5+ for some 30-day views), so smaller local earthquakes won't appear.
- **Neither part predicts earthquakes.** Both only report shaking or earthquakes that have already happened.

## Safety reminder

If the ground shakes: **Drop, Cover and Hold On.** If you're near the coast and the shaking is strong or long, move to high ground right away. Don't wait for an official warning.
