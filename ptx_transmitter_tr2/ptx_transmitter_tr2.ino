/*
* Transmitter Code
* Library: TMRh20/RF24, https://github.com/tmrh20/RF24/
* Code updates: Arilson Jr.
*/
#include <nRF24L01.h>
#include <printf.h>
#include <RF24.h>
#include <RF24_config.h>
#include <DHT.h>

RF24 radio(7, 8); // CE, CSN
DHT dht_1 (2, DHT22);
DHT dht_2 (3, DHT22);

#define WHICH_NODE 2    //number from 1 -> 6 identifying the PTX node
const uint64_t wAddress[] = {0x7878787878, 0xB3B4B5B6F1, 0xB3B4B5B6CD, 0xB3B4B5B6A3, 0xB3B4B5B60F, 0xB3B4B5B605};
const uint64_t PTXpipe = wAddress[WHICH_NODE - 1];   // Pulls the address from the above array for this node's pipe

// calibration coefficients for temperature and relative humidity (sensor 1 & 2)
#define coef_A_dht1_ta 0.49377
#define coef_B_dht1_ta 1.01613
#define coef_A_dht1_ur -6.12016
#define coef_B_dht1_ur 1.37153
#define coef_A_dht2_ta 1.18659
#define coef_B_dht2_ta 0.99208

void setup() {
  Serial.begin(9600);
  radio.begin();    
  radio.setPALevel(RF24_PA_LOW);  // "short range setting" - increase if you want more range AND have a good power supply
  radio.setChannel(108);          // the higher channels tend to be more "open"
  radio.openWritingPipe(PTXpipe);     
  radio.stopListening();
  dht_1.begin();
  dht_2.begin();
}

void loop() {
  double ta_1 = dht_1.readTemperature();
  double ur_1 = dht_1.readHumidity();
  double ta_1_calibra = (coef_A_dht1_ta + (ta_1 * coef_B_dht1_ta));
  double ur_1_calibra = (coef_A_dht1_ur + (ur_1 * coef_B_dht1_ur));
  double ta_2 = dht_2.readTemperature();
  double ta_2_calibra = (coef_A_dht2_ta + (ta_2 * coef_B_dht2_ta));
  double num[4] = {ta_1_calibra, ta_2_calibra, ur_1_calibra, WHICH_NODE};
  radio.write(&num, sizeof(num));
  delay(2000);
}
