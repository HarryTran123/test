#include <SoftwareSerial.h>
#include <RTClib.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>

#define TIMEOUT 5000 // mS
#define LED 12 //pin 12 for LED light
#define BUZZER 11// pin 11 for the BUzzer

String server = "172.20.10.5"; // np. www.examble.pl albo adres ip 129.75.1.32
String ssid = "MillerD";
String pwd = "Dung310105";


const int TXPin = 3;
const int RXPin = 2;

const int DHTPIN = 8;
const int DHTTYPE = DHT11;

byte degree[8] = {
  0B01110,
  0B01010,
  0B01110,
  0B00000,
  0B00000,
  0B00000,
  0B00000,
  0B00000
};

char daysOfTheWeek[7][12] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};

DHT dht(DHTPIN, DHTTYPE);
SoftwareSerial esp8266(3, 2); // TX, RX
DS1302 rtc(5, 7, 6);
LiquidCrystal_I2C lcd(0x27,16,2);


void setup()
{
    pinMode(LED, OUTPUT); //init the pin for the led light
    pinMode(BUZZER, OUTPUT); //init the pin for the BUZZER

    
    Serial.begin(9600);
    esp8266.begin(9600); //start the serial for the esp8266 module.
    WifiInit(); // Run the WifiInit() function.
    
    lcd.begin();  //Enable the lcd module 
    lcd.backlight();
    
    rtc.begin(); //Enable the RTC (real-time-clock) module
    if (! rtc.isrunning()) 
    {
      lcd.print("Couldn't find RTC");
    }
    
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));//auto update from computer time
   
    dht.begin();  //Enable the DHT module
}
 
void loop()
{
    float h = dht.readHumidity(); //Get the Humidity float number from the DHT module
    float t = dht.readTemperature(); //Get the Temperature float number from the DHT module
    DateTime now = rtc.now(); //Get the current time
    
    lcd.setCursor(0, 1);
    if(now.hour()<=9) //Print the current hour to the LCD
    {
      lcd.print("0");
      lcd.print(now.hour());
    }
    else {
     lcd.print(now.hour()); 
    }
    
    lcd.print(':');
    
    if(now.minute()<=9) //Print the current minute to the LCD
    {
      lcd.print("0");
      lcd.print(now.minute());
    }
    else {
     lcd.print(now.minute()); 
    }
    
    lcd.print(':');
    
    if(now.second()<=9) //Print the current seccond to the LCD
    {
      lcd.print("0");
      lcd.print(now.second());
    }
    else {
     lcd.print(now.second()); 
    }
    
    lcd.print(" ");

    lcd.setCursor(0, 0);
    if(now.day()<=9) //Print the current day to the LCD
    {
      lcd.print("0");
      lcd.print(now.day());
    }
    else {
     lcd.print(now.day()); 
    }
    
    lcd.print('/');
    
    if(now.month()<=9) //Print the current month to the LCD
    {
      lcd.print("0");
      lcd.print(now.month());
    }
    else {
     lcd.print(now.month()); 
    }
    
    lcd.print('/');
    
    if(now.year()<=9) //Print the current year to the LCD
    {
      lcd.print("0");
      lcd.print(now.year());
    }
    else {
     lcd.print(now.year()); 
    }

    if (isnan(t) || isnan(h)) // check if the value t or h is available or not - if it is not available the system will do nothing - if the variable is available the system will print out the information to the LCD
    {
      
    } else
    {
     lcd.setCursor(10,1);
     lcd.print("T:");
     lcd.print(round(t));
     lcd.write((char)223);
     lcd.print("C");
  
     lcd.setCursor(11,0);
     lcd.print("H:");
     lcd.print(round(h));
     lcd.print("%");
    }

 if(round(h) > 80)
 {
    digitalWrite(LED, HIGH);
    delay(100);
    digitalWrite(LED, LOW);
    delay(100);
    digitalWrite(BUZZER, HIGH);
    delay(100);
 } else
 {
    digitalWrite(BUZZER, LOW);
    delay(100);
 }


  String cmd;
  cmd = "AT+CIPSTART=\"TCP\",\"";
  cmd += server;
  cmd += "\",80";
  esp8266.println(cmd);

  Serial.println(cmd);
  if(esp8266.find("OK"))
  {
    Serial.println("Connected to the web!");
  }
 
  cmd =  "GET /update?temp=" + String(t, 1) + "&humd=" + String(h, 1) + " HTTP/1.1\r\n"; 
  cmd += "Host: "+ server +"\r\n\r\n";
  esp8266.print("AT+CIPSEND=");
  esp8266.println(cmd.length()); 

  
   Serial.println(cmd);

  if(esp8266.find(">"))
  {
    Serial.println(">");
  }
 
  esp8266.println(cmd);
 
  if(esp8266.find("OK"))
  {
    Serial.println("send!");
  }

  
}




//Send the command to the esp8266 module
boolean SendCommand(String cmd, String ack)
{
   esp8266.println(cmd); // Send "AT+" command to module
   if (!echoFind(ack)) // timed out waiting for ack string
   {
    return true; // ack blank or ack found
   }
}
 
boolean echoFind(String keyword)
{
   byte current_char = 0;
   byte keyword_length = keyword.length();
   long deadline = millis() + TIMEOUT;
   while(millis() < deadline)
   {
      if (esp8266.available())
      {
          char ch = esp8266.read();
          Serial.write(ch);
          if (ch == keyword[current_char])
            if (++current_char == keyword_length)
            {
               Serial.println();
               return true;
            }
       }
    }
   return false; // Timed out
}

void WifiInit()
{
   SendCommand("AT+RST \r\n", "Ready");
   delay(3000);
   SendCommand("AT+CWQAP\r\n", "OK");
   SendCommand("AT+CWJAP=\""+ ssid +"\",\"" + pwd + "\"\r\n","OK"); // SSID + PASSWORD
   SendCommand("AT+CWMODE=1\r\n","OK");
   SendCommand("AT+CIFSR\r\n", "OK");

//   SendCommand("AT+CIPMUX=1","OK");
//   SendCommand("AT+CIPSERVER=1,80","OK");
   
}
