#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>

#define SCREEN_WIDTH 128 
#define SCREEN_HEIGHT 64 
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

#define DHTPIN 15
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

#define MQ135_PIN 34 
#define RL_VALUE 10.0       
#define RO_CLEAN_AIR 3.63   
#define NH3_A 102.2      
#define NH3_B -2.473        

float R0 = 10.0; 

void setup() {
  Serial.begin(115200);

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("Gagal inisialisasi OLED"));
    for(;;); 
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 28);
  display.println("Welcome to Pintelor");
  display.display();
  delay(2000);

  dht.begin();
  pinMode(MQ135_PIN, INPUT);

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
    Serial.println("dht not connected, not reading values");
    return;
  }

 
  float voltage = rawValue * (3.3 / 4095.0);
  
  float ppmAmonia = 0;
  if (voltage > 0) {
    float Rs = RL_VALUE * (3.3 - voltage) / voltage;
    float ratio = Rs / R0;
    ppmAmonia = NH3_A * pow(ratio, NH3_B);
  }


  Serial.print("Suhu: "); Serial.print(suhu); Serial.print(" C | ");
  Serial.print("RH: "); Serial.print(kelembapan); Serial.print(" % | ");
  Serial.print("Amonia: "); Serial.print(ppmAmonia); Serial.println(" PPM");

 display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("Suhu   : "); display.print(suhu, 1); display.print(" C");

  display.setCursor(0, 16);
  display.print("Lembap : "); display.print(kelembapan, 0); display.print(" %");

  display.setCursor(0, 32);
  display.print("Amonia : "); display.print(ppmAmonia, 1); display.print(" PPM");

  display.setCursor(0, 52); 
  display.display();

  delay(2000); 
}