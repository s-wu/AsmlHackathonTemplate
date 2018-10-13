#include <Arduino.h>

#include "Debug.hpp"
#include "painlessMesh.h"
#include "Facilities_MeshNetwork.hpp"
#include "Tasks_ExampleTransmitTask.hpp"
#include "Tasks_ExampleDisplayTask.hpp"
#include "ESP8266WiFi.h"

WiFiServer server(80);
// Variable to store the HTTP request
String header;
String pattern = "";
vector <String> img;

// Translation unit local variables
namespace {

Scheduler                  taskScheduler;
Facilities::MeshNetwork    meshNetwork;
Tasks::ExampleTransmitTask exampleTransmitTask(meshNetwork, img);
Tasks::ExampleDisplayTask  exampleDisplayTask(meshNetwork, exampleTransmitTask.img);
}

void setupwebserver() {
    //WiFi.begin("ASML Digital Gold Hackathon", "DigitalGold");
    
    IPAddress local(10,33,38,101);
    IPAddress gateway(10,33,38,1);
    IPAddress subnet (255,255,255,0);
    WiFi.config(local, gateway, subnet);
    WiFi.begin("MeshGreen", "Mesh-Green-Password");
    while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    }
    // Print local IP address and start web server
    Serial.println("");
    Serial.println("WiFi connected.");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    server.begin();
}

void updatewebserver() {
    WiFiClient client = server.available();   // Listen for incoming clients
 if (client) {                             // If a new client connects,
   Serial.println("New Client.");          // print a message out in the serial port
   String currentLine = "";                // make a String to hold incoming data from the client
   while (client.connected()) {            // loop while the client's connected
     if (client.available()) {             // if there's bytes to read from the client,
       char c = client.read();             // read a byte, then
       Serial.write(c);                    // print it out the serial monitor
       header += c;
       if (c == '\n') {                    // if the byte is a newline character
         // if the current line is blank, you got two newline characters in a row.
         // that's the end of the client HTTP request, so send a response:
         if (currentLine.length() == 0) {
           // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
           // and a content-type so the client knows what's coming, then a blank line:
           client.println("HTTP/1.1 200 OK");
           client.println("Content-type:text/html");
           client.println("Connection: close");
           client.println();
           // turns the GPIOs on and off
           if ((header.indexOf("GET /pattern") > 0) || (header.indexOf("OPTIONS /pattern")  >= 0)) {
             Serial.println("got the pattern");
             
             pattern = "";
             for(int i = 25; i < header.length() && header[i] != ' '; ++i) {
                 pattern += header[i];
             }

             Serial.println(pattern);
             String pp = pattern;
             if (pattern.length() > 0) {
               for(int i = 0; i < 3; ++i) {
                 meshNetwork.sendBroadcast(pp);
               }          
             } 
           } 
           // Display the HTML web page
           client.println("<!DOCTYPE html><html>");
           client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
           client.println("<link rel=\"icon\" href=\"data:,\">");
           client.println("</body></html>");
           // The HTTP response ends with another blank line
           client.println();
           // Break out of the while loop
           break;
         } else { // if you got a newline, then clear currentLine
           currentLine = "";
         }
       } else if (c != '\r') {  // if you got anything else but a carriage return character,
         currentLine += c;      // add it to the end of the currentLine
       }
     }
   }
   // Clear the header variable
   header = "";
   // Close the connection
   client.stop();
   Serial.println("Client disconnected.");
   Serial.println("");
 }
}

//! Called once at board startup.
void setup()
{
   MY_DEBUG_BEGIN(115200);

   // Create MeshNetwork
    meshNetwork.initialize(F("MeshGreen"), F("Mesh-Green-Password"), taskScheduler);
    //meshNetwork.initialize(F("ASML Digital Gold Hackathon"), F("DigitalGold"));

   // Create and add tasks.
   taskScheduler.addTask( exampleDisplayTask );
   taskScheduler.addTask( exampleTransmitTask );
   exampleDisplayTask.enable();
   delay(100);
   exampleTransmitTask.enable();

   setupwebserver();

   MY_DEBUG_PRINTLN(F("Setup completed"));
}

//! Called repeatedly after setup().
void loop()
{
   //String s = "hello";
   //meshNetwork.sendBroadcast(s);

  updatewebserver();
   
   taskScheduler.execute();
   meshNetwork.update();

   

   //
}


