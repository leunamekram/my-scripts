/*!
 *  @brief  Code from Rui Santos. Complete project details at http://randomnerdtutorials.com
 */

// Include Wi-Fi library
#include <ESP8266WiFi.h>

// Replace with your network credentials
const char* ssid = "AndroidAPA7EF";
const char* password = "lygw0291";

// Set web server port number to 80
WiFiServer server(80);

// Variable to store the HTTP request
String header;

// Auxiliar variables to store the current output state
String output0State = "OFF";
String output2State = "OFF";

// Assign output variables to GPIO pins
const int output0 = 0;
const int output2 = 2;

// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0; 
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;

void setup()
{
    Serial.begin(115200);

    // Set pin mode
    pinMode(output0, OUTPUT);
    pinMode(output2, OUTPUT);

    // Set output pins to LOW
    digitalWrite(output0, LOW);
    digitalWrite(output2, LOW);

    // Connect to Wi-Fi network with SSID and password
    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }

    // Print local IP address
    Serial.println("");
    Serial.println("WiFi connected.");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

    // Start web server
    server.begin();
}

void loop()
{
    // Listen for incoming clients
    WiFiClient client = server.available();

    if (client)
    {
        // If a new client connects, print a message out in the serial port
        Serial.println("New Client.");

        // Make a String to hold incoming data from the client
        String currentLine = "";

        // Take note of time
        currentTime = millis();
        previousTime = currentTime;

        // Loop while the client's connected
        while (client.connected() && currentTime - previousTime <= timeoutTime)
        {
            currentTime = millis();

            // If there's bytes to read from the client, read a byte the print it out the serial monitor
            if (client.available())
            {
                char c = client.read();
                Serial.write(c);
                header += c;

                // If the byte is a newline character
                if (c == '\n')
                {
                    // If the current line is blank, you got two newline characters in a row.
                    // that's the end of the client HTTP request, so send a response:
                    if (currentLine.length() == 0)
                    {
                        // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
                        // and a content-type so the client knows what's coming, then a blank line:
                        client.println("HTTP/1.1 200 OK");
                        client.println("Content-type:text/html");
                        client.println("Connection: close");
                        client.println();

                        // Turns the GPIOs ON and OFF
                        if (header.indexOf("GET /0/ON") >= 0)
                        {
                            Serial.println("GPIO 0 ON");
                            output0State = "ON";
                            digitalWrite(output0, HIGH);
                        }
                        else if (header.indexOf("GET /0/OFF") >= 0)
                        {
                            Serial.println("GPIO 0 OFF");
                            output0State = "OFF";
                            digitalWrite(output0, LOW);
                        }
                        else if (header.indexOf("GET /2/ON") >= 0)
                        {
                            Serial.println("GPIO 2 ON");
                            output2State = "ON";
                            digitalWrite(output2, HIGH);
                        }
                        else if (header.indexOf("GET /2/OFF") >= 0)
                        {
                            Serial.println("GPIO 2 OFF");
                            output2State = "OFF";
                            digitalWrite(output2, LOW);
                        }

                        // Display the HTML web page
                        client.println("<!DOCTYPE html><html>");
                        client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
                        client.println("<link rel=\"icon\" href=\"data:,\">");

                        // CSS to style the ON/OFF buttons 
                        // Feel free to change the background-color and font-size attributes to fit your preferences
                        client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
                        client.println(".button { background-color: #77878A; border: none; color: white; padding: 16px 40px;");
                        client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
                        client.println(".button2 {background-color: #195B6A;}</style></head>");

                        // Web Page Heading
                        client.println("<body><h1>ESP8266 Wifi-Controlled Switches</h1>");

                        // Display current state, and ON/OFF buttons for GPIO 5
                        client.println("<p>Switch 1 is <b>" + output0State + "</b></p>");

                        // If the output0State is OFF, it displays the ON button
                        if (output0State=="OFF")
                        {
                            client.println("<p><a href=\"/0/ON\"><button class=\"button\">TURN ON</button></a></p>");
                        }
                        else
                        {
                            client.println("<p><a href=\"/0/OFF\"><button class=\"button button2\">TURN OFF</button></a></p>");
                        }

                        // Display current state, and ON/OFF buttons for GPIO 4
                        client.println("<p>Switch 2 is <b>" + output2State + "</b></p>");

                        // If the output2State is OFF, it displays the ON button
                        if (output2State=="OFF")
                        {
                            client.println("<p><a href=\"/2/ON\"><button class=\"button\">TURN ON</button></a></p>");
                        }
                        else
                        {
                            client.println("<p><a href=\"/2/OFF\"><button class=\"button button2\">TURN OFF</button></a></p>");
                        }

                        client.println("</body></html>");

                        // The HTTP response ends with another blank line
                        client.println();

                        // Break out of the while loop
                        break;
                    }
                    // If you got a newline, then clear currentLine
                    else
                    {
                        currentLine = "";
                    }
                }
                // If you got anything else but a carriage return character, add it to the end of the currentLine
                else if (c != '\r')
                {
                    currentLine += c;
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
