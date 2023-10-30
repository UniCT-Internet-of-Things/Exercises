#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>

MFRC522 mfrc522(13, 0); 

void setup() {
  Serial.begin(115200);
  pinMode(25, OUTPUT);
  digitalWrite(25, LOW);

  SPI.begin(14, 2, 15, 13);
  mfrc522.PCD_Init();


  Serial.println("Avvio del dispositivo riuscito");
  
}

void loop() {

  
  delay(1000);

  	// Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
	if ( ! mfrc522.PICC_IsNewCardPresent()) {
		return;
	}

	// Select one of the cards
	if ( ! mfrc522.PICC_ReadCardSerial()) {
		return;
	}

	// Dump debug info about the card; PICC_HaltA() is automatically called
  digitalWrite(25, HIGH);
	mfrc522.PICC_DumpToSerial(&(mfrc522.uid));
  digitalWrite(25, LOW);
}
