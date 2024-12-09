#include <WiFi.h>
#include <PubSubClient.h>
#include <SPI.h>
#include <Wire.h>
#include <ESP32Servo.h>
// #include <Adafruit_GFX.h>
// #include <Adafruit_SSD1306.h>

#define in1 26
#define in2 27
#define in3 32
#define in4 33
#define PWMPin1 22
#define PWMPin2 23

String username = "";
String speed = "";
String pull = "";

Servo servo1;

//=================================================================================================
// WiFi และ MQTT Configuration
const char* ssid = "Pak24";//CE-NextGen
const char* password = "xit9^ohe";//##CEnextgen2024
const char* mqtt_broker = "mqtt.cloud.ce.kmitl.ac.th";
const char* mqtt_username = "iot_new_02";
const char* mqtt_password = "iot_new_02";
const int mqtt_port = 1883;
const char* topic_username = "TOPIC_USERNAME";
const char* topic_move = "teslana/moving";
const char* topic_speed = "teslana/Speed";
const char* topic_pull = "teslana/pull";


WiFiClient espClient;
PubSubClient client(espClient);

//=================================================================================================

void setup_wifi() {
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char *topic, byte *payload, unsigned int length) {
  Serial.print("Message arrived in topic: ");
  Serial.println(topic);

  String message = "";
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.print("Message: ");
  Serial.println(message);
  Serial.println("-----------------------");

  if (strcmp(topic, topic_username) == 0) {
    username = message;
  } else if (strcmp(topic, topic_speed) == 0){
    Serial.println("speed");
    analogWrite(PWMPin1, message.toInt());
    analogWrite(PWMPin2, message.toInt());
    
    
  } else if (strcmp(topic, topic_move) == 0){
    if(message == "b"){
      digitalWrite(in1, HIGH);
      digitalWrite(in2, LOW);
      digitalWrite(in3, HIGH);
      digitalWrite(in4, LOW);
      // Serial.println("เดินหน้าจ่ะ");
    }

    if(message == "l"){
      digitalWrite(in1, HIGH);
      digitalWrite(in2, LOW);
      digitalWrite(in3, LOW);
      digitalWrite(in4, LOW);
      // Serial.println("เลี้ยวซ้ายจ่ะ");
    }

    if(message == "r"){
      digitalWrite(in1, LOW);
      digitalWrite(in2, LOW);
      digitalWrite(in3, HIGH);
      digitalWrite(in4, LOW);
      // Serial.println("เลี้ยวซ้ายจ่ะ");
    }

    if(message == "f"){
      digitalWrite(in1, LOW);
      digitalWrite(in2, HIGH);
      digitalWrite(in3, LOW);
      digitalWrite(in4, HIGH);
      // Serial.println("ถอยหลังจ่ะ");

    }

    else{
      digitalWrite(in1, LOW);
      digitalWrite(in2, LOW);
      digitalWrite(in3, LOW);
      digitalWrite(in4, LOW);

    }
  }
  
  //

  else if (strcmp(topic, topic_pull) == 0){
    pull = message;
    if (pull == "ON") { 
      servo1.write(180); 
      Serial.println("Pull");
    } 
    else if (pull == "OFF") { 
      servo1.write(0); 
      Serial.println("Push");
    } 
  }
}


void reconnect() {
  while (!client.connected()) {
    String client_id = "esp32-client-";
    client_id += String(WiFi.macAddress());
    Serial.printf("The client %s connects to the public mqtt broker\n", client_id.c_str());

    if (client.connect(client_id.c_str(), mqtt_username, mqtt_password)) {
      Serial.println("Public MQTT broker connected");
      client.subscribe(topic_username);
      client.subscribe(topic_move);
      client.subscribe(topic_speed);
      client.subscribe(topic_pull);
    } else {
      Serial.print("Failed with state ");
      Serial.println(client.state());
      delay(2000);
    }
  }
}

void setup() {

  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);

  pinMode(PWMPin1, OUTPUT);
  pinMode(PWMPin2, OUTPUT);

  analogWrite(PWMPin1, 222);
  analogWrite(PWMPin2, 222);

  pinMode(18, INPUT);

  servo1.attach(2); 
  servo1.attach(15); 

  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_broker, mqtt_port);
  client.setCallback(callback);
  
  reconnect();

}

void loop() {
  if (!client.connected()) {
    reconnect();
  }

  if(digitalRead(18) == HIGH){
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
    digitalWrite(in3, HIGH);
    digitalWrite(in4, LOW);
    Serial.println("short distance");
    delay(1000);
  }
  
  client.loop();
}
