/*
This code is based on 
-http://usemodj.com/2016/03/21/esp8266-display-2-8-inch-touch-lcd-with-wemos-d1-mini-board/    and
-http://teknolojiprojeleri.com/arduino/esp8266-modulu-ile-gmailden-mesaj-gonderme 
Big thanks to @usemodj and @teknolojiprojeleri.com
                                                                                 15.10.2017 @rajaz
                                                                              rajaz@protonmail.com
      

 Required libraries:
  - XPT2046: https://github.com/spapadim/XPT2046
  - UTFT-ESP8266: https://github.com/gnulabis/UTFT-ESP8266

 Modify: XPT2046.cpp
 void XPT2046::begin(uint16_t width, uint16_t height) { 
   ...
   //SPI.begin(); //comment out
   ...
 }  

 Arduino ide version: 1.6.5
 Wemos D1-R2 installation: http://www.instructables.com/id/Programming-the-WeMos-Using-Arduino-SoftwareIDE/ 

 driver for mac: CH34x_Install_V1.3.pkg https://drive.google.com/file/d/0BwVeB5VOPU4RY2xVdUU3Tmx5Rmc/view
      
 

 UPDATE 16.03.2018: 1.publish/subscribe functionality is added, mail methods are removed.
                      based on: - https://sonyarouje.com/2016/03/15/mqtt-communication-with-arduino-using-esp8266-esp-01/ @Sony Arouje
                                - default PubSubClient example named mqtt_esp8266
                      extra requirements: - PubSubClient.h: https://github.com/knolleary/pubsubclient
                                          - ESP8266WiFi.h: default wifi package in ESP8266 installation.(http://arduino.esp8266.com/stable/package_esp8266com_index.json)
                                        
 UPDATE 27.06.2018: 1.display and touch are rotated 90 as PORTRAIT mode. adjustments to header and buttons are made.
                    2.locker circle is removed. two new images, green and red locker, are added.(made by MSpaint, converted by http://www.rinkydinkelectronics.com/_t_doimageconverter565.php )
                      then line "#else #include <pgmspace.h>" is added to produced files "closed.c" and "open.c" to handle attribute errors on ESP8266.
                      then lines "extern unsigned short openLock[];" and "extern unsigned short closedLock[];" are added to main ino file. 
                      the skecth to continue the flow in the end of loop() is added to makeGreen() and related corrections are done.                     
                      based on: -https://www.youtube.com/watch?v=rw1nzJSd8Q0 @Andr.oid Eric
                    3.pin0 is declared as OUTPUT to manage electronic-locker.
                    4.clear button fucntionality is changed to delete last digit.             
 
 UPDATE 12.08.2018: passwords are changed to 6 dgits from 8 digits.                  
 UPDATE 25.05.2020: 1. to rotate 180, change this line LCD_Write_DATA(0x48): to LCD_Write_DATA(0x48 ^ 0xC0); in the file arduino\libraries\ESP8266_UTFT-master\tft_drivers\ili9341\s5p\initlcd.h 
                    2. openLock ve closedLock types changed to int from short due to variable type error.
*/

#include <Arduino.h>
#include <ESP8266WiFi.h>

#include <UTFT.h>
#include <SPI.h>
#include <XPT2046.h>
#include <PubSubClient.h>

extern unsigned int openLock[];                            //declare green lock image
extern unsigned int closedLock[];                          //declare red lock image
extern unsigned short length;
// Update these with values suitable for your network.


#pragma region Globals
const char* ssid = "necmettin";                           // WIFI network name
const char* password = "22600erbakan";                       // WIFI network password
//const char* mqtt_server = "45.77.114.70";         //local ip of mqtt broker or cloud adress
const char* mqtt_server = "192.168.1.27";
const char* mqttUser = "Rajaz";
const char* mqttPass = "22600vultr";


uint8_t connection_state = 0;                    // Connected to WIFI or not
uint16_t reconnect_interval = 8000;             // If not connected wait time to try again
#pragma endregion Globals


#define ESP_SPI_FREQ 4000000

// Modify the following two lines to match your hardware
// Also, update calibration parameters below, as necessary

// Modify the line below to match your display and wiring:
//UTFT(byte model, int CS, int RST, int SER=0);
UTFT myGLCD ( ILI9341_S5P, 15, 5, 2);

XPT2046 myTouch(/*cs=*/ 4, /*irq=*/ 5);

// Declare which fonts we will be using
extern uint8_t BigFont[];
extern uint8_t SmallFont[];

//MODES
enum MODES 
{
  CARRIER,
  CLIENT,
  //IDLEBOX,
};



//definitions
char stCurrent[7]="";
int stCurrentLen=0;
char stLast[7]="";
char carrierKey[7]="";
char clientKey[7]="";
int dropped=0;
int checked=0;
char oldPayload[]="";
WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
long past=0;
char msg[25];
char temp1[2]="";
char temp2[2]="";
int value = 0;
char cid[11]="";
int MODE=CARRIER;
int MODCOLOR1=VGA_OLIVE, MODCOLOR2=VGA_YELLOW, MODFONTCOLOR1=VGA_GRAY, MODFONTCOLOR2=VGA_BLACK;
int LOCKER=0;   //0 for closed, 1 for open, 2 for disable touch
int count=0;  //count for locker on/off

/*************************
**   Custom functions   **
*************************/

//connect wifi
uint8_t WiFiConnect(const char* nSSID = nullptr, const char* nPassword = nullptr)  
{
    myGLCD.setColor(255, 105, 0);
    myGLCD.print("WELCOME TO KARGOLUK :)", CENTER, 20);
    static uint16_t attempt = 0;
    Serial.print("Connecting to ");
    myGLCD.setColor(255, 255, 255);
    myGLCD.print("Connecting to ", 10, 60);
    myGLCD.setColor(0, 50, 174);
    if(nSSID) {
        WiFi.begin(nSSID, nPassword);  
        Serial.println(nSSID);
        myGLCD.print(nSSID, 123, 60);
    } else {
        WiFi.begin(ssid, password);
        Serial.println(ssid);
        myGLCD.print(ssid, 123, 60);
    }

    uint8_t i = 0;
    while(WiFi.status()!= WL_CONNECTED && i++ < 44)
    {
        delay(200);
        Serial.print(".");
        myGLCD.print("> ", 10+5*i, 90);
    }
    ++attempt;
    Serial.println("");
    if(i == 45) {
        Serial.print("Connection: TIMEOUT on attempt: ");
        Serial.println(attempt);
        myGLCD.setColor(255, 255, 255);
        myGLCD.print("Connection: ", 10, 130);
        myGLCD.setColor(216, 30, 53);
        myGLCD.print("TIMEOUT", 105, 130);
        myGLCD.print("on attempt: ", 105, 145);
        char str[10];
        sprintf(str, "%d", attempt);
        myGLCD.setColor(0, 0, 0);
        myGLCD.fillRect (201, 145, 240, 155); //ekrandaki attempt sayısını sil
        myGLCD.setColor(216, 30, 53);
        myGLCD.print(str, 201, 145);
        myGLCD.setColor(0, 0, 0);
        myGLCD.fillRect (10, 90, 300, 105); // ekrandaki noktaları sil
        myGLCD.setColor(255, 255, 255);
        if(attempt % 2 == 0) {
            Serial.println("Check if access point is available or wrong SSID and Password\r\n");
            myGLCD.print("Check if access point is", 10, 170);
            myGLCD.print("available", 10, 190);
            myGLCD.print("or wrong SSID and Password", 10, 210);
        }
        return false;
    }
    Serial.println("Connection: ESTABLISHED");
    myGLCD.setColor(0, 0, 0);
    myGLCD.fillRect (105, 130, 240, 160); // delete timeout
    myGLCD.fillRect (10, 170, 240, 320);  // delete warnings
    myGLCD.setColor(255, 255, 255);
    myGLCD.print("Connection: ", 10, 130);
    myGLCD.setColor(18, 173, 42);
    myGLCD.print("ESTABLISHED", 105, 130);
    Serial.print("Got IP address: ");
    Serial.println(WiFi.localIP());
    delay(1000);
    return true;
}

//try to connect wifi again
void Awaits()
{
    uint32_t ts = millis();
    while(!connection_state)
    {
        
        delay(50);
        if(millis() > (ts + reconnect_interval) && !connection_state){
            connection_state = WiFiConnect();
            ts = millis();
        }
    }
}

// reconnect to mqtt broker
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    int randNumber = random(100000,999999);
    sprintf(cid,"ESP%d",randNumber);
    if (client.connect(cid)) {                                          //create random id for esp client. authentification can be added here
      Serial.println("connected");
      // Once connected, publish an announcement...
     /* sprintf(temp1,"%d",dropped);
      sprintf(temp2,"%d",checked);
      client.publish("dropstatus", temp1);
      client.publish("checkstatus", temp2);*/
      // ... and resubscribe
      client.subscribe("newdelivery");
      delay(5000);                                                    //added to chill ESP after subscription
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

//initial callback:saving passwords
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");

  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '0') {
    //Serial.print("first part of payload : ");
    for (int i = 0; i < 6; i++) {
      //Serial.print((char)payload[i+1]);
      sprintf(carrierKey+i,"%c",(char)payload[1+i]);
    }
   
                                                 
  } 
  if ((char)payload[7] == '1') {
    //Serial.print(" second part of payload : ");
    for (int i = 0; i < 6; i++) {
      //Serial.print((char)payload[i+10]);
      sprintf(clientKey+i,"%c",(char)payload[8+i]);
    }
   
    
  }
  else {
    
      strcpy(carrierKey, "");
      strcpy(clientKey, "");
      
      
      Serial.print("client key and/or carrier key are invalid.");
    
  }
  Serial.print(" carrier key is : ");
  Serial.print(carrierKey);
  Serial.print(" client key is : ");
  Serial.print(clientKey);
      
  Serial.println();
  sprintf(oldPayload,"%c",(char)payload[0]);                   //save the message arrived
}

/*
//what to do when receive a new message:saving passwords                  //deneme callbacki
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  
  if(strcmp((char*)payload, oldPayload)!=0){
    dropped=0;
    checked=0;
    sprintf(temp1,"%d",dropped);
    client.publish("dropstatus:", temp1);
    sprintf(temp2,"%d",checked);
    client.publish("checkstatus:", temp2);
    
    // Switch on the LED if an 1 was received as first character
    if ((char)payload[0] == '0') {
      //Serial.print("first part of payload : ");
      for (int i = 0; i < 6; i++) {
        //Serial.print((char)payload[i+1]);
        sprintf(carrierKey+i,"%c",(char)payload[1+i]);
      }
     
                                                   
    } 
    if ((char)payload[7] == '1') {
      //Serial.print(" second part of payload : ");
      for (int i = 0; i < 6; i++) {
        //Serial.print((char)payload[i+10]);
        sprintf(clientKey+i,"%c",(char)payload[10+i]);
      }
     
      
    }
    else {
      
        strcpy(carrierKey, "");
        strcpy(clientKey, "");
        
        
        Serial.print("client key and/or carrier key are invalid.");
      
    }
    Serial.print(" carrier key is : ");
    Serial.print(carrierKey);
    Serial.print(" client key is : ");
    Serial.print(clientKey);
        
    Serial.println();

    //deneme
    sprintf(oldPayload,"%c",(char)payload[0]);
    //count++;
    LOCKER==0;
    digitalWrite(16, LOW);
    //switchModepage();
    //delay(100);
    //myGLCD.clrScr();
    //drawButtons();
    //touchFunc();    
    
  }
}*/

void swapValues(int *a, int *b)
  {
   int temp;
 
   temp = *b;
   *b   = *a;
   *a   = temp;   
  }


void drawButtons()
{
  int x, y;
  
//draw mode headers
  myGLCD.setBackColor (VGA_TRANSPARENT);
  myGLCD.setColor(MODCOLOR2);
  myGLCD.fillRoundRect (8, 10, 120, 40);
  myGLCD.setColor(MODFONTCOLOR2);
  myGLCD.setFont(SmallFont);
  myGLCD.print("C A R R I E R", 13, 19);
  myGLCD.setColor(MODCOLOR1);
  myGLCD.fillRoundRect (125, 10, 230, 40);
  myGLCD.setColor(MODFONTCOLOR1);
  myGLCD.print("C L I E N T", 135, 19);
  
  
  myGLCD.setColor(255, 255, 255);
  myGLCD.print("Please enter the key from", 10, 50);
  myGLCD.print("your notification mail:", 10, 60);
  myGLCD.setFont(BigFont);
  
// Draw the first row of buttons
  for (x=0; x<3; x++)
  {
    myGLCD.setColor(0, 0, 255);
    myGLCD.fillRoundRect (10+(x*77), 120, 77+(x*77), 160);
    myGLCD.setColor(255, 255, 255);
    myGLCD.drawRoundRect (10+(x*77), 120, 77+(x*77), 160);
    myGLCD.printNumI(x+1, 35+(x*77), 132);
  }

// Draw the second row of buttons
  for (x=0; x<3; x++)
  {
    myGLCD.setColor(0, 0, 255);
    myGLCD.fillRoundRect (10+(x*77), 170, 77+(x*77), 210);
    myGLCD.setColor(255, 255, 255);
    myGLCD.drawRoundRect (10+(x*77), 170, 77+(x*77), 210);
    myGLCD.printNumI(x+4, 35+(x*77), 182);
  }

  // Draw the third row of buttons
  for (x=0; x<3; x++)
  {
    myGLCD.setColor(0, 0, 255);
    myGLCD.fillRoundRect (10+(x*77), 220, 77+(x*77), 260);
    myGLCD.setColor(255, 255, 255);
    myGLCD.drawRoundRect (10+(x*77), 220, 77+(x*77), 260);
    myGLCD.printNumI(x+7, 35+(x*77), 232);
  }

  // Draw the fourth row of buttons
  for (x=0; x<3; x++)
  {
    myGLCD.setColor(0, 0, 255);
    myGLCD.fillRoundRect (10+(x*77), 270, 77+(x*77), 310);
    myGLCD.setColor(255, 255, 255);
    myGLCD.drawRoundRect (10+(x*77), 270, 77+(x*77), 310);
    myGLCD.print("0", 112, 282);
    myGLCD.setFont(SmallFont);
    myGLCD.print("DELETE", 22, 284);
    myGLCD.print("ENTER", 180, 284);
    myGLCD.setFont(BigFont);
  }

  myGLCD.drawBitmap(195,75, 35, 40, closedLock);
  myGLCD.setBackColor (0, 0, 0);
}

void drawIdle()
{
  
  myGLCD.clrScr();
  myGLCD.setColor(255, 255, 255);
  myGLCD.print("BOX IS IDLE", CENTER, 120);
  
}

void touchFunc(){
  uint16_t x, y;
  
  if (LOCKER==0 && myTouch.isTouching())
  {
    myTouch.getPosition(x, y);
    //Serial.println("Touching... x: "+ String(x) + ", y: " + String(y));

    if ((y>=120) && (y<=160))  // first row of numbers
    {
      if ((x>=10) && (x<=77))  // Button: 1
      {
        waitForIt(10, 120, 77, 160);
        updateStr('1');
      }
      if ((x>=87) && (x<=154))  // Button: 2
      {
        waitForIt(87, 120, 154, 160);
        updateStr('2');
      }
      if ((x>=164) && (x<=231))  // Button: 3
      {
        waitForIt(164, 120, 231, 160);
        updateStr('3');
      }
      
    }

    if ((y>=170) && (y<=210))  // second row of numbers
    {
      if ((x>=10) && (x<=77))  // Button: 4
      {
        waitForIt(10, 170, 77, 210);
        updateStr('4');
      }
      if ((x>=87) && (x<=154))  // Button: 5
      {
        waitForIt(87, 170, 154, 210);
        updateStr('5');
      }
      if ((x>=164) && (x<=231))  // Button: 6
      {
        waitForIt(164, 170, 231, 210);
        updateStr('6');
      }
    }

    if ((y>=220) && (y<=260))  // third row of numbers
    {
      if ((x>=10) && (x<=77))  // Button: 7
      {
        waitForIt(10, 220, 77, 260);
        updateStr('7');
      }
      if ((x>=87) && (x<=154))  // Button: 8
      {
        waitForIt(87, 220, 154, 260);
        updateStr('8');
      }
      if ((x>=164) && (x<=231))  // Button: 9
      {
        waitForIt(164, 220, 231, 260);
        updateStr('9');
      }
    }
    

    if ((y>=270) && (y<=310))  // bottom row
    {
      if ((x>=10) && (x<=77))  // Button: Delete
      {
        waitForIt(10, 270, 77, 310);
        stCurrent[stCurrentLen-1]='\0';
        stCurrentLen--;
        myGLCD.setColor(0, 0, 0);
        myGLCD.fillRect(0, 72, 195, 112);
        myGLCD.setColor(204, 204, 204);
        myGLCD.print(stCurrent, 10, 80);
      }
      if ((x>=87) && (x<=154))  // Button: 0
      {
        waitForIt(87, 270, 154, 310);
        updateStr('0');
      }
      if ((x>=164) && (x<=231))  // Button: Enter
      {
        waitForIt(164, 270, 231, 310);
        if (stCurrentLen<6)
        {
          myGLCD.setColor(255, 0, 0);
          myGLCD.setFont(SmallFont);
          myGLCD.print("LESS THAN 6 DIGITS!", 10, 82);
          delay(500);
          myGLCD.print("                   ", 10, 82);
          delay(500);
          myGLCD.print("LESS THAN 6 DIGITS!", 10, 82);
          delay(500);
          myGLCD.print("                   ", 10, 82);
          myGLCD.setFont(BigFont);
          myGLCD.setColor(204, 204, 204);
          myGLCD.print(stCurrent, 10, 80);
        }
        if (stCurrentLen==6)
        {
          for (int a=0; a<stCurrentLen+1; a++)
          {
            stLast[a]=stCurrent[a];
          }
          stCurrent[0]='\0';
          stCurrentLen=0;
          
          if(MODE==CARRIER){
          if (strcmp(stLast, carrierKey)==0)
          {
            myGLCD.print("          ", 10, 80);
            myGLCD.setColor(0, 255, 0);
            myGLCD.print(stLast, 50, 80);
            delay(200);
            myGLCD.print("        ", 50, 80);
            delay(200);
            myGLCD.print(stLast, 50, 80);
            delay(200);
            myGLCD.print("        ", 50, 80);
            delay(200);
            myGLCD.print(stLast, 50, 80);
            stLast[0]='\0';
            makeGreen();
            
          }
          else {            
            myGLCD.print("           ", 10, 80);
            myGLCD.setColor(255, 0, 0);
            myGLCD.setFont(SmallFont);
            myGLCD.print("WRONG KEY!", 10, 82);
            delay(500);
            myGLCD.print("          ", 10, 82);
            delay(500);
            myGLCD.print("WRONG KEY!", 10, 82);
            delay(500);
            myGLCD.print("          ", 10, 82);
            myGLCD.setColor(204, 204, 204);
            myGLCD.setFont(BigFont);
                     
          }
         }
         if(MODE==CLIENT){
          if (strcmp(stLast, clientKey)==0)
          {
            myGLCD.print("          ", 10, 80);
            myGLCD.setColor(0, 255, 0);
            myGLCD.print(stLast, 50, 80);
            delay(200);
            myGLCD.print("        ", 50, 80);
            delay(200);
            myGLCD.print(stLast, 50, 80);
            delay(200);
            myGLCD.print("        ", 50, 80);
            delay(200);
            myGLCD.print(stLast, 50, 80);
            stLast[0]='\0';
            makeGreen();
            
          }
          else {
            
            myGLCD.print("            ", 10, 80);
            myGLCD.setColor(255, 0, 0);
            myGLCD.setFont(SmallFont);
            myGLCD.print("WRONG KEY!", 10, 82);
            delay(500);
            myGLCD.print("          ", 10, 82);
            delay(500);
            myGLCD.print("WRONG KEY!", 10, 82);
            delay(500);
            myGLCD.print("          ", 10, 82);
            myGLCD.setColor(204, 204, 204);
            myGLCD.setFont(BigFont);
                     
          }
         }  
        }
        
      }
    }
  }
}


//make locker image green
void makeGreen()
{
  LOCKER=1;
  digitalWrite(0, HIGH);                                                 // turn the LOCK on (HIGH is the voltage level)
  myGLCD.setColor(0, 0, 0);
  for(int i=0;i<3;i++){
    myGLCD.fillRect(195, 75, 230, 115);
    myGLCD.drawBitmap(195,75, 35, 40, openLock);
    delay(1000);
    myGLCD.fillRect(195, 75, 230, 115);
    delay(1000);
  }
  myGLCD.fillRect(195, 75, 230, 115);
  myGLCD.drawBitmap(195,75, 35, 40, closedLock);
  delay(100);
  
  if(count%2==0){                                                    
    switchModepage();
    dropped=1;
    LOCKER=0;
    digitalWrite(0, LOW);                    
  }
  if(count%2==1) {
    checked=1;
    drawIdle();
    LOCKER=2;
  }
  count++; 
  
}
  




//switch colors of the header
void switchModepage()
{
  myGLCD.clrScr();
  swapValues(&MODCOLOR1, &MODCOLOR2);
  swapValues(&MODFONTCOLOR1, &MODFONTCOLOR2);
  drawButtons();
}

void updateStr(int val)
{
  if (stCurrentLen<6)
  {
    stCurrent[stCurrentLen]=val;
    stCurrent[stCurrentLen+1]='\0';
    stCurrentLen++;
    myGLCD.setColor(204, 204, 204);
    myGLCD.print(stCurrent, 10, 80);
  }
  else
  {
    myGLCD.setColor(255, 0, 0);
    myGLCD.setFont(SmallFont);
    myGLCD.print("MORE THAN 6 DIGITS!", 10, 82);
    delay(500);
    myGLCD.print("                   ", 10, 82);
    delay(500);
    myGLCD.print("MORE THAN 6 DIGITS!", 10, 82);
    delay(500);
    myGLCD.print("                   ", 10, 82);
    myGLCD.setColor(0, 255, 0);
    myGLCD.setFont(BigFont);
    stCurrent[0]='\0';
    stCurrentLen=0;
    
  }
  
}

// Draw a red frame while a button is touched
void waitForIt(int x1, int y1, int x2, int y2)
{
  myGLCD.setColor(255, 0, 0);
  myGLCD.drawRoundRect (x1, y1, x2, y2);
  while (myTouch.isTouching()) yield(); //loop
  myGLCD.setColor(255, 255, 255);
  myGLCD.drawRoundRect (x1, y1, x2, y2);
}

/*************************
**  Required functions  **
*************************/

void setup()
{
  delay(1000);

  Serial.begin(115200);

  
  SPI.setFrequency(ESP_SPI_FREQ);

// Initialize the BUILTIN_LED and pin0 as an output
  pinMode(0, OUTPUT);
  pinMode(BUILTIN_LED, OUTPUT);     

  
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  
 
  
  


// Initial setup
  myGLCD.InitLCD(PORTRAIT); //LANDSCAPE, PORTRAIT
  myGLCD.clrScr();
  Serial.print("tftx ="); Serial.print(myGLCD.getDisplayXSize()); Serial.print(" tfty ="); Serial.println(myGLCD.getDisplayYSize());
//  begin(uint16_t width, uint16_t height);  // width and height with no rotation!
  myTouch.begin(240, 320);  // Must be done before setting rotation
  //myTouch.begin((uint16_t)myGLCD.getDisplayYSize(), (uint16_t)myGLCD.getDisplayXSize());  // Must be done before setting rotation
  // Replace these for your screen module
  //touch.setCalibration(209, 1759, 1775, 273);
  myTouch.setCalibration(224, 1856, 1872, 255);
  myTouch.setRotation(myTouch.ROT0);

  
  myGLCD.setFont(SmallFont);
  myGLCD.setBackColor(VGA_TRANSPARENT);
  myGLCD.setColor(255, 255, 255);

  connection_state = WiFiConnect();
  if(!connection_state)  // if not connected to WIFI
  Awaits();          // constantly trying to connect

  
  myGLCD.setFont(BigFont);
  myGLCD.setBackColor(0, 0, 255);
  myGLCD.setColor(255, 255, 255);
  
  myGLCD.clrScr();
  drawButtons();
  
}

void loop()
{
  long current= millis();
  if(current - past>6000){
    //run  reconnect function to mqtt
    if (!client.connected()) {
      reconnect();
    }
   
    client.loop();
    past= current;
    delay(100);
  }

  long now = millis();
  if (now - lastMsg > 10000) {
    lastMsg = now;
    ++value;
    snprintf (msg, 25, "drop status: %d #%ld", dropped, value);
    Serial.print("Publish ");
    Serial.println(msg);
    sprintf(temp1,"%d",dropped);
    client.publish("dropstatus:", temp1);

    snprintf (msg, 25, "check status: %d #%ld", checked, value);
    Serial.print("Publish ");
    Serial.println(msg);
    sprintf(temp2,"%d",checked);
    client.publish("checkstatus:", temp2);
    
  }



  //sketch for switching MODE 
  if(count%2==0) {
    MODE=CARRIER;
  }
  if(count%2==1){
    MODE=CLIENT;
  }

  /*if(count%3==2){
    client.setCallback(callback); //%3 yap digerlerini deneme callbacki için
  }*/
  
  touchFunc();
  yield();
}
