// Code received instructions input from GPIO pins on Altera DE2 board, running NIOS II processor
// Received distance values transmitted over Nrf24L01 transcievers 
// Sent distance data to GPIO pins of DE2 board

// transmitter
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

/*-----( Declare Constants and Pin Numbers )-----*/
#define CE_PIN   9
#define CSN_PIN 10
#define to_GPIO_PIN1 3 // GPIO 10
#define to_GPIO_PIN2 4 // GPIO 11

// NOTE: the "LL" at the end of the constant is "LongLong" type
const uint64_t pipe_toboard = 0xE8E8F0F0E1LL; // Define the transmit pipe
const uint64_t pipe_tocar = 0xE8E8F0F0F0LL; // Define the transmit pipe

/*-----( Declare objects )-----*/
RF24 radio(CE_PIN, CSN_PIN); // Create a Radio

/*-----( Declare Variables )-----*/
unsigned int instructions[5];

#define from_GPIO_PIN1 A5 // A5
#define from_GPIO_PIN2 A3 //A3
#define from_GPIO_PIN3 A2 //A2
#define from_GPIO_PIN4 A1 //A1
#define from_GPIO_PIN5 A0 //A0

void setup() {
  Serial.begin(9600);
  delay(1000);

  // instructions
  pinMode(A0,INPUT_PULLUP); // bit 4
  pinMode(A1,INPUT_PULLUP); // bit 3
  pinMode(A2,INPUT_PULLUP); // bit 2
  pinMode(A3,INPUT_PULLUP); // bit 1
  pinMode(A5,INPUT_PULLUP); // bit 0
  
  pinMode(to_GPIO_PIN1,OUTPUT); // 
  pinMode(to_GPIO_PIN2,OUTPUT); // 
  
  Serial.println("Nrf24L01 Receiver Starting");
  radio.begin();
  radio.setPayloadSize(16);
  radio.setDataRate(RF24_250KBPS);
  radio.enableAckPayload();
  radio.enableDynamicPayloads();  
  
  radio.openWritingPipe(pipe_tocar);
  radio.openReadingPipe(1,pipe_toboard);
  
  //digitalWrite(to_GPIO_PIN, LOW);
  
  // test
                           // 1 0
   instructions[0] = 0; // on off
   instructions[1] = 0; // forward backwards
   instructions[2] = 0; // turn on/off
   instructions[3] = 0; // left right
   instructions[4] = 0; // speed 
}

static uint32_t distance = 0;

void loop() {
   instructions[0] = digitalRead(from_GPIO_PIN1); // on off
   instructions[1] = digitalRead(from_GPIO_PIN2);; // forward backwards
   instructions[2] = digitalRead(from_GPIO_PIN3);; // turn on/off
   instructions[3] = digitalRead(from_GPIO_PIN4);; // left right
   instructions[4] = digitalRead(from_GPIO_PIN5);; // speed 
   
   for (int i = 4; i>=0; i--)
     Serial.print(instructions[i]);
    
   Serial.println();
   bool yee = false;
  
   while (!yee) {
     yee = radio.write( instructions, sizeof(instructions) );
     Serial.println("-");
     if (yee){
       if(radio.isAckPayloadAvailable()) {
         radio.read(&distance,sizeof(distance));
       }
       Serial.println(distance);
     }
   }
   
   Serial.println("~");
   if (distance != 0 && distance < 50) {
       digitalWrite(to_GPIO_PIN1, HIGH); // stop - GPIO10, 6
       digitalWrite(to_GPIO_PIN2, LOW); 
       Serial.println("close");
   } else {
       digitalWrite(to_GPIO_PIN1, LOW); // go - GPIO11, 7
       digitalWrite(to_GPIO_PIN2, HIGH);
       Serial.println("far");  
   }
   
  //#define to_GPIO_PIN1 6 // GPIO 10
  //#define to_GPIO_PIN2 7 // GPIO 11

   
}
