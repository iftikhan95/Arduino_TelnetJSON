/*
 Chat Server modified to receive commands from a telnet client and control a LED. 

 A simple server that distributes any incoming messages to all
 connected clients.  To use, telnet to your device's IP address and type.
 You can see the client's input in the serial monitor as well.
 Using an Arduino WIZnet Ethernet shield.

 Circuit:
 * Ethernet shield attached to pins 10, 11, 12, 13

 created 18 Dec 2009
 by David A. Mellis
 modified 9 Apr 2012
 by Tom Igoe

 modified by Iftikhar Khan Munthasir Islam Mahdi Mahi
 */

#include <SPI.h>
#include <Ethernet.h>
#include <EEPROM.h>

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network.
// gateway and subnet are optional:
byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};
IPAddress ip(192, 168, 3, 69);
IPAddress myDns(192, 168, 3, 1);
IPAddress subnet(255, 255, 255, 0);

// telnet defaults to port 23
EthernetServer server(23);
bool alreadyConnected = false;  // whether or not the client was connected previously
//char to read from the telnet client
char thisChar;
String Command;

int ledPin = 7; //ledPin
int ledNVMAddress = 0; //led EEPROM storage address
int ledState; //led state high/low

void processCommand(String Command) {
  Serial.print("Process command -> ");
  Serial.println(Command);
 
  if (Command.indexOf("ledon") > -1) {
    server.println("LED On command received");
    digitalWrite(ledPin, HIGH);  // sets the LED on
    EEPROM.update(ledNVMAddress,HIGH);
    server.println("LED was turned on");
    return;
  }

  if (Command.indexOf("ledoff") > -1) {
    server.println("LED Off command received");
    digitalWrite(ledPin, LOW);  // sets the LED off
    EEPROM.update(ledNVMAddress,LOW);
    server.println("LED was turned off");
    return;
  }

  instructions();
}

void setup() {
  //restore last known value of the LED
  ledState = EEPROM.read(ledNVMAddress);
  digitalWrite(ledPin, ledState);

  // initialize the Ethernet device
  Ethernet.begin(mac, ip, myDns, subnet);
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ;  // wait for serial port to connect. Needed for native USB port only
  }

  // Check for Ethernet hardware present
  if (Ethernet.hardwareStatus() == EthernetNoHardware) {
    Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
    while (true) {
      delay(1);  // do nothing, no point running without Ethernet hardware
    }
  }
  if (Ethernet.linkStatus() == LinkOFF) {
    Serial.println("Ethernet cable is not connected.");
  }

  // start listening for clients
  server.begin();
  // local server address on the terminal
  Serial.print("Chat server address:");
  Serial.println(Ethernet.localIP());
}

void loop() {
  // wait for a new client:
  EthernetClient client = server.available();

  // when the client sends the first byte, say hello:
  if (client) {
    if (!alreadyConnected) {
      // clear out the input buffer:
      client.flush();
      Serial.println("We have a new client");
      client.println("Hello, client!");
      alreadyConnected = true;
      Command = "";
    }

    if (client.available() > 0) {
      // read the bytes incoming from the client:
      thisChar = client.read();
    }

    if (thisChar == 0x0D) {
      //if the user presses Enter, then process the command. 0x0D is the ASCII for ENTER
      processCommand(Command);
      Command = "";
    } else {
      //read the acceptable command bytes and store them in the command word
      if (thisChar == 'l' || thisChar == 'e' || thisChar == 'd' || thisChar == 'o' || thisChar == 'f'
          || thisChar == 'n') {
        Command += thisChar;
      }
    }
  }
}

void instructions() {
  //Provide instructions for the client
  server.println("I don't understand");
  server.println("Please use one of these commands:");
  server.println("* ledon, to turn on the LED");
  server.println("* ledoff, to turn off the LED");
}


