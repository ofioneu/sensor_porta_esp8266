#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <ESPAsyncTCP.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>

// Locais dos itens da configuração
#define VERSION_START  500
#define CONFIG_START   6

// ID de Configuração 
#define CONFIG_VERSION "1b"

// Estrutura de configuração da EEPROM
struct ConfigStruct
{
  char ssid[50]; //nome da rede
  char senha[50]; //senha da rede
  IPAddress ip; // ip do ESP8266
  IPAddress gateway; // gateway do ESP8266
  char url[100];    // url para enviar o post da leitura do sensor
  
} wifiConfig; //nome da estrutura

ESP8266WebServer server(80); // inicia o servidor na porta 80


String urlpost; //variável que guarda a url em formato de string 

byte door = 4; // porta do sensor
int httpCode; 


void saveConfig() 
{
  for (unsigned int t = 0; t < sizeof(wifiConfig); t++) {
    EEPROM.write(CONFIG_START + t, *((char*)&wifiConfig + t));
  }
  // Salvando o ID da versão para puxar da EEPROM
  // da proxima vez que for carregar
  EEPROM.write(VERSION_START + 0, CONFIG_VERSION[0]);
  EEPROM.write(VERSION_START + 1, CONFIG_VERSION[1]);
  EEPROM.commit();
}

//loop que salva as configurações de rede e urlpost
void loadConfig()
{
  if (EEPROM.read(VERSION_START + 0) == CONFIG_VERSION[0] &&
      EEPROM.read(VERSION_START + 1) == CONFIG_VERSION[1]) {
    // Carregando a estrutura main config
    for (unsigned int t = 0; t < sizeof(wifiConfig); t++)
      *((char*)&wifiConfig + t) = EEPROM.read(CONFIG_START + t);
  }
  else {
    // Configuração inicial
    String ssid = "ctc";
    String senha = "ctcwebapnew";
    ssid.toCharArray(wifiConfig.ssid, 50);
    senha.toCharArray(wifiConfig.senha, 50);
    wifiConfig.ip = IPAddress(192, 168, 1, 147);  
    wifiConfig.gateway = IPAddress(192, 168, 1, 1);
    String url = "http://192.168.1.18:5000/postjson";
   
    saveConfig();
  }
}

//pagina inicial
void handleRoot() 
{
  // HTML da pagina principal
  String html = "<html><head><title>CTC SENSOR DE PORTA</title>";
  html += "<style>body { background-color: #cccccc; ";
  html += "font-family: Arial, Helvetica, Sans-Serif; ";
  html += "Color: #000088; }</style>";
  html += "</head><body>";
  html += "<h1>CTC SENSOR DE PORTA - Configuracao WiFi</h1>";
  html += "<p>CTC-SENSOR DE PORTA</p>";
  html += "<p><a href=/config>Configurar WiFi</a></p>";
  html += "</body></html>";
  // Enviando HTML para o servidor
  server.send(200, "text/html", html);
}

// pagina configuração Wifi e urlpost
void configWifi() 
{
  String html = "<html><head><title>Configurar WiFi</title>";
  html += "<style>body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }</style>";
  html += "</head><body>";
  html += "<h1>Configurar WiFi</h1>";
  html += "<form method=POST>";
  html += "<p>SSID: <input name=txtSSID type=text value=\"";
  html += wifiConfig.ssid; // todas as variaveis exibem as informações já gravadas na eeprom
  html += "\" /></p>";
  html += "<p>Senha: <input name=txtSenha type=text value=\"";
  html += wifiConfig.senha;
  html += "\" /></p>";
  html += "<p>IP: <input name=txtIP type=text value=\"";
  html += wifiConfig.ip[0];
  html += ".";
  html += wifiConfig.ip[1];
  html += ".";
  html += wifiConfig.ip[2];
  html += ".";
  html += wifiConfig.ip[3];
  html += "\" /></p>";
  html += "<p>Gateway: <input name=txtGateway type=text value=\"";
  html += wifiConfig.gateway[0];
  html += ".";
  html += wifiConfig.gateway[1];
  html += ".";
  html += wifiConfig.gateway[2];
  html += ".";
  html += wifiConfig.gateway[3];
  html += "\" /></p>";
  html += "<p>URL POST: <input name=txtURL type=text value=\"";
  html += wifiConfig.url;
  html += "\" /></p>";
  html += "<p><input name=button1 type=submit value=Enviar /></p></form>";
  html += "<p><a href=/>Voltar</a></p>";
  html += "</body></html>";
  
  server.send(200, "text/html", html);

}

void configWifiSubmit() 
{
  String html = "<html><head><title>Configurar WiFi Submit</title>";
  html += "<style>body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }</style>";
  html += "</head><body>";
  html += "<h1> Configurar WiFi</h1>";
  html += "<p>Dados gravados com sucesso!</p>";

  String ssid = server.arg("txtSSID");
  String senha = server.arg("txtSenha");
  String ip = server.arg("txtIP");
  String gateway = server.arg("txtGateway");
  String url = server.arg("txtURL");
  ssid.toCharArray(wifiConfig.ssid, 50);
  senha.toCharArray(wifiConfig.senha, 50);
  wifiConfig.ip.fromString(ip);
  wifiConfig.gateway.fromString(gateway);
  url.toCharArray(wifiConfig.url, 100);
  
  html += "<p>SSID: <b>";
  html += wifiConfig.ssid; 
  html += "</b></p>";
  html += "<p>Senha: <b>";
  html += wifiConfig.senha; 
  html += "</b></p>";
  html += "<p>IP: <b>";
  html += ip;
  html += "</b></p>";
  html += "<p>Gateway: <b>";
  html += gateway;
  html += "</b></p>";
  html += "<p>URL POST: <b>";
  html += wifiConfig.url;
  html += "</b></p>";
  html += "<form method=GET>";
  html += "<p><input name=button2 type=submit value=Voltar /></p></form>";
  html += "</body></html>";
  
  server.send(200, "text/html", html);

  saveConfig(); //executa a função que salva as configurações
  ESP.restart(); // reestarta o ESP8266
}

//**************************************************************************************************************************
void setup () {
  
  Serial.begin(115200);
  // Iniciando EEPROM
  EEPROM.begin(512);
  // Carrega configuração da EEPROM
  // Se não existir, cria
  loadConfig();

  // Iniciando WiFi
  WiFi.begin(wifiConfig.ssid, wifiConfig.senha);
  IPAddress subnet(255, 255, 255, 0);  
  WiFi.config(wifiConfig.ip, wifiConfig.gateway, subnet); 
  
//Declara a porta door como entrada
  pinMode(door, INPUT_PULLUP);

//checa se o Wifi conectou
  while (WiFi.status() != WL_CONNECTED) {

    delay(1000);
    Serial.println("Connecting..");


  }

    // Apresenta dados da conexão
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(wifiConfig.ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  
  // Atribuindo urls para funções
  // Quando não especificado método, uma função trata todos
  server.on("/", handleRoot);

  // Chamada dos métodos de configuração
  server.on("/config", HTTP_GET, configWifi);
  server.on("/config", HTTP_POST, configWifiSubmit);
  
  // Iniciando servidor
  server.begin();
  
  // Apenas informando que servidor iniciou
  Serial.println("HTTP server started");

}


void loop() {
  
  if (WiFi.status() == WL_CONNECTED) { //Check WiFi connection status

    HTTPClient http;    //Declare object of class HTTPClient
    urlpost = wifiConfig.url;
    //http.begin("http://192.168.1.18:5000/postjson");      
    http.begin(urlpost); //especifica a url de destino
    Serial.print("URL POST: "); //imprime no munitor serial a url de destino
    Serial.println(urlpost); //imprime no munitor serial a url de destino
    http.addHeader("Content-Type", "application/json");  //Especifica o cabeçalho
    //http.addHeader("Content-Type", "text/plain");  //Specify content-type header

    // Abaixo um exemplo para caso precise enviar um json, mas ainda não está terminado

    /*const int capacity = 6; //JSON_OBJECT_SIZE(3);
      StaticJsonDocument<capacity> doc;

      doc["teste1"] = 1;
      doc["teste2"] = 2;
      doc["teste3"] = 3;
    */

    if (digitalRead(door) == HIGH) { //verifica se a porta está em estado alto 
      httpCode = http.POST("PORTA ABERTA");   //Envia a mensagem para a url especificada em urlpost

      String payload = http.getString();                  //pega a resposta

      Serial.println(httpCode);   //imprime o retorno da requisição
      Serial.println(payload);    //Print request response payload
      //serializeJson(doc, Serial);
      http.end();  //Close connection
    }
    //se a porta door estiver em LOW retorna que a porta está fechada
    else {
      httpCode = http.POST("PORTA FECHADA!"); //Envia a mensagem para a url especificada em urlpost
    }

    delay(1000);  //Aguarda 1 segundo
  }
  else {

    Serial.println("ERRO AO CONECTAR AO WIFI!");

  }
}
