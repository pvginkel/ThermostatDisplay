// GPIO15 is TXD2
// GPIO16 is RXD2
#include <HardwareSerial.h>

HardwareSerial SerialPort(2); // use UART2

void setup() {
  Serial2.begin(115200, SERIAL_8N1, 15, 16);
}

void loop() {
  if (Serial2.available()) {
    char buffer[256];  // Buffer to store input data
    size_t bufferSize = 0;  // Current size of data in buffer

    while (Serial2.available() > 0) {
      char input = Serial2.read();
      buffer[bufferSize++] = input;  // Store input in buffer
      // Check if the buffer is full, or a newline character is received
      if (bufferSize >= sizeof(buffer) || input == '\n') {
        // Send the entire buffer via Serial2
        Serial2.println(buffer);
        delay(10);
        // Reset buffer and size for the next input
        bufferSize = 0;
        memset(buffer, 0, sizeof(buffer));
      }
    }
  }
}
