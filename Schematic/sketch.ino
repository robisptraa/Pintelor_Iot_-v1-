#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

#define DHTPIN 15
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

#define MQ135_PIN 34 

#define RELAY_KIPAS 26    
#define RELAY_PEMANAS 27 

void setup() {
  Serial.begin(115200);

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Smart Farm Init");
  delay(2000);
  lcd.clear();

  dht.begin();
  pinMode(MQ135_PIN, INPUT);
  
  pinMode(RELAY_KIPAS, OUTPUT);
  pinMode(RELAY_PEMANAS, OUTPUT);
  
  
  digitalWrite(RELAY_KIPAS, LOW); 
  digitalWrite(RELAY_PEMANAS, LOW);
}

void loop() {
  float suhu = dht.readTemperature();
  float kelembapan = dht.readHumidity();
  int gasAmonia = analogRead(MQ135_PIN); 

  if (isnan(suhu) || isnan(kelembapan)) {
    Serial.println("Gagal membaca sensor Suhu!");
    return;
  }

  if (suhu > 30.0 || gasAmonia > 2500) {
    digitalWrite(RELAY_KIPAS, HIGH); 
  } else {
    digitalWrite(RELAY_KIPAS, LOW); 
  }

  if (suhu < 25.0) {
    digitalWrite(RELAY_PEMANAS, HIGH); 
  } else {
    digitalWrite(RELAY_PEMANAS, LOW);  
  }

  Serial.print("Suhu: "); Serial.print(suhu); Serial.print(" C | ");
  Serial.print("RH: "); Serial.print(kelembapan); Serial.print(" % | ");
  Serial.print("Gas (Analog): "); Serial.println(gasAmonia);

  lcd.setCursor(0, 0);
  lcd.print("T:"); lcd.print(suhu, 1); lcd.print("C ");
  lcd.print("H:"); lcd.print(kelembapan, 0); lcd.print("%");

  lcd.setCursor(0, 1);
  lcd.print("Gas:"); lcd.print(gasAmonia);
  lcd.print("      "); 

  delay(2000); 
}