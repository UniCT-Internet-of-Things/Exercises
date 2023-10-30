#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 13
#define RST_PIN 0

MFRC522 rfid(SS_PIN, RST_PIN); // Instance of the class

MFRC522::MIFARE_Key key;
// put function declarations here:

// Init array that will store new NUID
byte nuidPICC[4];

void printHex(byte *buffer, byte bufferSize)
{
  for (byte i = 0; i < bufferSize; i++)
  {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
  }
}

/**
 * Helper routine to dump a byte arprintHexray as dec values to Serial.
 */
void printDec(byte *buffer, byte bufferSize)
{
  for (byte i = 0; i < bufferSize; i++)
  {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], DEC);
  }
}

void dump_byte_array(byte *buffer, byte bufferSize) {
    for (byte i = 0; i < bufferSize; i++) {
        Serial.print(buffer[i] < 0x10 ? " 0" : " ");
        Serial.print(buffer[i], HEX);
    }
}


void setup()
{
  Serial.begin(115200);
  SPI.begin(14, 2, 15, 13);
  rfid.PCD_Init(); // Init MFRC522

  for (byte i = 0; i < 6; i++)
  {
    key.keyByte[i] = 0xFF;
  }

  Serial.println(F("This code scan the MIFARE Classsic NUID."));
  Serial.print(F("Using the following key:"));
  printHex(key.keyByte, MFRC522::MF_KEY_SIZE);
}

void loop()
{

  // Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
  if (!rfid.PICC_IsNewCardPresent())
    return;

  // Verify if the NUID has been readed
  if (!rfid.PICC_ReadCardSerial())
    return;

  //writeBlock(block, blockcontent);

  Serial.print(F("PICC type: "));
  MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
  Serial.println(rfid.PICC_GetTypeName(piccType));

  // Check is the PICC of Classic MIFARE type
  if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI &&
      piccType != MFRC522::PICC_TYPE_MIFARE_1K &&
      piccType != MFRC522::PICC_TYPE_MIFARE_4K)
  {
    Serial.println(F("Your tag is not of type MIFARE Classic."));
    return;
  }

  if (rfid.uid.uidByte[0] != nuidPICC[0] ||
      rfid.uid.uidByte[1] != nuidPICC[1] ||
      rfid.uid.uidByte[2] != nuidPICC[2] ||
      rfid.uid.uidByte[3] != nuidPICC[3])
  {
    Serial.println(F("A new card has been detected."));

    // Store NUID into nuidPICC arrayuidByte
    for (byte i = 0; i < 4; i++)
    {
      nuidPICC[i] = rfid.uid.uidByte[i];
    }

    Serial.println(F("The NUID tag is:"));
    Serial.print(F("In hex: "));
    printHex(rfid.uid.uidByte, rfid.uid.size);
    Serial.println();
    Serial.print(F("In dec: "));
    printDec(rfid.uid.uidByte, rfid.uid.size);
    Serial.println();
    rfid.PICC_DumpToSerial(&(rfid.uid));
  }
  //else
    //Serial.println(F("Card read previously."));

    // In this sample we use the second sector,
    // that is: sector #1, covering block #4 up to and including block #7
    // 63 69 61 6F 20 6D 65 6C 6F
    byte sector         = 5;
    byte blockAddr      = 20;
    byte dataBlock[]    = "ciao meli\0\0\0\0\0\0\0";
    // {
    //     0x63, 0x69, 0x61, 0x6F, //  1,  2,   3,  4,
    //     0x20, 0x6D, 0x65, 0x6C, //  5,  6,   7,  8,
    //     0x6F, 0x00, 0x00, 0x00, //  9, 10, 255, 11,
    //     0x00, 0x00, 0x00, 0x00  // 12, 13, 14, 15
    // };

    MFRC522::StatusCode status;
    byte buffer[18];
    byte size = sizeof(buffer);

    // Show the whole sector as it currently is
    Serial.println(F("Current data in sector:"));
    rfid.PICC_DumpMifareClassicSectorToSerial(&(rfid.uid), &key, sector);
    Serial.println();

    // Write data to the block
    Serial.print(F("Writing data into block ")); Serial.print(blockAddr);
    Serial.println(F(" ..."));
    dump_byte_array(dataBlock, 16); 
    Serial.println();
    status = (MFRC522::StatusCode) rfid.MIFARE_Write(blockAddr, dataBlock, 16);
    if (status != MFRC522::STATUS_OK) {
        Serial.print(F("MIFARE_Write() failed: "));
        Serial.println(rfid.GetStatusCodeName(status));
    }
    Serial.println();

    // Read data from the block (again, should now be what we have written)
    Serial.print(F("Reading data from block ")); 
    Serial.print(blockAddr);
    Serial.println(F(" ..."));
    status = (MFRC522::StatusCode) rfid.MIFARE_Read(blockAddr, buffer, &size);
    if (status != MFRC522::STATUS_OK) {
        Serial.print(F("MIFARE_Read() failed: "));
        Serial.println(rfid.GetStatusCodeName(status));
    }
    Serial.print(F("Data in block ")); Serial.print(blockAddr); Serial.println(F(":"));
    dump_byte_array(buffer, 16); 
    Serial.println();
    const char * str = (const char *) buffer;
    Serial.println(str);

  // Halt PICC
  rfid.PICC_HaltA();

  // Stop encryption on PCD
  rfid.PCD_StopCrypto1();
}
