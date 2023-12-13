#include "U8glib.h"
#include <DHT.h>
#include <SGP30.h>

#define pResistor A0 // photo resistor kopplad till analog 0 
#define DHTPIN 2     // temp and air humidity connected to pin 2
#define DHTTYPE DHT11   // DHT 11  (AM2302)

DHT dht(DHTPIN, DHTTYPE); // Initialize DHT sensor for normal 16mhz Arduino

SGP30 SGP;

// data read
float hum;  //Stores humidity value
float temp; //Stores temperature value
float co2;
float voc;


const int buttonPin = 4;  // the number of the pushbutton pin
int buttonState = 0;  // variable for reading the pushbutton status

const int arraySize = 128;
float dataArray[arraySize];

int currentIndex = 0;

U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_DEV_0 | U8G_I2C_OPT_NO_ACK | U8G_I2C_OPT_FAST);  // Fast I2C / TWI


void setup(void) {
  pinMode(pResistor, INPUT); //
  dht.begin();
  Serial.begin(9600);
  SGP.begin();
  pinMode(buttonPin, INPUT);
}

void loop(void) {
  // picture loop

  SGP.measure(true);

  buttonState = digitalRead(buttonPin);
  // check if the pushbutton is pressed. If it is, the buttonState is HIGH:
  if (buttonState == HIGH) {
    graf();
  } else {
    draw();
  }
  

  co2 = SGP.getCO2();
  voc = SGP.getTVOC();
  hum = dht.readHumidity();
  temp = dht.readTemperature();
  addToArray(temp);

  for (int i = 0; i < arraySize; i++)
  {
    Serial.println(dataArray[i]);
  }
}


void draw(void) {
  // graphic commands to redraw the complete screen should be placed here

  u8g.firstPage();
  do {

    u8g.setFont(u8g_font_unifont);

    u8g.drawStr( 15, 23, String(co2).c_str());
    u8g.drawStr( 15, 9, String("co2:").c_str());

    u8g.drawStr( 89, 23, String(voc).c_str());
    u8g.drawStr( 89, 9, String("voc:").c_str());

    u8g.drawStr( 15, 63, String(hum).c_str());
    u8g.drawStr( 15, 49, String("hum:").c_str());

    u8g.drawStr( 89, 63, String(temp).c_str());
    u8g.drawStr( 89, 49, String("temp:").c_str());

  } while ( u8g.nextPage() );


}

void graf(void) {
  //jag älksar dig klara / Nicholas

  u8g.firstPage();
  do {

    u8g.setFont(u8g_font_unifont);

    for (int i = 0; i < arraySize; i++)
    {
      if (i == 0) {
        u8g.drawPixel(i, (63 - dataArray[i]));
      } else {

        int y1 = 32+(dataArray[i]-20);
        int y2 = 32+(dataArray[i-1]-20);
        u8g.drawLine((i-1), ((((20 - dataArray[i-1]))*3)+40), i, ((((20 - dataArray[i])))*3)+40);
      }
      // max 30 min 10

      // 25* man får 4 som y, man har 30 pixlar upp, 
    }


  } while ( u8g.nextPage() );



}

void addToArray(float a) {
  dataArray[currentIndex] = a;

  currentIndex = (currentIndex + 1) % arraySize;
}


/////////////////////////////////////////////////////////
