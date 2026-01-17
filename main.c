#define F_CPU 8000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <string.h>

// Servo Configuration
#define SERVO_MIN   1000   // Open (1ms)
#define SERVO_MAX   2000   // Closed (2ms)

// Ultrasonic Pins 
#define US_PORT PORTC
#define US_PIN  PINC
#define US_DDR  DDRC
#define TRIG_PIN PC0
#define ECHO_PIN PC5

// Safety Settings 
#define OBSTACLE_DIST 15

// LCD Pin Definitions
// LCD Control is on PD4/PD5.
#define LCD_DATA_PORT PORTC
#define LCD_CTRL_PORT PORTD
#define LCD_RS        PD4
#define LCD_EN        PD5

// Keypad Configuration
// MOVED R1/R2 to PD6/PD7 to avoid UART conflict on PD0/PD1
// R1=PD6, R2=PD7, R3=PD2, R4=PD3
#define ROW_PORT    PORTD
#define ROW_DDR     DDRD
#define ROW_PIN     PIND

#define COL_PORT    PORTB
#define COL_DDR     DDRB
#define COL_PIN     PINB

// Global Variables
volatile uint8_t is_closing = 0;
uint16_t current_pwm = SERVO_MIN;
char password[5] = "1234";  
char input_buffer[5];      
uint8_t key_index = 0;

// LCD Functions
void LCD_EnablePulse() {
    LCD_CTRL_PORT |= (1 << LCD_EN);
    _delay_us(2);
    LCD_CTRL_PORT &= ~(1 << LCD_EN);
    _delay_us(100);
}

void LCD_SendNibble(unsigned char nibble) {
    unsigned char current_port = LCD_DATA_PORT & 0xE1;
    unsigned char new_data = (nibble & 0x0F) << 1;
    LCD_DATA_PORT = current_port | new_data;
    LCD_EnablePulse();
}

void LCD_Command(unsigned char cmnd) {
    LCD_CTRL_PORT &= ~(1 << LCD_RS);
    LCD_SendNibble(cmnd >> 4);
    LCD_SendNibble(cmnd);
    _delay_ms(2);
}

void LCD_Char(unsigned char data) {
    LCD_CTRL_PORT |= (1 << LCD_RS);
    LCD_SendNibble(data >> 4);
    LCD_SendNibble(data);
    _delay_us(50);
}

void LCD_Init() {
    DDRD |= (1 << LCD_RS) | (1 << LCD_EN);
    // PC1-PC4 for LCD Data
    DDRC |= (1 << PC1) | (1 << PC2) | (1 << PC3) | (1 << PC4);
    _delay_ms(50);
    LCD_CTRL_PORT &= ~(1 << LCD_RS);
    LCD_SendNibble(0x03); _delay_ms(5);
    LCD_SendNibble(0x03); _delay_us(150);
    LCD_SendNibble(0x03);
    LCD_SendNibble(0x02);
    _delay_ms(2);
    LCD_Command(0x28);
    LCD_Command(0x0C);
    LCD_Command(0x06);
    LCD_Command(0x01);
    _delay_ms(2);
}

void LCD_String(char *str) {
    while(*str) LCD_Char(*str++);
}

void LCD_Clear() {
    LCD_Command(0x01); _delay_ms(2);
}

void LCD_SetCursor(uint8_t row, uint8_t col) {
    uint8_t address = (row == 0) ? (0x80 + col) : (0xC0 + col);
    LCD_Command(address);
}

// Ultrasonic Functions
void Ultrasonic_Init() {
    US_DDR |= (1 << TRIG_PIN);
    US_DDR &= ~(1 << ECHO_PIN);
    US_PORT &= ~(1 << TRIG_PIN);
}

uint16_t Ultrasonic_Read() {
    uint16_t count = 0;
    
    // Trigger Pulse
    US_PORT |= (1 << TRIG_PIN); 
    _delay_us(10); 
    US_PORT &= ~(1 << TRIG_PIN);
    
    // Wait for Echo to START
    uint32_t timeout = 0; // Changed to uint32_t for safety
    while (!(US_PIN & (1 << ECHO_PIN))) {
        if (++timeout > 10000) return 999; // Short wait for start
        _delay_us(1);
    }
    
    // INCREASED TIMEOUT HERE to fix the "999" 
    while (US_PIN & (1 << ECHO_PIN)) {
        _delay_us(1);
        if (++count > 60000) break; 
    }
    
    // Conversion factor time into distance
    return (count / 27); 
}

// Peripheral Init
void PWM_Init() {
    DDRB |= (1<<PB1);
    TCCR1A = (1<<COM1A1) | (1<<WGM11);
    TCCR1B = (1<<WGM13) | (1<<WGM12) | (1<<CS11);
    ICR1 = 19999;
}

void Set_Servo(uint16_t pulse_width) {
    if(pulse_width < SERVO_MIN) pulse_width = SERVO_MIN;
    if(pulse_width > SERVO_MAX) pulse_width = SERVO_MAX;
    OCR1A = pulse_width;
}

// Keypad Function
// Row mappings: R1=PD6, R2=PD7, R3=PD2, R4=PD3
const uint8_t row_pins[4] = {PD6, PD7, PD2, PD3};

void Keypad_Init() {
    // Row Pins as Output
    ROW_DDR |= (1<<PD6)|(1<<PD7)|(1<<PD2)|(1<<PD3);
    // Rows High initially (Idle)
    ROW_PORT |= (1<<PD6)|(1<<PD7)|(1<<PD2)|(1<<PD3);

    // Columns (PB2-PB5) as Input
    COL_DDR &= ~((1<<PB2)|(1<<PB3)|(1<<PB4)|(1<<PB5));
    // Enable Pull-up resistors on Columns
    COL_PORT |= ((1<<PB2)|(1<<PB3)|(1<<PB4)|(1<<PB5));
}

unsigned char Keypad_Scan() {
    unsigned char keypad[4][4] = {
        {'7','8','9','/'},  
        {'4','5','6','*'},  
        {'1','2','3','-'},  
        {'C','0','=','+'}   
    };

    uint8_t r, c;

    for (r = 0; r < 4; r++) {
        // All Rows High first
        ROW_PORT |= (1<<PD6)|(1<<PD7)|(1<<PD2)|(1<<PD3);
       
        // CURRENT Row LOW
        ROW_PORT &= ~(1 << row_pins[r]);

        // Check Columns
        _delay_us(5); // Small delay for signal stabilization
       
        if (!(PINB & (1 << PB2))) { _delay_ms(200); return keypad[r][0]; }
        if (!(PINB & (1 << PB3))) { _delay_ms(200); return keypad[r][1]; }
        if (!(PINB & (1 << PB4))) { _delay_ms(200); return keypad[r][2]; }
        if (!(PINB & (1 << PB5))) { _delay_ms(200); return keypad[r][3]; }
    }
    return 0; // No key pressed
}

// Main System

void Reset_Entry() {
    key_index = 0;
    memset(input_buffer, 0, 5);
    LCD_SetCursor(1, 0);
    LCD_String("Enter PIN:      ");
}

// 3 modes the system can be in
#define STATE_LOCKED  0
#define STATE_OPEN    1
#define STATE_CLOSING 2

int main(void) {
    // SETUP
    DDRB |= (1<<PB0); // LED Output
    PWM_Init();
    LCD_Init();
    Ultrasonic_Init();
    Keypad_Init();
    
    // Start with Gate Closed
    Set_Servo(SERVO_MAX); 
    current_pwm = SERVO_MAX;
    
    // Default State: Locked
    uint8_t system_state = STATE_LOCKED;
    char buffer[16];
    char key;
    uint16_t distance = 0;
    
    // Initial Screen
    LCD_Clear();
    LCD_String("SECURE GATE");
    _delay_ms(1000);
    Reset_Entry(); // Shows "Enter PIN:"

    while(1) {
        // We always scan the keypad, but we only USE it in STATE_LOCKED
        key = Keypad_Scan();

        // MODE 1: LOCKED (Waiting for PIN)
        if (system_state == STATE_LOCKED) {
            
            if (key) {
                // Handle Digits 0-9
                if (key >= '0' && key <= '9') {
                    if (key_index < 4) {
                        input_buffer[key_index] = key;
                        LCD_SetCursor(1, 11 + key_index);
                        LCD_Char('*');
                        key_index++;
                    }
                    // Anti-bounce: Wait for release
                    while(Keypad_Scan() != 0) _delay_ms(10);
                }
                
                // Handle Clear
                else if (key == 'C') {
                    Reset_Entry();
                    while(Keypad_Scan() != 0) _delay_ms(10);
                }
                
                // Handle Enter (=)
                else if (key == '=') {
                    if (strncmp(input_buffer, password, 4) == 0) {
                        LCD_Clear();
                        LCD_String("ACCESS GRANTED");
                        PORTB |= (1<<PB0); // LED ON
                        
                        // OPEN THE GATE
                        LCD_SetCursor(1, 0);
                        LCD_String("Opening...");
                        
                        while(current_pwm > SERVO_MIN) {
                            current_pwm -= 20;
                            Set_Servo(current_pwm);
                            _delay_ms(20);
                        }
                        
                        PORTB &= ~(1<<PB0); // LED OFF
                        
                        // Switchs to OPEN state. 
                        system_state = STATE_OPEN; 
                        LCD_Clear();
                        LCD_String("Gate Open");
                    } else {
                        LCD_Clear();
                        LCD_String("ACCESS DENIED");
                        _delay_ms(1000);
                        Reset_Entry();
                    }
                    while(Keypad_Scan() != 0) _delay_ms(10);
                }
            }
        }

        // MODE 2: OPEN (Watching Sensor)
        else if (system_state == STATE_OPEN) {
            
            // Read Sensor constantly
            distance = Ultrasonic_Read();

            // Update LCD (only every 20th loop to prevent flickering)
            static uint8_t lcd_timer = 0;
            lcd_timer++;
            if(lcd_timer > 20) {
                sprintf(buffer, "DIST: %d cm      ", distance);
                LCD_SetCursor(0, 0);
                LCD_String(buffer);
                LCD_SetCursor(1, 0);
                LCD_String("Waiting exit...");
                lcd_timer = 0;
            }

            // CHECK TRIGGER: If object is detected (< 17cm)
            if (distance < 17 && distance > 0) {
                LCD_Clear();
                LCD_String("");
                PORTB |= (1<<PB0); // Warning LED
                _delay_ms(1500);   // Give user a moment to see the message
                
                // Switchs to CLOSING state
                system_state = STATE_CLOSING;
                is_closing = 1; 
            }
            
            // Manual Close with '-' button
            if (key == '-') {
                system_state = STATE_CLOSING;
                is_closing = 1;
            }
        }

        // MODE 3: CLOSING (Moving Servo)
        else if (system_state == STATE_CLOSING) {
            
            // Just move the servo until it is closed
            if (current_pwm < SERVO_MAX) {
                current_pwm += 20; 
                Set_Servo(current_pwm);
                _delay_ms(20);
            } 
            else {
                // Gate is now fully closed
                is_closing = 0;
                PORTB &= ~(1<<PB0); // LED OFF
                
                // Update Screen
                LCD_Clear();
                LCD_String("Gate Closed");
                _delay_ms(1000);
                
                // Reset System to Locked Mode
                system_state = STATE_LOCKED;
                Reset_Entry();
            }
        }
    }
}
