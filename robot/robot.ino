#include "BluetoothSerial.h"

const char *pin = "1234"; 

String device_name = "ROBOT_TALLER_H";

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

#if !defined(CONFIG_BT_SPP_ENABLED)
#error Serial Bluetooth not available or not enabled. It is only available for the ESP32 chip.
#endif

BluetoothSerial SerialBT;

//VARIABLES GLOBALES
int numeros[18] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
int indice_numeros = 0;
float matrizFinal[10] = { 0,0,0,0,0,0,0,0,0,0 };
String cadena = "";
//FUNCIONES AUXILIARES
void lector_numeros_serial() {
  String temp = SerialBT.readString(); // Almacena la lectura en una variable temporal

  if (temp.length() > 0) { // Verifica si la longitud de la cadena temporal es mayor que 0
    cadena += temp; // Concatena la cadena temporal con la cadena principal
  }
  indice_numeros++;
  if(indice_numeros == 2){
    separarCadena(cadena, ';', numeros, 18);
    indice_numeros++;
  }
  
}
void lector_string_RESET() {

  for(int i = 0; i < 18; i++){
    numeros[i] = 0;
  }
  for(int i = 0; i < 18; i++){
    matrizFinal[i] = 0;
  }
  indice_numeros = 0;
  cadena = "";
  
  for(int i = 0; i < 18; i++){
    Serial.println(numeros[i]); 
  }  
}

void separarCadena(String datos, char delimitador, int lista[], int longitudLista) {
  int indice = 0;
  int indiceAnterior = 0;

  for (int i = 0; i < datos.length(); i++) {
    if (datos.charAt(i) == delimitador || i == datos.length() - 1) {
      String subcadena = datos.substring(indiceAnterior, i + 1);
      subcadena.trim();  // Elimina espacios en blanco adicionales

      // Si la subcadena no está vacía, convierte a entero y almacena en la lista
      if (!subcadena.equals("")) {
        lista[indice] = subcadena.toInt();
        indice++;

        // Reinicia el índice anterior para el próximo elemento
        indiceAnterior = i + 1;
      }
    }
  }
}
void multiplicarMatrices(float matrizA[2][2], float matrizB[2][2],float resultado[2][2]) {
    
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            resultado[i][j] = 0;
            for (int k = 0; k < 2; k++) {
                 resultado[i][j] += matrizA[i][k] * matrizB[k][j];
            }
        }
    }
}
void multiplicarMatrizPorVectores(float matriz[2][2], float vectores[5][2], float resultados[], int indice, int longitud) {
    if (indice + 9 < longitud) {
        for (int i = 0; i < 5; i++) {
            for (int j = 0; j < 2; j++) {
                int suma = 0;
                for (int k = 0; k < 2; k++) {
                    suma += matriz[j][k] * vectores[i][k];
                }
                resultados[indice] = suma;
                indice++;
            }
        }
    }
}

void matriz_cambio()
{
    float baseB[2][2] = { {numeros[0],numeros[1]},{numeros[2],numeros[3]}};
    int baseN[2][2] = { {numeros[4],numeros[5]},{numeros[6],numeros[7]}};
    float matrizCambio[2][2] = {};
    float matrizInversa[2][2] = { {baseN[0][0], baseN[0][1]}, {baseN[1][0], baseN[1][1]}};
    float inversa[2][2] = { {0,0},{0,0} };
    float determinante = matrizInversa[0][0] * matrizInversa[1][1] - matrizInversa[0][1] * matrizInversa[1][0];
    float vectores[5][2] = { {numeros[8],numeros[9]},{numeros[10],numeros[11]},{numeros[12],numeros[13]},{numeros[14],numeros[15]},{numeros[16],numeros[17]} };
    // Verificar si el determinante es cero
    if (determinante != 0) {
        // Calcular la matriz adjunta
        float adjunta[2][2] = { {matrizInversa[1][1], -matrizInversa[0][1]}, {-matrizInversa[1][0], matrizInversa[0][0]} };
        for (int i = 0; i < 2; i++) {
            for (int j = 0; j < 2; j++) {
                inversa[i][j] = adjunta[i][j] / determinante;
            }
        }   
    }  
    multiplicarMatrices(baseB, inversa,  matrizCambio);
    multiplicarMatrizPorVectores( matrizCambio,  vectores, matrizFinal, 0, 10);
}


void setup() {
  analogReadResolution(10);
  Serial.begin(115200);
  SerialBT.begin(device_name);
  Serial.printf("The device with name \"%s\" is started.\nNow you can pair it with Bluetooth!\n", device_name.c_str());
  #ifdef USE_PIN
    SerialBT.setPin(pin);
    Serial.println("Using PIN");
  #endif
  cadena.reserve(200);
}

void loop() {
  if (Serial.available()) {
    SerialBT.write(Serial.read());
  }
  if (SerialBT.available()) {
    Serial.write(SerialBT.read()); 
    if(indice_numeros <= 2){
      lector_numeros_serial();
      if(indice_numeros == 3){
        matriz_cambio();
      
      for(int i = 0; i < 10; i++){
        SerialBT.print(matrizFinal[i]);
        SerialBT.println(';');
      }
      Serial.print("HOLA");
      indice_numeros++;
      }
    }
    else{
      lector_string_RESET();
    }   
  }
  delay(20);
  if(indice_numeros == 4){
      int x = analogRead(15);
      int y = analogRead(2);

      float voltajex = 3.3*x/1023;
      float voltajey = 3.3*y/1023;

      float aceleracionx = (voltajex - 1.41)/0.8;
      float aceleraciony = (voltajey + 0.28)/0.8;

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
  }
}



