
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <WiFiManager.h>       



const char* ssid = "Batard";
const char* password = "66666666";
const char* mqtt_server = "broker.mqttdashboard.com";


WiFiClient espClient;
PubSubClient client(espClient);

long lastMsg = 0;
char msg[50];
int value = 0;

char* buttonTopic = "bariot-BATARD-feedback";

char* ledTopic = "bariot-BATARD-light1";

String pins_jobs[] = {"","","","","","","","","","","","","","","",""};


void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println("-----------");  //TODO replace with led indication
}


void callback(char* topic, byte* payload, unsigned int length) {

  String message = "";


  ///////////////////// read message char by char from MQTT /////////////////////
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.println(message);
  

  
  ///////////////////// split message to command /////////////////////
  int comma1i = message.indexOf(",");
  int comma2i = message.indexOf(",",comma1i+1);
  int comma3i = message.indexOf(",",comma2i+1);

  String pin_num = message.substring(0,comma1i);
  String command = message.substring(comma1i+1,comma2i);
  String dovalue = message.substring(comma2i+1,comma3i);

  Serial.print("pin_num: ");Serial.println(pin_num);
  Serial.print("command: ");Serial.println(command);
  Serial.print("dovalue: ");Serial.println(dovalue);

  int pin = pin_num.toInt();
  


  ///////////////////// execute command /////////////////////
  if ( (command == "pwm") || (command == "PWM") )
  {
                    Serial.print("analogWrite: ");Serial.print(pin);Serial.print(",");Serial.println(dovalue);
                    pinMode(pin, OUTPUT);
                    analogWrite(pin,dovalue.toInt());
  }
                    
  if ( (command == "set") || (command == "switch") )
  {
                    Serial.print("digitalWrite: ");Serial.print(pin);Serial.print(",");Serial.println(dovalue);
                    pinMode(pin, OUTPUT);
                    digitalWrite(pin,(dovalue=="1")?HIGH:LOW);
  }
  


  digitalWrite(12, ((char)payload[0] == '1') ? LOW : HIGH); Serial.print("pin12: "); Serial.println((char)payload[0]); //TODO verify above and remove
  digitalWrite(13, ((char)payload[1] == '1') ? LOW : HIGH); Serial.print("pin13: "); Serial.println((char)payload[1]);
  digitalWrite(14, ((char)payload[2] == '1') ? LOW : HIGH); Serial.print("pin14: "); Serial.println((char)payload[2]);

}

void reconnect() {  //TODO understand this function and rewrite
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      Serial.println(clientId);
      // Once connected, publish an announcement...
      client.publish("jjjbbb01", "conn");
      // ... and resubscribe
      client.subscribe(ledTopic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  WiFiManager wifiManager;

  ///wifiManager.autoConnect("AutoConnectAP");  //TODO enable connecting any SSID
  
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

bool currentState = false;
bool lastButtonState = false;

void loop() {

    if ((millis()%6000)>5800) { Serial.println("alive"); }  //TODO replace with led blink

    if (!client.connected()) { reconnect(); }

    client.loop();

/*    bool buttonState = digitalRead(buttonPin);
    if(buttonState == LOW && lastButtonState == HIGH)
    {
        currentState = !currentState;
        Serial.print("button pressed, state");
        Serial.println(currentState);
        
        if(currentState){
          client.publish(buttonTopic, "1");
        } else{
          client.publish(buttonTopic, "0");
        }
        lastButtonState = LOW;
        delay(500);
    } else if(buttonState == HIGH) {
        lastButtonState = HIGH;
    }*/

    
    delay(100);

}
