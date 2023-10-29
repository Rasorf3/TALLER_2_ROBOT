
#include "BluetoothSerial.h"
#include "string.h"

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
//#define USE_PIN // Uncomment this to use PIN during pairing. The pin is specified on the line below
const char *pin = "1234"; // Change this to more secure PIN.

String device_name = "ROBOT_GRUPO_H";

float list_1[8] = {}; // Ajusta el tamaño del array según tus necesidades
int index1 = 0;
float list_2[20] = {}; // Ajusta el tamaño del array según tus necesidades
int index2 = 0;
String inputString = "";         // String para almacenar los datos entrantes
bool stringComplete = false;

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

#if !defined(CONFIG_BT_SPP_ENABLED)
#error Serial Bluetooth not available or not enabled. It is only available for the ESP32 chip.
#endif

BluetoothSerial SerialBT;
void Task1(void *pvParameters) {
  for (;;) {
    if (Serial.available()) {
    SerialBT.write(Serial.read());
    }
    if (SerialBT.available()) {
    Serial.write(SerialBT.read());
    }
    delay(20);
  
  }
}

void Task2(void *pvParameters) {
  for (;;) {
      
      if (SerialBT.available()) {
      char inChar = (char)SerialBT.read();
      if (inChar == ';') {
        stringComplete = true;
      } else {
        inputString += inChar;
      }
    }

    if (stringComplete) {
      //char *token = strtok(inputString.c_str(), ";"); // Divide la cadena usando el punto y coma como delimitador
      if(list_1[7] == -100){
        //while (token != NULL) {
        list_1[index1] = stringToInt(inputString); // Convierte cada token a un entero y almacénalo en el array
        index1++;

        //token = strtok(NULL, ";");
        }
      //}
      else if(list_2[19] == -100){
        //while (token != NULL) {
        list_2[index2] = stringToInt(inputString); // Convierte cada token a un entero y almacénalo en el array
        index2++;

        //token = strtok(NULL, ";");
        //}
      }
      else{
          for(int i = 0; i < 20; i++){
          Serial.println(list_2[i]);
        } 
          for(int i = 0; i < 8; i++){
          Serial.println(list_1[i]);
        }      
        
        
      }
      

        
      

    

      // Reinicia la cadena y marca la bandera como falso para esperar la siguiente lista
      inputString = "";
      stringComplete = false;
    }
        int x = analogRead(34);
        int y = analogRead(35);

        float voltajex = 3.3*x/1023;
        float voltajey = 3.3*y/1023;

        float aceleracionx = (voltajex - 1.41)/0.8;
        float aceleraciony = (voltajey- 1.52)/0.8;

        SerialBT.print(aceleracionx);
        SerialBT.print(";");
        SerialBT.print(aceleraciony);
        SerialBT.print(";");
        SerialBT.print(aceleracionx);
        SerialBT.print(";");
        SerialBT.print(aceleraciony);
        SerialBT.print(";");
        SerialBT.print(aceleracionx);
        SerialBT.print(";");
        SerialBT.print(aceleraciony);
        SerialBT.println(";");
        delay(10);
  }
}

void setup() {
  for(int i = 0; i < 8; i++){
    list_1[i] = -100;
  }
  for(int i = 0; i < 20; i++){
    list_2[i] = -100;
  }
  analogReadResolution(10);
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
