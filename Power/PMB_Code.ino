#include <OneButton.h>
#include <EEPROM.h>
#include <ACAN2515.h>

// Sensor Stuff //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define max_current_address 0 // address 0  for max current drawn
#define batt_A_calibrated_address 1 // 4 bytes per float
#define batt_B_calibrated_address 5 // 4 bytes per float

#define batt_A_sensor A1
#define batt_B_sensor A0
#define current_sensor A7
// End of Sensor Stuff //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define button_pin 4

#define Pink_LED 3
#define WarmW_LED 5
#define Green_LED 6
#define relay_set A2
#define relay_reset A3

#define relay_set_pulse 120

// MCP Stuff //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static const byte MCP2515_SCK  = 13 ; // SCK input of MCP2515 (adapt to your design)
static const byte MCP2515_MOSI = 11 ; // SDI input of MCP2515 (adapt to your design)
static const byte MCP2515_MISO = 12 ; // SDO output of MCP2515 (adapt to your design)
static const byte MCP2515_CS   = 10 ;  // CS input of MCP2515 (adapt to your design)

ACAN2515 can (MCP2515_CS, SPI, 255) ;

static const uint32_t QUARTZ_FREQUENCY = 8UL * 1000UL * 1000UL ; // 8 MHz
// End of MCP Stuff //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// OneButton Stuff //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// The 2. parameter activeLOW is true, because external wiring sets the button to LOW when pressed.
OneButton button(button_pin, true);

// save the millis when a press has started.
unsigned long pressStartTime;
// End of OneButton stuff //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool debug_mode = false;

#if 

bool debug_print_on = true;               // set to false to clear away serial monitor

bool relay_state = HIGH;                  // relay state flag

float batt_A_volt = 0;                    // storing average battery A voltage
float batt_B_volt = 0;                    // storing average battery B voltage
float current = 0;                        // storing average current
float max_current = 0;                    // storing peak current drawn, currently no way to clear except for restarting nano

uint8_t counter = 0;

float low_batt = 15.0;                    // gives warning of low battery and would not on if shut offed
float shut_off_voltage = 14.8;            // calls for shut down and shut down
float low_batt_recover = low_batt * 1.02; // hysteresis so no flickers

float shut_down_current = 0.5;            // check for current less than this before shutting down

bool low_battery = false;                 // low battery flag
bool shut_down = true;                    // shut down everything flag

unsigned long prev_comms_millis;
unsigned long prev_sample_millis;

void setup() {
  pinMode(Pink_LED, OUTPUT);
  pinMode(WarmW_LED, OUTPUT);
  pinMode(Green_LED, OUTPUT);

  pinMode(relay_set, OUTPUT);
  pinMode(relay_reset, OUTPUT);

  pinMode(batt_A_sensor, INPUT);
  pinMode(batt_B_sensor, INPUT);
  pinMode(current_sensor, INPUT);

  digitalWrite(Pink_LED, LOW);
  digitalWrite(WarmW_LED, LOW);
  digitalWrite(Green_LED, HIGH);

  digitalWrite(relay_set, LOW);
  digitalWrite(relay_reset, LOW);

  read_current(true); // Calibrate current sensor and on relay

  Serial.begin(9600);

  // OneButton Stuff //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  attachInterrupt(digitalPinToInterrupt(button_pin), checkTicks, CHANGE);
  button.attachClick(singleClick);
  button.attachDoubleClick(doubleClick);
  button.attachMultiClick(multiClick);
  button.attachLongPressStart(pressStart);
  button.attachLongPressStop(pressStop);
  // End of OneButton Stuff //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  MCP_begin();
}

void loop() {
  button.tick();
  //  voltage_monitoring();

  //todo
  // average samples
  // shut down call
  // max current
  // check current and shut down call before shutting down


  //  if (millis() - prev_sample_millis > 100) {
  //    batt_A_volt += read_voltage(0, false);
  //    batt_B_volt += read_voltage(1, false);
  //    current += read_current(false);
  //    counter += 1;
  //  }


  if (millis() - prev_comms_millis > 1000) {
    batt_A_volt = read_voltage(0, false);
    batt_B_volt = read_voltage(1, false);
    current = read_current(false);
    
    if (batt_A_volt > low_batt_recover || batt_B_volt > low_batt_recover) {
      low_battery = false;
      shut_down = false;
      digitalWrite(Pink_LED, low_battery);
      digitalWrite(Green_LED, !shut_down);
    } else if (batt_A_volt > low_batt || batt_B_volt > low_batt) {
      digitalWrite(Pink_LED, low_battery);
      digitalWrite(Green_LED, !shut_down);
    } else if (batt_A_volt > shut_off_voltage || batt_B_volt > shut_off_voltage) {
      low_battery = true;
      digitalWrite(Pink_LED, low_battery);
      digitalWrite(Green_LED, !shut_down);
    } else {
      shut_down = true;
      digitalWrite(Pink_LED, shut_down);
      digitalWrite(Green_LED, !shut_down);
    }

    // mulitple debug print else string too long can't print for some reason
    debug_serial_print(debug_print_on, (String)"Battery A = " + batt_A_volt + " V   " +
                                               "Battery B = " + batt_B_volt + " V");

    debug_serial_print(debug_print_on, (String)"Current = " + current + " A   " + 
                                               "Max Current = " + max_current + " A");

    debug_serial_print(debug_print_on, (String)"Low Battery, resurface = " + low_battery  + "   " +
                                               "Shutting down = " + shut_down);

    if (relay_state) {
      CAN_bus_send(byte(batt_A_volt * 10), byte(batt_B_volt * 10), byte(current * 10), byte(max_current * 10), byte(low_battery), byte(shut_down), byte(0), byte(0));
    }

    prev_comms_millis = millis();
  }
}
