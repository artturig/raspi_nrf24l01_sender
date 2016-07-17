#include <string>
#include <getopt.h>
#include <cstdlib>
#include <sstream>
#include <iostream>
#include <RF24/RF24.h>
#include <string>

using namespace std;
//RF24 radio("/dev/spidev0.0",8000000 , 25);
//RF24 radio(RPI_V2_GPIO_P1_15, RPI_V2_GPIO_P1_24, BCM2835_SPI_SPEED_8MHZ);

RF24 radio(RPI_V2_GPIO_P1_22, RPI_V2_GPIO_P1_24, BCM2835_SPI_SPEED_8MHZ);

//const int role_pin = 7;
//const uint64_t pipes[2] = { 0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL };
const uint8_t addresses[][6] = {"1Node","2Node"};

// hack to avoid SEG FAULT, issue #46 on RF24 github https://github.com/TMRh20/RF24.git
unsigned long  got_message;

void setup(void){
  //Prepare the radio module
  printf("\nPreparing interface\n");
  radio.begin();
  //  radio.setRetries( 2, 2);
  //  radio.setChannel(0x4c);
  //  radio.setPALevel(RF24_PA_MAX);
  //  radio.setPALevel(RF24_PA_MAX);

  radio.printDetails();
  radio.openWritingPipe(addresses[0]);
  radio.openReadingPipe(1,addresses[1]);
  //  radio.startListening();

}

void sendMessage(int action){
  //This function send a message, the 'action', to the arduino and wait for answer
  //Returns true if ACK package is received
  //Stop listening
  radio.stopListening();
  sleep(2);
  unsigned long message = action;
  printf("Now sending  %lu...", message);

  //Send the message
  bool ok = radio.write( &message, sizeof(unsigned long) );
  if (!ok){
    printf("failed...\n\r");
  }else{
    printf("ok!\n\r");
  }
  //Listen for ACK
  radio.startListening();
  //Let's take the time while we listen
  unsigned long started_waiting_at = millis();
  bool timeout = false;
  while ( ! radio.available() && ! timeout ) {
    //printf("%d", !radio.available());
    if (millis() - started_waiting_at > 2000 ){
      timeout = true;
    }
  }
}

int main( int argc, char ** argv){

  char choice;
  setup();

  //Define the options

  choice = getopt( argc, argv, "m:");

    if (choice == 'm'){

      printf("\n Talking with my NRF24l01+ friends out there....\n");
      sendMessage(atoi(optarg));
    }else{
      // A little help:
      printf("\n\rIt's time to make some choices...\n");
      printf("\n\rTIP: Use -m idAction for the message to send. ");


      printf("\n\rExample (id number 12, action number 1): ");
      printf("\nsudo ./remote -m 121\n");
    }
    radio.stopListening();
  }

