#include <DHT.h> 
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

String apiKey = "9AMIF9AVT69KQCXN";                             // API key from ThingSpeak
char blynk_auth[] = "a3NExXVklbOc5g1x52HoDAVV-jyymjyC";         // Blynk_Auth_Token

const char *ssid =  "JioFi2_CCFB01";                            // replace with your wifi ssid and wpa2 key
const char *pass =  "ysi67zmi9k";
const char* server = "api.thingspeak.com";

#define DHTPIN D3                                               //pin where the dht11 is connected
#define mq135 A0                                                //pin where MQ135 is connected

int a=0;

WidgetLCD lcd(V1);
WidgetTerminal terminal(V4);
DHT dht(DHTPIN, DHT11);

WiFiClient client;

void setup() 
{
      Serial.begin(115200);
      delay(10);
      dht.begin();
      pinMode(mq135, INPUT);
      Serial.println("");
      Serial.println("Connecting to ");
      Serial.println(ssid);
 
      WiFi.begin(ssid, pass);
 
      while (WiFi.status() != WL_CONNECTED) 
      {
            delay(500);
            Serial.print(".");
      }
      
      Serial.println("");
      Serial.println("WiFi connected");
      Serial.println("");
      Serial.print("Use This IP:");
      Serial.println(WiFi.localIP());

      Blynk.begin(blynk_auth,ssid,pass);
      Blynk.notify("NODEMCU Online");
      Blynk.email("kbandooni1@gmail.com", "NODEMCU Online");                                                      
      
      //blynk lcd
      lcd.clear();
}

void loop() 
{
      Blynk.run();                                                              // Blynk magic 
      float m = analogRead(mq135);
      float h = dht.readHumidity();
      float t = dht.readTemperature();
      float x=m/1023*100;
      
      if (isnan(h) || isnan(t)) 
      {
           Serial.println("Failed to read from the DHT11 sensor!");
           return;
      }
      if (isnan(m)) 
      {
           Serial.println("Failed to read from the MQ135 sensor!");
           return;
      }

      //Sending Value to Gauge in Blynk
      Blynk.virtualWrite(V3,x);
      Blynk.virtualWrite(V0,h);
      Blynk.virtualWrite(V2,t);
      
      Serial.print("Temperature: ");
      Serial.print(t);
      Serial.print("degrees Celcius\nHumidity: ");
      Serial.print(h);
      Serial.print("%\nAir Quality = ");
      Serial.print(x);
      Serial.print(" *PPM\n");
      Serial.println("Waiting...");

      terminal.println("");
      terminal.print("Temperature: ");
      terminal.print(t);
      terminal.println("degree Celcius");
      terminal.print("Humidity: ");
      terminal.print(h);
      terminal.println("%");
      terminal.print("Air Quality = ");
      terminal.print(x);
      terminal.println(" *PPM");
      
      if (client.connect(server,80))                                             //   "184.106.153.149" or api.thingspeak.com
      {  
            String postStr = apiKey;
            postStr +="&field1=";
            postStr += String(t);
            postStr +="&field2=";
            postStr += String(h);
            postStr += "\r\n\r\n";
            client.print("POST /update HTTP/1.1\n");
            client.print("Host: api.thingspeak.com\n");
            client.print("Connection: close\n");
            client.print("X-THINGSPEAKAPIKEY: "+apiKey+"\n");
            client.print("Content-Type: application/x-www-form-urlencoded\n");
            client.print("Content-Length: ");
            client.print(postStr.length());
            client.print("\n\n");
            client.print(postStr);
            Serial.println("\nSent to Thingspeak.");
      }
      
      // blynk lcd, mobile and email notification 
      if(t<0&&a!=1){
        a=1;
        Blynk.notify("It's freezing, turn on the heater for some warmth ;)");                                                   //notification sent to your mobile
        Blynk.email("kbandooni1@gmail.com", "Message from NODEMCU :-\nIt's freezing, turn on the heater for some warmth ;)");   //email sent 
      }
      if(t>50&&a!=2){
        a=2;
        Blynk.notify("LMAO, Already half boiled, you might want to switch on Air Conditioning :') ");                                             //notification sent to your mobile
        Blynk.email("kbandooni1@gmail.com", "Message from NODEMCU :-\nLMAO, Already half boiled, you might want to switch on Air Conditioning."); //email sent 
      }
      if(h<20&&a!=3){
        a=3;
        Blynk.notify("A little bit of rain, or water being sprinkled might help.");                                                 //notification sent to your mobile
        Blynk.email("kbandooni1@gmail.com", "Message from NODEMCU :-\nA little bit of rain, or water being sprinkled might help."); //email sent 
      }
      
      if(a!=4&&x<=30){
        a=4;
        Blynk.notify("AIR QUALITY AT OPTIMAL CONDITION :)");
        lcd.clear();
        lcd.print(3,1,":D GOOD :D");
        Blynk.email("kbandooni1@gmail.com","Message from NODEMCU :-\nAIR QUALITY AT OPTIMAL CONDITION :)");
      }
      else if(x>30&&x<=60&&a!=5){
        a=5;
        lcd.clear();
        lcd.print(0,1,":)  SATISFACTORY");
        Blynk.notify("    AIR QUALITY SATISFACTORY! \nNOT TOO GOOD FOR ASTHMATIC PEOPLE!");                                        //notification sent to your mobile
        Blynk.email("kbandooni1@gmail.com", "Message from NODEMCU :-\nAIR QUALITY SATISFACTORY! \nNOT TOO GOOD FOR ASTHMATIC PEOPLE!");  //email sent 
      }
      else if(x>60&&x<=90&&a!=6){
        a=6;
        lcd.clear();
        lcd.print(1,1,":') MODERATE");
        Blynk.notify("    AIR QUALITY MODERATE! \nYOU MIGHT WANT TO LEAVE THE AREA!");                                              //notification sent to your mobile
        Blynk.email("kbandooni1@gmail.com", "Message from NODEMCU :-\nAIR QUALITY MODERATE! \nYOU MIGHT WANT TO LEAVE THE AREA!");  //email sent 
     }  
      else if(x>90&&x<=120&&a!=7){
        a=7;
        lcd.clear();
        lcd.print(3,1,":( POOR :(");
        Blynk.notify("    AIR QUALITY POOR!!! \nLEAVE THE AREA ASAP -_-");                                                          //notification sent to your mobile
        Blynk.email("kbandooni1@gmail.com", "Message from NODEMCU :-\nAIR QUALITY POOR!!! \nLEAVE THE AREA ASAP -_-");              //email sent 
     }
      else if(x>120&&x<=250&&a!=8){
        a=8;
        lcd.clear();
        lcd.print(1,1,":O VERY POOR :O");
        Blynk.notify("    AIR QUALITY VERY POOR!!!\nLEAVE THE AREA ASAP -_-");                                                      //notification sent to your mobile
        Blynk.email("kbandooni1@gmail.com", "Message from NODEMCU :-\nAIR QUALITY VERY POOR!!!\nLEAVE THE AREA ASAP -_-");          //email sent 
      }
      else if(x>250){
        lcd.clear();
        lcd.print(0,1,"SEVERE CONDITION");
        Blynk.notify("\tSEVERE CONDITION!!!\nLEAVING THIS PLANET MIGHT HELP -_-!!!");                                                //notification sent to your mobile
        Blynk.email("kbandooni1@gmail.com", "Message from NODEMCU :-\nSEVERE CONDITION!!!\nLEAVING THIS PLANET MIGHT HELP -_-!!!");  //email sent 
       }  
      lcd.print(3,0,"AIR QUALITY");
      delay(500);
      
      // thingspeak needs minimum 15 sec delay between updates, i've set it to 20 seconds
      // delay(20000);
}