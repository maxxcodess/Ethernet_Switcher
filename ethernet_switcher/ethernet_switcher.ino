
#include <UIPEthernet.h>
#include <SPI.h>
#include <Ethernet.h>

#define ETHERNET_CS_PIN	16
#define DEBUG		1

String SendHTML(uint8_t led1stat, uint8_t led2stat, uint8_t led3stat);
void handle_PinControl(uint8_t pin, bool state);
void handle_OnConnect(bool state);
void handle_NotFound();

const uint8_t pin1 = 5;
const uint8_t pin2 = 4;
const uint8_t pin3 = 0;
const uint8_t pin4 = 2;

bool pin1status = LOW;
bool pin2status = LOW;
bool pin3status = LOW;
bool pin4status = LOW;

byte mac[] = { 0xDE, 0xAD, 0xBB, 0xE5, 0x7E, 0xED };   // Physical MAC address
// byte ip[] = { 10, 126, 12, 57 };                       // D port, IP in LAN
// byte ip[] = { 172, 25, 3, 57 };                        // RD port
// byte gateway[] = { 192, 168, 1, 1 };                   // Internet access via router
// byte subnet[] = { 255, 255, 255, 0 };                  // subnet mask
EthernetServer server(80);                             // Server port
EthernetClient client ;

String readString;

void setup() {
#if DEBUG
	// Open serial communications and wait for port to open
	Serial.begin(9600);
	Serial.println("Initialize Ethernet with DHCP:");
#endif

	// Setup with Ethernet Chip Select (CS) pin
	Ethernet.init(ETHERNET_CS_PIN);

	// Start the Ethernet connection and the server:
	// Ethernet.begin(mac, ip); //, gateway, subnet);
	if (Ethernet.begin(mac) == 0) {
#if DEBUG
		Serial.println("Failed to configure Ethernet using DHCP");
		if (Ethernet.hardwareStatus() == EthernetNoHardware) {
			Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
		} else if (Ethernet.linkStatus() == LinkOFF) {
			Serial.println("Ethernet cable is not connected.");
		}
#endif
		// no point in carrying on, so do nothing forevermore:
		while (true) {
			delay(1);
		}
	}

#if DEBUG
	// print your local IP address:
	Serial.print("My IP address: ");
	Serial.println(Ethernet.localIP());
#endif

	pinMode(LED_BUILTIN, OUTPUT);
	pinMode(pin1, OUTPUT);
	pinMode(pin2, OUTPUT);
	pinMode(pin3, OUTPUT);
	// pinMode(pin4, OUTPUT);

	// turn the LED on (HIGH is the voltage level)
	// digitalWrite(LED_BUILTIN, LOW);
	digitalWrite(pin1, LOW);
	digitalWrite(pin2, LOW);
	digitalWrite(pin3, LOW);
	// digitalWrite(pin4, HIGH);

	delay(100);
	server.begin();
}


void loop() {
	// Create a client connection
	client = server.available();
	bool onConnect_Set_Once = true;

	if (client) {
		while (client.connected()) {
			if (onConnect_Set_Once) {
				// by default esp work on sink-in mode, LOW = GND,  HIGH = VCC
				digitalWrite(LED_BUILTIN, LOW);     // LED is connected to VCC, so on LOW circuit get complete and LED is ON
				digitalWrite(pin1, LOW);
				digitalWrite(pin2, LOW);
				digitalWrite(pin3, LOW);
				onConnect_Set_Once = false;
			}

			if (client.available()) {
				char c = client.read();    //read char by char HTTP request

				if (readString.length() < 100) {
					readString += c;    //store characters to string
				}

				//if HTTP request has ended
				if (c == '\n') {
#if DEBUG
					Serial.println(readString); //print to serial monitor for debuging
#endif
					if ((readString.indexOf("/ HTTP/1.1") > 0) || (readString.indexOf("/favicon.ico") > 0)) {
						handle_OnConnect(true);
					}
					if (readString.indexOf("/relay1on")  > 0) {
						handle_PinControl(pin1, true);
					}
					if (readString.indexOf("/relay1off") > 0) {
						handle_PinControl(pin1, false);
					}
					if (readString.indexOf("/relay2on")  > 0) {
						handle_PinControl(pin2, true);
					}
					if (readString.indexOf("/relay2off") > 0) {
						handle_PinControl(pin2, false);
					}
					if (readString.indexOf("/relay3on")  > 0) {
						handle_PinControl(pin3, true);
					}
					if (readString.indexOf("/relay3off") > 0) {
						handle_PinControl(pin3, false);
					}
					readString="";    //clearing string for next read
				}
			}
		}
	}
}


void handle_OnConnect(bool state) {
	pin1status = state;
	pin2status = state;
	pin3status = state;
#if DEBUG
	Serial.println("Webpage active");
	Serial.println("GPIO5 Status: OFF | GPIO4 Status: OFF | GPIO0 Status: OFF");
#endif
	client.println("HTTP/1.1 200 OK");
	client.println("Content-Type: text/html");
	// client.println("Connection: close");  // the connection will be closed after completion of the response
	client.println();
	client.println(SendHTML(pin1status, pin2status, pin3status));

	delay(1);
	client.stop();    //stopping client
}

void handle_PinControl(uint8_t pin, bool state) {
	switch(pin) {
		case pin1  :	pin1status = state;
				break;
		case pin2  :	pin2status = state;
				break;
		case pin3  :	pin3status = state;
				break;
		default    :	return;
	}

/*	if      (pin == pin1)	pin1status = state;
	else if (pin == pin2)	pin2status = state;
	else if (pin == pin3)	pin3status = state;
	else			return;
*/

#if DEBUG
	Serial.print("GPIO");
	Serial.print(pin);
	Serial.print(" Status: ");
	Serial.println(state);
#endif

	digitalWrite(pin, !state);

	client.println("HTTP/1.1 200 OK");
	client.println("Content-Type: text/html");
	// client.println("Connection: close");    // the connection will be closed after completion of the response
	client.println();
	client.println(SendHTML(pin1status, pin2status, pin3status));

	delay(1);
	client.stop();    //stopping client
}

void handle_NotFound(){
	client.println("HTTP/1.1 404 Not Found");
	client.println("Content-Type: text/html");
	// client.println("Connection: close");  // the connection will be closed after completion of the response
	client.println();
	// server.send(404, "text/plain", "Not found");
}

String SendHTML(uint8_t led1stat, uint8_t led2stat, uint8_t led3stat){
	String ptr = "<!DOCTYPE html> <html>\n";
	ptr +="<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
	ptr +="<title>Relay Control</title>\n";
	ptr +="<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
	ptr +="body{margin-top: 50px;} h1 {color: #444444;margin: 50px auto 30px;} h3 {color: #444444;margin-bottom: 50px;}\n";
	ptr +=".button {display: block;width: 80px;background-color: #1abc9c;border: none;color: white;padding: 13px 30px;text-decoration: none;font-size: 25px;margin: 0px auto 35px;cursor: pointer;border-radius: 4px;}\n";
	ptr +=".button-on {background-color: #1abc9c;}\n";
	ptr +=".button-on:active {background-color: #16a085;}\n";
	ptr +=".button-off {background-color: #34495e;}\n";
	ptr +=".button-off:active {background-color: #2c3e50;}\n";
	ptr +="p {font-size: 14px;color: #888;margin-bottom: 10px;}\n";
	ptr +="</style>\n";
	ptr +="</head>\n";
	ptr +="<body>\n";
	ptr +="<h1>Ethernet Web Server</h1>\n";
	ptr +="<h3>Relay Control using Ethernet</h3>\n";

	if (led1stat)
		{ptr +="<p>relay1 Status: OFF</p><a class=\"button button-off\" href=\"/relay1off\">OFF</a>\n";}
	else
		{ptr +="<p>relay1 Status: ON</p><a class=\"button button-on\" href=\"/relay1on\">ON</a>\n";}

	if (led2stat)
		{ptr +="<p>relay2 Status: OFF</p><a class=\"button button-off\" href=\"/relay2off\">OFF</a>\n";}
	else
		{ptr +="<p>relay2 Status: ON</p><a class=\"button button-on\" href=\"/relay2on\">ON</a>\n";}

	if (led3stat)
		{ptr +="<p>relay3 Status: OFF</p><a class=\"button button-off\" href=\"/relay3off\">OFF</a>\n";}
	else
		{ptr +="<p>relay3 Status: ON</p><a class=\"button button-on\" href=\"/relay3on\">ON</a>\n";}

	ptr +="</body>\n";
	ptr +="</html>\n";
	return ptr;
}
