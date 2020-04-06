# TVAudioReg
regolatore automatico del voume della TV

## Feature
The aim of the project is to create a device capable of automatically managing the volume of the TV in a domestic environment.

### Components & wired
1. 0,96" OLED Display SPI 128x64 7 Pin (GND,VCC,D0,D1,RES,DC,CS)    
   - D7 - DC                    
   - D6 - CS
   - D5 - D1
   - D4 - D0
   - 5V - RES
   - GND - GND 
   - 5V - VCC       
2. Infrared Diode LED IR Emitter
   - PWD D3 - SIGNAL
   - 5V - VCC
   - GND - GND      
3. BUTTON
   - D2 - SIGNAL - 10Khom - GND      
   - 5V - 5V     
4. MAX9814 HiFi microphone AGC amplifier (AR,OUT,GAIN,VCC,GND)
   - A1 - OUT       
   - 5V - VCC      
   - GND - GND 
