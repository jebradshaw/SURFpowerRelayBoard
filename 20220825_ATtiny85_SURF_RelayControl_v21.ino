// ATtiny85-20PU Internal PLL clock, 
// Relay Power Control Board for Surface Vessel v1.0

// J. Bradshaw 20220211
//                           ATtiny85 Pin 1 -> RESET
//  Arduino Pin 3 (D3)   ->  ATtiny85 Pin 2 -> RST_inputPin
//  Arduino Pin 4 (AN2)  ->  ATtiny85 Pin 3 -> SW1_EN_inputPin
//                           ATtiny85 Pin 4 -> GND
//  Arduino Pin 0 (PWM)  ->  ATtiny85 Pin 5 -> EN1_inputPin
//  Arduino Pin 1 (PWM)  ->  ATtiny85 Pin 6 -> LEDPin
//  Arduino Pin 2 (AN1)  ->  ATtiny85 Pin 7 -> RLY_outputPin
//                           ATtiny85 Pin 8 -> VCC

//Pin declarations
int RST_inputPin = 3;    // latch reset input signal monitor (must go high then low to reset)
int EN1_inputPin = 0;    // latch enable input signal monitor (must go high after RST pulse)

int LEDPin = 1;  // PWM pin for status indicator

int SW1_EN_inputPin = 4;    // reset input from manual pushbutton switch
int RLY_outputPin = 2;  // MOSFET drive output to kill power to VESC's

int rlyState = 0;      // Initialize as relay off (rlyState = 0)

void setup()
{ 
  delay(100);
  pinMode(RST_inputPin, INPUT);       // Make RST_inputPin an input
  pinMode(EN1_inputPin, INPUT);       // Make EN1_inputPin an input
  pinMode(SW1_EN_inputPin, INPUT);    // Make SW1_EN_inputPin an input

  digitalWrite(RLY_outputPin, LOW); // Start with the MOSFET drive signal LOW (disables the VESC's)
  pinMode(RLY_outputPin, OUTPUT);   // Make the MOSFET drive signal an output  
  
  digitalWrite(LEDPin, LOW);  //make status LED on (active low, current sink)
  pinMode(LEDPin, OUTPUT);  //Make timer1's PWM pin an output    
}

void loop()
{
  static int led_delay = 100;   

    switch(rlyState){
      case 0:
        digitalWrite(RLY_outputPin, LOW);// deactivate the relay for the VESC power
        if((digitalRead(RST_inputPin) == HIGH) && (digitalRead(EN1_inputPin) == LOW)){
          delay(20);
          led_delay = 100;
          rlyState = 1;
        }
        if(digitalRead(SW1_EN_inputPin) == LOW){
          delay(20);
          led_delay = 75;
          rlyState = 2;
        }
        break;
      case 1:
        if((digitalRead(RST_inputPin) == HIGH) && (digitalRead(EN1_inputPin) == HIGH)){
          delay(20);
          led_delay = 75;
          rlyState = 4;          
        }
        if(digitalRead(SW1_EN_inputPin) == LOW){
          delay(20);
          led_delay = 30;
          rlyState = 0;
        }        
        break;

      case 2:
        if(digitalRead(SW1_EN_inputPin) == HIGH){
          delay(20);
          digitalWrite(RLY_outputPin, HIGH);// activate the relay for the VESC power           
          led_delay = 50;
          rlyState = 3;
        }
        if(digitalRead(EN1_inputPin) == HIGH){
          digitalWrite(RLY_outputPin, LOW);// deactivate the relay for the VESC power         
          rlyState = 0;          
        }
        if(digitalRead(RST_inputPin) == HIGH){
          digitalWrite(RLY_outputPin, LOW);// deactivate the relay for the VESC power
          led_delay = 20;
          rlyState = 0;          
        }                
        break;        
      case 3:
        if(digitalRead(SW1_EN_inputPin) == LOW){
          digitalWrite(RLY_outputPin, LOW);// deactivate the relay for the VESC power
          led_delay = 20;
          rlyState = 6;          
        }
        if(digitalRead(RST_inputPin) == HIGH){
          digitalWrite(RLY_outputPin, LOW);// deactivate the relay for the VESC power
          led_delay = 20;
          rlyState = 0;          
        }
        if(digitalRead(EN1_inputPin) == HIGH){
          digitalWrite(RLY_outputPin, LOW);// deactivate the relay for the VESC power
          led_delay = 100;
          rlyState = 0;          
        }
        break;

      case 4:
        if((digitalRead(RST_inputPin) == LOW) && (digitalRead(EN1_inputPin) == HIGH)){
          delay(100);
          led_delay = 50;
          rlyState = 5;          // Go to state machine part that turns on the relay
          digitalWrite(RLY_outputPin, HIGH);// activate the relay for the VESC power
        }
        if(digitalRead(SW1_EN_inputPin) == LOW){
          digitalWrite(RLY_outputPin, LOW);// deactivate the relay for the VESC power
          led_delay = 100;
          rlyState = 6;          
        }        
        break;
      case 5:
        //Relay is currently on in this state
        if((digitalRead(RST_inputPin) != LOW) || (digitalRead(EN1_inputPin) != HIGH)){
          digitalWrite(RLY_outputPin, LOW);// deactivate the relay for the VESC power
          led_delay = 100;
          rlyState = 0;          // Go to state machine part that turns on the relay          
        }
        if(digitalRead(SW1_EN_inputPin) == LOW){
          digitalWrite(RLY_outputPin, LOW);// deactivate the relay for the VESC power
          led_delay = 100;
          rlyState = 6;          // Go to state machine part that turns on the relay
          delay(100);
        }
        break;
      case 6:
          if(digitalRead(SW1_EN_inputPin) == HIGH){
             delay(100);
            led_delay = 100;
            rlyState = 0;          // Go to state machine part that turns on the relay
          }          
        break;
      default:
        digitalWrite(RLY_outputPin, LOW);   // default is to disengage the relay
        break;
    }

    // toggle LED to indicate the current state
    digitalWrite(LEDPin, HIGH);
    delay(led_delay);

    // If the relay is currently on (software or hardware switch caused activation), leave the LED ON
    if(rlyState != 5 && rlyState != 3){
      digitalWrite(LEDPin, LOW);
      delay(led_delay);
    }
}
