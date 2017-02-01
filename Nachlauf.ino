const int RELAIS = 3;
const int IGNITION = 1;
const int SHOWSTATUS = 0;

const int SHUTDOWNDELAY = 200; //Time for delay in 100ms-steps

bool bootUp = true;
int countSwitching = 0;
int countSeconds = 0;
bool stayUp = false;
bool lastStateSwitching = false;

enum enumState {
  bootup,
  normalOn,
  waitShutdown,
  stayOn
};

enumState state = bootup;

void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(RELAIS, OUTPUT);
  digitalWrite(RELAIS, LOW);
  pinMode(SHOWSTATUS, OUTPUT);
  digitalWrite(SHOWSTATUS, 0);
  pinMode(IGNITION, INPUT); // Sensor for ignition
  switchRadio(false);
  state = bootup;
}

// the loop function runs over and over again forever
void loop() {
    delay(100);                  // wait for a second
    switch (state) {
      //Bootup: Wait 2 Seconds, then switch on Radio
      case bootup:
        
        if (digitalRead(IGNITION) == HIGH && countSeconds++ >= 20) {
          countSeconds = 0;
          state = normalOn;
          switchRadio(true);
        }
        break;

      //NormalOn: While IGNITION != LOW. Stay on.
      case normalOn:
        digitalWrite(SHOWSTATUS, 1);
        countSeconds = 0;
        countSwitching = 0;
        if (digitalRead(IGNITION) == LOW) {
          state = waitShutdown;
        }
        break;

      //WaitShutdown: Wait for shutdown
      case waitShutdown:
        digitalWrite(SHOWSTATUS, 0);
        countSeconds++;
        //IGNITION = LOW is normal. Wait for SHUTDOWNDELAY, then switch off.
        if (digitalRead(IGNITION) == LOW) {
          if (lastStateSwitching == true) {
            lastStateSwitching = false;
            if ( countSwitching++ == 5 ) {
              countSeconds = 0;
              state = stayOn;
            }
          }
          if ( countSeconds >= SHUTDOWNDELAY ) {
            switchRadio(false);
            state = bootup;
            countSeconds = 0;
          }
          
        //IGNITION = HIGH? OK. Maybe the user switches between ignition and off - then count.
        // If ignition if high for more than SHUTDOWNDELAY switch back to normal operation.
        } else {
          if ( countSeconds >= SHUTDOWNDELAY ) {
              state = normalOn;
          }
          if (lastStateSwitching == false) {
            lastStateSwitching = true;
            if ( countSwitching++ == 5 ) {
              state = stayOn;
            }

          }
        }
        break;

      //StayOn = While shutting down, the user switched IGNITION several times between on and off: This means, stay on.
      case stayOn:
        digitalWrite(SHOWSTATUS, !digitalRead(SHOWSTATUS));
        if (digitalRead(IGNITION) == HIGH) {
          if ( countSeconds++ >= 100 ) {
            state = normalOn;
          }
        } else {
          countSeconds = 0;
        }
        
    }
}

void switchRadio(bool state){
  if (state == true) {
    digitalWrite(RELAIS, HIGH);
  } else {
    digitalWrite(RELAIS, LOW);
  }
}

