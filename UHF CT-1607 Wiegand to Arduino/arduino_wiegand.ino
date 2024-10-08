#define MAX_BITS 100                 // max number of bits 
#define WEIGAND_WAIT_TIME  3000      // time to wait for another weigand pulse.  
unsigned char databits[MAX_BITS];    // stores all of the data bits
unsigned char bitCount;              // number of bits currently captured
unsigned char flagDone;              // goes low when data is currently being captured
unsigned int weigand_counter;        // countdown until we assume there are no more bits

unsigned long facilityCode=0;        // decoded facility code
unsigned long cardCode=0;            // decoded card code

// interrupt that happens when INTO goes low (0 bit)
void ISR_INT0()
{
  //Serial.print("0");   // uncomment this line to display raw binary
  bitCount++;
  flagDone = 0;
  weigand_counter = WEIGAND_WAIT_TIME;  
  
}

// interrupt that happens when INT1 goes low (1 bit)
void ISR_INT1()
{
  
  databits[bitCount] = 1;
  bitCount++;
  flagDone = 0;
  weigand_counter = WEIGAND_WAIT_TIME;  
}

void setup()
{
  
  pinMode(2, INPUT);     // DATA0 (INT0)
  pinMode(3, INPUT);     // DATA1 (INT1)
  
  Serial.begin(9600);
  Serial.println("RFID Readers & Arduino Access Door");
  Serial.println("---------By Andum Kawruh----------");
  Serial.println();
  // binds the ISR functions to the falling edge of INTO and INT1
  attachInterrupt(0, ISR_INT0, FALLING);  
  attachInterrupt(1, ISR_INT1, FALLING);
  weigand_counter = WEIGAND_WAIT_TIME;
}

void loop()
{
  
  if (!flagDone) {
    if (--weigand_counter == 0)
      flagDone = 1;	
  }
  
 
  if (bitCount > 0 && flagDone) {
    unsigned char i;
    
    Serial.print("Read ");
    Serial.print(bitCount);
    Serial.print(" bits. ");
    
    if (bitCount == 34)
    {
      // 35 bit HID Corporate 1000 format
      // facility code = bits 2 to 14
      for (i=2; i<14; i++)
      {
         facilityCode <<=1;
         facilityCode |= databits[i];
      }
      
      // card code = bits 15 to 34
      for (i=13; i<34; i++)
      {
         cardCode <<=1;
         cardCode |= databits[i];
      }
      
      printBits();
    }
    else if (bitCount == 26)
    {
      // standard 26 bit format
      // facility code = bits 2 to 9
      for (i=1; i<9; i++)
      {
         facilityCode <<=1;
         facilityCode |= databits[i];
      }
      
      // card code = bits 10 to 23
      for (i=9; i<25; i++)
      {
         cardCode <<=1;
         cardCode |= databits[i];
      }
      
      printBits();  
    }
    else {
      
      Serial.println("Unable to decode."); 
    }

     // cleanup and get ready for the next card
     bitCount = 0;
     facilityCode = 0;
     cardCode = 0;
     for (i=0; i<MAX_BITS; i++) 
     {
       databits[i] = 0;
     }
  }
}

void printBits()
{
      
      Serial.print("FC = ");
      Serial.print(facilityCode);
      Serial.print(", CC = ");
      Serial.println(cardCode); 
      Serial.println();
     if ((cardCode == 786977) || (cardCode == 0000)|| (cardCode == 0000))
      {
        
        Serial.println("Akses Diijinkan");
        Serial.println("Pintu Terbuka");
        delay(3000);
        Serial.println("Pintu Tertutup");
        Serial.println();
      }
      else {
            Serial.println("Access Ditolak!!!");
            Serial.println();
            delay(3000)
      }
}
