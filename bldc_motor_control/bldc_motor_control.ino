**** CAUTION ****  **** CAUTION ****  **** CAUTION ****  **** CAUTION ****  **** CAUTION ****
 *
 * This code is written specifically for an Arduino Nano Every. It will NOT work on other Arduino
 * microcontrollers as it depends on direct SFR manipulation to operate in a fast PWM mode.
 * For fast PWM technique refer to https://forum.digikey.com/t/fast-pwm-for-the-arduino-nano-every/40023
 *
 */

#define AL_PIN 10  // Phase A
#define AH_PIN 9   // PWM

#define BL_PIN 7  // Phase B
#define BH_PIN 6  // PWM

#define CL_PIN 4  // Phase C
#define CH_PIN 3  // PWM

#define H3_PIN 18  // Hall sensor inputs
#define H2_PIN 19
#define H1_PIN 20

#define VREF_PIN A7  // Duty cycle setpoint

#define MAX_PWM_VAL 250

void setup( ) {

TCA0.SINGLE.CTRLA = 0b00000011;  // All PWM to 32 kHz (ATMega4809 specific SFR)

  pinMode(AL_PIN, OUTPUT); 
  pinMode(AH_PIN, OUTPUT);
  pinMode(BL_PIN, OUTPUT);
  pinMode(BH_PIN, OUTPUT);
  pinMode(CL_PIN, OUTPUT);
  pinMode(CH_PIN, OUTPUT);

  pinMode(H3_PIN, INPUT);
  pinMode(H2_PIN, INPUT);
  pinMode(H1_PIN, INPUT);

}

void loop( ) {

  cli( ); // Disable interrupts

  while (1) {

    static uint8_t last_hall, last_D;

    uint16_t setpoint = analogRead(VREF_PIN);

    uint8_t D = setpoint >> 2; //D controls speed using PWM
    if (D > MAX_PWM_VAL) {
      D = MAX_PWM_VAL;
    }

    uint8_t hall = 0x00;
    hall = hall + digitalRead(H3_PIN);
    hall = hall << 1;
    hall = hall + digitalRead(H2_PIN);
    hall = hall << 1;
    hall = hall + digitalRead(H1_PIN);

    if ((hall != last_hall) || (D != last_D)) {

      if (hall != last_hall) {   // All off for new commutation
        analogWrite(AH_PIN, 0);  // Upper PWM drivers off
        analogWrite(BH_PIN, 0);
        analogWrite(CH_PIN, 0);
      }

      last_hall = hall;
      last_D = D;

      switch (hall) { //A is U in diagram, B is W, C is V,
        // this going backwards of normal cycle but will still work 

        case 0b001: //D voltage sent to motor A, C open receieves -V
          digitalWrite(AL_PIN, LOW);
          digitalWrite(BL_PIN, LOW);
          digitalWrite(CL_PIN, HIGH);
          analogWrite(AH_PIN, D);
          break;

        case 0b011: //D voltage sent to motor B, C open receieves -V
          digitalWrite(AL_PIN, LOW);
          digitalWrite(BL_PIN, LOW);
          digitalWrite(CL_PIN, HIGH);
          analogWrite(BH_PIN, D);
          break;

        case 0b010: //D voltage sent to motor B, A open receieves -V
          digitalWrite(BL_PIN, LOW);
          digitalWrite(CL_PIN, LOW);
          digitalWrite(AL_PIN, HIGH);
          analogWrite(BH_PIN, D);
          break;

        case 0b110: //D voltage sent to motor C, A open receieves -V
          digitalWrite(BL_PIN, LOW);
          digitalWrite(CL_PIN, LOW);
          digitalWrite(AL_PIN, HIGH);
          analogWrite(CH_PIN, D);
          break;

        case 0b100: //D voltage sent to motor C, B open receieves -V
          digitalWrite(AL_PIN, LOW);
          digitalWrite(CL_PIN, LOW);
          digitalWrite(BL_PIN, HIGH);
          analogWrite(CH_PIN, D);
          break;

        case 0b101: //D voltage sent to motor A, B open receieves -V
          digitalWrite(AL_PIN, LOW);
          digitalWrite(CL_PIN, LOW);
          digitalWrite(BL_PIN, HIGH);
          analogWrite(AH_PIN, D);
          break;

        default:
          while (1) ;  // should never get here
          break;
      }
    }
  }
}
