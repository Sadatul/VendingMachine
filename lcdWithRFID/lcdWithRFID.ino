#include <SPI.h>      //include the SPI bus library
#include <MFRC522.h>  //include the RFID reader library
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// RFID
#define SS_PIN 10  //slave select pin
#define RST_PIN 9  //reset pin

const int rfidReadButton = 3;
const int rfidWriteButton = 4;

const char clearRow[16] = "                ";

MFRC522 mfrc522(SS_PIN, RST_PIN);  // instatiate a MFRC522 reader object.
MFRC522::MIFARE_Key key;          //create a MIFARE_Key struct named 'key', which will hold the card information

//this is the block number we will write into and then read.
int block=2;  

// FOR LCD
LiquidCrystal_I2C lcd(0x27, 16, 2); // set the LCD address to 0x27 for a 16 chars and 2 line display

// For debouncing the button
int rfidReadButtonState;            
int rfidReadLastButtonState = LOW;  
unsigned long rfidReadLastDebounceTime = 0;  
unsigned long debounceDelay = 50;   

int rfidWriteButtonState;            
int rfidWriteLastButtonState = LOW;  
unsigned long rfidWriteLastDebounceTime = 0;

unsigned long int curBalance = 0;

void setup() 
{
    
    Serial.begin(9600);        // Initialize serial communications with the PC
    lcd.init();         // initialize the lcd
    lcd.backlight();    // Turn on the LCD screen backlight
    SPI.begin();               // Init SPI bus
    mfrc522.PCD_Init();        // Init MFRC522 card (in case you wonder what PCD means: proximity coupling device)
    Serial.println("Scan a MIFARE Classic card");
    pinMode(rfidReadButton, INPUT);
    pinMode(rfidWriteButton, INPUT);
  
  // Prepare the security key for the read and write functions.
    for (byte i = 0; i < 6; i++) {
      key.keyByte[i] = 0xFF;  //keyByte is defined in the "MIFARE_Key" 'struct' definition in the .h file of the library
    }
}

void loop()
{  
  // Look for new cards
  lcd.setCursor(0,1);
  lcd.print("Balance: ");
  char s[6];
  sprintf(s, "%ld", curBalance);
  for (int i = strlen(s); i < 5; i++){
    s[i] = " ";
  }
  s[5] = '\0';
  // Serial.println(strlen(s));
  lcd.print(s);

  int rfidReadReading = digitalRead(rfidReadButton);
  if(rfidReadReading != rfidReadLastButtonState){
    rfidReadLastDebounceTime = millis();
  }

  if((millis() - rfidReadLastDebounceTime) > debounceDelay){
    if(rfidReadReading != rfidReadButtonState){
      rfidReadButtonState = rfidReadReading;
      if(rfidReadButtonState == HIGH){
        readBalFromRFID();
      }
    }
  }

  rfidReadLastButtonState = rfidReadReading;

  int rfidWriteReading = digitalRead(rfidWriteButton);
  if(rfidWriteReading != rfidWriteLastButtonState){
    rfidWriteLastDebounceTime = millis();
  }

  if((millis() - rfidWriteLastDebounceTime) > debounceDelay){
    if(rfidWriteReading != rfidWriteButtonState){
      rfidWriteButtonState = rfidWriteReading;
      if(rfidWriteButtonState == HIGH){
        writeBalToRFID();
      }
    }
  }

  rfidWriteLastButtonState = rfidWriteReading;
  // readBalFromRFID();
  
}

void readBalFromRFID(){
  if ( ! mfrc522.PICC_IsNewCardPresent()) {
    // This helps with realiablity during button press.
    if ( ! mfrc522.PICC_IsNewCardPresent()) {
      lcd.setCursor(0, 0);
      lcd.clear();
      lcd.print("No Card Found");
      Serial.println("Blocked");
      delay(3000);
      lcd.clear();
      return;
    }
  }
  
  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) 
  {
    return;
  }
  Serial.println("card selected");
   
  //read the block back
  byte readData[18];
  readBlock(block, readData);
  unsigned long int result = extractNum(readData);
  Serial.print("Added: ");
  Serial.println(result, DEC);
  curBalance += result;
  if(result == 0){
    lcd.setCursor(0,0);
    lcd.clear();
    lcd.print("Empty Card");
    delay(3000);
    lcd.clear();
  }
  // As the card has been read, we make its value zero

  byte balanceData[8];
  byte hashData[8];
  byte writeData[16];
  numberToBytes(0, balanceData);
  hashInfo(hashData);
  mergeData(hashData, balanceData, writeData);
  writeBlock(block, writeData);

  // Halt PICC
  mfrc522.PICC_HaltA();
  // Stop encryption on PCD
  mfrc522.PCD_StopCrypto1();
}
unsigned long int extractNum(byte readData[]){
  byte tmp[8];
  for (int i = 0; i < 8; i++)
    tmp[i] = readData[i];
  // Serial.print(tmp[0], DEC);
  unsigned long int result = bytesToNumber(tmp);
  return result;

}

void writeBalToRFID(){
  if ( ! mfrc522.PICC_IsNewCardPresent()) {
    // This helps with realiablity during button press.
    if ( ! mfrc522.PICC_IsNewCardPresent()) {
      lcd.setCursor(0, 0);
      lcd.clear();
      lcd.print("No Card Found");
      Serial.println("Blocked");
      delay(3000);
      lcd.clear();
      return;
    }
  }
  
  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) 
  {
    return;
  }
  Serial.println("card selected");

  byte balanceData[8];
  byte hashData[8];
  byte writeData[16];
  numberToBytes(curBalance, balanceData);
  hashInfo(hashData);
  mergeData(hashData, balanceData, writeData);
  writeBlock(block, writeData);

  curBalance = 0;

  // Halt PICC
  mfrc522.PICC_HaltA();
  // Stop encryption on PCD
  mfrc522.PCD_StopCrypto1();
}

void numberToBytes(unsigned long int num, byte s[8])
{
    for (int i = 0; i < 8; i++)
    {
        int tmp = num % 256;
        s[i] = (byte)tmp;
        num = num >> 8;
    }
}

void hashInfo(byte s[8]){
  for (int i = 0; i < 8; i++){
    s[i] = 0;
  }
}

void mergeData(byte hashInfo[], byte numInfo[], byte writeInfo[]){
  for (int i = 0; i < 8; i++){
    writeInfo[i] = numInfo[i];
  }

  for (int i = 8; i < 16; i++){
    writeInfo[i] = hashInfo[i - 8];
  }
}
unsigned long int bytesToNumber(byte s[8])
{
    unsigned long int num = 0;
    for (int i = 7; i >= 0; i--)
    {
        num = num << 8;
        // Serial.println(s[i], DEC);
        num = num | (unsigned long int)s[i];
    }
    // Serial.println(num, DEC);
    return num;
}

//Write specific block    
int writeBlock(int blockNumber, byte arrayAddress[]) 
{
  //this makes sure that we only write into data blocks. Every 4th block is a trailer block for the access/security info.
  int largestModulo4Number=blockNumber/4*4;
  int trailerBlock=largestModulo4Number+3;//determine trailer block for the sector
  if (blockNumber > 2 && (blockNumber+1)%4 == 0){Serial.print(blockNumber);Serial.println(" is a trailer block:");return 2;}
  Serial.print(blockNumber);
  Serial.println(" is a data block:");
  
  //authentication of the desired block for access
  byte status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, trailerBlock, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) {
         Serial.print("PCD_Authenticate() failed: ");
         Serial.println(mfrc522.GetStatusCodeName(status));
         return 3;//return "3" as error message
  }
  
  //writing the block 
  status = mfrc522.MIFARE_Write(blockNumber, arrayAddress, 16);
  //status = mfrc522.MIFARE_Write(9, value1Block, 16);
  if (status != MFRC522::STATUS_OK) {
           Serial.print("MIFARE_Write() failed: ");
           Serial.println(mfrc522.GetStatusCodeName(status));
           return 4;//return "4" as error message
  }
  Serial.println("block was written");
}



//Read specific block
int readBlock(int blockNumber, byte arrayAddress[]) 
{
  int largestModulo4Number=blockNumber/4*4;
  int trailerBlock=largestModulo4Number+3;//determine trailer block for the sector

  //authentication of the desired block for access
  byte status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, trailerBlock, &key, &(mfrc522.uid));

  if (status != MFRC522::STATUS_OK) {
         Serial.print("PCD_Authenticate() failed (read): ");
         Serial.println(mfrc522.GetStatusCodeName(status));
         return 3;//return "3" as error message
  }

//reading a block
byte buffersize = 18;//we need to define a variable with the read buffer size, since the MIFARE_Read method below needs a pointer to the variable that contains the size... 
status = mfrc522.MIFARE_Read(blockNumber, arrayAddress, &buffersize);//&buffersize is a pointer to the buffersize variable; MIFARE_Read requires a pointer instead of just a number
  if (status != MFRC522::STATUS_OK) {
          Serial.print("MIFARE_read() failed: ");
          Serial.println(mfrc522.GetStatusCodeName(status));
          return 4;//return "4" as error message
  }
  Serial.println("block was read");
}