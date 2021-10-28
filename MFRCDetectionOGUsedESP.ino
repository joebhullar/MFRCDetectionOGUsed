#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN         D9           // Configurable, see typical pin layout above
#define SS_PIN          D8          // Configurable, see typical pin layout above

MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance

//*****************************************************************************************//
void setup() {
  Serial.begin(9600);                                           // Initialize serial communications with the PC
  SPI.begin();                                                  // Init SPI bus
  mfrc522.PCD_Init();                                              // Init MFRC522 card
}
void PrintHex(uint8_t *data, uint8_t length) // prints 8-bit data in hex with leading zeroes
{
     char tmp[16];
       for (int i=0; i<length; i++) { 
         sprintf(tmp, "0x%.2X",data[i]); 
         Serial.print(tmp); Serial.print(" ");
       }
}
uint8_t buf[10]= {};
MFRC522::Uid id;
MFRC522::Uid id2;
bool is_card_present = false;
//*****************************************************************************************//

void cpid(MFRC522::Uid *id){
  memset(id, 0, sizeof(MFRC522::Uid));
  memcpy(id->uidByte, mfrc522.uid.uidByte, mfrc522.uid.size);
  id->size = mfrc522.uid.size;
  id->sak = mfrc522.uid.sak;
}

bool cmpid(MFRC522::Uid *id1, MFRC522::Uid *id2){
  return memcmp(id1, id2, sizeof(MFRC522::Uid));
}

void deregister_card(){
  is_card_present = false;
  memset(&id,0, sizeof(id));
}

void finishfunct(uint8_t stag){
   Serial.println("CardRemoved");
   Serial.println(stag);
  delay(500); //change value if you want to read cards faster

  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();  
}
uint8_t control = 0x00;
void loop() {

  MFRC522::MIFARE_Key key;
  for (byte i = 0; i < 6; i++) key.keyByte[i] = 0xFF;
  MFRC522::StatusCode status;

  //-------------------------------------------

  // Look for new cards
  if ( !mfrc522.PICC_IsNewCardPresent()) {
    return;
  }
  if ( !mfrc522.PICC_ReadCardSerial()) {
    return;
  }
  //PrintHex(id.uidByte, id.size);
  //Serial.println("hello");
  bool result = true;
  uint8_t buf_len=4;
  cpid(&id);
  mfrc522.PICC_DumpDetailsToSerial(&(mfrc522.uid)); //dump some details about the card
  //START TRY THIS1
  unsigned int hex_num;
          hex_num =  mfrc522.uid.uidByte[0] << 24;
          hex_num += mfrc522.uid.uidByte[1] << 16;
          hex_num += mfrc522.uid.uidByte[2] <<  8;
          hex_num += mfrc522.uid.uidByte[3];

         int  NFC_id=  (int)hex_num;
         String strtwo =  String(NFC_id, HEX);
         strtwo.toUpperCase();
         Serial.println(strtwo);
  
  Serial.print("NewCard ");
  PrintHex(id.uidByte, id.size);
  Serial.println("");
  while(true){
    control=0;
    for(int i=0; i<3; i++){
      if(!mfrc522.PICC_IsNewCardPresent()){
        if(mfrc522.PICC_ReadCardSerial()){
//          Serial.print('a');
          control |= 0x16;
        }
        if(mfrc522.PICC_ReadCardSerial()){
//          Serial.print('b');
          control |= 0x16;
        }
//        Serial.print('c');
          control += 0x1;
      }
//      Serial.print('d');
      control += 0x4;
    }
    if(control == 13 || control == 14){
      delay(500);
      control = 0;
      } else {
      break;
    }
  }
  
finishfunct(control);
}
