
#ifndef RFL_Protocol_Prologue_Sensor_h
#define RFL_Protocol_Prologue_Sensor_h 0.1

// ***********************************************************************************
// dit is het oude protocol 071
// ***********************************************************************************

class _RFL_Protocol_Prologue_Sensor : public _RFL_Protocol_BaseClass
{

public:
  // ***********************************************************************
  // Creator,
  // ***********************************************************************
  _RFL_Protocol_Prologue_Sensor(int Receive_Pin, int Transmit_Pin)
  {
    this->_Receive_Pin = Receive_Pin;
    this->_Transmit_Pin = Transmit_Pin;
    Name = "Prologue_Sensor";
  }

  // ***********************************************************************
  // ***********************************************************************
  bool RF_Decode()
  {
#define PulseCount 72

#define short_width 2000;
#define long_width 4000;
#define gap_limit 7000;
#define reset_limit 10000;

    unsigned long s_short = short_width;
    unsigned long s_long = long_width;
    unsigned long s_reset = reset_limit;
    unsigned long s_gap = gap_limit;

    unsigned long zero_l, zero_u;
    unsigned long one_l, one_u;

    zero_l = 0;
    zero_u = (s_short + s_long) / 2 + 1;
    one_l = zero_u - 1;
    one_u = s_gap ? s_gap : s_reset;

    // ****************************************************
    // Check the length of the sequence
    // ****************************************************

    Serial.print("RawSignal.Number:");
    Serial.println(RawSignal.Number);
    if (RawSignal.Number != PulseCount + 3)
      return false;

    // ****************************************************
    //  Translate the sequence in bit-values and
    //     jump out this method if errors are detected
    // 0-bit = a short high followed by a long low
    // 1-bit = a long  high followed by a short low
    // ****************************************************

    bitbuffer_t bits = {0};
    // uint8_t *b;

    unsigned long P1;
    unsigned long P2;
    Serial.print("Bitstream: ");
    for (byte x = 2; x < PulseCount + 1; x = x + 2)
    {
      P1 = RawSignal.Pulses[x];
      P2 = RawSignal.Pulses[x + 1];
      if (P2 > zero_l && P2 < zero_u)
      {
        bitbuffer_add_bit(&bits, 0); // append "0"
        Serial.print("0");
      }
      else if (P2 > one_l && P2 < one_u)
      {
        bitbuffer_add_bit(&bits, 1); // append "1"
        Serial.print("1");
      }
    }
    Serial.println("");

    // int r = bitbuffer_find_repeated_row(&bits, 4, 36);

    // Serial.println ( r ) ;
    /*
      b = bits.bb[r];

      Serial.println ( "" ) ;
       sprintf ( _RFLink_pbuffer, "BitStream: %X", b);
      Serial.println  (  _RFLink_pbuffer ) ;
*/
    //==================================================================================
    // Prevent repeating signals from showing up
    //==================================================================================

    /*
        if ( ( b != Last_BitStream ) ||
             ( millis() > 700 + Last_Detection_Time ) ) {
         // not seen the RF packet recently
         Last_BitStream = BB.BitStream;
      } else {
         // already seen the RF packet recently
         return true;
      }
      */
    /*
    int type =0;
    int id =0;
    int battery = 0;
    int button = 0;
    int channel = 0;
    int temp_raw = 0;
    int humidity = 0;

    type     = b[0] >> 4;
    id       = ((b[0] & 0x0F) << 4) | ((b[1] & 0xF0) >> 4);
    battery  = b[1] & 0x08;
    button   = (b[1] & 0x04) >> 2;
    channel  = (b[1] & 0x03) + 1;
    temp_raw = (int16_t)((b[2] << 8) | (b[3] & 0xF0)); // uses sign-extend
    temp_raw = temp_raw >> 4;
    humidity = ((b[3] & 0x0F) << 4) | (b[4] >> 4);
*/
    // byte Chime = 0 ;

    // BB.BitStream  = ( BB.BitStream >> 16 ) & 0xFFFFFFFF ;
    /*
     sprintf ( _RFLink_pbuffer, "20;%02X;b[0] %02X, b[1] %02X, b[2] %02X, b[3] %02X, b[4] %02X", PKSequenceNumber++, b[0], b[1], b[2], b[3], b[4]);
      Serial.println  (  _RFLink_pbuffer ) ;


      sprintf ( _RFLink_pbuffer, "20;%02X;type %02X, id %02X, battery %02X, button %02X, channel %02X, temp_raw %02X, humidity %02X;", PKSequenceNumber++, type, id, battery, button, channel, temp_raw, humidity);
      Serial.println  (  _RFLink_pbuffer ) ;
 */
    // bitbuffer_clear(&bits);
    return true;
  }

  // ***********************************************************************************
  // ***********************************************************************************
private:
  int _Receive_Pin;
  int _Transmit_Pin;
};

#endif

/** @fn int prologue_callback(r_device *decoder, bitbuffer_t *bitbuffer)
Prologue sensor protocol,
also FreeTec NC-7104 sensor for FreeTec Weatherstation NC-7102,
and Pearl NC-7159-675.
The sensor can be bought at Clas Ohlson.

The sensor sends 36 bits 7 times, before the first packet there is a sync pulse.
The packets are ppm modulated (distance coding) with a pulse of ~500 us
followed by a short gap of ~2000 us for a 0 bit or a long ~4000 us gap for a
1 bit, the sync gap is ~9000 us.

The data is grouped in 9 nibbles

     0101  0011   1000   1001   0000    1100    1111     0001 0000   ch2

     0101  0011   1000   1100   0000   1101    0000     0001 0000 ch1
     0101 0011 1000 1000 0000 1100 1110 0001 0000
        5    3    8    8    0    C    E    1    0
        3880CE10
20;12;b[0] CE, b[1] 10, b[2] 00, b[3] 00, b[4] F0

bitstream: 0100 1110 0011 1000 0011 0101 1000 0100 0000
BitStream:    4    E    3    8    3    5    8    4    0
20;XX;DEBUG;Pulses=70;Pulses(uSec)=512,1920,512,3840,512,1920,512,1920,480,3872,512,3872,512,3840,512,1920,512,1920,480,1920,512,3872,480,3872,512,3840,512,1920,512,1920,480,1920,512,1920,512,1920,512,3840,512,3840,512,1920,512,3840,512,1920,512,3840,512,3840,512,1920,512,1920,512,1920,480,1920,512,3840,512,1920,512,1920,512,1920,512,1888,512,864;

    [type] [id0] [id1] [flags] [temp0] [temp1] [temp2] [humi0] [humi1]

- type: 4 bit fixed 1001 (9) or 0110 (5)
- id: 8 bit a random id that is generated when the sensor starts, could include battery status
  the same batteries often generate the same id
- flags(3): is 0 when the battery is low, otherwise 1 (ok), first reading always says low
- flags(2): is 1 when the sensor sends a reading when pressing the button on the sensor
- flags(1,0): the channel number that can be set by the sensor (1, 2, 3, X)
- temp: 12 bit signed scaled by 10
- humi: 8 bit always 11001100 (0xCC) if no humidity sensor is available



#include "decoder.h"

extern int alecto_checksum(r_device *decoder, bitrow_t *bb);

static int prologue_callback(r_device *decoder, bitbuffer_t *bitbuffer)
{
    uint8_t *b;
    data_t *data;
    int ret;

    int type;
    int id;
    int battery;
    int button;
    int channel;
    int temp_raw;
    int humidity;

    if (bitbuffer->bits_per_row[0] <= 8 && bitbuffer->bits_per_row[0] != 0)
        return DECODE_ABORT_EARLY; // Alecto/Auriol-v2 has 8 sync bits, reduce false positive

    int r = bitbuffer_find_repeated_row(bitbuffer, 4, 36); // only 3 repeats will give false positives for Alecto/Auriol-v2
    if (r < 0)
        return DECODE_ABORT_EARLY;

    if (bitbuffer->bits_per_row[r] > 37) // we expect 36 bits but there might be a trailing 0 bit
        return DECODE_ABORT_LENGTH;

    b = bitbuffer->bb[r];

    if ((b[0] & 0xF0) != 0x90 && (b[0] & 0xF0) != 0x50)
        return DECODE_FAIL_SANITY;

 Check for Alecto collision
    ret = alecto_checksum(decoder, bitbuffer->bb);
    // if the checksum is correct, it's not Prologue/ThermoPro-TX2
    if (ret > 0)
        return DECODE_FAIL_SANITY;

   Prologue/ThermoPro-TX2 sensor
    type     = b[0] >> 4;
    id       = ((b[0] & 0x0F) << 4) | ((b[1] & 0xF0) >> 4);
    battery  = b[1] & 0x08;
    button   = (b[1] & 0x04) >> 2;
    channel  = (b[1] & 0x03) + 1;
    temp_raw = (int16_t)((b[2] << 8) | (b[3] & 0xF0)); // uses sign-extend
    temp_raw = temp_raw >> 4;
    humidity = ((b[3] & 0x0F) << 4) | (b[4] >> 4);

    /* clang-format off
    data = data_make(
            "model",         "",            DATA_STRING, _X("Prologue-TH","Prologue sensor"),
            _X("subtype","id"),       "",            DATA_INT, type,
            _X("id","rid"),            "",            DATA_INT, id,
            "channel",       "Channel",     DATA_INT, channel,
            "battery",       "Battery",     DATA_STRING, battery ? "OK" : "LOW",
            "temperature_C", "Temperature", DATA_FORMAT, "%.02f C", DATA_DOUBLE, temp_raw * 0.1,
            "humidity",      "Humidity",    DATA_COND, humidity != 0xcc, DATA_FORMAT, "%u %%", DATA_INT, humidity,
            "button",        "Button",      DATA_INT, button,
            NULL);
    /* clang-format on

    decoder_output_data(decoder, data);
    return 1;
}

static char *output_fields[] = {
        "model",
        "subtype",
        "id",
        "rid", // TODO: delete this
        "channel",
        "battery",
        "temperature_C",
        "humidity",
        "button",
        NULL,
};

r_device prologue = {
        .name        = "Prologue, FreeTec NC-7104, NC-7159-675 temperature sensor",
        .modulation  = OOK_PULSE_PPM,
        .short_width = 2000,
        .long_width  = 4000,
        .gap_limit   = 7000,
        .reset_limit = 10000,
        .decode_fn   = &prologue_callback,
        .disabled    = 0,
        .fields      = output_fields,
};
**/
