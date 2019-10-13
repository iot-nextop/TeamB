#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>

#define WIFI_SSID "Nextop1"
#define WIFI_PASSWORD "20183365"
#define FIREBASE_HOST "firealarm-6114f.firebaseio.com"
#define FIREBASE_AUTH "UNaZfYmna3qzzXrHFutUBwAtNLw1yZ38HftGvSeM"

int redLed = 12;
int greenLed = 11;
int buzzer = 10;
int smokeA0 = A0;
const int buzzerPin = 12;
const int flamePin = 11;

int FB_buzzer;
int FB_led;
int FB_gas;
int FB_fire;

WiFiServer server(80);
 
void setup() {
  Serial.begin(115200);
  delay(10);
  wifiConnect();
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  FB_gas = Firebase.getInt("gas");
  FB_fire = Firebase.getInt("fire");
  FB_led = Firebase.getInt("led");
  FB_buzzer = Firebase.getInt("buzzer");


  pinMode(buzzerPin, OUTPUT);
  pinMode(flamePin, INPUT);
  pinMode(redLed, OUTPUT);
  pinMode(greenLed, OUTPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(smokeA0, INPUT);  
}
 
void loop() {
  String line = "";
  
  // Check if a client has connected
  WiFiClient client = server.available(); 
  if (!client) {
    return;
  }
 
  // Wait until the client sends some data
  Serial.println("new client");
  while(!client.available()){
    delay(1);
  }
 
  // Read the first line of the request
  String request = client.readStringUntil('\r');
  Serial.println(request);
  client.flush();
  
  
  
  // Return the response
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println( "Refresh: 5");        
  client.println(""); //  do not forget this one
  client.println("<!DOCTYPE HTML>");
  client.println("<html>");

  client.println("<p>gas value is:<b> ");
  client.println(FB_gas);
  client.println("<p>fire value is:<b> ");
  client.println(FB_fire); 
  client.println("</b></p>");
  

    
  client.println("<a href=\"/C\"> <button>LED ON/OFF </button></a>");
  
  client.println("</html>");
 
  delay(1);
  Serial.println("Client disonnected");
  Serial.println("");

  int analogSensor = analogRead(smokeA0);
  
  Serial.print("Pin A0: ");
  Serial.println(analogSensor);

/*
 * int Flame = HIGH;
 * 
   if (analogSensor > sensorThres)
    {
      digitalWrite(redLed, HIGH);
      digitalWrite(greenLed, LOW);
      tone(buzzer, 1000, 20000);
    }
    else
    {
      digitalWrite(redLed, LOW);
      digitalWrite(greenLed, HIGH);
      noTone(buzzer);
    }
    delay(1000);
    Flame = digitalRead(flamePin);
    if (Flame== LOW)
    {
      Serial.println("Fire!!!");
      digitalWrite(buzzerPin, HIGH);
    }
    else
    {
      Serial.println("No worries");
      digitalWrite(buzzerPin, LOW);
    }
    delay(1000);
  }*/
  
  if(analogSensor> 800)
   {
      digitalWrite(redLed, HIGH);
      digitalWrite(greenLed, LOW);

      FB_led = 1;
      FB_gas = analogSensor;
      
      Firebase.setInt("gas",FB_gas);
      Firebase.setInt("LED",FB_led);
      Serial.println("fck u");
   }
   else{
    digitalWrite(redLed, LOW);
    digitalWrite(greenLed, HIGH);
    
    FB_led = 0;
    FB_gas = analogSensor;
    
    Firebase.setInt("gas",FB_gas);
    Firebase.setInt("LED",FB_led);
   }
   
   if(FB_fire > 1000){
      tone(buzzer, 1000, 20000);
      
      Firebase.setInt("buzzer",FB_buzzer);
      Firebase.setInt("fire",FB_fire);
      Serial.println("fck u2");
   }
   else{
     noTone(buzzer);

     Firebase.setInt("buzzer",FB_buzzer);
     Firebase.setInt("fire",FB_fire);
   }
}



void wifiConnect()
{
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);             // Connect to the network
  Serial.print("Connecting to ");
  Serial.print(WIFI_SSID); Serial.println(" ...");
  
  while (WiFi.status() != WL_CONNECTED)
  {                                       // Wait for the Wi-Fi to connect
    delay(1000);
    Serial.print('.'); Serial.print(' ');
  }
    // Start the server
  server.begin();
  Serial.println("Server started");
  
  Serial.println('\n');
  Serial.println("Connection established!");  
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP());         // Send the IP address of the ESP8266 to the computer
}
