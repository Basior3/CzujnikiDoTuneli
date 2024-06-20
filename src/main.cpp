#include <Arduino.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include <FS.h>

#define DHTPIN D4    // Pin, do którego podłączony jest czujnik DHT11 (D4 odpowiada GPIO2)
#define DHTTYPE DHT11   // DHT 11

DHT dht(DHTPIN, DHTTYPE);

void setup() {
    Serial.begin(115200);
    if (!SPIFFS.begin()) {
        Serial.println("Failed to mount file system");
        return;
    }
    dht.begin();
}

void loop() {
    // Odczyt danych z czujnika
    float humidity = dht.readHumidity();
    float temperature = dht.readTemperature();

    // Sprawdzenie, czy odczyt się powiódł
    if (isnan(humidity) || isnan(temperature)) {
        Serial.println("Nie można odczytać danych z czujnika DHT11!");
        return;
    }

    // Wyświetlenie danych na serial monitorze
    Serial.print("Wilgotność: ");
    Serial.print(humidity);
    Serial.print(" %\t");
    Serial.print("Temperatura: ");
    Serial.print(temperature);
    Serial.println(" *C");

    // Zapis do pliku
    File file = SPIFFS.open("/dht11_data.txt", "a");
    if (!file) {
        Serial.println("Failed to open file for writing");
        return;
    }
    file.print("Wilgotność: ");
    file.print(humidity);
    file.print(" %, Temperatura: ");
    file.print(temperature);
    file.println(" *C");
    file.close();

    delay(2000); // Opóźnienie przed kolejnym odczytem
}