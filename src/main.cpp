#include <Arduino.h>
#include <Servo.h>
#include <EasyButtonVirtual.h>
Servo myservo; // create servo object to control a servo
#define SERVO_PIN 9
#define MOSFET_PIN 8
#define ANALOG_PIN A0
#define LIGHT_TRESHOLD 650
// twelve servo objects can be created on most boards
#define runEvery(t) for (static uint16_t _lasttime;                         \
                         (uint16_t)((uint16_t)millis() - _lasttime) >= (t); \
                         _lasttime += (t))

uint16_t light_value;
bool light_triggered = true;
// Instance of the button.
EasyButtonVirtual vButton(light_triggered);

int pos = 0; // variable to store the servo position
uint8_t release_state = 0;
long release_start_time = 0;


// Callback function to be called when the button is pressed.
void long_press_handler()
{
   Serial.println("long press");
   release_state = 1;
}

void setup()
{
   Serial.begin(9600);
   myservo.attach(SERVO_PIN); // attaches the servo on pin 9 to the servo object

   pinMode(MOSFET_PIN, OUTPUT);

   digitalWrite(MOSFET_PIN, LOW);

   // Initialize the button.
   vButton.begin();

   // so ling time used because mavic turns on leds on start for 6 seconds
   vButton.onPressedFor(9000, long_press_handler);
   // vButton.onSequence(2, 3000, double_click_handler);

   myservo.write(0); // tell servo to go to position in variable 'pos'
}

void loop()
{
   // Continuously read the status of the button.
   vButton.read();

   runEvery(20)
   {
      light_value = analogRead(ANALOG_PIN);
      bool was_trigered = light_triggered;
      light_triggered = !(light_value >= LIGHT_TRESHOLD);
      // if(!light_triggered){
      //    Serial.print("light value ");
      //    Serial.println(light_value);
      // }
   }

   if (release_state == 1)
   {
      digitalWrite(MOSFET_PIN, HIGH);
      delay(50);
      myservo.write(40); // tell servo to go to position in variable 'pos'
      release_state = 2;
      release_start_time = millis();
      Serial.println("release start");
   }

   if (release_state == 2 && millis() - release_start_time > 2000)
   {
       digitalWrite(MOSFET_PIN, LOW);
       
      myservo.write(0); // tell servo to go to position in variable 'pos'
      release_state = 0;

      Serial.println("release end");
   }
}
