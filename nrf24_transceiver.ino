#include <printf.h>
#include <nRF24L01.h>
#include <RF24.h>

#define CE	9
#define CSN	10

RF24 radio(CE, CSN);
String radio_received;
uint64_t RF_PIPE = 0xE8E8F0F0E1LL;
uint8_t RF_CHANNEL = 14;

/*if available return radio_received*/
bool radio_available() {
	static unsigned long _timeout = 50;
	radio_received = "";
	char buffer[33];
	if (radio.available()) {
		while (radio.available()) {
			radio.read(buffer, 32);
			buffer[32] = '\0';
			radio_received += buffer;
			for (byte i = 0; i < _timeout; i++) {
				if (!radio.available()) {
					delay(1);
				}
				else {
					continue;
				}
			}
		}
		return true;
	}

	return false;
}

void radio_send(String data) {
	String buffer = data;
	//int segment_i = 0;
	radio.stopListening();
	radio.openWritingPipe(RF_PIPE);
	while (buffer.length() > 32) {
		String segment = buffer.substring(0, 32);
		buffer = buffer.substring(32);
		//Db("Segment");
		//Db(segment_i++);
		//Db("\t[32] ");
		//DB(segment);
		radio.write(segment.c_str(), 32);
	}
	if ((0 < buffer.length()) && (buffer.length() <= 32)) {
		//Db("Segment");
		//Db(segment_i++);
		//Db("\t[");
		//Db(buffer.length());
		//Db("] ");
		//DB(buffer);
		radio.write(buffer.c_str(), buffer.length());
	}
	radio.openReadingPipe(1, RF_PIPE);
	radio.startListening();
	Serial.print("#RF << ");
	Serial.println(data);
}

void setup() {
	Serial.begin(115200);
	Serial.setTimeout(100);

	radio_received.reserve(200);
	radio.begin();
	radio.setAutoAck(true);
	radio.setRetries(1, 5);
	radio.setDataRate(RF24_1MBPS);    // T?c ?? truy?n
	radio.setPALevel(RF24_PA_MAX);      // Dung l??ng t?i ?a
	radio.setChannel(14);               // ??t k�nh
	radio.setPayloadSize(32);
	radio.openReadingPipe(1, RF_PIPE);
	radio.startListening();

	Serial.print("channel: ");
	Serial.println(radio.getChannel());
}

void loop(){
	if (Serial.available()) {
		radio_send(Serial.readString());
	}
	if (radio_available()) {
		Serial.println(radio_received);
	}
}