#include <LiquidCrystal.h>
#include "DHT.h"
#include "ThingSpeak.h"
#include "WiFiEsp.h" 
 
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
char ssid[] = "unknown";    //  your network SSID (name) 
char pass[] = "ALE25011";   // your network password
int keyIndex = 0;            // your network key Index number (needed only for WEP)
unsigned long myChannelNumber = 1238594;
const char * myWriteAPIKey = "9TLQ2JFE7IXDH2WP";


#define SECRET_SSID "unknown"    // replace MySSID with your WiFi network name
#define SECRET_PASS "ALE25011"  // replace MyPassword with your WiFi password

#define SECRET_CH_ID 000000     // replace 0000000 with your channel number
#define SECRET_WRITE_APIKEY "XYZ"   // replace XYZ with your channel write API Key

WiFiEspClient  client;


#ifndef HAVE_HWSERIAL1
#include "SoftwareSerial.h"
SoftwareSerial Serial1(6, 7); // RX, TX
#define ESP_BAUDRATE  19200
#else 
#define ESP_BAUDRATE  115200
#endif


#define DHTPIN 8
#define DHTTYPE DHT22 

LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  
  Serial.begin(115200);  
  setEspBaudRate(ESP_BAUDRATE); 
  while (!Serial) {
    ; 
  } 
  Serial.print("Searching for ESP8266..."); 
  WiFi.init(&Serial1); 
  if (WiFi.status() == WL_NO_SHIELD) 
  {
    Serial.println("WiFi shield not present"); 
    while (true);
  }
  Serial.println("found it!"); 
  ThingSpeak.begin(client);  // Initialize ThingSpeak

  
  // put your setup code here, to run once:
  float ctemp, ftemp, humidity, fheat, cheat;
  int drain, arain, error = 0;
  Serial.begin(9600);
  
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  Serial.println(F("TEST TEST"));

  lcd.begin(16, 2);
  dht.begin();

  lcd.print("Temp  Humd  Rain"); 
}

void loop() {

  
  // put your main code here, to run repeatedly:
  float ctemp, ftemp, drain, arain, humidity, fheat, cheat;
  int error = 0;
  drain = analogRead(A0);
  arain = analogRead(A1); 
  humidity = dht.readHumidity();
  ctemp = dht.readTemperature();
  ftemp = dht.readTemperature(true);
  fheat = dht.computeHeatIndex(ftemp, humidity);
  cheat = dht.computeHeatIndex(ctemp, humidity, false);
  
  if (isnan(humidity) || isnan(ctemp) || isnan(ftemp)) {
    error = 1; 
  }

  
  
  lcd.setCursor(0, 1);
  lcd.print(ctemp);
  lcd.print(" ");
  lcd.print(humidity);
  if(arain > 900)
  {
    lcd.setCursor(12, 1);
    lcd.print("None");
  }  
  else if (arain > 500)
  {
    lcd.setCursor(12, 1);
    lcd.print("Low ");
  }  
  else if (arain > 300)
  {
    lcd.setCursor(12, 1);
    lcd.print("Mid ");  
  }  
  else 
  {
    lcd.setCursor(12, 1);
    lcd.print("high");
  }   


  
  if(WiFi.status() != WL_CONNECTED)
  {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(SECRET_SSID);
    while(WiFi.status() != WL_CONNECTED)
    {
      WiFi.begin(ssid, pass);  // Connect to WPA/WPA2 network. Change this line if using open or WEP network
      Serial.print(".");    
    } 
    Serial.println("\nConnected.");
  }
  
  if(millis() % 60000 < 1000)
  {
    int x = ThingSpeak.writeField(myChannelNumber, 1, ctemp, myWriteAPIKey);
    if(x == 200)
    { 
      Serial.println("Channel update successful.");
    }     
  else 
    Serial.println("Problem updating channel. HTTP error code " + String(x)); 
  }  
  else if(millis() % 40000 < 1000)
  {
    int y = ThingSpeak.writeField(myChannelNumber, 2, humidity, myWriteAPIKey);
    if( y == 200 )
    { 
      Serial.println("Channel update successful.");
    }     
    else 
      Serial.println("Problem updating channel. HTTP error code " + String(y)); 
  }  
  else if(millis() % 20000 < 1000)
  {
    int z = ThingSpeak.writeField(myChannelNumber, 3, arain, myWriteAPIKey);
    if( z == 200)
    { 
      Serial.println("Channel update successful.");
    }     
    else 
      Serial.println("Problem updating channel. HTTP error code " + String(z));

  }  
 

 /*   

  Serial.print(F("Humidity: "));
  Serial.print(humidity);
  Serial.print(F("%  Temperature: "));
  Serial.print(ctemp);
  Serial.print(F("°C "));
  Serial.print(ftemp);
  Serial.print(F("°F  Heat index: "));
  Serial.print(cheat);
  Serial.print(F("°C "));
  Serial.print(drain);
  Serial.print(F("  "));
  Serial.print(arain);
  Serial.print(F("  ")); 
  Serial.print(fheat);
  Serial.println(F("°F"));

  */

}


//setting wifi boudrate 
void setEspBaudRate(unsigned long baudrate)
{
  long rates[6] = {115200,74880,57600,38400,19200,9600}; 
  Serial.print("Setting ESP8266 baudrate to ");
  Serial.print(baudrate);
  Serial.println("..."); 
  for(int i = 0; i < 6; i++)
  {
    Serial1.begin(rates[i]);
    delay(100);
    Serial1.print("AT+UART_DEF=");
    Serial1.print(baudrate);
    Serial1.print(",8,1,0,0\r\n");
    delay(100);  
  } 
  Serial1.begin(baudrate);
}
