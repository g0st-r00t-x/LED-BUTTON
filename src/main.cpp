#include <Arduino.h>

// Pin definitions
const int LED_GREEN_PIN = 2;
const int LED_YELLOW_PIN = 3;
const int LED_RED_PIN = 4;
const int BTN_SPEED_PIN = 12;
const int BTN_TOGGLE_PIN = 13;
const int BTN_ALTERNATE_PIN = 11;

// Timing variables for LED blinking
unsigned long previousMillis = 0;
const unsigned long NORMAL_INTERVAL = 1000; // Normal blinking interval (1 second)
const unsigned long FAST_INTERVAL = 200;    // Fast blinking interval (200 ms)
unsigned long alternateDelay = 500;         // Delay for alternating LEDs
int ledState = 0;                           // Current state of the LEDs
bool ledEnabled = false;                    // LED enabled/disabled state
bool fastBlink = false;                     // Fast blinking mode
bool alternateMode = false;                 // Alternate mode flag

void setup() {
    Serial.begin(9600);

    // Set pin modes
    pinMode(BTN_SPEED_PIN, INPUT_PULLUP);    // Speed button with pull-up resistor
    pinMode(BTN_TOGGLE_PIN, INPUT_PULLUP);   // Toggle button with pull-up resistor
    pinMode(BTN_ALTERNATE_PIN, INPUT_PULLUP); // Alternate button with pull-up resistor
    pinMode(LED_GREEN_PIN, OUTPUT);
    pinMode(LED_YELLOW_PIN, OUTPUT);
    pinMode(LED_RED_PIN, OUTPUT);
}

// Toggle LED enabled/disabled state
void handleToggleButton() {
    static bool lastToggleState = HIGH;
    int toggleState = digitalRead(BTN_TOGGLE_PIN);

    if (toggleState == LOW && lastToggleState == HIGH) {
        ledEnabled = !ledEnabled;
        alternateMode = false; // Exit alternate mode if toggled off
        Serial.println(ledEnabled ? "LED ON" : "LED OFF");

        if (!ledEnabled) {
            // Turn off LEDs when disabled
            digitalWrite(LED_GREEN_PIN, LOW);
            digitalWrite(LED_YELLOW_PIN, LOW);
            digitalWrite(LED_RED_PIN, LOW);
        }
        delay(300); // Simple debounce
    }

    lastToggleState = toggleState;
}

// Update blinking speed based on speed button
void handleSpeedButton() {
    static bool lastSpeedState = HIGH;
    int speedState = digitalRead(BTN_SPEED_PIN);

    if (speedState == LOW && lastSpeedState == HIGH) {
        fastBlink = !fastBlink;
        Serial.println(fastBlink ? "Fast Blink" : "Normal Blink");
        delay(50); // Simple debounce
    }

    lastSpeedState = speedState;
}

// Activate alternate mode when alternate button is pressed
void handleAlternateButton() {
    static bool lastAlternateState = HIGH;
    int alternateState = digitalRead(BTN_ALTERNATE_PIN);
   
    if (alternateState == LOW && lastAlternateState == HIGH) {
        alternateMode = !alternateMode;
        Serial.println(alternateMode ? "Alternate Mode ON" : "Alternate Mode OFF");

        if (!alternateMode) {
            // Turn off LEDs when exiting alternate mode
            digitalWrite(LED_GREEN_PIN, LOW);
            digitalWrite(LED_YELLOW_PIN, LOW);
            digitalWrite(LED_RED_PIN, LOW);
        }
        delay(50); // Simple debounce
    }

    lastAlternateState = alternateState;
}

// Control LED blinking based on current settings
void updateLEDs() {
    unsigned long currentMillis = millis();
    unsigned long interval = fastBlink ? FAST_INTERVAL : NORMAL_INTERVAL;

    if (alternateMode && ledEnabled) {
        // Alternate mode: LEDs blink in sequence with adjustable delay
        if (currentMillis - previousMillis >= (fastBlink ? FAST_INTERVAL : alternateDelay)) {
            previousMillis = currentMillis; // Reset timer

            // Turn off all LEDs before updating ledState
            digitalWrite(LED_GREEN_PIN, LOW);
            digitalWrite(LED_YELLOW_PIN, LOW);
            digitalWrite(LED_RED_PIN, LOW);
            
            // Turn on LED based on ledState and switch state
            switch (ledState) {
                case 0:
                    digitalWrite(LED_GREEN_PIN, HIGH);
                    break;
                case 1:
                    digitalWrite(LED_YELLOW_PIN, HIGH);
                    break;
                case 2:
                    digitalWrite(LED_RED_PIN, HIGH);
                    break;
            }
            ledState = (ledState + 1) % 3; // Cycle through LEDs
        }
    } else if (ledEnabled) {
        // Normal or fast blinking mode: All LEDs blink together
        if (currentMillis - previousMillis >= interval) {
            ledState = !ledState; // Toggle all LEDs together
            digitalWrite(LED_GREEN_PIN, ledState ? HIGH : LOW);
            digitalWrite(LED_YELLOW_PIN, ledState ? HIGH : LOW);
            digitalWrite(LED_RED_PIN, ledState ? HIGH : LOW);
            previousMillis = currentMillis; // Reset timer
        }
    }
}

void loop() {
    handleToggleButton();   // Check toggle button
    handleSpeedButton();    // Check speed button
    handleAlternateButton(); // Check alternate button
    updateLEDs();           // Update LEDs based on mode
}
