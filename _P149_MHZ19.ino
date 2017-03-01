/*

  This plug in is written by Dmitry (rel22 ___ inbox.ru)
  Plugin is based upon SenseAir plugin by Daniel Tedenljung info__AT__tedenljungconsulting.com
  
  This plugin reads the CO2 value from MH-Z19 NDIR Sensor
  DevicePin1 - is RX for ESP
  DevicePin2 - is TX for ESP

  Some updates with extra functionality by infernix
  https://github.com/infernix/ESPEasy/tree/stable_mh-z19

  Additional sensor info taken from https://geektimes.ru/post/285572/
*/

#define PLUGIN_149
#define PLUGIN_ID_149         149
#define PLUGIN_NAME_149       "NDIR CO2 Sensor MH-Z19"
#define PLUGIN_VALUENAME1_149 "PPM"
#define PLUGIN_VALUENAME2_149 "TEMP" // Temperature in C
#define PLUGIN_VALUENAME3_149 "S" // Undocumented, measurement stability? 4/8/16/32/64, higher=better?
#define PLUGIN_VALUENAME4_149 "U" // Undocumented, minimum measurement per time period?

boolean Plugin_149_init = false;

#include <SoftwareSerial.h>
SoftwareSerial *Plugin_149_S8;

// 9-bytes CMD PPM read command
byte mhzCmdReadPPM[9] = {0xFF,0x01,0x86,0x00,0x00,0x00,0x00,0x00,0x79}; 
byte mhzResp[9];    // 9 bytes bytes response
byte mhzCmdCalibrateZero[9] = {0xFF,0x01,0x87,0x00,0x00,0x00,0x00,0x00,0x78}; 
byte mhzCmdABCEnable[9] = {0xFF,0x01,0x79,0xA0,0x00,0x00,0x00,0x00,0xE6}; 
byte mhzCmdABCDisable[9] = {0xFF,0x01,0x79,0x00,0x00,0x00,0x00,0x00,0x86}; 
byte mhzCmdReset[9] = {0xFF,0x01,0x8d,0x00,0x00,0x00,0x00,0x00,0x72}; 
byte mhzCmdMeasurementRange1000[9] = {0xFF,0x01,0x99,0x00,0x00,0x00,0x03,0xE8,0x7B};
byte mhzCmdMeasurementRange2000[9] = {0xFF,0x01,0x99,0x00,0x00,0x00,0x07,0xD0,0x8F}; 
byte mhzCmdMeasurementRange3000[9] = {0xFF,0x01,0x99,0x00,0x00,0x00,0x0B,0xB8,0xA3}; 
byte mhzCmdMeasurementRange5000[9] = {0xFF,0x01,0x99,0x00,0x00,0x00,0x13,0x88,0xCB}; 


boolean Plugin_149(byte function, struct EventStruct *event, String& string)
{
  boolean success = false;

  switch (function)
  {

    case PLUGIN_DEVICE_ADD:
      {
        Device[++deviceCount].Number = PLUGIN_ID_149;
        Device[deviceCount].Type = DEVICE_TYPE_DUAL;
        Device[deviceCount].VType = SENSOR_TYPE_QUADRUPLE;
        Device[deviceCount].Ports = 0;
        Device[deviceCount].PullUpOption = false;
        Device[deviceCount].InverseLogicOption = false;
        Device[deviceCount].FormulaOption = true;
        Device[deviceCount].ValueCount = 4;
        Device[deviceCount].SendDataOption = true;
        Device[deviceCount].TimerOption = true;
        Device[deviceCount].GlobalSyncOption = true;
        break;
      }

    case PLUGIN_GET_DEVICENAME:
      {
        string = F(PLUGIN_NAME_149);
        break;
      }

    case PLUGIN_GET_DEVICEVALUENAMES:
      {
        strcpy_P(ExtraTaskSettings.TaskDeviceValueNames[0], PSTR(PLUGIN_VALUENAME1_149));
        strcpy_P(ExtraTaskSettings.TaskDeviceValueNames[1], PSTR(PLUGIN_VALUENAME2_149));
        strcpy_P(ExtraTaskSettings.TaskDeviceValueNames[2], PSTR(PLUGIN_VALUENAME3_149));
        strcpy_P(ExtraTaskSettings.TaskDeviceValueNames[3], PSTR(PLUGIN_VALUENAME4_149));
        break;
      }

    case PLUGIN_INIT:
      {
        Plugin_149_init = true;
        Plugin_149_S8 = new SoftwareSerial(Settings.TaskDevicePin1[event->TaskIndex], Settings.TaskDevicePin2[event->TaskIndex]); 
        success = true;
        break;
      }

    case PLUGIN_WRITE:
      {
        String log = "";
        String command = parseString(string, 1);

        if (command == F("mhzcalibratezero"))
        {
          Plugin_149_S8->write(mhzCmdCalibrateZero, 9);
          log = String(F("MHZ19: Calibrated zero point!"));
          addLog(LOG_LEVEL_INFO, log);
          success = true;
        }

        if (command == F("mhzreset"))
        {
          Plugin_149_S8->write(mhzCmdReset, 9);
          log = String(F("MHZ19: Sent sensor reset!"));
          addLog(LOG_LEVEL_INFO, log);
          success = true;
        }

        if (command == F("mhzabcenable"))
        {
          Plugin_149_S8->write(mhzCmdABCEnable, 9);
          log = String(F("MHZ19: Sent sensor ABC Enable!"));
          addLog(LOG_LEVEL_INFO, log);
          success = true;
        }

        if (command == F("mhzabcdisable"))
        {
          Plugin_149_S8->write(mhzCmdABCDisable, 9);
          log = String(F("MHZ19: Sent sensor ABC Disable!"));
          addLog(LOG_LEVEL_INFO, log);
          success = true;
        }

        if (command == F("mhzmeasurementrange1000"))
        {
          Plugin_149_S8->write(mhzCmdMeasurementRange1000, 9);
          log = String(F("MHZ19: Sent measurement range 0-1000PPM!"));
          addLog(LOG_LEVEL_INFO, log);
          success = true;
        }

        if (command == F("mhzmeasurementrange2000"))
        {
          Plugin_149_S8->write(mhzCmdMeasurementRange2000, 9);
          log = String(F("MHZ19: Sent measurement range 0-2000PPM!"));
          success = true;
          addLog(LOG_LEVEL_INFO, log);
        }

        if (command == F("mhzmeasurementrange3000"))
        {
          Plugin_149_S8->write(mhzCmdMeasurementRange3000, 9);
          log = String(F("MHZ19: Sent measurement range 0-3000PPM!"));
          addLog(LOG_LEVEL_INFO, log);
          success = true;
        }

        if (command == F("mhzmeasurementrange5000"))
        {
          Plugin_149_S8->write(mhzCmdMeasurementRange5000, 9);
          log = String(F("MHZ19: Sent measurement range 0-5000PPM!"));
          addLog(LOG_LEVEL_INFO, log);
          success = true;
        }

        break;
      }

    case PLUGIN_READ:
      {

        if (Plugin_149_init)
        {
          Plugin_149_S8->write(mhzCmdReadPPM, 9);
          memset(mhzResp, 0, 9);
          Plugin_149_S8->readBytes(mhzResp, 9);
          int i;
          unsigned int ppm = 0;
          signed int temp = 0;
          unsigned int s = 0;
          float u = 0;
          byte crc = 0;
          for (i = 1; i < 8; i++) crc+=mhzResp[i];
              crc = 255 - crc;
              crc++;
              
          // log and ignore any messages that fail CRC
          if ( !(mhzResp[8] == crc) ) {
             String log = F("MHZ19: CRC error: ");
             log += String(crc); log += " / "; log += String(mhzResp[8]);
             log += F(" raw: ");
             log += String(mhzResp[0], HEX);
             log += F(" ");
             log += String(mhzResp[1], HEX);
             log += F(" ");
             log += String(mhzResp[2], HEX);
             log += F(" ");
             log += String(mhzResp[3], HEX);
             log += F(" ");
             log += String(mhzResp[4], HEX);
             log += F(" ");
             log += String(mhzResp[5], HEX);
             log += F(" ");
             log += String(mhzResp[6], HEX);
             log += F(" ");
             log += String(mhzResp[7], HEX);
             log += F(" ");
             log += String(mhzResp[8], HEX);
             addLog(LOG_LEVEL_ERROR, log);
             // Sometimes there is a misalignment in the serial read 
             // and the starting byte 0xFF isn't the first read byte.
             // This goes on forever.
             // There must be a better way to handle this, but here 
             // we're trying to shift it so that 0xFF is the next byte
             byte crcshift;
             for (i = 1; i < 8; i++) {
                crcshift = Plugin_149_S8->peek();
                if (crcshift == 0xFF) {
                  String log = F("MHZ19: Shifted ");
                  log += i;
                  log += F(" bytes to attempt to fix buffer alignment");
                  addLog(LOG_LEVEL_ERROR, log);
                  break;
                } else {
                 crcshift = Plugin_149_S8->read();
                }
             }
             success = false;
             break;

          // Process responses to 0x86
          } else if (mhzResp[0] == 0xFF && mhzResp[1] == 0x86 && mhzResp[8] == crc)  {
          
              unsigned int mhzRespHigh = (unsigned int) mhzResp[2];
              unsigned int mhzRespLow = (unsigned int) mhzResp[3];
              ppm = (256*mhzRespHigh) + mhzRespLow;
              unsigned int mhzRespTemp = (unsigned int) mhzResp[4];
              temp = mhzRespTemp - 40;
              unsigned int mhzRespS = (unsigned int) mhzResp[5];
              s = mhzRespS;
              unsigned int mhzRespUHigh = (unsigned int) mhzResp[6];
              unsigned int mhzRespULow = (unsigned int) mhzResp[7];
              u = (256*mhzRespUHigh) + mhzRespULow;

              String log = F("MHZ19: ");
              // Sensor reports this as 15000 during boot, so don't process readings during that time
              if (u == 15000) {

                log += F("Bootup detected! PPM value: ");
                success = false;

              // If s = 0x40 the reading is stable; anything else should be ignored
              } else if (s < 64) {
                log += F("Unstable reading, ignoring! PPM value: ");
                success = false;

              // Stable readings are used
              } else {

                log += F("PPM value: ");
                success = true;

                UserVar[event->BaseVarIndex] = (float)ppm;
                UserVar[event->BaseVarIndex + 1] = (float)temp;
                UserVar[event->BaseVarIndex + 2] = (float)s;
                UserVar[event->BaseVarIndex + 3] = (float)u;
              }

              // Log values in all cases
              log += ppm;
              log += F(" Temp/S/U values: ");
              log += temp;
              log += F("/");
              log += s;
              log += F("/");
              log += u;
              addLog(LOG_LEVEL_INFO, log);
              break;


          } else if (mhzResp[0] == 0xFF && mhzResp[1] == 0x99 && mhzResp[8] == crc)  {

            String log = F("MHZ19: Received measurement range acknowledgment! ");
            log += F("Expecting sensor reset...");
            addLog(LOG_LEVEL_INFO, log);
            success = false;
            break;

          // log verbosely anything else that the sensor reports
          } else {

              String log = F("MHZ19: Unknown response: ");
              log += String(mhzResp[0], HEX);
              log += F(" ");
              log += String(mhzResp[1], HEX);
              log += F(" ");
              log += String(mhzResp[2], HEX);
              log += F(" ");
              log += String(mhzResp[3], HEX);
              log += F(" ");
              log += String(mhzResp[4], HEX);
              log += F(" ");
              log += String(mhzResp[5], HEX);
              log += F(" ");
              log += String(mhzResp[6], HEX);
              log += F(" ");
              log += String(mhzResp[7], HEX);
              log += F(" ");
              log += String(mhzResp[8], HEX);
              addLog(LOG_LEVEL_INFO, log);
              success = false;
              break;
           
          }

        }
        break;
      }
  }
  return success;
}
