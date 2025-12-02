//CÓDIGO DE COMPILAÇÃO NO VSCODE: gcc main.c renderer.c utils.c -o game -lallegro -lallegro_image -lallegro_primitives -lallegro_font -lallegro_ttf -lm

#include <allegro5/allegro5.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>             // Para gerar o número de cartas aleatório

#include "input.h"
#include "logica.h"
#include "structs.h"
#include "constants.h"
#include "renderer.h"
#include "utils.h"

int main() {

  srand(time(NULL));          // Para gerar o número de cartas aleatório

  must_init(al_init(), "allegro");
  must_init(al_init_image_addon(), "allegro");
  must_init(al_init_primitives_addon(), "primitives");
  must_init(al_install_keyboard(), "keyboard");

  ALLEGRO_TIMER* timer = al_create_timer(1.0 / 60.0);
  must_init(timer, "timer");

  ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue();
  must_init(queue, "queue");

  al_register_event_source(queue, al_get_keyboard_event_source());
  al_register_event_source(queue, al_get_timer_event_source(timer));
  unsigned char keyboard_keys[ALLEGRO_KEY_MAX];
  ClearKeyboardKeys(keyboard_keys);
  ALLEGRO_EVENT event;

  Renderer renderer;
  FillRenderer(&renderer);
  al_register_event_source(queue,
                           al_get_display_event_source(renderer.display));

  EstadoJogo jogo;          // Cria a "caixa" que guarda os dados
  inicializar_jogo(&jogo);  // Preenche a caixa com vida e cartas

  renderer.jogo = &jogo;    // Conecta o jogo limpo ao desenhista

  printf("JOGO INICIADO!\nVida do Jogador: %d\n", jogo.jogador.entidade.vida_atual);
  printf("Cartas no baralho: %d\n", jogo.jogador.qtd_baralho);

  al_start_timer(timer);
  while (1) {
    al_wait_for_event(queue, &event);
    int done = 0, print_combat = 0, redraw = 0;

    switch (event.type) {
      case ALLEGRO_EVENT_TIMER:
        redraw = 1;
        if (keyboard_keys[ALLEGRO_KEY_Q]) {
          done = 1;
          break;
        }

        for (int i = 0; i < ALLEGRO_KEY_MAX; i++) {
          keyboard_keys[i] &= ~GAME_KEY_SEEN;
        }
        break;

      case ALLEGRO_EVENT_KEY_DOWN:
        keyboard_keys[event.keyboard.keycode] = GAME_KEY_SEEN | GAME_KEY_DOWN;
        break;
      case ALLEGRO_EVENT_KEY_UP:
        keyboard_keys[event.keyboard.keycode] &= ~GAME_KEY_DOWN;
        break;
      case ALLEGRO_EVENT_DISPLAY_CLOSE:
        done = true;
        break;
    }
    if (done) {
      break;
    }

    processar_input(&jogo, keyboard_keys, &redraw);

    if (redraw) {
      Render(&renderer);
      redraw = 0;
    }
  }
  al_destroy_timer(timer);
  al_destroy_event_queue(queue);
  ClearRenderer(&renderer);
  return 0;
}