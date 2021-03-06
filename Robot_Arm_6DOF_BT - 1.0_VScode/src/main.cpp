#include <Arduino.h> //libraria do PlatIO
#include <Wire.h> //comunicação I2C
#include "PCA9685.h" //placa de ligação dos servos
#include <SoftwareSerial.h> //comunicação bluetooth


PCA9685 driver; //elemento servos

// PCA9685 outputs = 12-bit = 4096 steps
// 2.5% of 20ms = 0.5ms ; 12.5% of 20ms = 2.5ms
// 2.5% of 4096 = 102 steps; 12.5% of 4096 = 512 steps
// (-90deg, +90deg)
//definição dos servos
PCA9685_ServoEvaluator Servo01(102, 470); //cintura
PCA9685_ServoEvaluator Servo02(102,470); //ombro
PCA9685_ServoEvaluator Servo03(102,470); //cotovelo
PCA9685_ServoEvaluator Servo04(104,512); //pulso rotação
PCA9685_ServoEvaluator Servo05(104,470); //pulso subida
PCA9685_ServoEvaluator Servo06(102,470); //gripper

//portas para comunicação BT
SoftwareSerial Bluetooth(3, 4); // Arduino(RX, TX) - HC-05 Bluetooth (TX, RX)

//LED de interação

int ledRed = 10;
int ledGreen = 11;
int ledBlue = 12;

//posições dos seros
int servo1Pos, servo2Pos, servo3Pos, servo4Pos, servo5Pos, servo6Pos;
int servo1PPos, servo2PPos, servo3PPos, servo4PPos, servo5PPos, servo6PPos;
//guardar as posições até 50 movimentos
int servo01SP[50], servo02SP[50], servo03SP[50], servo04SP[50], servo05SP[50], servo06SP[50];
//contagem de posições gravadas
int index = 0;
//velocidade de movimentos
int speedDelay = 10;
//string de dados recebidos
String dataIn = "";

//Limites de curso máximo
int servo01_max = 90;
int servo02_max = 90;
int servo03_max = 50;
int servo04_max = 90;
int servo05_max = 85;
int servo06_max = 90;

//Limites de curso mínimo
int servo01_min = -90;
int servo02_min = -60;
int servo03_min = -90;
int servo04_min = -90;
int servo05_min = -90;
int servo06_min = -10;

//declaração de funçoes
void runSteps();

void setup() {
  //primeiro elemento a ser iniciado
  Wire.begin();

  //baud rates de 100, 400, 1000 kHz
  Wire.setClock(400000);

  //reeniciar PCA9685
  driver.resetDevices();

  //inicializar comunicação
  driver.init(B000000);

  //escolher a frequência para os servos, em geral 50Hz
  driver.setPWMFrequency(50);

  //definir pinos para LED's de estado como saídas
  pinMode(ledBlue, OUTPUT);
  pinMode(ledGreen, OUTPUT);
  pinMode(ledRed, OUTPUT);

  //iniciar BT e definir velocidades de cominicação
  Bluetooth.begin(9600); // Default baud rate of the Bluetooth module
  Bluetooth.setTimeout(5);
  delay(20);
  Serial.begin(9600);

  //equipamento ligado: LED verde pisca 3 vezes, LED azul acende durante um segundo
  digitalWrite(ledGreen, HIGH);
  delay(200);
  digitalWrite(ledGreen, LOW);
  delay(200);
  digitalWrite(ledGreen, HIGH);
  delay(200);
  digitalWrite(ledGreen, LOW);
  delay(200);
  digitalWrite(ledGreen, HIGH);
  delay(200);
  digitalWrite(ledGreen, LOW);
  delay(200);

  digitalWrite(ledBlue,HIGH);
  delay(1000);
  digitalWrite(ledBlue,LOW);


  //Posiçoes iniciais dos servos de 90 a -90 para esta biblioteca
  servo1PPos = 0;
  driver.setChannelPWM(0,Servo01.pwmForAngle(servo1PPos));

  servo2PPos = 85;
  driver.setChannelPWM(1,Servo02.pwmForAngle(servo2PPos));

  servo3PPos = 0;
  driver.setChannelPWM(2,Servo03.pwmForAngle(servo3PPos));

  servo4PPos = 0;
  driver.setChannelPWM(3, Servo04.pwmForAngle(servo4PPos));

  servo5PPos = -60;
  driver.setChannelPWM(4,Servo05.pwmForAngle(servo5PPos));

  servo6PPos = 90;
  driver.setChannelPWM(5,Servo06.pwmForAngle(servo6PPos));
}

//rotina de movimentos gravados
void runSteps() {
  
  //Correr as posições gravadas até o butão RESET ou PAUSE ser pressionado
  while (dataIn != "50") {
    digitalWrite(ledRed, LOW);
    digitalWrite(ledBlue, LOW);
    digitalWrite(ledGreen, HIGH);

    Serial.print(dataIn);
    
    for (int i = 0; i <= index - 2; i++) {  //correr todos as posições guardadas em index
      if (Bluetooth.available() > 0) {      //cerificar se existe dados recebidos
        dataIn = Bluetooth.read();
        Serial.print(dataIn);
        if (dataIn == "52") {              //se botao PAUSE carregado
          digitalWrite(ledGreen, LOW);
          digitalWrite(ledRed, HIGH);
          while (dataIn != "51") {          //aguardar por botao RUN
            if (Bluetooth.available() > 0) { 
              dataIn = Bluetooth.read();
              Serial.print(dataIn);
              digitalWrite(ledGreen, HIGH);
          digitalWrite(ledRed, LOW);
              if (dataIn == "50") {
                break;
              }
            }
          }
        }

        //alterar a velocidade de execução de movimentos
        int verifyspeedchange = dataIn.toInt();
        //Serial.println(verifyspeedchange);
        if (verifyspeedchange > 52){
          speedDelay = (-10*(verifyspeedchange))+580; //alterar o tempo de execução dos movimentos
        //Serial.println(speedDelay);
        }
      }
     
      // Servo 1
      if (servo01SP[i] == servo01SP[i + 1]) {
      }
      if (servo01SP[i] > servo01SP[i + 1]) {
        for (int j = servo01SP[i]; j >= servo01SP[i + 1]; j--) {
          driver.setChannelPWM(0,Servo01.pwmForAngle(j));
          delay(speedDelay);
        }
      }
      if (servo01SP[i] < servo01SP[i + 1]) {
        for (int j = servo01SP[i]; j <= servo01SP[i + 1]; j++) {
          driver.setChannelPWM(0,Servo01.pwmForAngle(j));
          delay(speedDelay);
        }
      }

      // Servo 2
      if (servo02SP[i] == servo02SP[i + 1]) {
      }
      if (servo02SP[i] > servo02SP[i + 1]) {
        for (int j = servo02SP[i]; j >= servo02SP[i + 1]; j--) {
          driver.setChannelPWM(1,Servo02.pwmForAngle(j));
          delay(speedDelay);
        }
      }
      if (servo02SP[i] < servo02SP[i + 1]) {
        for (int j = servo02SP[i]; j <= servo02SP[i + 1]; j++) {
          driver.setChannelPWM(1,Servo02.pwmForAngle(j));
          delay(speedDelay);
        }
      }

      // Servo 3
      if (servo03SP[i] == servo03SP[i + 1]) {
      }
      if (servo03SP[i] > servo03SP[i + 1]) {
        for (int j = servo03SP[i]; j >= servo03SP[i + 1]; j--) {
          driver.setChannelPWM(2,Servo03.pwmForAngle(j));
          delay(speedDelay);
        }
      }
      if (servo03SP[i] < servo03SP[i + 1]) {
        for (int j = servo03SP[i]; j <= servo03SP[i + 1]; j++) {
          driver.setChannelPWM(2,Servo03.pwmForAngle(j));
          delay(speedDelay);
        }
      }

      // Servo 4
      if (servo04SP[i] == servo04SP[i + 1]) {
      }
      if (servo04SP[i] > servo04SP[i + 1]) {
        for (int j = servo04SP[i]; j >= servo04SP[i + 1]; j--) {
          driver.setChannelPWM(3, Servo04.pwmForAngle(j));
          delay(speedDelay);
        }
      }
      if (servo04SP[i] < servo04SP[i + 1]) {
        for (int j = servo04SP[i]; j <= servo04SP[i + 1]; j++) {
          driver.setChannelPWM(3, Servo04.pwmForAngle(j));
          delay(speedDelay);
        }
      }

      // Servo 5
      if (servo05SP[i] == servo05SP[i + 1]) {
      }
      if (servo05SP[i] > servo05SP[i + 1]) {
        for (int j = servo05SP[i]; j >= servo05SP[i + 1]; j--) {
          driver.setChannelPWM(4,Servo05.pwmForAngle(j));
          delay(speedDelay);
        }
      }
      if (servo05SP[i] < servo05SP[i + 1]) {
        for (int j = servo05SP[i]; j <= servo05SP[i + 1]; j++) {
          driver.setChannelPWM(4,Servo05.pwmForAngle(j));
          delay(speedDelay);
        }
      }

      // Servo 6
      if (servo06SP[i] == servo06SP[i + 1]) {
      }
      if (servo06SP[i] > servo06SP[i + 1]) {
        for (int j = servo06SP[i]; j >= servo06SP[i + 1]; j--) {
          driver.setChannelPWM(5,Servo06.pwmForAngle(j));
          delay(speedDelay);
        }
      }
      if (servo06SP[i] < servo06SP[i + 1]) {
        for (int j = servo06SP[i]; j <= servo06SP[i + 1]; j++) {
          driver.setChannelPWM(5,Servo06.pwmForAngle(j));
          delay(speedDelay);
        }
      }
    }
  }
}

void loop() {
  
  //verificar a chegada de dados pelo bluetooth
  if (Bluetooth.available() > 0) {
    dataIn = Bluetooth.readString();  // Read the data
    Serial.print(dataIn);

        //print positions of servos
        Serial.println(servo1PPos);
        Serial.println(servo2PPos);
        Serial.println(servo3PPos);
        Serial.println(servo4PPos);
        Serial.println(servo5PPos);
        Serial.println(servo6PPos);

    //------------------------------------Servo citura rotação esquerda
      while(dataIn == "24") {
      if (Bluetooth.available() > 0) {
      dataIn = Bluetooth.readString();
      }
      
        if (servo1PPos > servo01_min) {
          driver.setChannelPWM(0,Servo01.pwmForAngle(servo1PPos));
          servo1PPos--;
          delay(speedDelay);
          digitalWrite(ledBlue, HIGH);
          digitalWrite(ledGreen, LOW);
        }else{
          digitalWrite(ledBlue, LOW);
          digitalWrite(ledGreen, HIGH);
        }
      }

    //----------------------------------------Servo citura rotação direita
      while(dataIn == "25") {
      if (Bluetooth.available() > 0) {
      dataIn = Bluetooth.readString();
      }
      
         if (servo1PPos < servo01_max) {
          driver.setChannelPWM(0,Servo01.pwmForAngle(servo1PPos));
          servo1PPos++;
          delay(speedDelay);
          digitalWrite(ledBlue, HIGH);
          digitalWrite(ledGreen, LOW);
         }else{
          digitalWrite(ledBlue, LOW);
          digitalWrite(ledGreen, HIGH);
         }
      }

    //-------------------------------------------Servo ombro frente
      while(dataIn == "22") {
      if (Bluetooth.available() > 0) {
      dataIn = Bluetooth.readString();
      }
      
        if (servo2PPos > servo02_min) {
          driver.setChannelPWM(1,Servo02.pwmForAngle(servo2PPos));
          servo2PPos--;
          delay(speedDelay);
          digitalWrite(ledBlue, HIGH);
          digitalWrite(ledGreen, LOW);
        }else{
          digitalWrite(ledBlue, LOW);
          digitalWrite(ledGreen, HIGH);
        }
      }

    //-------------------------------------------------Servo ombro trás
      while(dataIn == "23") {
      if (Bluetooth.available() > 0) {
      dataIn = Bluetooth.readString();
      }
     
        if (servo2PPos < servo02_max) {
          driver.setChannelPWM(1,Servo02.pwmForAngle(servo2PPos));
          servo2PPos++;
          delay(speedDelay); 
          digitalWrite(ledBlue, HIGH);
          digitalWrite(ledGreen, LOW);
        }else{
          digitalWrite(ledBlue, LOW);
          digitalWrite(ledGreen, HIGH);
        }
      }

    //------------------------------------------Servo cotovelo frente
      while(dataIn == "21") {
      if (Bluetooth.available() > 0) {
      dataIn = Bluetooth.readString();
      }

        if (servo3PPos > servo03_min) {  
          driver.setChannelPWM(2,Servo03.pwmForAngle(servo3PPos));
          servo3PPos--;
          delay(speedDelay);
          digitalWrite(ledBlue, HIGH);
          digitalWrite(ledGreen, LOW);
        }else{
          digitalWrite(ledBlue, LOW);
          digitalWrite(ledGreen, HIGH);
        }
      }

    //---------------------------------------Servo cotovelo trás
      while(dataIn == "20") {
      if (Bluetooth.available() > 0) {
      dataIn = Bluetooth.readString();
      }
      
        if (servo3PPos < servo03_max) {
          driver.setChannelPWM(2,Servo03.pwmForAngle(servo3PPos));
          servo3PPos++;
          delay(speedDelay);
          digitalWrite(ledBlue, HIGH);
          digitalWrite(ledGreen, LOW);
        }else{
          digitalWrite(ledBlue, LOW);
          digitalWrite(ledGreen, HIGH);
        }
      }

    //-------------------------------------------------Servo pulso rotação direita
      while(dataIn == "19") {
      if (Bluetooth.available() > 0) {
      dataIn = Bluetooth.readString();
      }
        if (servo4PPos > servo04_min) {
          digitalWrite(ledBlue, HIGH);
          digitalWrite(ledGreen, LOW);
          driver.setChannelPWM(3,Servo04.pwmForAngle(servo4PPos));
          servo4PPos--;
          delay(speedDelay);
        }else{
          digitalWrite(ledBlue, LOW);
          digitalWrite(ledGreen, HIGH);
        }

      }

    //----------------------------------------Servo pulso rotação esquerda
      while(dataIn == "18") {
      if (Bluetooth.available() > 0) {
      dataIn = Bluetooth.readString();
      }
        if (servo4PPos < servo04_max) {
          digitalWrite(ledBlue, HIGH);
          digitalWrite(ledGreen, LOW);
          driver.setChannelPWM(3,Servo04.pwmForAngle(servo4PPos));
          servo4PPos++;
          delay(speedDelay);
        }else{
          digitalWrite(ledBlue, LOW);
          digitalWrite(ledGreen, HIGH);
        }
      }

    //-------------------------------------------Servo pulso subida
      while(dataIn == "16") {
      if (Bluetooth.available() > 0) {
      dataIn = Bluetooth.readString();
      }
      if (servo5PPos > servo05_min) {
        digitalWrite(ledBlue, HIGH);
        digitalWrite(ledGreen, LOW);
        driver.setChannelPWM(4,Servo05.pwmForAngle(servo5PPos));
        servo5PPos--;
        delay(speedDelay);
      }else{
        digitalWrite(ledBlue, LOW);
        digitalWrite(ledGreen, HIGH);
      }
     
      }

    //----------------------------------------------Servo pulso descida
      while(dataIn == "17") {
      if (Bluetooth.available() > 0) {
      dataIn = Bluetooth.readString();
      }
      if (servo5PPos < servo05_max) {
        digitalWrite(ledBlue, HIGH);
        digitalWrite(ledGreen, LOW);
        driver.setChannelPWM(4,Servo05.pwmForAngle(servo5PPos));
        servo5PPos++;
        delay(speedDelay);
      }else{
        digitalWrite(ledBlue, LOW);
        digitalWrite(ledGreen, HIGH);
      }
      }

    //---------------------------------------Gripper abrir
      while(dataIn == "14") {
      if (Bluetooth.available() > 0) {
      dataIn = Bluetooth.readString();
      }
      if (servo6PPos > servo06_min) {
        digitalWrite(ledBlue, HIGH);
        digitalWrite(ledGreen, LOW);
        driver.setChannelPWM(5,Servo06.pwmForAngle(servo6PPos));
        servo6PPos--;
        delay(speedDelay);
      }else{
        digitalWrite(ledBlue, LOW);
        digitalWrite(ledGreen, HIGH);
      }
      
      }

    //--------------------------------------------Gripper fechar
      while(dataIn == "15") {
      if (Bluetooth.available() > 0) {
      dataIn = Bluetooth.readString();
      }
      if (servo6PPos < servo06_max) {
        digitalWrite(ledBlue, HIGH);
        digitalWrite(ledGreen, LOW);
        driver.setChannelPWM(5,Servo06.pwmForAngle(servo6PPos));
        servo6PPos++;
        delay(speedDelay);
      }else{
        digitalWrite(ledBlue, LOW);
        digitalWrite(ledGreen, HIGH);
      }  
      }

    // ---------------------------------------------Parar movimentos
      if(dataIn == "0"){
        digitalWrite(ledBlue,LOW);
      }

    //----------------------------------botão SAVE gravar posição atual nos arrays
    if (dataIn == "1") {
      servo01SP[index] = servo1PPos;  
      servo02SP[index] = servo2PPos;
      servo03SP[index] = servo3PPos;
      servo04SP[index] = servo4PPos;
      servo05SP[index] = servo5PPos;
      servo06SP[index] = servo6PPos;
      //incrementar o indice para nova posição
      index++;
      dataIn == "0";
    }

    //----------------------------------------botao RUN - correr rotina runSteps
    if (dataIn == "3") {
      runSteps();

      //carregar no botao RESET
      if (dataIn != "3") {
        //apagar as posições gravadas
        memset(servo01SP, 0, sizeof(servo01SP));
        memset(servo02SP, 0, sizeof(servo02SP));
        memset(servo03SP, 0, sizeof(servo03SP));
        memset(servo04SP, 0, sizeof(servo04SP));
        memset(servo05SP, 0, sizeof(servo05SP));
        memset(servo06SP, 0, sizeof(servo06SP));
        //resetar o index para 0 posições
        index = 0;
        digitalWrite(ledGreen, LOW);
        digitalWrite(ledRed, LOW);
      }
    }

    //------------------------------------------------Ir para Home Position
    if (dataIn == "26"){

      // Servo 1
      if (servo1PPos == 0) {
      }
      if (servo1PPos > 0) {
        for (int j = servo1PPos; j >= 0; j--) {
          driver.setChannelPWM(0,Servo01.pwmForAngle(j));
          delay(speedDelay);
        }
      }
      if (servo1PPos < 0) {
        for (int j = servo1PPos; j <= 0; j++) {
          driver.setChannelPWM(0,Servo01.pwmForAngle(j));
          delay(speedDelay);
        }
      }

      // Servo 2
      if (servo2PPos == 85) {
      }
      if (servo2PPos > 85) {
        for (int j = servo2PPos; j >= 85; j--) {
          driver.setChannelPWM(1,Servo02.pwmForAngle(j));
          delay(speedDelay);
        }
      }
      if (servo2PPos < 85) {
        for (int j = servo2PPos; j <= 85; j++) {
          driver.setChannelPWM(1,Servo02.pwmForAngle(j));
          delay(speedDelay);
        }
      }

      // Servo 3
      if (servo3PPos == 0) {
      }
      if (servo3PPos > 0) {
        for (int j = servo3PPos; j >= 0; j--) {
          driver.setChannelPWM(2,Servo03.pwmForAngle(j));
          delay(speedDelay);
        }
      }
      if (servo3PPos < 0) {
        for (int j = servo3PPos; j <= 0; j++) {
          driver.setChannelPWM(2,Servo03.pwmForAngle(j));
          delay(speedDelay);
        }
      }

      // Servo 4
      if (servo4PPos == 0) {
      }
      if (servo4PPos > 0) {
        for (int j = servo4PPos; j >= 0; j--) {
          driver.setChannelPWM(3, Servo04.pwmForAngle(j));
          delay(speedDelay);
        }
      }
      if (servo4PPos < 0) {
        for (int j = servo4PPos; j <= 0; j++) {
          driver.setChannelPWM(3, Servo04.pwmForAngle(j));
          delay(speedDelay);
        }
      }

      // Servo 5
      if (servo5PPos == -60) {
      }
      if (servo5PPos > -60) {
        for (int j = servo5PPos; j >= -60; j--) {
          driver.setChannelPWM(4,Servo05.pwmForAngle(j));
          delay(speedDelay);
        }
      }
      if (servo5PPos < -60) {
        for (int j = servo5PPos; j <= -60; j++) {
          driver.setChannelPWM(4,Servo05.pwmForAngle(j));
          delay(speedDelay);
        }
      }

      // Servo 6
      if (servo6PPos == 90) {
      }
      if (servo6PPos > 90) {
        for (int j = servo6PPos; j >= 90; j--) {
          driver.setChannelPWM(5,Servo06.pwmForAngle(j));
          delay(speedDelay);
        }
      }
      if (servo6PPos < 90) {
        for (int j = servo6PPos; j <= 90; j++) {
          driver.setChannelPWM(5,Servo06.pwmForAngle(j));
          delay(speedDelay);
        }
      }

    servo1PPos = 0;
    servo2PPos = 85;
    servo3PPos = 0;
    servo4PPos = 0;
    servo5PPos = -60;
    servo6PPos = 90;
    }

        //------------------------------------------------Ir para Position1
    if (dataIn == "27"){

      

      // Servo 4
      if (servo4PPos == -17) {
      }
      if (servo4PPos > -17) {
        for (int j = servo4PPos; j >= -17; j--) {
          driver.setChannelPWM(3, Servo04.pwmForAngle(j));
          delay(speedDelay);
        }
      }
      if (servo4PPos < -17) {
        for (int j = servo4PPos; j <= -17; j++) {
          driver.setChannelPWM(3, Servo04.pwmForAngle(j));
          delay(speedDelay);
        }
      }

      // Servo 5
      if (servo5PPos == 78) {
      }
      if (servo5PPos > 78) {
        for (int j = servo5PPos; j >= 78; j--) {
          driver.setChannelPWM(4,Servo05.pwmForAngle(j));
          delay(speedDelay);
        }
      }
      if (servo5PPos < 78) {
        for (int j = servo5PPos; j <= 78; j++) {
          driver.setChannelPWM(4,Servo05.pwmForAngle(j));
          delay(speedDelay);
        }
      }

      // Servo 6
      if (servo6PPos == 90) {
      }
      if (servo6PPos > 90) {
        for (int j = servo6PPos; j >= 90; j--) {
          driver.setChannelPWM(5,Servo06.pwmForAngle(j));
          delay(speedDelay);
        }
      }
      if (servo6PPos < 90) {
        for (int j = servo6PPos; j <= 90; j++) {
          driver.setChannelPWM(5,Servo06.pwmForAngle(j));
          delay(speedDelay);
        }
      }

      // Servo 1
      if (servo1PPos == 0) {
      }
      if (servo1PPos > 0) {
        for (int j = servo1PPos; j >= 0; j--) {
          driver.setChannelPWM(0,Servo01.pwmForAngle(j));
          delay(speedDelay);
        }
      }
      if (servo1PPos < 0) {
        for (int j = servo1PPos; j <= 0; j++) {
          driver.setChannelPWM(0,Servo01.pwmForAngle(j));
          delay(speedDelay);
        }
      }

      // Servo 2
      if (servo2PPos == -13) {
      }
      if (servo2PPos > -13) {
        for (int j = servo2PPos; j >= -13; j--) {
          driver.setChannelPWM(1,Servo02.pwmForAngle(j));
          delay(speedDelay);
        }
      }
      if (servo2PPos < -13) {
        for (int j = servo2PPos; j <= -13; j++) {
          driver.setChannelPWM(1,Servo02.pwmForAngle(j));
          delay(speedDelay);
        }
      }

      // Servo 3
      if (servo3PPos == 47) {
      }
      if (servo3PPos > 47) {
        for (int j = servo3PPos; j >= 47; j--) {
          driver.setChannelPWM(2,Servo03.pwmForAngle(j));
          delay(speedDelay);
        }
      }
      if (servo3PPos < 47) {
        for (int j = servo3PPos; j <= 47; j++) {
          driver.setChannelPWM(2,Servo03.pwmForAngle(j));
          delay(speedDelay);
        }
      }

    servo1PPos = 0;
    servo2PPos = -13;
    servo3PPos = 47;
    servo4PPos = -17;
    servo5PPos = 78;
    servo6PPos = 90;
    }

            //------------------------------------------------Ir para Position2
    if (dataIn == "28"){

            // Servo 1
      if (servo1PPos == 0) {
      }
      if (servo1PPos > 0) {
        for (int j = servo1PPos; j >= 0; j--) {
          driver.setChannelPWM(0,Servo01.pwmForAngle(j));
          delay(speedDelay);
        }
      }
      if (servo1PPos < 0) {
        for (int j = servo1PPos; j <= 0; j++) {
          driver.setChannelPWM(0,Servo01.pwmForAngle(j));
          delay(speedDelay);
        }
      }

      // Servo 2
      if (servo2PPos == 42) {
      }
      if (servo2PPos > 42) {
        for (int j = servo2PPos; j >= 42; j--) {
          driver.setChannelPWM(1,Servo02.pwmForAngle(j));
          delay(speedDelay);
        }
      }
      if (servo2PPos < 42) {
        for (int j = servo2PPos; j <= 42; j++) {
          driver.setChannelPWM(1,Servo02.pwmForAngle(j));
          delay(speedDelay);
        }
      }

      // Servo 3
      if (servo3PPos == -90) {
      }
      if (servo3PPos > -90) {
        for (int j = servo3PPos; j >= -90; j--) {
          driver.setChannelPWM(2,Servo03.pwmForAngle(j));
          delay(speedDelay);
        }
      }
      if (servo3PPos < -90) {
        for (int j = servo3PPos; j <= -90; j++) {
          driver.setChannelPWM(2,Servo03.pwmForAngle(j));
          delay(speedDelay);
        }
      }

      // Servo 4
      if (servo4PPos == -15) {
      }
      if (servo4PPos > -15) {
        for (int j = servo4PPos; j >= -15; j--) {
          driver.setChannelPWM(3, Servo04.pwmForAngle(j));
          delay(speedDelay);
        }
      }
      if (servo4PPos < -15) {
        for (int j = servo4PPos; j <= -15; j++) {
          driver.setChannelPWM(3, Servo04.pwmForAngle(j));
          delay(speedDelay);
        }
      }

      // Servo 5
      if (servo5PPos == -20) {
      }
      if (servo5PPos > -20) {
        for (int j = servo5PPos; j >= -20; j--) {
          driver.setChannelPWM(4,Servo05.pwmForAngle(j));
          delay(speedDelay);
        }
      }
      if (servo5PPos < -20) {
        for (int j = servo5PPos; j <= -20; j++) {
          driver.setChannelPWM(4,Servo05.pwmForAngle(j));
          delay(speedDelay);
        }
      }

      // Servo 6
      if (servo6PPos == -10) {
      }
      if (servo6PPos > -10) {
        for (int j = servo6PPos; j >= -10; j--) {
          driver.setChannelPWM(5,Servo06.pwmForAngle(j));
          delay(speedDelay);
        }
      }
      if (servo6PPos < -10) {
        for (int j = servo6PPos; j <= -10; j++) {
          driver.setChannelPWM(5,Servo06.pwmForAngle(j));
          delay(speedDelay);
        }
      }



    servo1PPos = 0;
    servo2PPos = 42;
    servo3PPos = -90;
    servo4PPos = -15;
    servo5PPos = -20;
    servo6PPos = -10;
    }

                //------------------------------------------------Ir para Position3
    if (dataIn == "29"){

            // Servo 4
      if (servo4PPos == 69) {
      }
      if (servo4PPos > 69) {
        for (int j = servo4PPos; j >= 69; j--) {
          driver.setChannelPWM(3, Servo04.pwmForAngle(j));
          delay(speedDelay);
        }
      }
      if (servo4PPos < 69) {
        for (int j = servo4PPos; j <= 69; j++) {
          driver.setChannelPWM(3, Servo04.pwmForAngle(j));
          delay(speedDelay);
        }
      }

      // Servo 5
      if (servo5PPos == 75) {
      }
      if (servo5PPos > 75) {
        for (int j = servo5PPos; j >= 75; j--) {
          driver.setChannelPWM(4,Servo05.pwmForAngle(j));
          delay(speedDelay);
        }
      }
      if (servo5PPos < 75) {
        for (int j = servo5PPos; j <= 75; j++) {
          driver.setChannelPWM(4,Servo05.pwmForAngle(j));
          delay(speedDelay);
        }
      }

      // Servo 6
      if (servo6PPos == -10) {
      }
      if (servo6PPos > -10) {
        for (int j = servo6PPos; j >= -10; j--) {
          driver.setChannelPWM(5,Servo06.pwmForAngle(j));
          delay(speedDelay);
        }
      }
      if (servo6PPos < -10) {
        for (int j = servo6PPos; j <= -10; j++) {
          driver.setChannelPWM(5,Servo06.pwmForAngle(j));
          delay(speedDelay);
        }
      }

            // Servo 1
      if (servo1PPos == 0) {
      }
      if (servo1PPos > 0) {
        for (int j = servo1PPos; j >= 0; j--) {
          driver.setChannelPWM(0,Servo01.pwmForAngle(j));
          delay(speedDelay);
        }
      }
      if (servo1PPos < 0) {
        for (int j = servo1PPos; j <= 0; j++) {
          driver.setChannelPWM(0,Servo01.pwmForAngle(j));
          delay(speedDelay);
        }
      }

      // Servo 2
      if (servo2PPos == -10) {
      }
      if (servo2PPos > -10) {
        for (int j = servo2PPos; j >= -10; j--) {
          driver.setChannelPWM(1,Servo02.pwmForAngle(j));
          delay(speedDelay);
        }
      }
      if (servo2PPos < -10) {
        for (int j = servo2PPos; j <= -10; j++) {
          driver.setChannelPWM(1,Servo02.pwmForAngle(j));
          delay(speedDelay);
        }
      }

      // Servo 3
      if (servo3PPos == -90) {
      }
      if (servo3PPos > -90) {
        for (int j = servo3PPos; j >= -90; j--) {
          driver.setChannelPWM(2,Servo03.pwmForAngle(j));
          delay(speedDelay);
        }
      }
      if (servo3PPos < -90) {
        for (int j = servo3PPos; j <= -90; j++) {
          driver.setChannelPWM(2,Servo03.pwmForAngle(j));
          delay(speedDelay);
        }
      }

    servo1PPos = 0;
    servo2PPos = -10;
    servo3PPos = -90;
    servo4PPos = 69;
    servo5PPos = 75;
    servo6PPos = -10;
    }
  }
}
