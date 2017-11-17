/* 
 *  w5500 example
 *  Written by David Dold
 * 
 *  PIN OUTS for Atmel328
 *  SCK   13 - 2
 *  M1S0  12 - 8
 *  M0S1  11 - 6
 *  SS    10 - 5
 *  RST   3  - 3
 * 
 * TROUBLE SHOOTING 
 *  Confirm Arduino has SS correct for your processor
 *  .\Arduino\libraries\Ethernet2\src\w5500.cpp
 * 
      void W5500Class::init(uint8_t ss_pin)
      {
        //Confirm Arduino sees correct processor      
        Serial.print("W5500Class::init: ss_pin: ");
        Serial.println(ss_pin);
      
        SPI_CS = ss_pin;
      
        delay(1000);
        initSS();
        SPI.begin();
        w5500.swReset();
        for (int i=0; i<MAX_SOCK_NUM; i++) {
          uint8_t cntl_byte = (0x0C + (i<<5));
          write( 0x1E, cntl_byte, 2); //0x1E - Sn_RXBUF_SIZE
          write( 0x1F, cntl_byte, 2); //0x1F - Sn_TXBUF_SIZE
        }
      }
 * 
 * 
 *  (optional)
 *  Name your device changes to dhcp.h and dhcp.cpp
 *  .\Arduino\libraries\Ethernet2\src\dhcp.h
 *  //#define HOST_NAME "WIZnet"
 *   #define HOST_NAME "w5500-Test"
 *  
 *  .\Arduino\libraries\Ethernet2\src\dhcp.cpp 
        buffer[16] = hostName;
        buffer[17] = strlen(HOST_NAME) + 6; // length of hostname + last 3 bytes of mac address
        buffer[17] = strlen(HOST_NAME);
        strcpy((char*)&(buffer[18]), HOST_NAME);

        //printByte((char*)&(buffer[24]), _dhcpMacAddr[3]);
        //printByte((char*)&(buffer[26]), _dhcpMacAddr[4]);
        //printByte((char*)&(buffer[28]), _dhcpMacAddr[5]);

 *  
 */


#include <SPI.h>
#include <Ethernet2.h>

//analog pins
#define LEDPIN_GREEN A3
#define LEDPIN_RED A4
//housekeeping
#define RESET_W5500 3
#define DEBUG true

byte mac[] = { 0xA0, 0x00, 0x00, 0x00, 0x0d, 0x9D }; //physical mac address
char server[] = { "google.com" };

bool bIPAssigned=false;
byte heartBeat;

EthernetClient client;

void setup()
{
  //setup pins
  pinMode(LEDPIN_RED, OUTPUT);
  pinMode(LEDPIN_GREEN, OUTPUT);
  pinMode(RESET_W5500, INPUT);

  heartBeat=0;
  digitalWrite(LEDPIN_RED, HIGH);
  digitalWrite(LEDPIN_GREEN, HIGH);
  digitalWrite(RESET_W5500, LOW);

  if (DEBUG){
    Serial.begin(9600);
    Serial.println(F("w5500 Starting Up"));
  }
  //w5500 hardware reset
  pinMode(RESET_W5500, OUTPUT);
  digitalWrite(RESET_W5500, HIGH);
  delay(5);
  digitalWrite(RESET_W5500, LOW);
  delay(500);
  pinMode(RESET_W5500, INPUT);
  delay(500);

  if (DEBUG){
      printMACAddress();
      Serial.println(F("Obtaining IP Address from DHCP Server"));
  }
  digitalWrite(LEDPIN_RED, HIGH);
  digitalWrite(LEDPIN_GREEN, LOW);

  int iBeginResult=Ethernet.begin(mac);
  digitalWrite(LEDPIN_GREEN, LOW);
  if (iBeginResult == 0) {
    if (DEBUG)
      Serial.print(F("Failed to configure Ethernet using DHCP"));
  }
  else
  {
      if (DEBUG){
        printIPAddress();  
        Serial.println();
      }
      bIPAssigned=true;
      digitalWrite(LEDPIN_RED, LOW);
      digitalWrite(LEDPIN_GREEN, HIGH);
    
      delay(1000);
      if (DEBUG){
        Serial.print(F("Connecting to "));
        Serial.println(server);
      }
      delay(1000);
      
      int iConnect=client.connect(server, 80);
      if (iConnect==1) {
          digitalWrite(LEDPIN_GREEN, HIGH);
          if (DEBUG)
            Serial.println("Connected");
      } else {
          if (DEBUG){
            Serial.print(F("Failed: "));
            Serial.println(iConnect);
          }
      }
  }

  
}

void loop()
{
  if (bIPAssigned)
  {

    if (DEBUG){
      if (client.available()) {
        char c = client.read();
        Serial.print(c);
      }
    }
  
    if (!client.connected()) {
      digitalWrite(LEDPIN_GREEN, LOW);
      digitalWrite(LEDPIN_RED, HIGH);

      if (DEBUG){
        Serial.println();
        Serial.println(F("disconnecting."));
      }
      client.stop();
      while(true);
    }
    delay(2500);
    
  }
}

void printIPAddress()
{
  Serial.print(F("DHCP IP Assigned: "));
  for (byte thisByte = 0; thisByte < 4; thisByte++) {
    // print the value of each byte of the IP address:
    Serial.print(Ethernet.localIP()[thisByte], DEC);
    if (thisByte<3)
      Serial.print(".");
  }
  Serial.println();
}

void printMACAddress()
{
  Serial.print(F("DeviceAddress "));
  for (byte thisByte = 0; thisByte < 6; thisByte++) {
    printHex(mac[thisByte],2);
  }
  Serial.println();
}


void printHex(int num, int precision) 
{
     char tmp[16];
     char format[128];

     sprintf(format, "%%.%dX", precision);

     sprintf(tmp, format, num);
     Serial.print(tmp);
}


