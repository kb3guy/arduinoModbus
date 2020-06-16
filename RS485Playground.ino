/* L510 Inverter Current Speed Reader
 * Using RS485
*/

/*-----( Import needed libraries )-----*/
#include <SoftwareSerial.h>
/*-----( Declare Constants and Pin Numbers )-----*/
#define SSerialRX        10  //Serial Receive pin
#define SSerialTX        11  //Serial Transmit pin

#define SSerialTxControl 3   //RS485 Direction control

#define RS485Transmit    HIGH
#define RS485Receive     LOW

#define Pin13LED         13

    
/*-----( Declare objects )-----*/
SoftwareSerial RS485Serial(SSerialRX, SSerialTX); // RX, TX
String inputString = ""; // place to store serial port input

/*-----( Declare Variables )-----*/
int byteReceived;
int byteSend;
int i, j;
boolean menulevel; // keeps track of menu level

void print_menu(){

  // Menu level 1 is speed setting
  if(menulevel==1){
  Serial.println("##.##> ");
  }
  // Default to main menu
  else{
  // Print menu options
  Serial.println("\nArduino Motor Control Menu");
  Serial.println("Select option:");
  Serial.println("1) Set inverter to RUN");
  Serial.println("2) Set inverter to STOP");
  Serial.println("3) Set inverter to 60Hz");
  Serial.println("4) Set inverter to 10Hz");
  Serial.println("5) Set Custom Speed");
  Serial.println("> ");
  }
}
void setup()   /****** SETUP: RUNS ONCE ******/
{
  // MAIN SERIAL SETUP
  // Start the built-in serial port, probably to Serial Monitor
  Serial.begin(9600);

  // RS485 SETUP

  pinMode(Pin13LED, OUTPUT);   
  pinMode(SSerialTxControl, OUTPUT);    
  
  digitalWrite(SSerialTxControl, RS485Receive);  // Init Transceiver   
  
  // Start the software serial port, to another device
  RS485Serial.begin(9600);   // set the data rate 

  // MENU SETUP
  menulevel = 0;
  print_menu();
}//--(end setup )---

// Checksum: generate CR16 checksum and overwrite last two bytes of commmand with it - for RTU
char *checksum(byte *command, int len)
{
  word sum = 0xFFFF;
  //Serial.println(sum, HEX);
  
  for(i=0; i<(len-2); i++){
    //Serial.println(command[i], HEX);
    sum ^= command[i];
    //Serial.println(sum, BIN);
    
    for(j =0; j<8; j++){
      if(sum & 0x0001){
        sum = (sum >> 1)^0xa001;
      }
      else{
        sum = sum >> 1;
      }
    }
  }
     Serial.println(sum, HEX);
     command[6] = sum & 0xFF; // little byte, overwrite big side with zeroes
     command[7] = sum  >> 8; // big  byte, shift right into little zone and reassign

     return command;
}

void motor_ctl(const char *controlStatement, int len)
{
  // Perform checksum
  checksum(controlStatement, len);
  
  Serial.println("Sending motor control signal...");

  // Print out hex code for signal being sent
  for(i=0; i<8; i++){
    Serial.print(controlStatement[i],HEX);
    Serial.print(",");
  }
  Serial.println("\n");
 
    // Enable RS485 Transmit   
    digitalWrite(SSerialTxControl, RS485Transmit);  

    // Send command byte-wise
    for(i=0; i<8;i++){
      RS485Serial.print(controlStatement[i]);          
      Serial.print(("1"));

    }
      
    digitalWrite(Pin13LED, LOW);  // Show activity    
    delay(10);
    digitalWrite(SSerialTxControl, RS485Receive);  // Disable RS485 Transmit       
    Serial.println("Command Sent.");

  // Read back response message from inverter
  Serial.println("\n Response: ");
  if (RS485Serial.available())  //Look for data from inverter
   {
    digitalWrite(Pin13LED, HIGH);  // Show activity

    for(i=0; i<8; i++)
    {
         byteReceived = RS485Serial.read();    // Read received byte
         Serial.print(byteReceived,HEX);        // Show on Serial Monitor
         Serial.print(",");

         delay(10);
    }
    
    digitalWrite(Pin13LED, LOW);  // Show activity   
   }  
   

}
void customFreq(float freq){
  Serial.println("Custom frequency function!");
  char SPEED_CMD[] = {0x01, 0x06, 0x25, 0x02, 0x03, 0xE8, 0xFF, 0xFF};

  // make sure speed is within bounds
  if(freq < 60.00 && freq >= 0){

      // multiply by 10
      freq *= 100;
      
      // cast to 16-bit integer
      int ifreq = (int)freq;
      Serial.print("Frequency: ");
      Serial.print(ifreq);
      Serial.print (",");
      Serial.println(ifreq, HEX);

      SPEED_CMD[4] = (ifreq & 0xFF00) >> 8; // big byte
      SPEED_CMD[5] = ifreq & 0xFF; // little byte


    motor_ctl(SPEED_CMD, 8);
  }
  else{
    Serial.println("Error: frequency out of range");
  }
  
}

void loop()   /****** LOOP: RUNS CONSTANTLY ******/
{

  digitalWrite(Pin13LED, HIGH);  // Show activity
  if (Serial.available())  // If we have input from the terminal
  {  
    inputString = Serial.readString(); // get menu input
    Serial.print("Input: " );
    Serial.println(inputString);
    
    char RUN_CMD[] = {0x01, 0x06, 0x25, 0x01, 0x00, 0x01, 0x12, 0xC6};
    char STOP_CMD[] = {0x01, 0x06, 0x25, 0x01, 0x00, 0x00, 0xD3, 0x06};
    char SPEED_CMD[] = {0x01, 0x06, 0x25, 0x02, 0x17, 0x70, 0xFF, 0xFF};
    char SPEED_LO[] = {0x01, 0x06, 0x25, 0x02, 0x03, 0xE8, 0xFF, 0xFF};

    if(menulevel==1){
      Serial.print("Frequency command: ");
      Serial.println(inputString);
      customFreq(inputString.toFloat());
      menulevel = 0;
    }
    else{
    switch(inputString[0]){
       case '1':
        Serial.println("RUN Motor.");
        motor_ctl(RUN_CMD, 8);
        break;
       case '2':
        Serial.println("STOP Motor.");
        motor_ctl(STOP_CMD, 8);
        break;
       case '3':
        Serial.println("SET Speed at 60 Hz.");
        motor_ctl(SPEED_CMD, 8);
        break;
       case '4':
        Serial.println("SET Speed at 10 Hz.");
        motor_ctl(SPEED_LO, 8);
        break;
       case '5':
        Serial.println("SET Custom speed.");
        menulevel = 1;

        break;
       default:       
        Serial.println("Unknown option.");
    }
    }
    print_menu();
    
  }

}//--(end main loop )---

/*-----( Declare User-written Functions )-----*/

//NONE
//*********( THE END )***********

