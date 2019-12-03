#include <Arduino.h>
#include "wiring_private.h"
#include <SPI.h>
#include <SD.h>


boolean debugger = false;
File dataFile;
const int chipSelect = 4;
String cabecera = "R,S,T,U,V,W,G,H,I,J,K,L,A,B,C,D,E,F";
int Button_back;
// the setup routine runs once when you press reset:
void setup() {
  // initialize serial communication at 9600 bits per second:
  if (debugger) {
    Serial.begin(115200);
    while (!Serial);
  }
  Serial1.begin(115200);

  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    while (1);
  }
  if (debugger) {
    Serial.println("card initialized.");
  }
  dataFile = SD.open("datalog.txt", FILE_WRITE);

  if (dataFile) {
    dataFile.println(cabecera);
    dataFile.close();
  }

  pinMode(6, OUTPUT);
  digitalWrite(6, LOW);
  pinMode(7, INPUT_PULLUP);
  Button_back=digitalRead(7);
}

// the loop routine runs over and over again forever:
void loop() {
  String resp_1 = "";
  String respuesta_1 = "";
  char c;
  int index, Button;
  
  Button = digitalRead(7);
  if (Button!=Button_back) {
    digitalWrite(6, HIGH);
    Button_back=Button;
    delay(500);
    if (debugger) {
      Serial.println("enviando peticion a sensor");
    }
    Serial1.println("ATCDATA");
    while (!Serial1.available());
    while (Serial1.available()) {
      c = Serial1.read();
      resp_1 += c;
      delay(100);
    }
    if (debugger) {
      Serial.println("procesando informacion");
    }
    index = resp_1.indexOf("O");
    respuesta_1 = resp_1.substring(0, index);
    if (debugger) {
      Serial.println("guardando en la SD");
    }

    dataFile = SD.open("datalog.txt", FILE_WRITE);
    if (dataFile) {
      dataFile.println(respuesta_1);
      dataFile.close();
    }
    digitalWrite(6, LOW);
    delay(500);

  }
}
