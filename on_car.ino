// For Arduino attached to car
// received instructions from wireless transceiver and changed motor speeds (not included)
// transmitted distance values back to second Arduino via Acknowledge packets.

/*-----( Import needed libraries )-----*/
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
// ultrasound
#include <NewPing.h>

/*-----( Declare Constants and Pin Numbers )-----*/
#define CE_PIN   3
#define CSN_PIN 4

// ultrasound
#define TRIG_PIN 14
#define ECHO_PIN 15
#define MAX_DISTANCE 200 // units: cm so 2m

// NOTE: the "LL" at the end of the constant is "LongLong" type
const uint64_t pipe_toboard = 0xE8E8F0F0E1LL; // Define the transmit pipe
const uint64_t pipe_tocar = 0xE8E8F0F0F0LL; // Define the transmit pipe

/*-----( Declare objects )-----*/
RF24 radio(CE_PIN, CSN_PIN); // Create a Radio
// ultrasound
NewPing DistanceSensor(TRIG_PIN, ECHO_PIN, MAX_DISTANCE); 

/*-----( Declare Variables )-----*/
unsigned int instructions[5]; // recieving instructions
unsigned int cm;

void setup()   /****** SETUP: RUNS ONCE ******/
{
  Serial.begin(9600);
  radio.begin();
  radio.setPayloadSize(16);
  radio.setDataRate(RF24_250KBPS);
  radio.enableAckPayload();
  radio.enableDynamicPayloads();  
  
  radio.openReadingPipe(1,pipe_tocar);
  radio.openWritingPipe(pipe_toboard);
}//--(end setup )---

void loop()   /****** LOOP: RUNS CONSTANTLY ******/
{
  cm = DistanceSensor.ping_cm();
  
  // -- reading:
  radio.startListening();
  bool done = false;
  
  while (!done) {
     cm = DistanceSensor.ping_cm();
     if (cm <=20) {
     } else {
       cm = 0;
     }
     radio.writeAckPayload( 1, &cm, sizeof(cm) );
     
    if ( radio.available() )
    {      
      // Read the data payload until we've received everything
      while (!done)
      {
        // Fetch the data payload
        done = radio.read(instructions,sizeof(instructions));
        if (done) {
          Serial.print("value_instructions = ");
          Serial.print(instructions[0]);
          Serial.print(" ");
          Serial.print(instructions[1]);
          Serial.print(" ");
          Serial.print(instructions[2]);          
          Serial.print(" ");
          Serial.print(instructions[3]);          
          Serial.print(" ");
          Serial.println(instructions[4]);
        }
      }
    }
    else {
       Serial.println("-No radio available -");
    }
  }
  
  if (done){
    // instruction decoding
  }
  
}//--(end main loop )---
