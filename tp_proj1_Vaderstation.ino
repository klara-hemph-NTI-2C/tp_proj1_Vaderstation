
#include "U8glib.h"
#include <DHT.h>
#include <SGP30.h>


#define DHTPIN 2              // Temp- och luftfuktighetssensor ansluten till pin 2
#define DHTTYPE DHT11         // DHT 11 (AM2302)

DHT dht(DHTPIN, DHTTYPE);     // Initiera DHT-sensorn 

SGP30 SGP;                   

// Variabler för att lagra avläst data
float hum;                   // Lagrar luftfuktighetsvärde
float temp;                  // Lagrar temperaturvärde
float co2;                   // Lagrar co2-värde
float voc;                   // Lagrar voc-värde

const int buttonPin = 4;     // Pin för tryckknappen
int buttonState = 0;         // Variabel för att läsa knappens status

const int arraySize = 128;   // Storlek på dataarrayen
float dataArray[arraySize];   // Array för att lagra temperaturdata över tid

int currentIndex = 0;        // Nuvarande index i datarrayen

U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_DEV_0 | U8G_I2C_OPT_NO_ACK | U8G_I2C_OPT_FAST); // Snabb I2C / TWI

void setup(void) {
  dht.begin();
  Serial.begin(9600);
  SGP.begin();
  pinMode(buttonPin, INPUT);  // Ange knappen som inmatning
}

void loop(void) {
  // Mät gasvärden från SGP30
  SGP.measure(true);

  buttonState = digitalRead(buttonPin);

  // Kolla om knappen är nedtryckt. Om den är det, sätt buttonState till HIGH:
  if (buttonState == HIGH) {
    draw();    // Anropa funktionen för att rita information på displayen
  } else {
    graf();    // Anropa funktionen för att rita graf över tid
  }

  // Hämta aktuella värden för CO2, TVOC, luftfuktighet och temperatur
  co2 = SGP.getCO2();
  voc = SGP.getTVOC();
  hum = dht.readHumidity();
  temp = dht.readTemperature();
  
  addToArray(temp);  // Lägg till temperaturvärde i arrayen

  // Skriv ut innehållet i arrayen
  for (int i = 0; i < arraySize; i++) {
    Serial.println(dataArray[i]);
  }
}

// Funktion för att rita information på displayen
void draw(void) {
  u8g.firstPage();
  do {
    u8g.setFont(u8g_font_unifont);

    // Rita CO2-värde
    u8g.drawStr(15, 23, String(co2).c_str());
    u8g.drawStr(15, 9, String("co2:").c_str());

    // Rita TVOC-värde
    u8g.drawStr(89, 23, String(voc).c_str());
    u8g.drawStr(89, 9, String("voc:").c_str());

    // Rita luftfuktighet
    u8g.drawStr(15, 63, String(hum).c_str());
    u8g.drawStr(15, 49, String("hum:").c_str());

    // Rita temperatur
    u8g.drawStr(89, 63, String(temp).c_str());
    u8g.drawStr(89, 49, String("temp:").c_str());

  } while (u8g.nextPage());
}

// Funktion för att rita en graf över tid
void graf(void) {
  u8g.firstPage();
  do {
    u8g.setFont(u8g_font_unifont);

    // Rita datapunkter i grafen
    for (int i = 0; i < arraySize; i++) {
      if (i == 0) {
        u8g.drawPixel(i, (63 - dataArray[i]));
      } else {
        int y1 = 32 + (dataArray[i] - 20);
        int y2 = 32 + (dataArray[i - 1] - 20);
        u8g.drawLine((i - 1), ((((20 - dataArray[i - 1])) * 3) + 40), i, ((((20 - dataArray[i]))) * 3) + 40);
      }
    }

  } while (u8g.nextPage());
}

// Funktion för att lägga till temperaturvärde i arrayen och rotera om arrayen är full
void addToArray(float value) {
  dataArray[currentIndex] = value;
  currentIndex = (currentIndex + 1) % arraySize;
}
