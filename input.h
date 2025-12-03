#ifndef INPUT_H
#define INPUT_H

#include <allegro5/allegro.h>
#include "structs.h"
#include "constants.h"

// Função que recebe o jogo, as teclas apertadas e um ponteiro para avisar se precisa redesenhar
void processar_input(EstadoJogo *jogo, unsigned char *keys, int *redraw);

#endif