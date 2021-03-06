#include <Wire.h>
#include <SparkFun_APDS9960.h>
#include <LiquidCrystal_I2C.h>

SparkFun_APDS9960 apds = SparkFun_APDS9960();
LiquidCrystal_I2C lcd(0x3f,16,2);  // run ic2_scanner sketch and get the IC2 address, which is 0x3f in my case,it could be 0x3f in many cases

uint16_t ambient_light = 0;
uint16_t red_light = 0;
uint16_t green_light = 0;
uint16_t blue_light = 0;

void setup() {
  
  // Initialize Serial port
  Serial.begin(9600);
  Serial.println();
  Serial.println(F("--------------------------------"));
  Serial.println(F("SparkFun APDS-9960 - ColorSensor"));
  Serial.println(F("--------------------------------"));
  
  // Initialize APDS-9960 (configure I2C and initial values)
  if ( apds.init() ) {
    Serial.println(F("APDS-9960 initialization complete"));
  } else {
    Serial.println(F("Something went wrong during APDS-9960 init!"));
  }
  
  // Start running the APDS-9960 light sensor (no interrupts)
  if ( apds.enableLightSensor(false) ) {
    Serial.println(F("Light sensor is now running"));
  } else {
    Serial.println(F("Something went wrong during light sensor init!"));
  }

  // Wait for initialization and calibration to finish
  delay(500);

  lcd.init();
}

void loop() {
  char r1[16] = "";
  char r2[16] = "";

  // set cursor to first line
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("                ");
  lcd.setCursor(0,1);
  lcd.print("                ");
  
  // Read the light levels (ambient, red, green, blue)
  if (  !apds.readAmbientLight(ambient_light) ||
        !apds.readRedLight(red_light) ||
        !apds.readGreenLight(green_light) ||
        !apds.readBlueLight(blue_light) ) {
    Serial.println("Error reading light values");
  } else {    sprintf(r1, "Ambient:%d", ambient_light);
    sprintf(r2, "RGB:%d,%d,%d", red_light, green_light, blue_light);

    // Print a message to the LCD.
    lcd.setCursor(0,0);
    lcd.print(r1);
    lcd.setCursor(0,1);
    lcd.print(r2);
  }

  // Wait 1 second before next reading
  delay(1000);
}
