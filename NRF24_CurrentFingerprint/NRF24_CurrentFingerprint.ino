#define DEBUG

#include <SPI.h>
#include <RF24.h>
#include <RF24_config.h> 

// Hardware configuration
#define RF_CE_PIN                      (9)
#define RF_CS_PIN                      (10) 
#define SEND_IND_PIN                   (3)

// Set up nRF24L01 radio on SPI bus plus
static RF24 radio(RF_CE_PIN, RF_CS_PIN);

const uint64_t        addr         = 0xCAFEBABE00LL;
const uint8_t         channel      = 70;
const uint8_t         pa_level     = RF24_PA_MAX;
const rf24_datarate_e rate         = RF24_1MBPS;

// When using printf etc. in debugging RF24
int my_putc( char c, FILE *t )
{
  Serial.write( c );
  return 0;
}

void setup() {
  Serial.begin(115200);
  Serial.println(F("RF24 Current Fingerprint"));

  // When using printf etc. in debugging RF24
  fdevopen( &my_putc, 0);

#ifdef SEND_IND_PIN  
  pinMode(SEND_IND_PIN, OUTPUT);
  digitalWrite(SEND_IND_PIN, LOW);
#endif  

  radio.begin();
  radio.setChannel(channel);
  radio.setPALevel(pa_level);
  radio.setDataRate(rate);
  radio.enableDynamicPayloads();
  radio.openWritingPipe(addr);
  radio.setAutoAck(false);
  radio.stopListening();
  radio.flush_tx();
  radio.printDetails();
}

static long tx_counter=0;

void loop()
{
    tx_counter++;
  
    unsigned long tx_start_us = micros();
#ifdef SEND_IND_PIN  
    digitalWrite(SEND_IND_PIN, HIGH);
#endif

    const uint8_t payload_size = sizeof(tx_counter);
    // Write payload. This blocks until package has been sent.
    radio.write( &tx_counter, payload_size );
  
#ifdef SEND_IND_PIN  
    digitalWrite(SEND_IND_PIN, LOW);
#endif
    unsigned long tx_stop_us = micros();
    unsigned long tx_time_us = tx_stop_us-tx_start_us;
//    Serial.print("Sent "); Serial.print( payload_size ); Serial.print( " bytes in "); Serial.print( tx_time_us ); Serial.println(" us");
   
   delay(10 - (tx_time_us/1000));
}
