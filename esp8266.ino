#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

const char* ssid = "ctc";
const char* password = "ctcwebapnew";
byte door = 4;
int httpCode;
void setup () {

  Serial.begin(115200);
  WiFi.begin(ssid, password);

  pinMode(door, INPUT_PULLUP);

  while (WiFi.status() != WL_CONNECTED) {

    delay(1000);
    Serial.println("Connecting..");
    

  }


}


void loop() {

  if (WiFi.status() == WL_CONNECTED) { //Check WiFi connection status

    HTTPClient http;    //Declare object of class HTTPClient

    http.begin("http://192.168.1.18:5000/postjson");      //Specify request destination
    http.addHeader("Content-Type", "application/json");  //Specify content-type header
    //http.addHeader("Content-Type", "text/plain");  //Specify content-type header

    /*const int capacity = 6; //JSON_OBJECT_SIZE(3);
   StaticJsonDocument<capacity> doc;
    
    doc["teste1"] = 1;
    doc["teste2"] = 2;
    doc["teste3"] = 3;
    */
        
    if(digitalRead(door)==HIGH){
    httpCode = http.POST("PORTA ABERTA");   //Send the request
    
    String payload = http.getString();                  //Get the response payload

    Serial.println(httpCode);   //Print HTTP return code
    Serial.println(payload);    //Print request response payload
    //serializeJson(doc, Serial);
    http.end();  //Close connection
    }
   else{
      httpCode = http.POST("PORTA FECHADA!"); //Send the request
    }

   

  delay(1000);  //Send a request every 5 seconds
}
else {

    Serial.println("ERRO AO CONECTAR AO WIFI!");

  }
}
