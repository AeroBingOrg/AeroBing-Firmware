/*License: CC 4.0 - Attribution, NonCommercial (by Mitch Davis, github.com/thekakester)
* https://creativecommons.org/licenses/by-nc/4.0/   (See README for details)*/
#include <LoraSx1262.h>

char payload[1024] = "Hello world.  Th";
LoraSx1262 radio;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println("Booted");

  while (!radio.begin(915000000, 0, 2, 12)) { //Initialize radio
    Serial.println("Failed to initialize radio.");
  }

  /************************
  * OPTIONAL CONFIGURATION
  *************************
  * This is intended for people who know what these settings mean.
  * If you don't know what these mean, you can just ignore them to use defaults instead.
  * You can also choose to use presets instead if you'd like customization without needing to understand
  * the underlying concepts.  See RadioConfigPresets example for details.
  *
  * ALL TRANSMITTERS/RECEIVERS MUST HAVE MATCHING CONFIGS, otherwise
  * they can't communicate with eachother
  */
 
}

void loop() {
  Serial.print("Transmitting... ");
  char rec_buf[1024] = {0};

  
  //radio.setRxDutyCycle(5000,20000);
  // radio.setModeReceive();
  // if (radio.lora_receive_async((byte *) rec_buf,1024) > 0) {
  //   Serial.print("received: ");
  //   Serial.println(rec_buf);
  // }
  
  radio.transmit((byte *) payload, strlen(payload));
  delay(500);
  radio.printStatus();
  //radio.printErrors();

  //delay(3000);
}
