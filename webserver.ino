/*
  Online Relay controller

 A simple web server that allows the user to control a relay module.
 using an Arduino Wiznet Ethernet shield and a 4 Relay Module.

 Circuit:
 * Ethernet shield attached to pins 10, 11, 12, 13
 * Relay inputs connected to pins 2, 3, 5, 6
 * 
 created November 11, 2016
 by Ian McTavish based on work:
 
 created 18 Dec 2009
 by David A. Mellis
 modified 9 Apr 2012
 by Tom Igoe
 modified 02 Sept 2015
 by Arturo Guadalupi

 */

#include <SPI.h>
#include <Ethernet.h>

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};
IPAddress ip(192, 168, 1, 11);

// Initialize the Ethernet server library
// with the IP address and port you want to use
// (port 80 is default for HTTP):
EthernetServer server(80);

int relayPin1 = 2;                 // IN1 connected to digital pin 2
int relayPin2 = 3;                 // IN2 connected to digital pin 3
int relayPin3 = 5;                 // IN3 connected to digital pin 5
int relayPin4 = 6;                // IN4 connected to digital pin 6
void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }


  // start the Ethernet connection and the server:
  Ethernet.begin(mac, ip);
  server.begin();
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());

  pinMode(relayPin1, OUTPUT);      // sets the digital pin as output
  pinMode(relayPin2, OUTPUT);      // sets the digital pin as output
  pinMode(relayPin3, OUTPUT);      // sets the digital pin as output
  pinMode(relayPin4, OUTPUT);      // sets the digital pin as output
  digitalWrite(relayPin1, HIGH);        // Prevents relays from starting up engaged
  digitalWrite(relayPin2, HIGH);        // Prevents relays from starting up engaged
  digitalWrite(relayPin3, HIGH);        // Prevents relays from starting up engaged
  digitalWrite(relayPin4, HIGH);        // Prevents relays from starting up engaged
}


void loop() {
  // listen for incoming clients

  char prevc = 'q';
  boolean inGetMode = false;
  String getText = "";
  EthernetClient client = server.available();
  if (client) {
    Serial.println("new client");
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);

        // code to determine if getting relay information
        if(c == 'G')
        {
          prevc = 'G';
        }else if(c == 'E' && prevc == 'G')
        {
          prevc = 'E';
        }else if(c == 'T' && prevc == 'E')
        {
          inGetMode = true;
          prevc = 'q';
        }else if(inGetMode)
        {
          getText += c;
        }

        
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) {
          // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close");  // the connection will be closed after completion of the response
          //client.println("Refresh: 5");  // refresh the page automatically every 5 sec
          client.println();
          client.println("<!DOCTYPE HTML>");
          client.println("<html>");

          //output form
          client.println("<form>Relay (1-4):<br><input type='text' name='relay' value='relay'>");
          client.println("<br>On (1) or Off (0):<br><input type='text' name='status' value='status'>");
          client.println("<br><br>  <input type='submit' value='Submit'></form>");
          client.println("</html>");
         
          break;
        }
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
          inGetMode = false;
          
          if(getText.indexOf("?relay=")>=0 && getText.indexOf("&status=")>=0)
          {
            Serial.print(getText);
            //get relay (7 is the number of characters after ?relay=
            String relayV = getText.substring(getText.indexOf("?relay=")+7,getText.indexOf("?relay=")+8);
            String statusV = getText.substring(getText.indexOf("&status=")+8,getText.indexOf("&status=")+9);
            Serial.print("\n\nRelay: " + relayV + "\nStatus: " + statusV+"\n");
            getText = "";
            //**********************************************************
            //Turn on or off the relays based on what was entered.
            if(relayV == "1")
            {
              if(statusV == "0"){ digitalWrite(relayPin1, HIGH);}    // de-energizes the relay and LED is off
              else{digitalWrite(relayPin1, LOW);}   // energizes the relay and lights the LED
            }
            if(relayV == "2")
            {
              if(statusV == "0"){digitalWrite(relayPin2, HIGH);}    // de-energizes the relay and LED is off
              else{digitalWrite(relayPin2, LOW);}   // energizes the relay and lights the LED
            }
            if(relayV == "3")
            {
              if(statusV == "0"){digitalWrite(relayPin3, HIGH); }   // de-energizes the relay and LED is off
              else{digitalWrite(relayPin3, LOW); }  // energizes the relay and lights the LED
            }
            if(relayV == "4")
            {
              if(statusV == "0"){digitalWrite(relayPin4, HIGH);}    // de-energizes the relay and LED is off
              else{digitalWrite(relayPin4, LOW);}   // energizes the relay and lights the LED
            }
              delay(1000);                  // waits for a second
            }// end if(getText.indexOf("?relay=")>=0 && getText.indexOf("&status=")>=0)

          
        } else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    // give the web browser time to receive the data
    delay(1);
    // close the connection:
    client.stop();
    Serial.println("client disconnected");
  }
}
