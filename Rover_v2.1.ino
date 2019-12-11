 // ----- Libraries -----
  
    // LEDs
       #include <Adafruit_NeoPixel.h>
    // Transciever
      // https://github.com/bakercp/PacketSerial
       #include <PacketSerial.h>
  
 // ----- Define pins -----
  
    // Water Sensor 
      #define WATER_SENSOR 4 // Digital Pin 4
    // LEDs
      #define LED_PIN 3      // Digital Pin 3
      #define LED_COUNT 16
        Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);     // Declare our NeoPixel strip object:
    // Light Sensor
      #define LIGHT_SENSOR 0  // Analog Pin 0
    // Temperature Sensor 
      #define TEMP_SENSOR 2   // Digital Pin 2
       const float TEMP_CONVERSION = 0.48828125;
    // Motors
      #define LEFT_MOTOR 9    // Digital Pin 10
      #define RIGHT_MOTOR 10   // Digital Pin 10

 // ----- Packet Serial Commmunication -----
  
    // Packert Serial communication object
      PacketSerial packetSerialObj;
    // Refresh time in milliseconds
      const int refreshMs = 20;
      
 // ----- Format/Send Data ----- 
    
      struct sendData
      {
        int inWater;
        int lightOnOff;
        float temp;
      } roverData;
    
        // roverData.inWater
        
 // ----- Format/Recieve Data -----
  
    // Joystick value array
     uint8_t joyValues[4];
  
  void decodePacket(const uint8_t * dataInBuffer, size_t size)
  {
    for(int i = 0; i < size; i++)
    {
      joyValues[i] = dataInBuffer[i];
    }
  }

 // ----- Motor functions ----- //
 void drive(int y, int r)
 {
   analogWrite(LEFT_MOTOR, y + r);
   analogWrite(RIGHT_MOTOR, -y + r);
 }

 // ----- Initialization ----- //
  
  void setup() 
  {
    // Begin serial stream
      Serial.begin(9600);
      packetSerialObj.begin(9600);
  
    // LEDs  
      strip.begin();  
        for(int n = 0; n < 30; n++)
            {
              strip.setPixelColor(n, 0, 0, 5);
              strip.show();
            }
    // Motors
      // Set Mode to Output
        pinMode(LEFT_MOTOR, OUTPUT);
        pinMode(RIGHT_MOTOR, OUTPUT);

      // Start up with motors off
        digitalWrite(LEFT_MOTOR, HIGH);
        digitalWrite(RIGHT_MOTOR, HIGH);
  }
  
  // ----- Continuous Loop -----
  
  void loop() 
  {
    // ----- DATA IN -----
    
      // Water Sensor data
        roverData.inWater = digitalRead( WATER_SENSOR );
    
      // Light Sensor data
        roverData.lightOnOff = analogRead( LIGHT_SENSOR );
    
      // Temperature Sensor data
        roverData.temp = analogRead( TEMP_SENSOR ) * TEMP_CONVERSION;
    
    // ----- CONDITIONAL STATEMENTS -----
        
      // Water Sensor 
        if( roverData.inWater > 0 )
        {
          roverData.inWater = 1;  // 1 as in water and 0 as not 
        }
        else
        {
          roverData.inWater = 0;
        }
         
    
      // Light Sensor
         if( roverData.lightOnOff < 10 /*(low light sensor value)*/ )
         {
            for(int n = 0; n < 17; n++)
            {
              strip.setPixelColor(n, 255, 255, 255);
              strip.show();
    
              roverData.lightOnOff = 1;
            }
         }
         else
         {
            roverData.lightOnOff = 0;
         }

      // ----- Format/Send Data -----
   
       uint8_t dataOutBuffer[1 + 1 + 4] = {roverData.lightOnOff, roverData.inWater, roverData.temp};   // Array of values beinng sent to the controller
    
       packetSerialObj.send(dataOutBuffer, sizeof(dataOutBuffer));       
       packetSerialObj.update();      // Call update to receive, decode and process incoming packets.
    
       delay(refreshMs);
    
    
     // ----- JOYSTICKS TO MOTORS -----

       // Set local variables from array
         int leftJoyX = joyValues[1];
         int leftJoyY = joyValues[2];
         int rightJoyX = joyValues[3];
         int rightJoyY = joyValues[4];   


      // Forward or Backward
      drive(leftJoyX, leftJoyY);

      // Map joystick values
        // Variable = map(value, fromLow, fromHigh, toLow, toHigh)
       
  }
