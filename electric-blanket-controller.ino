/*
 * The following firmware runs on an ATTiny85 at 1 MHz
 */

volatile int interrupt_count = 0;
volatile int on_time = 2; // start with the blanket on 25% duty cycle
volatile uint8_t level = 1;
unsigned long last_debounce_time, last_level_flash_time = millis();
#define DEBOUNCE_DELAY 50 // ms 
#define LEVEL_UPDATE_INTERVAL 1500 // ms
uint8_t reading = 0, last_button_state = 0, last_valid_reading = 0;

/*
 * This timer interrupt handles switching the electric blanket off or on
 * 
 * Assuming a 1 MHz clock and 1024 prescaler
 * 0.26 or roughly 0.25 seconds pass every time the 
 * interrupt is triggered, so our duty cycle count should be 
 * 8 total interrupt events 
 */
#define TOTAL_CYCLES 8
ISR (TIMER1_OVF_vect) {
  if (2 * (level + 1) < on_time) {
    PORTB |= _BV(PB0); // MOSFET on
  } else {
    PORTB &= ~_BV(PB0); // MOSFET off
  }
  interrupt_count = (interrupt_count + 1) % TOTAL_CYCLES;
}

/*
 * Takes a positive integer as argument for how many times to flash LED
 */
#define PULSE_DURATION 250 // microseconds
#define FLASH_DURATION 300 // milliseconds
void flash_led(uint8_t num) {
  while (num) {
    // pulse light
    PORTB |= _BV(PB1);
    delayMicroseconds(PULSE_DURATION);
    PORTB &= ~_BV(PB1);
    delay(FLASH_DURATION);
    num--;
  }
}

void setup() {
  // setup pins
  DDRB |= _BV(PB0) | _BV(PB1); // pin 1 and pin 0 are outputs
  DDRB &= ~_BV(PB2); // pin 2 is input 

  // Timer 1
  TCCR1 = 0x00;
  TCCR1 |= (1 << CS13) | (1 << CS11) | (1 << CS10); // prescale timer by 1024
  sei();
  TCNT1=0;
  TIMSK|=(1<<TOIE1); //enable timer0 interrupt
}

void loop() {
  // listen/debounce button press
  reading = bit_is_set(PORTB, PB2);

  if (reading != last_button_state) {
    last_button_state = reading;
    last_debounce_time = millis();
  }

  // debounce input
  if (millis() - last_debounce_time >= DEBOUNCE_DELAY) {
    // valid input held greater than debounce period
    if (reading && ~last_valid_reading) {
      last_valid_reading = reading;
      level = (level + 1) % 4; // cycle through level
      flash_led(level + 1); // represent levels 1 to 4 with corresponding number of LED flashes
      last_level_flash_time = millis();
    } else { 
      last_valid_reading = reading;
    }
  }

  // periodically pulse LED to show current level
  if (millis() - last_level_flash_time >= LEVEL_UPDATE_INTERVAL) {
    flash_led(level + 1);
    last_level_flash_time = millis();
  }
}
