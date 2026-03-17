#include <Arduino.h>
#include <HardwareSerial.h>

//SoftwareSerial Serial2(11,10);



void awaitAndPrint(unsigned long ms) {
    unsigned long timeout = millis() + ms;  // 1-second timeout
    while (millis() < timeout) {
        if (Serial2.available()) {
            char c = Serial2.read();  // Read one byte from Serial2
            Serial.write(c);           // Send it to Serial Monitor
        }
    }
}

// Function to send an SMS
void sendSMS(String phoneNumber, String message) {
    // Send the AT command to set the recipient phone number
    Serial2.print("AT+CMGS=\"" + phoneNumber + "\"\r\n");
    awaitAndPrint(500);  // Wait for the GSM module to prompt for the message

    // Send the SMS content
    Serial2.print(message);
    awaitAndPrint(500);

    // Send Ctrl+Z (ASCII 26) to send the message
    Serial2.write(26);  // Ctrl+Z (End of message)
    awaitAndPrint(5000);  // Wait for the SMS to be sent

}

void setup() {
    Serial.begin(9600);           // Begin Serial for debugging at 9600 baud
    while (!Serial);              // Wait for Serial Monitor

    Serial2.begin(115200);        
    while (!Serial2);             // Wait for Serial2 to be ready
    Serial2.print("AT+CRESET\r\n");
    awaitAndPrint(5000);
    // SoftwareSerial cannot handle higher baud rates
    // Serial2.print("AT+IPR=9600\r\n");
    // awaitAndPrint(400);
    Serial2.print("AT+CMEE=2\r\n");
    awaitAndPrint(400);
    //Serial2.print("AT+CMGF=1\r\n");
    // awaitAndPrint(400);
    // Serial2.print("AT+COPS=?\r\n");
    // awaitAndPrint(400);
    // Serial2.print("AT+COPS=?\r\n");
    // awaitAndPrint(400);
    
    Serial2.print("AT+CENG?\r\n");
    awaitAndPrint(400);
    Serial2.print("AT+CREG?\r\n");
    awaitAndPrint(400);
    Serial2.print("AT+CSQ\r\n");
    awaitAndPrint(400);
    // Serial2.print("AT+COPS=?\r\n");
    // awaitAndPrint(400);
    Serial2.print("AT+CGDCONT=1,\"IP\",\"hologram\"\r\n");
    awaitAndPrint(400);
    Serial2.print("AT+CGATT=1\r\n");
    awaitAndPrint(4000);
    Serial2.print("AT+CGACT=1,1\r\n");
    awaitAndPrint(5000);
    //Serial2.print("AT+CBC\r\n");
    // Serial2.print("AT+CSMINS?\r\n"); // shoud return 0,1
    // awaitAndPrint(400);
    // Serial2.print("AT+CFUN?\r\n"); // shoud return 1
    // awaitAndPrint(400);
}

void loop() {
    //sendSMS("6464250666", "Hewo");
    // // Send AT command with a proper newline and carriage return
    Serial2.print("AT+CSQ\r\n");
    // //Serial2.print("AT+CGACT?\r\n");
    awaitAndPrint(1000);

}