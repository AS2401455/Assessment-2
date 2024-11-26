#include <avr/io.h>
#include <util/delay.h>
#include "Lcd.h"

// Pin Definitions
#define TEMP_SENSOR_PIN 5  // ADC5 pin for temperature sensor 
#define LED_PIN PD2        // LED connected to PD2
#define UP_BUTTON 0        // Up button on LCD shield
#define DOWN_BUTTON 1      // Down button on LCD shield

// Global Variables
int highLimit = 30; // Default high limit 
int lowLimit = 20;  // Default low limit 
int temperature = 0;

// Function to initialize ADC
void ADC_Init() {
    ADMUX = (1 << REFS0);                   
    ADCSRA = (1 << ADEN) | (7 << ADPS0);    
}

// Function to read ADC value from a specific channel
int ADC_Read(unsigned char channel) {
    ADMUX = (ADMUX & 0xF0) | (channel & 0x0F); // Select ADC channel
    ADCSRA |= (1 << ADSC);                     // Start ADC conversion
    while (ADCSRA & (1 << ADSC));              // Wait till done
    return ADC;
}

// Function to read temperature in °C
int readTemperature() {
    int adcValue = ADC_Read(TEMP_SENSOR_PIN); // Read ADC value
    return (adcValue * 500) / 1024;           // Convert ADC value to C
}

// Function to read button pressed on LCD shield
int readButton() {
    int adcValue = ADC_Read(1); // Read ADC value from the button
    if (adcValue < 50) return UP_BUTTON;     // UP button
    if (adcValue < 200) return DOWN_BUTTON; // DOWN button
    return -1; // No button pressed
}

void setup() {
    LCD_Init();      // Initialize LCD
    ADC_Init();      // Initialize ADC
    DDRB |= (1 << LED_PIN); // Set LED pin as output
    PORTB &= ~(1 << LED_PIN); // Turn off LED initially

    // Initial Display
    LCD_Clear();
    LCD_String("Temp Monitor");
    _delay_ms(1000);
}

void loop() {
    temperature = readTemperature(); // Read current temperature
    int button = readButton();       // Read button press

    // Adjust limits using buttons
    if (button == UP_BUTTON) {
        highLimit++;  // Increase high limit
        _delay_ms(500); // Debounce delay
    } else if (button == DOWN_BUTTON) {
        lowLimit--;  // Decrease low limit
        _delay_ms(500); // Debounce delay
    }

    // Display temperature, LH, LL on LCD
    LCD_Clear();
    LCD_String_xy(0, 0, "Temp:");
    char tempStr[4];
    itoa(temperature, tempStr, 10);
    LCD_String(tempStr);
    LCD_String("C");

    LCD_String_xy(1, 0, "LH:");
    itoa(highLimit, tempStr, 10);
    LCD_String(tempStr);
    LCD_String(" LL:");
    itoa(lowLimit, tempStr, 10);
    LCD_String(tempStr);

    // Check if temperature is in range
    if (temperature > highLimit || temperature < lowLimit) {
        LCD_String_xy(0, 10, "NOT OKAY");
        PORTD |= (1 << LED_PIN); // Turn on LED
    } else {
        LCD_String_xy(0, 10, "OKAY    ");
        PORTD &= ~(1 << LED_PIN); // Turn off LED
    }

    _delay_ms(2000); // Small delay for stability
}
