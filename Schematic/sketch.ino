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

#define RL_VALUE 10.0       
#define RO_CLEAN_AIR 3.63   
#define NH3_A 102.2      
#define NH3_B -2.473        

float R0 = 10.0; 

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

  // === (Kalibration logic R0) 
  // float calcR0 = 0;
  // for(int i = 0; i < 50; i++) {
  //   int raw = analogRead(MQ135_PIN);
  //   float v = raw * (3.3 / 4095.0);
  //   float rs = RL_VALUE * (3.3 - v) / v;
  //   calcR0 += (rs / RO_CLEAN_AIR);
  //   delay(100);
  // }
  // R0 = calcR0 / 50.0;
  // Serial.print("Kalibrasi R0: "); Serial.println(R0);
}

void loop() {
  float suhu = dht.readTemperature();
  float kelembapan = dht.readHumidity();
  
  int rawValue = analogRead(MQ135_PIN); 

  if (isnan(suhu) || isnan(kelembapan)) {
    Serial.println("Gagal membaca sensor Suhu!");
    return;
  }

 
  float voltage = rawValue * (3.3 / 4095.0);
  
  float ppmAmonia = 0;
  if (voltage > 0) {
    float Rs = RL_VALUE * (3.3 - voltage) / voltage;
    float ratio = Rs / R0;
    ppmAmonia = NH3_A * pow(ratio, NH3_B);
  }

  if (suhu > 30.0 || ppmAmonia > 25.0) {
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
  Serial.print("Amonia: "); Serial.print(ppmAmonia); Serial.println(" PPM");

  lcd.setCursor(0, 0);
  lcd.print("T:"); lcd.print(suhu, 1); lcd.print("C ");
  lcd.print("H:"); lcd.print(kelembapan, 0); lcd.print("%");

  lcd.setCursor(0, 1);
  lcd.print("NH3:"); lcd.print(ppmAmonia, 1); lcd.print(" PPM");
  lcd.print("      "); 

  delay(2000); 
}