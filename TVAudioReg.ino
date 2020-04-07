/************************************************************************************************
 * ARDUINO    OLED DISPLAY 7 pin (GND,VCC,D0,D1,RES,DC,CS)    
 *      D7      DC                    
 *      D6      CS
 *      D5      D1
 *      D4      D0
 *      5V      RES
 *     GND      GND 
 *      5V      VCC 
 *      
 * ARDUINO    LED IR TRASMITTER
 *  PWD D3      SIGNAL
 *      5V      VCC
 *     GND      GND   
 *     
 * ARDUINO    BUTTON
 *     D2       SIGNAL - 10Khom - GND      
 *     5V       5V
 *     
 * ARDUINO    MAX9814 5 pin (AR,OUT,GAIN,VCC,GND)
 *     A1       OUT       
 *     5V       VCC      
 *    GND       GND 
 *    
 *    
 */
 
#include "U8glib.h"
#include<IRremote.h>
#define POWER         0xE0E040BF            //from IRrecvDemo, Ken Shirriff
#define SAMSUNG_BITS  32
// volume up
const unsigned int S_vup[68] = {4600, 4350, 650, 1550, 700, 1500, 700, 1550, 700, 400, 700, 400, 700, 450, 650, 450, 700, 400, 700, 1500, 700, 1550, 650, 1550, 700, 400, 700, 400, 700, 450, 650, 450, 700, 400, 700, 1500, 700, 1550, 650, 1550, 700, 400, 700, 450, 700, 400, 700, 400, 700, 400, 700, 450, 650, 450, 650, 450, 650, 1550, 700, 1500, 700, 1550, 700, 1500, 700, 1550, 650};
// volume down
const unsigned int S_vdown[68] = {4600, 4350, 700, 1550, 650, 1550, 700, 1500, 700, 450, 650, 450, 700, 400, 700, 400, 700, 400, 700, 1550, 700, 1500, 700, 1550, 700, 400, 700, 400, 700, 400, 700, 450, 650, 450, 650, 1550, 700, 1500, 700, 450, 650, 1550, 700, 400, 700, 400, 700, 450, 700, 400, 700, 400, 700, 400, 700, 1550, 700, 400, 700, 1500, 700, 1500, 700, 1550, 700, 1500, 700};

const int sampleWindow = 50; // Sample window width in mS (50 mS = 20Hz)
unsigned int sample;


IRsend irsend;


U8GLIB_SH1106_128X64 u8g(4, 5, 6, 7); // SW SPI Com: SCK = 4, MOSI = 5, CS = 6, A0 = 7 (new blue HalTec OLED)

float sum = 0, avg = 0;
int  nread = 0;
int  nsample = 20;
int  flag;
unsigned long volumeMillis = millis();

int buttonState = 0;         // variable for reading the pushbutton status
const int buttonPin = 2;     // the number of the pushbutton pin
const int ledIR = 3;         // the numer of the IR LES
int gain = 57;               // livello audio
int stepDown=0;              // conta il numero di volumeDown consecutivi

void setup() {

  Serial.begin(9600);
  pinMode (ledIR, OUTPUT);  //output as used in library

}


void loop()
{
  unsigned long  startMillis = millis(); // Start of sample window
  unsigned int peakToPeak = 0;   // peak-to-peak level
  unsigned int signalMax = 0;
  unsigned int signalMin = 1024;

  // collect data for 50 mS
  while (millis() - startMillis < sampleWindow) {
    sample = analogRead(1);

    if (sample < 1024) { // toss out spurious readings
      if (sample > signalMax) {
        signalMax = sample;  // save just the max levels
      } else if (sample < signalMin) {
        signalMin = sample;  // save just the min levels
      }
    }
  }

  peakToPeak = signalMax - signalMin;  // max - min = peak-peak amplitude
  double volts = (peakToPeak * 5.0) / 1024;  // convert to volts
  float db = map(peakToPeak, 20, 900, 49.5, 90); //calibrate for decibel
  sum = sum + db;
  nread++;

  Serial.print("peakToPeak\t"); Serial.print(peakToPeak);
  Serial.print("\tVolt\t"); Serial.print(volts);
  Serial.print("\tAvg\t"); Serial.print(avg);
  Serial.print("\tDb:\t"); Serial.print(db);
  Serial.print("\tFLAG:\t"); Serial.print(flag);
  Serial.print("\tLEVEL:\t"); Serial.println(gain);


  u8g.firstPage();
  do {
    draw_Value(db, avg, flag);
  } while ( u8g.nextPage() );


  if (nread % nsample == 0 ) {
    avg = sum / nsample;
    flag = 0;
    
    if ((avg > gain + 1 ) && (millis() - volumeMillis > 2000) && db > 51 && db < 64 && stepDown < 5) {
      irsend.sendRaw(S_vdown, 68, 38);
      Serial.println("VOLUME DOWN");
      volumeMillis = millis();
      flag = 1;
      stepDown++;
    }
    if ((avg < gain - 1) && (millis() - volumeMillis > 2000) && db > 51 && db < 64) {
      irsend.sendRaw(S_vup, 68, 38);
      Serial.println("VOLUME UP");
      volumeMillis = millis();
      flag = 2;
      stepDown = 0;
    }
    sum = 0;
    nread = 0;
  }

  buttonState = digitalRead(buttonPin);
  if (buttonState == HIGH) {
    // pushBottom is pressed
    gain = avg;
    Serial.print("NEW LEVEL\t");
    Serial.println(gain);
  }

}


void draw_Value(float value, float avgValue, int f) {
  int offset = 20;
  u8g.setFont(u8g_font_unifont);

  if (f == 1) {
    u8g.drawStr( 70, 63, "DOWN");
  }
  if (f == 2) {
    u8g.drawStr( 70, 45, "UP");
  }
  
  if (f == 0) {
    u8g.drawStr( 70, 63, "     ");
    u8g.drawStr( 70, 45, "     ");
    }

  u8g.setFont(u8g_font_unifont);
  u8g.drawStr( 44, 27, "db");
  u8g.drawLine(44, 48, 58, 48);
  u8g.drawStr( 44, 63, "db");
  u8g.drawStr( 108, 27, "lv");
  
  u8g.setFont(u8g_font_freedoomr25n);
  u8g.setPrintPos(1, 27);  u8g.print(int(value));
  u8g.setPrintPos(68,27);  u8g.print(int(gain));
  
  u8g.setPrintPos(1, 63);  u8g.print(int(avgValue));
}

