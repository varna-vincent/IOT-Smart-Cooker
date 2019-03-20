#include <WiFi.h>
#include <HTTPClient.h>
#include <Servo.h>
 
const char* ssid = "Varna-Pixel2";
const char* password = "ae5c2c2088ad";

Servo riceServo; 
bool isCooking = false;

const int water_relay_pin = 22;
const int stove_relay_pin = 16;
const int rice_servo_pin = 13;

void setup() {
 
  Serial.begin(115200);
  delay(4000);

  pinMode (water_relay_pin, OUTPUT);
  pinMode (stove_relay_pin, OUTPUT);
  riceServo.attach(rice_servo_pin);

  Serial.print ("Connecting to ");
  Serial.print (ssid);
  WiFi.begin (ssid, password);     // Connecting to the wifi network

  while (WiFi.status() != WL_CONNECTED) // Waiting for the response of wifi network
  {
    delay (500);
    Serial.print (".");
  }

  printWithStatus();
 
}
 
void loop() {
 
  if ((WiFi.status() == WL_CONNECTED)) { //Check the current connection status
 
    HTTPClient http;
    
    http.begin("http://api.nanduris.com/cooker/status"); //Specify the URL
    int httpCode = http.GET();                                        //Make the request
 
    if (httpCode > 0) { //Check for the returning code
 
        String payload = http.getString();
        Serial.println(httpCode);
        Serial.println(payload);

        if(payload.equals("1") && isCooking == false) {
          dispenseRice();
          delay(1000);
          dispenseWater();
          delay(1000);
          startCooking();
          isCooking = true;
          Serial.println("Cooking....");
        }

        if(payload.equals("0") && isCooking == true) {
          stopCooking();
          isCooking = false;
          Serial.println("Finished Cooking....");
        }
      } else {
      Serial.println("Error on HTTP request");
    }
 
    http.end(); //Free the resources
  }
  delay(10000);
 
}

void printWithStatus() {

  Serial.println("");
  Serial.println("Connection Successful");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());           // Getting the IP address
  Serial.println("Type the above IP address into browser search bar"); 
}

void dispenseWater() {
  digitalWrite(water_relay_pin, HIGH);
  delay (21000);
  digitalWrite(water_relay_pin, LOW);
}

void dispenseRice() {
  riceServo.write(50);
  delay (2500);
  riceServo.write(0);
}

void startCooking() {
  digitalWrite(stove_relay_pin, HIGH);
}

void cook() {
  digitalWrite(stove_relay_pin, HIGH);
  delay (570000);
  digitalWrite(stove_relay_pin, LOW);
}

void stopCooking() {
  digitalWrite(water_relay_pin, LOW);
  riceServo.write(0);
  digitalWrite(stove_relay_pin, LOW);
}