#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include <ZMPT101B.h>
#include <math.h>
#include "RTClib.h"
#include <HTTPClient.h>
#include <WebServer.h>

/* -------------------- WIFI -------------------- */
const char* ssid = "Samsung F 54";
const char* password = "pgshah_2304";

/* ---------------- GOOGLE SHEET ---------------- */
const char* sheetURL = "https://script.google.com/macros/s/AKfycbzRZwVFjKaUbX1AEDHmWM45eGqS_wSIdmZuGN2UMp6lSjhlRFbebf1UBQTXWK7m5zZ9hA/exec";

/* ---------------- PINS ---------------- */
#define VOLTAGE_PIN 34
#define CURRENT_PIN 35

/* ---------------- ADC ---------------- */
#define ADC_MAX 4095.0
#define VREF 3.3

/* ---------------- SENSOR ---------------- */
#define VOLT_SENSITIVITY 500.0
#define VOLT_CAL_FACTOR 1.49
#define CURR_SENSITIVITY 0.185

/* ---------------- OBJECTS ---------------- */
ZMPT101B voltageSensor(VOLTAGE_PIN);
LiquidCrystal_I2C lcd(0x27, 20, 4);
RTC_DS3231 rtc;
WebServer server(80);

/* ---------------- VARIABLES ---------------- */
float currentOffset = 0;
float energy_kWh = 0.0;
float cost_rs = 0.0;
unsigned long lastTime = 0;
unsigned long lastSheetSend = 0;



/* ===========================================================
      MGVCL BILL CALCULATION
   =========================================================== */
float calculateMGVCLBill(float units) {

  float cost = 0;

  if (units <= 50)       cost = units * 3.05;
  else if (units <= 100) cost = (50 * 3.05) + ((units - 50) * 3.50);
  else if (units <= 250) cost = (50 * 3.05) + (50 * 3.50) + ((units - 100) * 4.15);
  else                   cost = (50 * 3.05) + (50 * 3.50) + (150 * 4.15) + ((units - 250) * 5.20);

  return cost + 15;
}

/* ===========================================================
      CURRENT OFFSET CALIBRATION
   =========================================================== */
void calibrateCurrentOffset() {
  long sum = 0;
  for (int i = 0; i < 5000; i++) {
    sum += analogRead(CURRENT_PIN);
    delayMicroseconds(200);
  }
  currentOffset = (sum / 5000.0) * (VREF / ADC_MAX);
}

/* ===========================================================
      CURRENT RMS
   =========================================================== */
float getCurrentRMS() {
  float sumSq = 0;
  for (int i = 0; i < 6000; i++) {
    float adc = analogRead(CURRENT_PIN);
    float voltage = adc * (VREF / ADC_MAX);
    float current = (voltage - currentOffset) / CURR_SENSITIVITY;
    sumSq += current * current;
    delayMicroseconds(200);
  }
  return sqrt(sumSq / 6000.0);
}

/* ===========================================================
      VOLTAGE RMS
   =========================================================== */
float getVoltageRMS() {
  float sum = 0;
  for (int i = 0; i < 20; i++) {
    sum += voltageSensor.getRmsVoltage();
    delay(5);
  }
  return (sum / 20.0) * VOLT_CAL_FACTOR;
}

/* ===========================================================
      GOOGLE SHEET SEND
   =========================================================== */
void sendToSheet(DateTime nowRTC, float V, float I, float P, float E, float C) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    String url = String(sheetURL);
    url += "?date=" + String(nowRTC.day()) + "/" + String(nowRTC.month()) + "/" + String(nowRTC.year());
    url += "&time=" + String(nowRTC.hour()) + ":" + String(nowRTC.minute());
    url += "&voltage=" + String(V, 2);
    url += "&current=" + String(I, 4);
    url += "&power=" + String(P, 2);
    url += "&energy=" + String(E, 5);
    url += "&cost=" + String(C, 2);

    http.begin(url);
    http.GET();
    http.end();
  }
}

/* ===========================================================
      ---------------- DASHBOARD ADDED BELOW ----------------
   =========================================================== */



// HTML DASHBOARD
String dashboardPage(float V, float I, float P, float E, float C) {
  String html = "<html><head><meta http-equiv='refresh' content='1'>";
  html += "<style>body{font-family:Arial;background:#f2f2f2;text-align:center;}";
  html += ".card{background:white;padding:20px;margin:20px;border-radius:10px;box-shadow:0 0 10px #ccc;font-size:30px;}</style>";
  html += "</head><body><h1> ESP32 Smart Energy Meter</h1>";

  html += "<div class='card'>Voltage: " + String(V,1) + " V</div>";
  html += "<div class='card'>Current: " + String(I,3) + " A</div>";
  html += "<div class='card'>Power: " + String(P,1) + " W</div>";
  html += "<div class='card'>Energy: " + String(E,4) + " kWh</div>";
  html += "<div class='card'>Cost: " + String(C,2) + "Rs</div>";

  html += "</body></html>";
  return html;
}

// SERVER HANDLER
void handleRoot() {
  float V = getVoltageRMS();
  float I = getCurrentRMS();
  if (V < 50) { V = 0; I = 0; }
  float P = V * I;
  float C = cost_rs;

  server.send(200, "text/html", dashboardPage(V, I, P, energy_kWh, C));
}



/* ===========================================================
                      SETUP
   =========================================================== */
void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  analogReadResolution(12);
  analogSetAttenuation(ADC_11db);
  voltageSensor.setSensitivity(VOLT_SENSITIVITY);

  Wire.begin(21, 22);
  lcd.init();
  lcd.backlight();

  rtc.begin();
  if (rtc.lostPower()) {
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

  calibrateCurrentOffset();
  lastTime = millis();

  /* --- START DASHBOARD SERVER --- */
  server.on("/", handleRoot);
  server.begin();

  Serial.print("Dashboard IP: ");
  Serial.println(WiFi.localIP());
}

/* ===========================================================
                        LOOP
   =========================================================== */
void loop() {

  server.handleClient();   // <-- Dashboard runs here

  DateTime nowRTC = rtc.now();

  unsigned long now = millis();
  float deltaTime = (now - lastTime) / 1000.0;
  lastTime = now;

  float Vrms = getVoltageRMS();
  float Irms = getCurrentRMS();

  if (Vrms < 50) { Vrms = 0; Irms = 0; }

  float Power = Vrms * Irms;
  energy_kWh += (Power * deltaTime) / 3600000.0;

  cost_rs = calculateMGVCLBill(energy_kWh);

  /* -------- LCD -------- */
  lcd.setCursor(0, 0);
  lcd.print("V:"); lcd.print(Vrms, 1); lcd.print(" I:");
  lcd.print(Irms, 3); lcd.print("A ");

  lcd.setCursor(0, 1);
  lcd.print("P:"); lcd.print(Power, 1);
  lcd.print("W Rs:"); lcd.print(cost_rs, 2);

  lcd.setCursor(0, 2);
  lcd.print("E:"); lcd.print(energy_kWh, 5); lcd.print("kWh");

  lcd.setCursor(0, 3);
  lcd.print(nowRTC.day()); lcd.print("/");
  lcd.print(nowRTC.month()); lcd.print(" ");
  lcd.print(nowRTC.hour()); lcd.print(":");
  lcd.print(nowRTC.minute());

  /* -------- GOOGLE SHEET -------- */
  if (millis() - lastSheetSend > 10000) {
    lastSheetSend = millis();
    sendToSheet(nowRTC, Vrms, Irms, Power, energy_kWh, cost_rs);
  }

  delay(1000);
}
