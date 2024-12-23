//---------------------------------------------------------RECEPTEUR 4BP--------------------------------------------------
// Claude DFOURMONT
// Mon YouTube : https://www.youtube.com/c/ClaudeDufourmont
// Mon GitHub : https://github.com/claudiusmarius
// Vidéo numérotée : DFT_#A150 DFT_#A181 DFT_#A184
// Utilisation de 2 Seeeduino XIAO et de 2 NRF24L01 ainsi que d'une barette de 8 Neopixels (WS2812B)
// ========================================================================================================================
// ****** N'hésitez pas à liker, commenter, partager et à vous abonner, cela contribue à promouvoir la chaine, merci ******
// ========================================================================================================================
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#define LED1 D0        // LED1 sur D0 (pour afficher l'état de LED1)
#define LED2 D1        // LED2 sur D1 (pour afficher l'état de LED2)
#define LED3 D5        // LED3 sur D5 (pour afficher l'état de LED3)
#define LED4 D6        // LED4 sur D6 (pour afficher l'état de LED4)

RF24 radio(2, 3);  // CE sur D2, CSN sur D3 pour le module nRF24L01
const byte addresses[][6] = {"00002", "00001"};  // Adresse radio pour la communication

// Structure du message reçu de l'émetteur
typedef struct {
  bool Msg1;  // État demandé de LED1
  bool Msg2;  // État demandé de LED2
  bool Msg3;  // État demandé de LED3
  bool Msg4;  // État demandé de LED4
} MsgSt;

// Structure du message envoyé à l'émetteur
typedef struct {
  bool ledState1;  // État de la LED1
  bool ledState2;  // État de la LED2
  bool ledState3;  // État de la LED3
  bool ledState4;  // État de la LED4
} MsgSt1;

MsgSt messageFromEmetteur;
MsgSt1 reponseToEmetteur;

void setup() {
  pinMode(LED1, OUTPUT);  // Initialisation de la LED1
  pinMode(LED2, OUTPUT);  // Initialisation de la LED2
  pinMode(LED3, OUTPUT);  // Initialisation de la LED3
  pinMode(LED4, OUTPUT);  // Initialisation de la LED4
  //Serial.begin(9600);  // Pour le débogage
  radio.begin();
  radio.openWritingPipe(addresses[1]);  // Émetteur
  radio.openReadingPipe(1, addresses[0]);  // Récepteur
  
                          //radio.setDataRate(RF24_250KBPS);
  
  
  radio.setPALevel(RF24_PA_MIN);
  //radio.setPALevel(RF24_PA_HIGH);
  //radio.setPALevel(RF24_PA_MAX);
  //radio.setPALevel(RF24_PA_LOW);
  radio.startListening();  // Démarre l'écoute
}

void loop() {
  // Vérification de la réception des messages de l'émetteur
  if (radio.available()) {
    while (radio.available()) {
      radio.read(&messageFromEmetteur, sizeof(messageFromEmetteur));
    }

    // Mettre à jour les états des LEDs en fonction des messages reçus
    digitalWrite(LED1, messageFromEmetteur.Msg1 ? HIGH : LOW);
    digitalWrite(LED2, messageFromEmetteur.Msg2 ? HIGH : LOW);
    digitalWrite(LED3, messageFromEmetteur.Msg3 ? HIGH : LOW);
    digitalWrite(LED4, messageFromEmetteur.Msg4 ? HIGH : LOW);
    
    // Prépare la réponse à l'émetteur
    reponseToEmetteur.ledState1 = digitalRead(LED1);
    reponseToEmetteur.ledState2 = digitalRead(LED2);
    reponseToEmetteur.ledState3 = digitalRead(LED3);
    reponseToEmetteur.ledState4 = digitalRead(LED4);

    // Envoie la réponse à l'émetteur
    radio.stopListening();
    radio.write(&reponseToEmetteur, sizeof(reponseToEmetteur));
    radio.startListening();

    // Pour le débogage
    //Serial.print("Msg1: "); Serial.print(messageFromEmetteur.Msg1);
    //Serial.print(", Msg2: "); Serial.print(messageFromEmetteur.Msg2);
    //Serial.print(", Msg3: "); Serial.print(messageFromEmetteur.Msg3);
    //Serial.print(", Msg4: "); Serial.println(messageFromEmetteur.Msg4);
  }
}
