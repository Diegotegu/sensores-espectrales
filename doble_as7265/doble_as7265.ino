#include <Arduino.h>
#include "wiring_private.h"
#include <SPI.h>
#include <SD.h>

Uart mySerial (&sercom3, 0, 1, SERCOM_RX_PAD_1, UART_TX_PAD_0); // Create the new UART instance assigning it to pin 0 and 1
boolean debugger = false;
File dataFile;
const int chipSelect = 4;
String cabecera = "R_inc,S_inc,T_inc,U_inc,V_inc,W_inc,G_inc,H_inc,I_inc,J_inc,K_inc,L_inc,A_inc,B_inc,C_inc,D_inc,E_inc,F_inc,R_ref,S_ref,T_ref,U_ref,V_ref,W_ref,G_ref,H_ref,I_ref,J_ref,K_ref,L_ref,A_ref,B_ref,C_ref,D_ref,E_ref,F_ref";
// the setup routine runs once when you press reset:
void setup() {
  // initialize serial communication at 9600 bits per second:
  if (debugger) {
    Serial.begin(115200);
    while (!Serial);
  }
  mySerial.begin(115200);
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

  pinPeripheral(0, PIO_SERCOM); //Assign RX function to pin 0
  pinPeripheral(1, PIO_SERCOM); //Assign TX function to pin 1
  pinMode(6, OUTPUT);
  digitalWrite(6, LOW);
  pinMode(7, INPUT_PULLUP);
}

// the loop routine runs over and over again forever:
void loop() {
  String resp = "";
  String respuesta = "";
  String resp_1 = "";
  String respuesta_1 = "";
  String resp_completa = "";
  char c;
  int index, Button;
  Button = digitalRead(7);
  if (!Button) {
    digitalWrite(6, HIGH);
    delay(500);
    if (debugger) {
      Serial.println("enviando peticion a sensor de incidencia");
    }
    mySerial.println("ATCDATA");
    while (!mySerial.available());
    while (mySerial.available()) {
      c = mySerial.read();
      resp += c;
      delay(100);
    }
    c = 0;
    if (debugger) {
      Serial.println("enviando peticion a sensor de reflectancia");
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

    index = resp.indexOf("O");
    respuesta = resp.substring(1, index);


    index = resp_1.indexOf("O");
    respuesta_1 = resp_1.substring(0, index);
    if (debugger) {
      Serial.println("guardando en la SD");
    }

    dataFile = SD.open("datalog.txt", FILE_WRITE);
    resp_completa = respuesta + "," + respuesta_1;


    if (dataFile) {
      dataFile.println(resp_completa);
      dataFile.close();
    }
    digitalWrite(6, LOW);
    delay(500);

  }
}

// Attach the interrupt handler to the SERCOM
void SERCOM3_Handler()
{
  mySerial.IrqHandler();
}
