
#include <SoftwareSerial.h>
// IMPORT ALL REQUIRED LIBRARIES

#include <math.h>

#ifndef ARDUINOJSON_H
#include <ArduinoJson.h>
#endif

#ifndef STDLIB_H
#include <stdlib.h>
#endif

#ifndef STDIO_H
#include <stdio.h>
#endif

#ifndef ARDUINO_H
#include <Arduino.h>
#endif

//**********ENTER IP ADDRESS OF SERVER******************//

#define HOST_IP     "172.16.193.22"       // REPLACE WITH IP ADDRESS OF SERVER ( IP ADDRESS OF COMPUTER THE BACKEND IS RUNNING ON) 
#define HOST_PORT   "8080"            // REPLACE WITH SERVER PORT (BACKEND FLASK API PORT)
#define route       "api/update"      // LEAVE UNCHANGED 
#define idNumber    "620163595"       // REPLACE WITH YOUR ID NUMBER 

// WIFI CREDENTIALS
#define SSID        "MonaConnect" // Add your Wi-Fi ssid 
#define password    ""  // Add your Wi-Fi password 

#define stay        100
 
//**********PIN DEFINITIONS******************//

#define espRX         10
#define espTX         11
#define espTimeout_ms 300
#define trigPin 12    // Trigger
#define echoPin 13    // Echo
long duration, radar;
int cm = 0; 
int inches = 0;
int wh = 0;

/* Declare your functions below */
double calWH(double radar);
double calWRes(double wh);
double calPercent(double res);
double calGal(double percent);
 

SoftwareSerial esp(espRX, espTX); 
 

void setup(){

  Serial.begin(9600); //9600 / 115200
  // Configure GPIO pins here
  pinMode(espTX, OUTPUT);
  pinMode(espRX, INPUT_PULLUP);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT_PULLUP);
  espInit();  
}

void loop(){ 
   
  // send updates with schema ‘{"id": "student_id", "type": "ultrasonic", "radar": 0, "waterheight": 0, "reserve": 0, "percentage": 0}’
  //espUpdate(char mssg[]);

  digitalWrite(trigPin, LOW);
  delayMicroseconds(5);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
 
  pinMode(echoPin, INPUT);
  duration = pulseIn(echoPin, HIGH);
 
  cm = (duration/2) / 29.1;     // Divide by 29.1 or multiply by 0.0343
  inches = (duration/2) / 74;   // Divide by 74 or multiply by 0.0135
  wh = calWH(inches);

  JsonDocument doc;  // Create JSon object
  char message[260] = { 0 };

  doc["id"] = "620163595";
  doc["type"] = "ultasonic";
  doc["radar"] = inches;
  doc["waterheight"] = wh;
  doc["reserve"] = calWRes(wh);
  doc["percentage"] = calPercent(wh);

  serializeJson(doc ,message);
  espUpdate(message);
  delay(1000);  
}

 
void espSend(char command[] ){   
    esp.print(command); // send the read character to the esp    
    while(esp.available()){ Serial.println(esp.readString());}    
}


void espUpdate(char mssg[]){ 
    char espCommandString[50] = {0};
    char post[290]            = {0};

    snprintf(espCommandString, sizeof(espCommandString),"AT+CIPSTART=\"TCP\",\"%s\",%s\r\n",HOST_IP,HOST_PORT); 
    espSend(espCommandString);    //starts the connection to the server
    delay(stay);

    // GET REQUEST 
    // snprintf(post,sizeof(post),"GET /%s HTTP/1.1\r\nHost: %s\r\nContent-Type: application/x-www-form-urlencoded\r\nContent-Length: %d\r\n\r\n%s\r\n\r\n",route,HOST_IP,strlen(mssg),mssg);

    // POST REQUEST
    snprintf(post,sizeof(post),"POST /%s HTTP/1.1\r\nHost: %s\r\nContent-Type: application/json\r\nContent-Length: %d\r\n\r\n%s\r\n\r\n",route,HOST_IP,strlen(mssg),mssg);
  
    snprintf(espCommandString, sizeof(espCommandString),"AT+CIPSEND=%d\r\n", strlen(post));
    espSend(espCommandString);    //sends post length
    delay(stay);
    Serial.println(post);
    espSend(post);                //sends POST request with the parameters 
    delay(stay);
    espSend("AT+CIPCLOSE\r\n");   //closes server connection
   }

void espInit(){
    char connection[100] = {0};
    esp.begin(115200); 
    Serial.println("Initiallizing");
    esp.println("AT"); 
    delay(1000);
    esp.println("AT+CWMODE=1");
    delay(1000);
    while(esp.available()){ Serial.println(esp.readString());} 

    snprintf(connection, sizeof(connection),"AT+CWJAP=\"%s\",\"%s\"\r\n",SSID,password);
    esp.print(connection);

    delay(3000);  //gives ESP some time to get IP

    if(esp.available()){   Serial.print(esp.readString());}
    
    Serial.println("\nFinish Initializing");    
   
}

//***** Design and implement all util functions below ******
double calWH(double radar)
{
  return 94.5 - radar;
}

double calWRes(double wh)
{
  return 3.14159265359 * (61.5/2.0) * (61.5/2.0)  * wh /231.0; // 231 cubic inches in a gallon
}

double calPercent(double res)
{
  return (res/77.763) * 100;
}

double calGal(double percent)
{
  return (percent * 1000)/100;
}