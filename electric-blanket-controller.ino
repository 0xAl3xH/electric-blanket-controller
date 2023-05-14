/*
 * The following firmware runs on an ATTiny85 at 1 MHz
 */

volatile uint8_t level = 1; // start with the blanket on 50% duty cycle
volatile unsigned long last_debounce_time = millis();
volatile unsigned long last_level_flash_time = millis();

/*
 * This timer interrupt handles switching the electric blanket off or on
 *
 * Assuming a 1 MHz clock and 1024 prescaler
 * 0.26 or roughly 0.25 seconds pass every time the
 * interrupt is triggered, so our duty cycle count should be
 * 8 total interrupt events
 */
volatile int interrupt_count = 0;
#define TOTAL_CYCLES 8
ISR(TIMER1_OVF_vect)
{
  if (interrupt_count < 2 * (level + 1))
  {
    PORTB |= _BV(PB0); // MOSFET on
  }
  else
  {
    PORTB &= ~_BV(PB0); // MOSFET off
  }
  interrupt_count = (interrupt_count + 1) % TOTAL_CYCLES;
}

/*
 * This external interrupt handles debouncing button input and
 * cycling through different power levels
 */
volatile unsigned long now;
volatile uint8_t state = 1;
volatile uint8_t reading = 0, last_button_state = 0, last_valid_reading = 0;
#define DEBOUNCE_DELAY 25 // ms
ISR(INT0_vect)
{
  now = millis();
  reading = bit_is_set(PINB, PB2);
  switch (state)
  {
  case 1: // wait for the rising edge / button press in this state
    if (reading)
    {
      last_debounce_time = now;
      state = 2;
    }
    break;
  case 2: // wait for the falling edge / button release in this state
    if (reading)
    { // invalid, go back to state 1
      state = 1;
    }
    else
    {
      // button press is valid if time between rising/falling edge exceeds
      // debounce delay
      bool valid = now - last_debounce_time > DEBOUNCE_DELAY;
      level = valid ? (level + 1) % 4 : level;
      last_level_flash_time = valid ? 0 : last_level_flash_time;
      state = 1;
    }
    break;
  default:
    state = 1;
  }
}

/*
 * Takes a positive integer as argument for how many times to flash LED
 */
#define PULSE_DURATION 250 // microseconds
#define FLASH_DURATION 300 // milliseconds
void flash_led(uint8_t num)
{
  while (num)
  {
    // pulse light
    PORTB |= _BV(PB1);
    delayMicroseconds(PULSE_DURATION);
    PORTB &= ~_BV(PB1);
    delay(FLASH_DURATION);
    num--;
  }
}

void setup()
{
  // setup pins
  DDRB |= _BV(PB0) | _BV(PB1); // pin 1 and pin 0 are outputs
  DDRB &= ~_BV(PB2);           // pin 2 is input

  // Timer 1
  TCCR1 = 0x00;
  TCCR1 |= (1 << CS13) | (1 << CS11) | (1 << CS10); // prescale timer by 1024
  TCNT1 = 0;
  TIMSK |= (1 << TOIE1); // enable TIMER0 interrupt

  // External interreupt INT0
  MCUCR |= _BV(ISC00); // trigger on any edge
  GIMSK |= _BV(INT0);  // enable INT0

  sei();
}

#define LEVEL_UPDATE_INTERVAL 1000 // ms
void loop()
{
  // periodically pulse LED to show current level
  if (millis() - last_level_flash_time >= LEVEL_UPDATE_INTERVAL)
  {
    flash_led(level + 1);
    last_level_flash_time = millis();
  }
}
