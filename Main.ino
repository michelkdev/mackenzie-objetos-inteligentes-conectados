#include <DHT.h>
#include <LiquidCrystal.h>
#include <EspMQTTClient.h>

#define LEDPLACA 2
#define LED1 D7

EspMQTTClient client(
  "Steve Rogers II 2G",  // SSID WiFi
  "4qynbpdsuttcp",       // PAssword WiFi
  "test.mosquitto.org",  // MQTT Broker
  "mqtt-wokwi",          // Client
  1883                   // MQTT port
);

int temperatura = 0;
int humidade = 0;
int count = 0;

DHT dht(D6, DHT11);

LiquidCrystal lcd(D0, D1, D2, D3, D4, D5);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  pinMode(LED1, OUTPUT);

  dht.begin();

  lcd.begin(16,2);
  lcd.clear();

  client.enableDebuggingMessages(); // Enable debugging messages sent to serial output
  client.enableHTTPWebUpdater(); // Enable the web updater. User and password default to values of MQTTUsername and MQTTPassword. These can be overridded with enableHTTPWebUpdater("user", "password").
  client.enableOTA(); // Enable OTA (Over The Air) updates. Password defaults to MQTTPassword. Port is the default OTA port. Can be overridden with enableOTA("password", port).
  client.enableLastWillMessage("TestClient/lastwill", "Vou ficar offline");
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(5000);

  //armazena dados sensor humidade
  temperatura = dht.readTemperature();
  humidade = dht.readHumidity();

  /**
    * Servidor
    */
  client.loop(); // Executa em loop
  if ( client.isConnected() )
    lerEnviarDados();

  Serial.println("Status Servidor: " + String(client.isConnected()));
  Serial.println("Qtde Servidor Enviada: " + String(client.getConnectionEstablishedCount()));

  _Debug();

  _showLCD();

  if ( temperatura > 30 && !digitalRead(LED1) ) {
    lcd.clear();
    lcd.setCursor(3,0);
    lcd.print("Temp. alta!");
    lcd.setCursor(1,1);
    lcd.print("ON ventilador!");
    digitalWrite(LED1, HIGH);
    delay(3000);
  } else if ( digitalRead(LED1) && temperatura < 27 ) {
    lcd.clear();
    lcd.setCursor(2,0);
    lcd.print("Temp. Normal.");
    lcd.setCursor(1,1);
    lcd.print("OFF ventilador!");
    digitalWrite(LED1, LOW);
    delay(3000);
  }
  
  if ( humidade < 50 || humidade > 80 ) {
    lcd.clear();
    lcd.setCursor(3,0);
    lcd.print("Atencao!!!");
    if ( humidade < 50 ) {
      lcd.setCursor(1,1);
      lcd.print("Hum.baixa: ");
      lcd.setCursor(12,1);
      lcd.print(humidade);
      lcd.setCursor(14,1);
      lcd.print("%");
    } else if ( humidade > 80 ) {
      lcd.setCursor(1,1);
      lcd.print("Hum. alta: ");
      lcd.setCursor(12,1);
      lcd.print(humidade);
      lcd.setCursor(14,1);
      lcd.print("%");
    }
}

int _Debug() {
  Serial.println("Temperature/Humidity " + String(temperatura) + "C/" + String(humidade) + "%");
  return false;
}

void _showLCD() {
  lcd.clear();
  lcd.setCursor(3,0);
  lcd.print("Temp/Humid");

  lcd.setCursor(4,1);
  lcd.print(temperatura);
  lcd.setCursor(6, 1);
  lcd.print("C/");

  lcd.setCursor(8,1);
  lcd.print(humidade);
  lcd.setCursor(10,1);
  lcd.print("%");
  delay(3000);
}

/**
 * @brief Ler os dados do Sensor imprime e envia via MQTT
 */
void lerEnviarDados() {
  count++;

  client.publish("Mackenzie/" + String(count) + "/Temp", String(temperatura) + "°C"); 
  client.publish("Mackenzie/" + String(count) + "/Hum", String(humidade) + "%");
}

void onConnectionEstablished() {

}
