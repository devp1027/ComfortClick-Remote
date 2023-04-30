#include <ADCTouch.h>
#include <IRLibAll.h>
#include <CapacitiveSensor.h>

//Create a receiver object to listen on pin 2
IRrecvPCI myReceiver(2);

//Create a decoder object 
IRdecode myDecoder;  

IRsend mySender;

unsigned long startMillis;
unsigned long currentMillis;

int ref0, ref1,ref2, ref3, ref4, ref5;     //Reference values to remove offset
CapacitiveSensor Sensor = CapacitiveSensor(7,8);
CapacitiveSensor Sensor2 = CapacitiveSensor(4,5);
CapacitiveSensor Sensor3 = CapacitiveSensor(10,11);
long sensorVal;
long sensorVal2;
long sensorVal3;

int pressed =  1; //button state of program mode toggle
int heldDown = 0; //button held down state

//Custom button arrays
long customButton1[3] = {0,0,0}; //temporary custom button array
long customButton101[3] = {0,0,0}; //custom button array
long customButton202[3] = {0,0,0};
long customButton303[3] = {0,0,0};

long prevVal = 0; //previous IR value when custom button mode is on
int counter = 0; //counter variable when custom button mode is on

void setup() {
  Serial.begin(9600);
  pinMode(12, OUTPUT);
  delay(2000); while (!Serial); //delay for Leonardo
  myReceiver.enableIRIn(); // Start the receiver
  Serial.println(F("Ready to receive IR signals"));

  ref0 = ADCTouch.read(A0, 500);  //Sets reference balues to analog pins A0 to A5
  ref1 = ADCTouch.read(A1, 500);    
  ref2 = ADCTouch.read(A2, 500);
  ref3 = ADCTouch.read(A3, 500);
  ref4 = ADCTouch.read(A4, 500);
  ref5 = ADCTouch.read(A5, 500);

  startMillis = millis();  //initial start time
}

void loop() {
  currentMillis = millis();
 
  sensorVal = Sensor.capacitiveSensor(30);
  sensorVal2 = Sensor2.capacitiveSensor(30);
  sensorVal3 = Sensor3.capacitiveSensor(30);
  
  int value0 = ADCTouch.read(A0); //sets value0 to value6 into capactitive touch reading
  int value1 = ADCTouch.read(A1);  
  int value2 = ADCTouch.read(A2);
  int value3 = ADCTouch.read(A3);
  int value4 = ADCTouch.read(A4);
  int value5 = ADCTouch.read(A5);


  int ledState = 1; //LED indictor state
  
  value0 -= ref0;       //remove offset
  value1 -= ref1;
  value2 -= ref2;
  value3 -= ref3;
  value4 -= ref4;
  value5 -= ref5;

//----------------------------------------------------Capacitive Button Functions----------------------------------//

  if (value0 >= 100) //Custom Button 101
  {
    if (heldDown == 0)
    {
      if (customButton101[0] != 0)
      {
        digitalWrite(12, HIGH); //LED ON
        Serial.println("Custom Button 101");
        for (int i =0; i<3; i++){
          digitalWrite(12, HIGH);
          mySender.send(SONY, customButton101[i], 12);
          Serial.println(customButton101[i]);
          delay(200);
          digitalWrite(12, LOW); //LED OFF
          delay(200);     
        }
      }
      else
      {
        Serial.println("Button is not mapped yet!");
      }

    }
    heldDown = 1; 
  }

  else if (value1 >= 100) //Custom Button 202
  {

    if (heldDown == 0)
    {
       if (customButton202[0] != 0)
      {
        Serial.println("Custom Button 202");
        for (int i =0; i<3; i++)
        {
          digitalWrite(12, HIGH);
          mySender.send(SONY, customButton202[i], 12);
          Serial.println(customButton202[i]);
          delay(200);  
          digitalWrite(12, LOW); 
          delay(200);  
        }
      }
      else
      {
        Serial.println("Button is not mapped yet!");
      }
    }
    heldDown = 1;
  }

  else if (value2 >= 100) //VOLUME DOWN
  {
    if (heldDown == 0)
    {
      digitalWrite(12, HIGH);
      mySender.send(SONY, 3216, 12);
      Serial.println("VOLUME DOWN");
    }
    heldDown = 1;
  }

  else if (value3 >= 100) //CHANNEL DOWN
  {


    if (heldDown == 0)
    {
      digitalWrite(12, HIGH);
      Serial.println("CHANNEL DOWN");
      mySender.send(SONY, 2192, 12);
    }
    heldDown = 1;
  }

  else if (value4 >= 100) //CHANNEL UP
  {
    if (heldDown == 0)
    {
      digitalWrite(12, HIGH);
      
      Serial.println("CHANNEL UP");
      mySender.send(SONY, 144, 12);
    }
    heldDown = 1;
  }

  else if (value5 >= 100) //POWER
  {
    if (heldDown == 0)
    {
      digitalWrite(12, HIGH);
      mySender.send(SONY, 2704, 12);
      Serial.println("POWER");      
    }

    heldDown = 1;  
  }

  else if (sensorVal2 >= 200) //Toggle reciever mode on/off
  {

    if (heldDown == 0)
    {
      digitalWrite(12, HIGH);
      pressed = pressed * -1;
      Serial.println("Receiver Mode ON/ OFF");
      Serial.println(pressed);
      memset(customButton1, 0, sizeof(customButton1));
      counter = 0;
      delay(150);
    }
    heldDown = 1;
  }
  
  else if (sensorVal3 >= 100) //Custom Button 303
  {
    if (heldDown == 0)
    {
      if (customButton303[0] != 0)
      {
        Serial.println("Custom Button 303");
        for (int i =0; i<3; i++)
        {
          digitalWrite(12, HIGH);
          mySender.send(SONY, customButton303[i], 12);
          Serial.println(customButton303[i]);
          delay(200);    
          digitalWrite(12, LOW); 
          delay(200);
        }
      }
      else
      {
        Serial.println("Button is not mapped yet!");
      }
    }
    heldDown = 1;
  }

  else if (sensorVal >= 150) //VOLUME UP
  {


    if (heldDown == 0)
    {
      digitalWrite(12, HIGH);
      mySender.send(SONY, 1168, 12);
      Serial.println("VOLUME UP");
      
    }
    heldDown = 1;
  }
  
  else{ //No Button Pressed
    heldDown = 0;
    digitalWrite(12, LOW);
  }

//------------------------------------------------------Custom Button Function---------------------------------------------------------------//

  
  if (pressed == -1) //If custom button mode is on
  {
    digitalWrite(12, HIGH);
    if (myReceiver.getResults()) {
    myDecoder.decode();           //Decode it
    myReceiver.enableIRIn();      //Restart receiver
    }
    if (myDecoder.value != 4294967295 && myDecoder.value != 0) //Ignores useless data 
    {
      if (counter < 3)
      {
        if (currentMillis - startMillis >= 400) //Same button can be mapped twice after 400ms elapsed, preventing acidental duplicate inputs
        {
          prevVal = 0;
          startMillis = currentMillis;
        }
        if (prevVal != myDecoder.value)
        {
          startMillis = currentMillis;
          customButton1[counter] = myDecoder.value;
          Serial.println(customButton1[counter]);
          counter += 1;
          prevVal = myDecoder.value;
          myDecoder.value = 0;
          Serial.println(prevVal);
          delay(100);
          digitalWrite(12, LOW);
          delay(100);
        }
        if (value5 >= 100){ //Map IR values onto one of three custom buttons
       
          for (int i = 0; i <3 ; i++)
          {
            customButton101[i] = customButton1[i];
          }
          pressed = pressed * -1;
          counter = 0;
          delay(500);
        }
        else if (sensorVal2 >= 100){
         
          for (int i = 0; i <3 ; i++)
          {
            customButton202[i] = customButton1[i];
          }
          pressed = pressed * -1;
          counter = 0;
          delay(500);
        }
        else if (sensorVal3 >= 100){

          for (int i = 0; i <3 ; i++)
          {
            customButton303[i] = customButton1[i];
          }
          pressed = pressed * -1;
          counter = 0;
          delay(500);    
        }
      }


    }
      if (counter > 2)
      {
        ledState *= -1;
        if (ledState == 1)
        {
          digitalWrite(12, HIGH);
        }
        else
        {
          digitalWrite(12, LOW);
        }
        delay(25);
        Serial.println("Press Button to Map");
        Serial.print(customButton1[0]);
        Serial.print("  ");
        Serial.print(customButton1[1]);
        Serial.print("  ");
        Serial.println(customButton1[2]);
        Serial.println("----------------------------------");
        
        if (value0 >= 100){  //Once max number of inputs are reached, constantly checks until one of three custom buttons are mapped
          for (int i = 0; i <3 ; i++)
          {
            customButton101[i] = customButton1[i];
          }
          pressed = pressed * -1;
          counter = 0;
          delay(500);
        }
        else if (value1 >= 100){
          for (int i = 0; i <3 ; i++)
          {
            customButton202[i] = customButton1[i];
          }
          pressed = pressed * -1;
          counter = 0;
          delay(500);
        }
        else if (sensorVal3 >= 100){
          for (int i = 0; i <3 ; i++)
          {
            customButton303[i] = customButton1[i];
          }
          pressed = pressed * -1;
          digitalWrite(12, LOW);
          counter = 0;
          delay(500);
        }
      }     
      
  }
}
