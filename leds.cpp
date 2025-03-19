#include "leds.h"

// Inicialização das barras
Adafruit_NeoPixel barra1(NUM_PIXELS, PIN_BARRA1, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel barra2(NUM_PIXELS, PIN_BARRA2, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel barra3(NUM_PIXELS, PIN_BARRA3, NEO_GRB + NEO_KHZ800);

void inicializarLEDs() {
  barra1.begin();
  barra1.show();
  barra2.begin();
  barra2.show();
  barra3.begin();
  barra3.show();
  
}

// Função para atualizar uma barra de LEDs com base no valor mapeado
void atualizarLEDs(Adafruit_NeoPixel& strip, int valor) {
  for (int i = 0; i < strip.numPixels(); i++) {
    if (valor >= (i * 5 + 1)) {
      if (i < 3) {
        strip.setPixelColor(i, strip.Color(0, 255, 0));
      } else if (i < 6) {
        strip.setPixelColor(i, strip.Color(255, 255, 0)); 
      } else {
        strip.setPixelColor(i, strip.Color(255, 0, 0)); 
      }
    } else {
      strip.setPixelColor(i, strip.Color(0, 0, 0));
    }
  }
  strip.show(); 
}
