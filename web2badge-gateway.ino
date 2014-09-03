#ifdef YUN
#include <Bridge.h>
#include <HttpClient.h>
#endif
#include <SPI.h>
#include "printf.h"
#include "RF24.h"

#define LED1 13
int led1 = 0;

const uint64_t pipes[2] = { 0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL };

// Set up nRF24L01 radio on SPI bus plus pins 9 & 10
RF24 radio(9,10);

void setup() {
#ifdef YUN
		Bridge.begin();
#endif

		Serial.begin(115200);
		while  (!Serial) {;}
		printf_begin();

		Serial.println("Setup BEGIN");
		pinMode(LED1, OUTPUT);
		led1 = 0;
		digitalWrite(LED1, led1);

		radio.begin();
		// enable dynamic payloads
		radio.enableDynamicPayloads();
		// optionally, increase the delay between retries & # of retries
		radio.setRetries(15,15);
		radio.setAutoAck(false);
		radio.setDataRate(RF24_250KBPS);
		//radio.setChannel(1);
		//radio.setPALevel(RF24_PA_MIN);

		radio.openWritingPipe(pipes[0]);
		radio.openReadingPipe(1,pipes[1]);

		radio.startListening();
		radio.printDetails();
		Serial.println("Setup DONE");
}

void loop() {
		delay(1000);

#ifdef YUN
		HttpClient client;
		client.get("http://192.168.240.238:8000/1.txt");

		int r = client.getResult();
		if(!r) {
				led1 = !led1;
				digitalWrite(LED1, led1);

				int buffer_len = 0;
				char buffer[128];

				while (client.available()) {
						char c = client.read();
						Serial.print(c);
						buffer[buffer_len] = c;
						buffer_len ++;
				}
				Serial.flush();
				buffer[buffer_len] = NULL;

				radio.stopListening();
				Serial.print("\n");
				Serial.print(buffer_len);
				Serial.print(">>>");
				Serial.println(buffer);
				radio.write(buffer, buffer_len);
				radio.startListening();

				led1 = !led1;
				digitalWrite(LED1, led1);

		}
#endif

		if(radio.available()) {
				led1 = !led1;
				digitalWrite(LED1, led1);

				Serial.println("Hmm...");
				uint8_t len = radio.getDynamicPayloadSize();
				char msg[len];
				radio.read(&msg, len );
				msg[len] = 0;
				Serial.println(msg);

				led1 = !led1;
				digitalWrite(LED1, led1);

		}
		if(Serial.available()) {
				led1 = !led1;
				digitalWrite(LED1, led1);

				char buffer[128];
				int buffer_len = 0;
				for(int i=0; i<128; i++) {
						buffer[i] = (char)Serial.read();
						buffer_len++;
						if(buffer[i] == '\r' || buffer[i] == '\n' || buffer[i] == -1) {
								buffer_len--;
								buffer[i] = NULL;
								break;
						} else
								Serial.write(buffer[i]);
				}
				radio.stopListening();
				Serial.print("\n");
				Serial.print(buffer_len);
				Serial.print(">>>");
				Serial.println(buffer);
				radio.write(buffer, buffer_len);
				radio.startListening();

				led1 = !led1;
				digitalWrite(LED1, led1);

		}

}
