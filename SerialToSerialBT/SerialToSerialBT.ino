#include "BluetoothSerial.h"

const char *pin = "1234";

String device_name = "ROBOT_GRUPO_H";

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

#if !defined(CONFIG_BT_SPP_ENABLED)
#error Serial Bluetooth not available or not enabled. It is only available for the ESP32 chip.
#endif

BluetoothSerial SerialBT;

int stringToInt(String str) {
  int result = 0;
  int sign = 1;
  int i = 0;
  if (str.charAt(0) == '-') {
    sign = -1;
    i = 1;
  }
  for (; i < str.length(); i++) {
    result = result * 10 + (str.charAt(i) - '0');
  }
  return sign * result;
}

bool imprimircoordenada = true;
float bases[8] = {}; // Ajusta el tamaño del array según tus necesidades
int base = 0;
float coordenadas[10] = {}; // Ajusta el tamaño del array según tus necesidades
int coordenada = 0;

String inputString = "";         // String para almacenar los datos entrantes
bool stringComplete = false;

void Task1(void *pvParameters) {
  for (;;) {
    
    if (Serial.available()) {
    SerialBT.write(Serial.read());
    }
    if (SerialBT.available()) {
    Serial.write(SerialBT.read());
    }
    
    if (SerialBT.available()) {
      char inChar = (char)SerialBT.read();
      if (inChar == ';') {
        stringComplete = true;
      } 
      else {
        inputString += inChar;
      }
      if (stringComplete) {
        if(base < 8){
          bases[base] = stringToInt(inputString);
         base++;
        }
      
        else if(coordenada < 10){
          coordenadas[coordenada] = stringToInt(inputString); 
          coordenada++; 
        }
        else if(coordenada == 10 && base == 8 && imprimircoordenada == true){
          for(int i = 0; i < 10; i++ ){
            SerialBT.print(coordenadas[i]);
            SerialBT.println(";");
        }
        
          imprimircoordenada = false;
        }      
    }
    inputString = "";
    stringComplete = false;
    }

    
    delay(20);
  }
}

void Task2(void *pvParameters) {
  
  for (;;) {
    if(coordenada == 10 && base == 8 && imprimircoordenada == false){
      int x = analogRead(15);
      int y = analogRead(2);

      float voltajex = 3.3*x/1023;
      float voltajey = 3.3*y/1023;

      float aceleracionx = (voltajex - 1.41)/0.8;
      float aceleraciony = (voltajey- 1.52)/0.8;

      float velocidadx = 0;
      float velocidady = 0;

      float posicionx = 0;
      float posiciony = 0;

      SerialBT.print(aceleracionx);
      SerialBT.print(";");
      SerialBT.print(aceleraciony);
      SerialBT.print(";");
      SerialBT.print(velocidadx);
      SerialBT.print(";");
      SerialBT.print(velocidady);
      SerialBT.print(";");
      SerialBT.print(posicionx);
      SerialBT.print(";");
      SerialBT.print(posiciony);
      SerialBT.println(";");
      delay(20);
      
    }  
     
    
  } 
}


void setup() {
  analogReadResolution(10);
  for(int i = 0; i < 8; i++){
    bases[i] = 0;
  }
  for(int i = 0; i < 10; i++){
    coordenadas[i] = 0;
  }
  Serial.begin(115200);
  SerialBT.begin(device_name); //Bluetooth device name
  Serial.printf("The device with name \"%s\" is started.\nNow you can pair it with Bluetooth!\n", device_name.c_str());
  //Serial.printf("The device with name \"%s\" and MAC address %s is started.\nNow you can pair it with Bluetooth!\n", device_name.c_str(), SerialBT.getMacString()); // Use this after the MAC method is implemented
  #ifdef USE_PIN
    SerialBT.setPin(pin);
    Serial.println("Using PIN");
  #endif
  xTaskCreatePinnedToCore(
    Task1,  
   "Task1",  
    10000,  
    NULL,  
    1,  
    NULL,  
    0);  

  xTaskCreatePinnedToCore(
    Task2,  
    "Task2", 
    10000,  
    NULL,  
    1,  
    NULL,  
    1);  
    
}

void loop() { 
        
}
