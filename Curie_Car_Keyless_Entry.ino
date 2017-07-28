/*
 * Created by Zach Brogan
 * July 2017
 * 
 * Curie Car Keyless Entry
 * Configures Arduino 101 as BLE central and searches for a specific BLE device by UUID. 
 * Designed to work with companion iPhone app BLE Key Fob for locking and unlocking your car automatically.
 */

#include <CurieBLE.h>

#define NOT_FOUND_COUNT_MAX 30 // When out of range, car locks after 15s
#define RSSI_MIN -72 // Approximately 5-10 meter radius around vehicle
#define UNLOCK_PIN 12
#define LOCK_PIN 9
#define TRUNK_PIN 11 // TODO: Add SW support for trunk and panic buttons
#define PANIC_PIN 10
#define DEBUG

bool locked = true;
bool found = false;
int notFoundCount = 0;

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(UNLOCK_PIN, OUTPUT);
  pinMode(LOCK_PIN, OUTPUT);
  digitalWrite(LOCK_PIN, LOW);
  digitalWrite(UNLOCK_PIN, LOW);
  Serial.begin(9600);

  // initialize the BLE hardware
  BLE.begin();

  Serial.println("BLE Passive Keyless Entry");
  Serial.println("Searching for peripheral to unlock vehicle...");

  // start scanning for peripheral
  BLE.scanForUuid("B309A400-23D7-4671-991F-2AC4A37A3131", true);
}

void loop() {
  BLEDevice peripheral = BLE.available();
  if(peripheral && peripheral.rssi() > RSSI_MIN) { // Found
    #ifdef DEBUG
    Serial.println(peripheral.rssi());
    #endif
    notFoundCount = 0;
    if(locked)  unlock();
    
  }
  else { // Not found
    #ifdef DEBUG
    Serial.print(".");
    #endif
    if(!locked) {
      notFoundCount++;
      if(notFoundCount > NOT_FOUND_COUNT_MAX) {
        lock();
      }
    }
  }
  delay(500); // Loop every half second
}

void unlock() {
  #ifdef DEBUG
  Serial.println();
  Serial.println("Unlocking");
  #endif
  locked = false;
  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on to indicate car is unlocked
  //GPIO on/off (2x) to assert relay connected to key fob pcb unlock button
  digitalWrite(UNLOCK_PIN, HIGH);
  delay(100);
  digitalWrite(UNLOCK_PIN, LOW);
  delay(500);
  digitalWrite(UNLOCK_PIN, HIGH);
  delay(100);
  digitalWrite(UNLOCK_PIN, LOW);
}

void lock(){
  #ifdef DEBUG
  Serial.println();
  Serial.println("Locking");
  #endif
  locked = true;
  digitalWrite(LED_BUILTIN, LOW);   // turn the LED off
  //GPIO on/off (1x) to assert relay connected to key fob pcb lock button
  digitalWrite(LOCK_PIN, HIGH);
  delay(100);
  digitalWrite(LOCK_PIN, LOW);
}


