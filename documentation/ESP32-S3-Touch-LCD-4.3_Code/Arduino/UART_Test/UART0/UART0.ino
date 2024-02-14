#include <HardwareSerial.h>

void setup() {
  Serial.begin(115200);
}

void loop() {
  if (Serial.available()) {
    char buffer[256];                 // Buffer to store input data
    size_t bufferSize = 0;            // Current size of data in buffer

    while (Serial.available() > 0) {
      char input = Serial.read();
      buffer[bufferSize++] = input;   // Store input in buffer
      // Check if the buffer is full, or a newline character is received
      delay(1);
      if (bufferSize >= sizeof(buffer) || input == '\n') {
        Serial.println(buffer);
        delay(20);
      // Reset buffer and size for the next input
        bufferSize = 0;
        memset(buffer, 0, sizeof(buffer));
      }
    }
  }
}
