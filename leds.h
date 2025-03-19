#ifndef LEDS_H
#define LEDS_H

#include <Adafruit_NeoPixel.h>

// Configuração das barras de LEDs
#define PIN_BARRA1 25  
#define PIN_BARRA2 26  
#define PIN_BARRA3 27  
 
#define NUM_PIXELS 8 

// Declaração das barras
extern Adafruit_NeoPixel barra1;
extern Adafruit_NeoPixel barra2;
extern Adafruit_NeoPixel barra3;
extern Adafruit_NeoPixel barra4;

// Funções para manipular LEDs
void inicializarLEDs();
void atualizarLEDs(Adafruit_NeoPixel &barra, int valor);

#endif // LEDS_H
