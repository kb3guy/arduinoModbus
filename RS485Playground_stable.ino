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

/*-----( Declare Variables )-----*/
int byteReceived;
int byteSend;
int i;
char rx_byte = 0; // place to store serial port input

void setup()   /****** SETUP: RUNS ONCE ******/
{
  // Start the built-in serial port, probably to Serial Monitor
  Serial.begin(9600);

  pinMode(Pin13LED, OUTPUT);   
  pinMode(SSerialTxControl, OUTPUT);    
  
  digitalWrite(SSerialTxControl, RS485Receive);  // Init Transceiver   
  
  // Start the software serial port, to another device
  RS485Serial.begin(9600);   // set the data rate 

  // Print menu options
  Serial.println("Arduino Motor Control Menu");
  Serial.println("Select option:");
  Serial.println("1) Set inverter to RUN");
  Serial.println("2) Set inverter to STOP");
  Serial.println("3) Set inverter to READ SPEED");

}//--(end setup )---

// Function that sends data to RS485 port to run inverter
void motor_stop()
{
  Serial.println("Sending motor control signal...");
    char controlStatement[] = {0x01, 0x06, 0x25, 0x01, 0x00, 0x00, 0xD3, 0x06}; // STATEMENT THAT REQUESTS MOTOR STOP
    
    digitalWrite(SSerialTxControl, RS485Transmit);  // Enable RS485 Transmit   
    
    for(i=0; i<sizeof(controlStatement);i++){
      RS485Serial.print(controlStatement[i]);          // Send byte to inverter 

    }
      Serial.print(sizeof(controlStatement));
      Serial.println(" bytes sent");
      
    digitalWrite(Pin13LED, LOW);  // Show activity    
    delay(10);
    digitalWrite(SSerialTxControl, RS485Receive);  // Disable RS485 Transmit       
  
  
  if (RS485Serial.available())  //Look for data from inverter
   {
    digitalWrite(Pin13LED, HIGH);  // Show activity

    for(i=0; i<16; i++)
    {
         byteReceived = RS485Serial.read();    // Read received byte
         Serial.print(byteReceived,HEX);        // Show on Serial Monitor
         Serial.print(",");

         delay(10);
    }
    
    digitalWrite(Pin13LED, LOW);  // Show activity   
   }  
   
   Serial.println("Command Sent.");
}
void motor_run()
{
  Serial.println("Sending motor control signal...");
    char controlStatement[] = {0x01, 0x06, 0x25, 0x01, 0x00, 0x01, 0x12, 0xC6}; // STATEMENT THAT REQUESTS MOTOR RUN
    //char controlStatement[] = {0x01, 0x08, 0x00, 0x00, 0xA5, 0x37, 0xDA, 0x8D}; // LOOPBACK REQUEST
    //char controlStatement[] = {0x01, 0x02, 0xA3, 0x04};
    // Statement should be: 
    
    digitalWrite(SSerialTxControl, RS485Transmit);  // Enable RS485 Transmit   
    
    for(i=0; i<sizeof(controlStatement);i++){
      RS485Serial.print(controlStatement[i]);          // Send byte to inverter 

    }
      Serial.print(sizeof(controlStatement));
      Serial.println(" bytes sent");
      
    digitalWrite(Pin13LED, LOW);  // Show activity    
    delay(10);
    digitalWrite(SSerialTxControl, RS485Receive);  // Disable RS485 Transmit       
  
  
  if (RS485Serial.available())  //Look for data from inverter
   {
    digitalWrite(Pin13LED, HIGH);  // Show activity

    for(i=0; i<16; i++)
    {
         byteReceived = RS485Serial.read();    // Read received byte
         Serial.print(byteReceived,HEX);        // Show on Serial Monitor
         Serial.print(",");

         delay(10);
    }
    
    digitalWrite(Pin13LED, LOW);  // Show activity   
   }  
   
   Serial.println("Command Sent.");
}
void loop()   /****** LOOP: RUNS CONSTANTLY ******/
{
 
  digitalWrite(Pin13LED, HIGH);  // Show activity
  if (Serial.available())  // If we have input from the terminal
  {
    rx_byte = Serial.read(); // get menu input
    Serial.print("Option: " );
    Serial.println(rx_byte);

    switch(rx_byte){
       case '1':
        Serial.println("RUN Motor.");
        motor_run();
        break;
       case '2':
        Serial.println("STOP Motor.");
        motor_stop();
        break;
       case '3':
        Serial.println("READ Speed.");
        break;
       default:
        Serial.println("Unknown option.");
       
    }
  }

}//--(end main loop )---

/*-----( Declare User-written Functions )-----*/

//NONE
//*********( THE END )***********

