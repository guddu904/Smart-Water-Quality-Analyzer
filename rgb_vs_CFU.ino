#include <Wire.h>
#include <Adafruit_TCS34725.h>
#include <LiquidCrystal_I2C.h>

// Initialize the TCS34725 sensor
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_600MS, TCS34725_GAIN_1X);

// Initialize the I2C LCD (address 0x27, 16 columns, 2 rows)
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Function to approximate CFU from clear channel with new calibrations
float calculateCFU(uint16_t clear) {
  if (clear > 2500) {
    return 0.5;  // Mineral water: CFU < 1
  } else if (clear > 1000) {
    return (clear - 1000) * 0.01;  // Moderate contamination
  } else {
    return clear * 0.01;  // Higher CFU for lower clear values
  }
}

void setup() {
  Serial.begin(115200);

  // Initialize the LCD
  lcd.init();
  lcd.backlight();

  // Display "Device Starting" on LCD
  lcd.setCursor(0, 0);
  lcd.print("Device Starting");
  delay(5000);  // Wait for 5 seconds

  // Display "Analyzing" on LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Analyzing");
  delay(7000);  // Wait for 7 seconds

  // Initialize the TCS34725 sensor
  if (!tcs.begin()) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Sensor Not Found");
    Serial.println("TCS34725 not found! Check wiring.");
    while (1);  // Halt execution
  }
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Device Ready");
  Serial.println("TCS34725 initialized.");
  delay(2000);  // Pause before starting readings
}

void loop() {
  uint16_t r, g, b, c;

  // Read sensor data
  tcs.getRawData(&r, &g, &b, &c);

  if (r == 0 && g == 0 && b == 0) {
    Serial.println("Invalid data from TCS34725. Skipping this cycle.");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Invalid Data");
    delay(10000);  // Wait before retrying
    return;
  }

  // Calculate CFU from clear channel using new calibrations
  float cfu = calculateCFU(c);

  // Print results to Serial Monitor
  Serial.print("Red: "); Serial.print(r);
  Serial.print(", Green: "); Serial.print(g);
  Serial.print(", Blue: "); Serial.print(b);
  Serial.print(", Clear: "); Serial.print(c);
  Serial.print(", Estimated CFU: "); Serial.println(cfu);

  // Display results on LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("RGB: ");
  lcd.print(r); lcd.print(",");
  lcd.print(g); lcd.print(",");
  lcd.print(b);

  lcd.setCursor(0, 1);
  lcd.print("CFU: ");
  lcd.print(cfu, 2);  // Display CFU with 2 decimal places

  delay(10000);  // Wait for 10 seconds before next reading
}
