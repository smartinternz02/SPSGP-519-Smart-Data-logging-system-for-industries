
#include <ESP8266WiFi.h>
const char* ssid = "Galaxy M30sFFEF";
const char* password = "balajiwifi";

const int analogIn = A0;
int trigPin=D5;
int echoPin=D6; 
int RawValue= 0;
double Voltage = 0;
double tempC = 0;
double tempF = 0;
int distance,duration;
// Replace with your unique IFTTT URL resource
const char* resource = "/trigger/datalogger/with/key/bvS4JsuzMDNbQGrrtAk_nb";
//https://maker.ifttt.com/trigger/datalogger/with/key/lwc9YxLFtBzVZVhPV3dFrKiG0DmwVOytHudBbzNO2lb
// Maker Webhooks IFTTT
const char* server = "maker.ifttt.com";

// Time to sleep
uint64_t uS_TO_S_FACTOR = 1000000;  // Conversion factor for micro seconds to seconds
// sleep for 30 minutes = 1800 seconds
uint64_t TIME_TO_SLEEP = 1800;

void sensorReadings(){
 RawValue = analogRead(analogIn);
Voltage = (RawValue / 2048.0) * 3300; // 5000 to get millivots.
tempC = Voltage * 0.1;
tempF = (tempC * 1.8) + 32; // convert to F
Serial.print("Raw Value = " ); // shows pre-scaled value
Serial.print(RawValue);
Serial.print("\t Temperature in C = ");
Serial.print(tempC,1);
Serial.print("\t Temperature in F = ");
Serial.println(tempF,1);
delay(500);

digitalWrite(trigPin,LOW);
delay(1000);
digitalWrite(trigPin,HIGH);//generate one pulse by making trig pin high and low
delay(1000);
digitalWrite(trigPin,LOW);
duration=pulseIn(echoPin,HIGH);
Serial.println("duration value");
Serial.println(duration);
distance=(duration*0.343)/2;
Serial.println("distance value");
Serial.println(distance);
}
 
void setup() {
  Serial.begin(115200);
  delay(2000);
  pinMode(trigPin,OUTPUT);
  pinMode(echoPin,INPUT);
  initWifi();
  
// esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  Serial.println("Going to sleep now");
// esp_deep_sleep_start();
}

void loop() {
  makeIFTTTRequest();
  delay(1000);
  delay(1000);
}

// Establish a Wi-Fi connection with your router
void initWifi() {
  Serial.print("Connecting to: ");
  Serial.print(ssid);
  WiFi.begin(ssid, password);

  int timeout = 10 * 4; // 10 seconds
  while (WiFi.status() != WL_CONNECTED  && (timeout-- > 0)) {
    delay(250);
    Serial.print(".");
  }
  Serial.println("");



  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Failed to connect, going back to sleep");
  }

  Serial.print("WiFi connected in: ");
  Serial.print(millis());
  Serial.print(", IP address: ");
  Serial.println(WiFi.localIP());
}

// Make an HTTP request to the IFTTT web service
void makeIFTTTRequest() {
  sensorReadings();
  Serial.print("Connecting to ");
  Serial.print(server);

  WiFiClient client;
  int retries = 5;
  while (!!!client.connect(server, 80) && (retries-- > 0)) {
    Serial.print(".");
  }
  Serial.println();
  if (!!!client.connected()) {
    Serial.println("Failed to connect...");
  }

  Serial.print("Request resource: ");
  Serial.println(resource);

  // Temperature in Celsius
  String jsonObject = String("{\"value1\":\"") + tempC + "\",\"value2\":\"" + distance
                      +"\"}";
  client.println(String("POST ") + resource + " HTTP/1.1");
  client.println(String("Host: ") + server);
  client.println("Connection: close\r\nContent-Type: application/json");
  client.print("Content-Length: ");
  client.println(jsonObject.length());
  client.println();
  client.println(jsonObject);

  int timeout = 5 * 10; // 5 seconds
  while (!!!client.available() && (timeout-- > 0)) {
    delay(100);
  }
  if (!!!client.available()) {
    Serial.println("No response...");
  }
  while (client.available()) {
    Serial.write(client.read());
  }

  Serial.println("\nclosing connection");
  client.stop();
}
