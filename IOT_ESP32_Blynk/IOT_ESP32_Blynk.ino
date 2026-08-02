// ================= MASUKKAN INI DI PALING ATAS (WAJIB!) =================
#define BLYNK_TEMPLATE_ID "TMPL6bOgfxpwb"
#define BLYNK_TEMPLATE_NAME "Inkubator Penetas Telur Otomatis"
#define BLYNK_AUTH_TOKEN "Woe3siEX83mSpVtcgGhyDc9kvZRE5kCl"

#define BLYNK_PRINT Serial

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>
#include <SPI.h>
#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>

// ================= KREDENSIAL WIFI HOTSPOT =================
char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "ROG Phone 9 PRO";
char pass[] = "tanpapassword";

// ================= PENGATURAN PIN & HARDWARE =================
#define DHTPIN 4
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

#define RELAY_LAMPU 26  
#define RELAY_KIPAS 14  
#define RELAY_MOTOR 27  

#define BTN_MODE  32
#define BTN_LAMPU 33    
#define BTN_MOTOR 13    
#define BTN_KIPAS 25    

LiquidCrystal_I2C lcd(0x27, 16, 2);

#define HARDWARE_TYPE MD_MAX72XX::FC16_HW 
#define MAX_DEVICES 4                     
#define CS_PIN 5                          

MD_Parola runningText = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);
char pesanMatrix[150]; 

// ================= VARIABEL GLOBAL =================
bool isOtomatisMode = true; 
float suhu = 0.0;
float kelembapan = 0.0;

bool statusLampu = false;
bool statusKipas = false;
bool statusMotor = false;

bool lastBtnMode = LOW;
bool lastBtnLampu = LOW;
bool lastBtnKipas = LOW;
bool lastBtnMotor = LOW;

unsigned long waktuTerakhirDHT = 0;
const unsigned long JEDA_DHT = 2000; 

// ================================
//  SETTINGAN WAKTU MOTOR SINKRON 
// ================================
unsigned long waktuTerakhirMuter = 0;
const unsigned long JEDA_PUTAR = 60000; //  JEDA WAKTU TUNGGU MOTOR (60000 ms = 1 Menit)
const unsigned long LAMA_PUTAR = 3000;  //  DURASI BERAPA LAMA MOTOR MUTAR (3000 ms = 3 Detik)   

unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 250; 

String teksDefault = "";
String pendingTransitionText = "";
bool isTransitioning = false;

// ================= SINKRONISASI JALUR BLYNK (V-PIN) =================
BLYNK_WRITE(V2) { // Mode Aplikasi Blynk
  int value = param.asInt();
  bool newMode = (value == 1);
  if (newMode != isOtomatisMode) {
    isOtomatisMode = newMode;
    pendingTransitionText = isOtomatisMode ? ">>> MODE OTOMATIS AKTIF <<<" : ">>> MODE MANUAL AKTIF <<<";
  }
}

BLYNK_WRITE(V3) { // Lampu Aplikasi Blynk
  if (!isOtomatisMode) {
    statusLampu = param.asInt();
    digitalWrite(RELAY_LAMPU, statusLampu ? HIGH : LOW); 
  }
}

BLYNK_WRITE(V4) { // Kipas Aplikasi Blynk
  if (!isOtomatisMode) {
    statusKipas = param.asInt();
    digitalWrite(RELAY_KIPAS, statusKipas ? HIGH : LOW); 
  }
}

BLYNK_WRITE(V5) { // Motor Aplikasi Blynk
  if (!isOtomatisMode) {
    statusMotor = param.asInt();
    digitalWrite(RELAY_MOTOR, statusMotor ? HIGH : LOW); 
  }
}

void setup() {
  Serial.begin(115200);
  
  runningText.begin();
  runningText.setIntensity(5); 
  runningText.displayClear();
  
  strcpy(pesanMatrix, "SYSTEM STARTING...");
  runningText.displayText(pesanMatrix, PA_CENTER, 40, 0, PA_SCROLL_LEFT, PA_SCROLL_LEFT);
  
  dht.begin();
  lcd.init();
  lcd.backlight();

  pinMode(RELAY_LAMPU, OUTPUT);
  pinMode(RELAY_KIPAS, OUTPUT);
  pinMode(RELAY_MOTOR, OUTPUT);

  digitalWrite(RELAY_LAMPU, LOW); 
  digitalWrite(RELAY_MOTOR, LOW); 
  digitalWrite(RELAY_KIPAS, LOW);  

  pinMode(BTN_MODE, INPUT_PULLDOWN);
  pinMode(BTN_LAMPU, INPUT_PULLDOWN);
  pinMode(BTN_KIPAS, INPUT_PULLDOWN);
  pinMode(BTN_MOTOR, INPUT_PULLDOWN);

  // --- SPLASH SCREEN LCD ---
  lcd.setCursor(0, 0);
  lcd.print("INKUBATOR TELUR ");
  lcd.setCursor(0, 1);
  lcd.print("KELOMPOK 1 PVTE ");
  delay(3500); 
  
  // --- OFFLINE MODE ---
  WiFi.begin(ssid, pass); 
  Blynk.config(auth);     
  
  suhu = dht.readTemperature();
  kelembapan = dht.readHumidity();
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    Blynk.run();
  }

  // 1. BACA SENSOR 
  if (millis() - waktuTerakhirDHT >= JEDA_DHT) {
    float t = dht.readTemperature();
    float h = dht.readHumidity();
    if (!isnan(t) && !isnan(h)) {
      suhu = t;
      kelembapan = h;
      
      if (WiFi.status() == WL_CONNECTED) {
        Blynk.virtualWrite(V0, (int)suhu);
        Blynk.virtualWrite(V1, (int)kelembapan);
      }
    }
    waktuTerakhirDHT = millis();
  }

  // 2. BACA TOMBOL MODE FISIK 
  bool bacaMode = digitalRead(BTN_MODE);
  if (bacaMode == HIGH && lastBtnMode == LOW) {
    if (millis() - lastDebounceTime > debounceDelay) {
      isOtomatisMode = !isOtomatisMode; 
      pendingTransitionText = isOtomatisMode ? ">>> MODE OTOMATIS AKTIF <<<" : ">>> MODE MANUAL AKTIF <<<";
      
      if (WiFi.status() == WL_CONNECTED) Blynk.virtualWrite(V2, isOtomatisMode ? 1 : 0);
      lastDebounceTime = millis();
    }
  }
  lastBtnMode = bacaMode;

  // 3. LOGIKA OTOMATIS (IoT) 
  if (isOtomatisMode) {
    
    // ====================================
    //  SETTINGAN BATAS SUHU LAMPU & KIPAS
    // ====================================
    
    // KONDISI 1: SUHU DINGIN (DI BAWAH 30°C) -> LAMPU NYALA, KIPAS MATI
    if (suhu < 30 && (!statusLampu || statusKipas)) { 
      digitalWrite(RELAY_LAMPU, HIGH);   
      digitalWrite(RELAY_KIPAS, LOW);    
      statusLampu = true; statusKipas = false;
      if (WiFi.status() == WL_CONNECTED) {
        Blynk.virtualWrite(V3, 1); 
        Blynk.virtualWrite(V4, 0); 
      }
    } 
    // KONDISI 2: SUHU PANAS (DI ATAS 31°C) -> LAMPU MATI, KIPAS NYALA
    else if (suhu > 31 && (statusLampu || !statusKipas)) { 
      digitalWrite(RELAY_LAMPU, LOW);    
      digitalWrite(RELAY_KIPAS, HIGH);   
      statusLampu = false; statusKipas = true;
      if (WiFi.status() == WL_CONNECTED) {
        Blynk.virtualWrite(V3, 0); 
        Blynk.virtualWrite(V4, 1); 
      }
    } 

    // KONDISI 3: RAK TELUR OTOMATIS (SINKRON MOTOR)
    unsigned long waktuSekarang = millis();
    if (waktuSekarang - waktuTerakhirMuter >= JEDA_PUTAR) {
      if (!statusMotor) {
        digitalWrite(RELAY_MOTOR, HIGH); // MOTOR SINKRON MULAI
        statusMotor = true;
        if (WiFi.status() == WL_CONNECTED) Blynk.virtualWrite(V5, 1);
      }
      if (waktuSekarang - waktuTerakhirMuter >= (JEDA_PUTAR + LAMA_PUTAR)) {
        digitalWrite(RELAY_MOTOR, LOW);  // MOTOR SINKRON BERHENTI
        statusMotor = false;
        if (WiFi.status() == WL_CONNECTED) Blynk.virtualWrite(V5, 0);
        waktuTerakhirMuter = waktuSekarang; 
      }
    }
  } 
  // 4. LOGIKA MANUAL FISIK
  else {
    bool bacaLampu = digitalRead(BTN_LAMPU);
    if (bacaLampu == HIGH && lastBtnLampu == LOW) { 
      if (millis() - lastDebounceTime > debounceDelay) {
        statusLampu = !statusLampu;
        digitalWrite(RELAY_LAMPU, statusLampu ? HIGH : LOW); 
        if (WiFi.status() == WL_CONNECTED) Blynk.virtualWrite(V3, statusLampu ? 1 : 0);
        lastDebounceTime = millis();
      }
    }
    lastBtnLampu = bacaLampu;
    
    bool bacaKipas = digitalRead(BTN_KIPAS);
    if (bacaKipas == HIGH && lastBtnKipas == LOW) { 
      if (millis() - lastDebounceTime > debounceDelay) {
        statusKipas = !statusKipas;
        digitalWrite(RELAY_KIPAS, statusKipas ? HIGH : LOW); 
        if (WiFi.status() == WL_CONNECTED) Blynk.virtualWrite(V4, statusKipas ? 1 : 0);
        lastDebounceTime = millis();
      }
    }
    lastBtnKipas = bacaKipas;
    
    bool bacaMotor = digitalRead(BTN_MOTOR);
    if (bacaMotor == HIGH && lastBtnMotor == LOW) { 
      if (millis() - lastDebounceTime > debounceDelay) {
        statusMotor = !statusMotor;
        digitalWrite(RELAY_MOTOR, statusMotor ? HIGH : LOW); 
        if (WiFi.status() == WL_CONNECTED) Blynk.virtualWrite(V5, statusMotor ? 1 : 0);
        lastDebounceTime = millis();
      }
    }
    lastBtnMotor = bacaMotor;
  }

  // 5. UPDATE TAMPILAN LCD 16x2
  updateLCD();

  // 6. UPDATE TEKS DEFAULT MATRIX 
  teksDefault = "INKUBATOR TELUR -- SUHU: " + String((int)suhu) + " C -- KELEMBAPAN: " + String((int)kelembapan) + "% -- KELOMPOK 1 PVTE UMRI";

  // 7. POTONG JALUR MATRIX 
  if (pendingTransitionText != "") {
    pendingTransitionText.toCharArray(pesanMatrix, 150);
    runningText.displayReset(); 
    isTransitioning = true;
    pendingTransitionText = ""; 
  }

  // 8. LOGIKA ANIMASI MATRIX
  if (runningText.displayAnimate()) {
    if (isTransitioning) {
      isTransitioning = false; 
    }
    if (!isTransitioning) {
      teksDefault.toCharArray(pesanMatrix, 150);
    }
    runningText.displayReset(); 
  }
}

// ================= FUNGSI TAMBAHAN =================
void updateLCD() {
  // Baris Atas
  lcd.setCursor(0, 0);
  lcd.print("S:");
  lcd.print((int)suhu);       
  lcd.print("C H:");
  lcd.print((int)kelembapan); 
  lcd.print("% ");
  
  // Indikator Jaringan (W:ON / W:OF) di pojok kanan atas
  lcd.setCursor(12, 0);
  lcd.print(WiFi.status() == WL_CONNECTED ? "W:ON" : "W:OF");

  // Baris Bawah (Status Sistem & Relay)
  lcd.setCursor(0, 1);
  lcd.print(isOtomatisMode ? "A " : "M ");
  lcd.print(statusLampu ? "L:ON " : "L:OF ");
  lcd.print(statusKipas ? "K:ON " : "K:OF ");
  lcd.print(statusMotor ? "M:ON" : "M:OF");
}