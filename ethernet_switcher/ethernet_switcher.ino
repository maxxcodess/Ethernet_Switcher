
#include <UIPEthernet.h>
#include <SPI.h>
#include <Ethernet.h>

String SendHTML(uint8_t led1stat, uint8_t led2stat, uint8_t led3stat);
void handle_OnConnect();
void handle_NotFound();

uint8_t LED1pin = 5;
bool LED1status = HIGH;

uint8_t LED2pin = 4;
bool LED2status = LOW;

uint8_t LED3pin = 0;
bool LED3status = LOW;

uint8_t LED4pin = 2;
bool LED4status = LOW;

byte mac[] = { 0xDE, 0xAD, 0xBB, 0xE5, 0x7E, 0xED };   //physical mac address
// byte ip[] = { 10, 126, 12, 57 };                       //D port, ip in lan (that's what you need to use in your browser. ("192.168.1.178")
// byte ip[] = { 172, 25, 3, 57 };                        //RD port
// byte gateway[] = { 192, 168, 1, 1 };                   // internet access via router
// byte subnet[] = { 255, 255, 255, 0 };                  //subnet mask
EthernetServer server(80);                             //server port
EthernetClient client ;

String readString;

void setup() {
	// Open serial communications and wait for port to open:
	Serial.begin(9600);
	Ethernet.init(16);

	// start the Ethernet connection:
	Serial.println("Initialize Ethernet with DHCP:");
	// start the Ethernet connection and the server:
	// Ethernet.begin(mac, ip); //, gateway, subnet);
	if (Ethernet.begin(mac) == 0) {
		Serial.println("Failed to configure Ethernet using DHCP");
		if (Ethernet.hardwareStatus() == EthernetNoHardware) {
			Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
		} else if (Ethernet.linkStatus() == LinkOFF) {
			Serial.println("Ethernet cable is not connected.");
		}
		// no point in carrying on, so do nothing forevermore:
		while (true) {
			delay(1);
		}
	}
	// print your local IP address:
	Serial.print("My IP address: ");
	Serial.println(Ethernet.localIP());

	pinMode(LED_BUILTIN, OUTPUT);
	pinMode(LED1pin, OUTPUT);
	pinMode(LED2pin, OUTPUT);
	pinMode(LED3pin, OUTPUT);
	// pinMode(LED4pin, OUTPUT);

	// digitalWrite(LED_BUILTIN, LOW);  // turn the LED on (HIGH is the voltage level)
	digitalWrite(LED1pin, HIGH);
	digitalWrite(LED2pin, HIGH);
	digitalWrite(LED3pin, HIGH);
	// digitalWrite(LED4pin, HIGH);

	delay(100);
	server.begin();
	client = server.available();
}


void loop() {
	// Create a client connection
	// EthernetClient client = server.available();
	client = server.available();
	if (client) {
		while (client.connected()) {
			digitalWrite(LED_BUILTIN, LOW);
			if (client.available()) {
				char c = client.read();

				//read char by char HTTP request
				if (readString.length() < 100) {
					//store characters to string
					readString += c;
					//Serial.print(c);
				}

				//if HTTP request has ended
				if (c == '\n') {
					Serial.println(readString); //print to serial monitor for debuging

					if ((readString.indexOf("/ HTTP/1.1") >0) || (readString.indexOf("/favicon.ico") >0)){
						Serial.println("Webpage active");
						handle_OnConnect();
						delay(1);
						//stopping client
						client.stop();
					}
					//controls the Arduino if you press the buttons
					if (readString.indexOf("/relay1on") >0){
						digitalWrite(LED1pin, LOW);
						Serial.println("button1 ON");
						handle_led1on();
						//stopping client
						client.stop();
					}
					if (readString.indexOf("/relay1off") >0){
						digitalWrite(LED1pin, HIGH);
						Serial.println("button1 OFF");
						handle_led1off();
						//stopping client
						client.stop();
					}
					if (readString.indexOf("/relay2on") >0){
						digitalWrite(LED2pin, LOW);
						Serial.println("button2 ON");
						handle_led2on();
						//stopping client
						client.stop();
					}
					if (readString.indexOf("/relay2off") >0){
						digitalWrite(LED2pin, HIGH);
						Serial.println("button2 OFF");
						handle_led2off();
						//stopping client
						client.stop();
					}
					if (readString.indexOf("/relay3on") >0){
						digitalWrite(LED3pin, LOW);
						Serial.println("button3 ON");
						handle_led3on();
						//stopping client
						client.stop();
					}
					if (readString.indexOf("/relay3off") >0){
						digitalWrite(LED3pin, HIGH);
						Serial.println("button3 OFF");
						handle_led3off();
						//stopping client
						client.stop();
					}
					//clearing string for next read
					readString="";
				}
			}
		}
	}
}


void handle_OnConnect() {
	LED1status = LOW;
	LED2status = LOW;
	LED3status = LOW;

	Serial.println("GPIO5 Status: OFF | GPIO4 Status: OFF | GPIO0 Status: OFF");

	client.println("HTTP/1.1 200 OK");
	client.println("Content-Type: text/html");
	// client.println("Connection: close");  // the connection will be closed after completion of the response
	client.println();
	client.println(SendHTML(LED1status,LED2status,LED3status));
	// server.send(200, "text/html", SendHTML(LED1status,LED2status));
}

void handle_led1on() {
	LED1status = HIGH;
	Serial.println("GPIO4 Status: ON");

	client.println("HTTP/1.1 200 OK");
	client.println("Content-Type: text/html");
	// client.println("Connection: close");  // the connection will be closed after completion of the response
	client.println();
	client.println(SendHTML(LED1status,LED2status,LED3status));
	// server.send(200, "text/html", SendHTML(true,LED2status));
}

void handle_led1off() {
	LED1status = LOW;
	Serial.println("GPIO4 Status: OFF");

	client.println("HTTP/1.1 200 OK");
	client.println("Content-Type: text/html");
	// client.println("Connection: close");  // the connection will be closed after completion of the response
	client.println();
	client.println(SendHTML(LED1status,LED2status,LED3status));
	// server.send(200, "text/html", SendHTML(false,LED2status));
}

void handle_led2on() {
	LED2status = HIGH;
	Serial.println("GPIO5 Status: ON");

	client.println("HTTP/1.1 200 OK");
	client.println("Content-Type: text/html");
	// client.println("Connection: close");  // the connection will be closed after completion of the response
	client.println();
	client.println(SendHTML(LED1status,LED2status,LED3status));
	// server.send(200, "text/html", SendHTML(LED1status,true));
}

void handle_led2off() {
	LED2status = LOW;
	Serial.println("GPIO5 Status: OFF");

	client.println("HTTP/1.1 200 OK");
	client.println("Content-Type: text/html");
	// client.println("Connection: close");  // the connection will be closed after completion of the response
	client.println();
	client.println(SendHTML(LED1status,LED2status,LED3status));
	// server.send(200, "text/html", SendHTML(LED1status,false));
}

void handle_led3on() {
	LED3status = HIGH;
	Serial.println("GPIO0 Status: ON");

	client.println("HTTP/1.1 200 OK");
	client.println("Content-Type: text/html");
	// client.println("Connection: close");  // the connection will be closed after completion of the response
	client.println();
	client.println(SendHTML(LED1status,LED2status,LED3status));
	// server.send(200, "text/html", SendHTML(LED1status,true));
}

void handle_led3off() {
	LED3status = LOW;
	Serial.println("GPIO0 Status: OFF");

	client.println("HTTP/1.1 200 OK");
	client.println("Content-Type: text/html");
	// client.println("Connection: close");  // the connection will be closed after completion of the response
	client.println();
	client.println(SendHTML(LED1status,LED2status,LED3status));
	// server.send(200, "text/html", SendHTML(LED1status,false));
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

	if(led1stat)
		{ptr +="<p>relay1 Status: OFF</p><a class=\"button button-off\" href=\"/relay1off\">OFF</a>\n";}
	else
		{ptr +="<p>relay1 Status: ON</p><a class=\"button button-on\" href=\"/relay1on\">ON</a>\n";}

	if(led2stat)
		{ptr +="<p>relay2 Status: OFF</p><a class=\"button button-off\" href=\"/relay2off\">OFF</a>\n";}
	else
		{ptr +="<p>relay2 Status: ON</p><a class=\"button button-on\" href=\"/relay2on\">ON</a>\n";}

	if(led3stat)
		{ptr +="<p>relay3 Status: OFF</p><a class=\"button button-off\" href=\"/relay3off\">OFF</a>\n";}
	else
		{ptr +="<p>relay3 Status: ON</p><a class=\"button button-on\" href=\"/relay3on\">ON</a>\n";}

	ptr +="</body>\n";
	ptr +="</html>\n";
	return ptr;
}
