//-----------------------------------------------EMETTEUR NEOPIXEL 4BP-----------------------------------------------------
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
#include <Adafruit_NeoPixel.h>

#define BROCHE_BP1 D0        // Bouton 1 sur D0
#define BROCHE_BP2 D1        // Bouton 2 sur D1
#define BROCHE_BP3 D5        // Bouton 3 sur D5
#define BROCHE_BP4 D6        // Bouton 4 sur D6
#define DINEOPIXEL D7        // NeoPixel sur D7
#define N_LEDS 8             // Utilisation de 4 NeoPixels

RF24 radio(2, 3);            // CE sur D2, CSN sur D3 pour le module nRF24L01
const byte addresses[][6] = {"00001", "00002"};  // Adresse radio pour la communication

Adafruit_NeoPixel strip(N_LEDS, DINEOPIXEL, NEO_GRB + NEO_KHZ800);  // Configuration NeoPixel

// Structure du message envoyé au récepteur
typedef struct {
  bool Msg1;  // État demandé de LED1
  bool Msg2;  // État demandé de LED2
  bool Msg3;  // État demandé de LED3
  bool Msg4;  // État demandé de LED4
} MsgSt;

MsgSt MessageToRecepteur;

// Structure du message reçu du récepteur
typedef struct {
  bool ledState1;  // État de la LED1 du récepteur
  bool ledState2;  // État de la LED2 du récepteur
  bool ledState3;  // État de la LED3 du récepteur
  bool ledState4;  // État de la LED4 du récepteur
} MsgSt1;

MsgSt1 reponseFromRecepteur;

// Variables pour le basculement (toggle)
bool etatLED1 = LOW;
bool etatLED2 = LOW;
bool etatLED3 = LOW;
bool etatLED4 = LOW;
bool lastBP1State = HIGH;
bool lastBP2State = HIGH;
bool lastBP3State = HIGH;
bool lastBP4State = HIGH;

void setup() {
  pinMode(BROCHE_BP1, INPUT_PULLUP);  // Initialisation du bouton BP1
  pinMode(BROCHE_BP2, INPUT_PULLUP);  // Initialisation du bouton BP2
  pinMode(BROCHE_BP3, INPUT_PULLUP);  // Initialisation du bouton BP3
  pinMode(BROCHE_BP4, INPUT_PULLUP);  // Initialisation du bouton BP4


  //Serial.begin(9600);  // Pour le débogage
  radio.begin();
  radio.openWritingPipe(addresses[1]);  // Émetteur
  radio.openReadingPipe(1, addresses[0]);  // Récepteur
  //radio.setPALevel(RF24_PA_MIN);
  radio.setPALevel(RF24_PA_HIGH);
  //radio.setPALevel(RF24_PA_MAX);
  
  // Initialisation du NeoPixel
  strip.begin();
  strip.clear();  // Efface toutes les LEDs
  strip.show();
  

  delay(50);
}

void loop() {
  // Lecture des boutons BP1 à BP4
  bool currentBP1State = digitalRead(BROCHE_BP1);
  bool currentBP2State = digitalRead(BROCHE_BP2);
  bool currentBP3State = digitalRead(BROCHE_BP3);
  bool currentBP4State = digitalRead(BROCHE_BP4);

  // Basculement de LED1 si BP1 est pressé
  if (currentBP1State == LOW && lastBP1State == HIGH) {
    etatLED1 = !etatLED1;
  }
  lastBP1State = currentBP1State;

  // Basculement de LED2 si BP2 est pressé
  if (currentBP2State == LOW && lastBP2State == HIGH) {
    etatLED2 = !etatLED2;
  }
  lastBP2State = currentBP2State;

  // Basculement de LED3 si BP3 est pressé
  if (currentBP3State == LOW && lastBP3State == HIGH) {
    etatLED3 = !etatLED3;
  }
  lastBP3State = currentBP3State;

  // Basculement de LED4 si BP4 est pressé
  if (currentBP4State == LOW && lastBP4State == HIGH) {
    etatLED4 = !etatLED4;
  }
  lastBP4State = currentBP4State;

  // Envoi des états au récepteur
  MessageToRecepteur.Msg1 = etatLED1;
  MessageToRecepteur.Msg2 = etatLED2;
  MessageToRecepteur.Msg3 = etatLED3;
  MessageToRecepteur.Msg4 = etatLED4;

  radio.stopListening();
  radio.write(&MessageToRecepteur, sizeof(MessageToRecepteur));

  // Réception des réponses du récepteur
  radio.startListening();
  if (radio.available()) {
    while (radio.available()) {
      radio.read(&reponseFromRecepteur, sizeof(reponseFromRecepteur));
    }
  }

  // Mise à jour des NeoPixels en fonction des réponses du récepteur
  if (reponseFromRecepteur.ledState1) {
      strip.setPixelColor(0, strip.Color(255, 0, 0));  // LED1 en rouge si allumée
  } else {
    strip.setPixelColor(0, strip.Color(0, 0, 0));    // LED1 éteinte
  }

  if (reponseFromRecepteur.ledState2) {
      strip.setPixelColor(2, strip.Color(0, 255, 0));  // LED2 en vert si allumée
  } else {
    strip.setPixelColor(2, strip.Color(0, 0, 0));    // LED2 éteinte
  }

  if (reponseFromRecepteur.ledState3) {
      strip.setPixelColor(4, strip.Color(0, 0, 255));  // LED3 en bleu si allumée
  } else {
    strip.setPixelColor(4, strip.Color(0, 0, 0));    // LED3 éteinte
  }

  if (reponseFromRecepteur.ledState4) {
      strip.setPixelColor(6, strip.Color(255, 255, 255));  // LED4 en blanc si allumée
  } else {
    strip.setPixelColor(6, strip.Color(0, 0, 0));      // LED4 éteinte
  }

  strip.show();  // Afficher les changements sur les NeoPixels

  delay(100);  // Petite pause
}
