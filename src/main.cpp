#include <Arduino.h>

#include <Adafruit_BMP085.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <DHT.h>
#include <ESP8266WiFi.h>
#include <NTPClient.h>
#include <time.h>
#include <WiFiUdp.h>
#include <Wire.h>
#include <ESPAsyncTCP.h>
// #include <ESPAsyncWebServer.h>

// Pinos
#define TFT_CS         0
#define TFT_RST        2
#define TFT_DC        15
#define TFT_MOSI      13
#define TFT_SCLK      14
#define DHT_PIN       12 

//Definições WiFi
const char* WIFI_SSID     = "ISI-3";
const char* WIFI_PASSWORD = "Inovacao548";
WiFiClient wifiClient;

// Definições Cliente NTP
WiFiUDP ntpUDP;
const long utcOffsetInSeconds = -10800;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);

// Definições Dias da Semana e Mês
const String WDAY_NAMES[] = {"Dom", "Seg", "Ter", "Qua", "Qui", "Sex", "Sab"};
const String MONTH_NAMES[] = {"Jan", "Fev", "Mar", "Abr", "Mai", "Jun", "Jul", "Ago", "Set", "Out", "Nov", "Dez"};

// Definições Display
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);

// Definições DHT11
#define DHTTYPE DHT11
DHT dht(DHT_PIN, DHTTYPE);

// Definições BMP180
Adafruit_BMP085 bmp;

// Definição de cores
#define PRETO     0x0000
#define BRANCO    0xFFFF
#define CHUMBO    0x4208
#define TEMP1     0xCE39
#define TEMP2     0xEBF0
#define UMID1     0x7DDD
#define UMID2     0xCF1F
#define PRES1     0x7DDD
#define PRES2     0xCF1F
#define SOL1      0xFC29
#define SOL2      0xFE0C
#define SEN1      0xFE8E
#define SEN2      0xD6FD
#define NUB1      0x8451
#define NUB2      0xD6FD
#define CHU1      0x43B3
#define CHU2      0x7DDD
#define TEM1      0xB659
#define TEM2      0xFE08
#define TEM3      0xA65F

// Caracteres externos
extern unsigned char temperatura1[];
extern unsigned char temperatura2[];
extern unsigned char temperatura3[];
extern unsigned char umidade1[];
extern unsigned char umidade2[];
extern unsigned char umidade3[];
extern unsigned char pressao1[];
extern unsigned char pressao2[];
extern unsigned char pressao3[];

int state = 0;
unsigned long previousMillis = 0;
const unsigned long interval = 10000;

void connectWifi() {
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Conectando...");
  Serial.println(WIFI_SSID);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("Conectado!");
  Serial.println(WiFi.localIP());
  Serial.println();
  delay(7000);
}

void data(){
  
  String weekDay = WDAY_NAMES[timeClient.getDay()];
  timeClient.update();
  
  tft.setCursor(15,2);
  tft.setTextSize(1);
  tft.setTextColor(BRANCO);
  tft.print(weekDay);
  
  tft.print(", ");
  
  unsigned long epochTime = timeClient.getEpochTime();
  struct tm*ptm = gmtime ((time_t *)&epochTime); 
  int monthDay = ptm->tm_mday;
  tft.print(monthDay);
  
  tft.print(" ");
  
  int currentMonth = ptm->tm_mon+1;
  String currentMonthName = MONTH_NAMES[currentMonth-1];
  int currentYear = ptm->tm_year+1900;
  tft.print(currentMonthName);
  
  tft.print(" ");
  
  tft.print(currentYear);
}

void horario(){
  int currentHour = timeClient.getHours();
  int currentMinute = timeClient.getMinutes();
  tft.setCursor(35,15);
  tft.setTextSize(2); 
  if (currentHour < 10){
    tft.print("0");
    tft.print(currentHour);
  }
  else {
    tft.print(currentHour); 
  } 
  tft.print(":");
  if (currentMinute < 10){
    tft.print("0");
    tft.print(currentMinute);
  }
  else {
    tft.print(currentMinute);
  } 
}

void ip(){
  tft.setCursor(25,35);
  tft.setTextSize(1);
  tft.print(WiFi.localIP());
}

void temperatura_simples(){
  float t = bmp.readTemperature();
  tft.setCursor (1,45);
  tft.setTextSize(1);
  tft.print("Temperatura: ");
  tft.print(t);
  tft.print(" C");
}

void umidade_simples(){
  float h = dht.readHumidity();
  tft.setCursor (1,55);
  tft.setTextSize(1);
  tft.print("Umidade: ");
  tft.print(h);
  tft.print(" %");
}

void pressao_simples(){
  float t = bmp.readPressure();
  tft.setCursor (1,65);
  tft.setTextSize(1);
  tft.print("Pressao: ");
  tft.print(t/100);
  tft.print("hPa");
}

void temperatura(){

  tft.drawBitmap(16,32,temperatura1,96,96,TEMP1);
  tft.drawBitmap(16,32,temperatura2,96,96,TEMP2);
  tft.drawBitmap(16,32,temperatura3,96,96,PRETO);
  float t = bmp.readTemperature();
  tft.setCursor (30,130);
  tft.setTextSize(1);
  tft.print("Temperatura");
  tft.setCursor(25,140);
  tft.setTextSize(2);
  tft.print(t);
  tft.print(" C");
  tft.setCursor(90,138);
  tft.setTextSize(1);
  tft.print("o");
  
}

void umidade(){

  tft.drawBitmap(16,32,umidade1,96,96,UMID1);
  tft.drawBitmap(16,32,umidade2,96,96,UMID2);
  tft.drawBitmap(16,32,umidade3,96,96,PRETO);
  float h = dht.readHumidity();
  tft.setCursor(45,130);
  tft.setTextSize(1);
  tft.print("Umidade");
  tft.setCursor(32,140);
  tft.setTextSize(2);
  tft.print(h);
  tft.print("%");
}

void pressao(){

  tft.drawBitmap(16,32,pressao1,96,96,PRES1);
  tft.drawBitmap(16,32,pressao2,96,96,PRES2);
  tft.drawBitmap(16,32,pressao3,96,96,PRETO);
  float t = bmp.readPressure();
  tft.setCursor (45,130);
  tft.setTextSize(1);
  tft.print("Pressao");
  tft.setCursor(3,140);
  tft.setTextSize(2);
  tft.print(t/100);
  tft.print("hPa"); 
}

void setup() {
  // Inicialização comunicação serial
  Serial.begin(115200);

  // Inicialização WiFi
  connectWifi();
  
  // Inicialização cliente NTP
  timeClient.begin();
  timeClient.update();

  // Inicialização display
  tft.initR(INITR_BLACKTAB);
  tft.fillScreen(CHUMBO);
  tft.setTextSize(1);
  tft.setTextColor(BRANCO);
  tft.setCursor(25,70);
  tft.print("Conectando...");
  delay(7000);
  
  // Inicialização DHT11
  dht.begin();
  
  // Inicialização BMP180
  bmp.begin();

}

void loop() {

  tft.fillScreen(CHUMBO);
  data();
  horario();
  ip();
  temperatura_simples();
  umidade_simples();
  pressao_simples();
  delay(10000);

  // tft.fillScreen(CHUMBO);
  // data();
  // horario();
  // temperatura();
  // delay(10000);
  // tft.fillScreen(CHUMBO);
  // data();
  // horario(); 
  // umidade();
  // delay(10000);
  // unsigned long currentMillis = millis();
  // unsigned long state = currentMillis - previousMillis;
  // switch (state)
  // {
  // case 0:
  //   tft.fillScreen(CHUMBO);
  //   data();
  //   horario();
  //   temperatura();
  //   break;
  
  // case 1:
  //   tft.fillScreen(CHUMBO);
  //   data();
  //   horario(); 
  //   umidade();
  //   break;

  // case 2:
  //   tft.fillScreen(CHUMBO);
  //   data();
  //   horario(); 
  //   pressao();
  //   break;
    
  // default:
  //   break;
  // }

}