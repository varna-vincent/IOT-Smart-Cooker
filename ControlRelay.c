#include <WiFi.h>
#include <Servo.h>

const char* wifi_name = "MyWifi"; // Your Wifi network name here
const char* wifi_pass = "123456789";    // Your Wifi network password here

WiFiServer server(80);    // Server will be at port 80
Servo riceServo; 

const int water_relay_pin = 22;
const int stove_relay_pin = 16;
const int rice_servo_pin = 13;

void setup() 
{
  Serial.begin (115200);

  pinMode (water_relay_pin, OUTPUT);
  pinMode (stove_relay_pin, OUTPUT);
  riceServo.attach(rice_servo_pin);

  Serial.print ("Connecting to ");
  Serial.print (wifi_name);
  WiFi.begin (wifi_name, wifi_pass);     // Connecting to the wifi network

  while (WiFi.status() != WL_CONNECTED) // Waiting for the response of wifi network
  {
    delay (500);
    Serial.print (".");
  }

  printWithStatus();
  server.begin();                           // Starting the server
} 

void loop() 
{
  WiFiClient client = server.available();     //Checking if any client request is available or not
  if (client)
  {
    boolean currentLineIsBlank = true;
    String buffer = "";  
    while (client.connected())
    {
      if (client.available())                    // if there is some client data available
      {
        char c = client.read(); 
        buffer+=c;                              // read a byte
        if (c == '\n' && currentLineIsBlank)    // check for newline character, 
        {
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println();    
          client.print("<HTML><title>ESP32</title>");
          client.print("<body><h1>ESP32 Standalone Relay Control </h1>");
          client.print("<p>Water Relay Control</p>");
          client.print("<a href=\"/wateron\"\"><button>Water ON</button></a>");
          client.print("<a href=\"/wateroff\"\"><button>Water OFF</button></a>");
          client.print("<p>Stove Relay Control</p>");
          client.print("<a href=\"/stoveon\"\"><button>Stove ON</button></a>");
          client.print("<a href=\"/stoveoff\"\"><button>Stove OFF</button></a>");
          client.print("<p>Rice Servo Relay Control</p>");
          client.print("<a href=\"/riceon\"\"><button>Rice ON</button></a>");
          client.print("<a href=\"/riceoff\"\"><button>Rice OFF</button></a>");
          client.print("</body></HTML>");
          break;        // break out of the while loop:
        }

        if (c == '\n') { 
          currentLineIsBlank = true;
          buffer="";       
        } 

        else if (c == '\r') { 

          if(buffer.indexOf("GET /cook")>=0) {            // Dispense 1/2 cup water and cook for 15 min
            dispenseRice();
            delay(1000);
            dispenseWater();
            delay(1000);
            cook();
          }

          if(buffer.indexOf("GET /wateron") >= 0)           // Start dispensing water
            digitalWrite(water_relay_pin, HIGH);
          
          if(buffer.indexOf("GET /wateroff") >= 0)          // Stop dispensing water
            digitalWrite(water_relay_pin, LOW);

          if(buffer.indexOf("GET /stoveon") >= 0)           // Switch on stove
            digitalWrite(stove_relay_pin, HIGH);

          if(buffer.indexOf("GET /stoveoff") >= 0)          // Switch off stove
            digitalWrite(stove_relay_pin, LOW);  

          if(buffer.indexOf("GET /riceon") >= 0)            // Start dispensing rice
            riceServo.write(85);     
          
          if(buffer.indexOf("GET /riceoff") >= 0)           // Stop dispensing rice
            riceServo.write(0);

        }
        else {
          currentLineIsBlank = false;
        }  
      }
    }
    client.stop();
  }
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
  delay (20000);
  digitalWrite(water_relay_pin, LOW);
}

void dispenseRice() {
  riceServo.write(85);
  delay (3000);
  riceServo.write(0);
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