#include "input.h"
#include "logica.h"

void processar_input(EstadoJogo *jogo, unsigned char *keys, int *redraw) {
    
    // TECLAS DE COMBATE (1 a 5) 
    
    // Carta 1
    if (keys[ALLEGRO_KEY_1] & GAME_KEY_DOWN) {
        jogar_carta(jogo, 0);                      // Tenta jogar a carta 0
        keys[ALLEGRO_KEY_1] &= ~GAME_KEY_DOWN;     // Trava tecla
        *redraw = 1;                               // Avisa o main para redesenhar a tela
    }

    // Carta 2
    if (keys[ALLEGRO_KEY_2] & GAME_KEY_DOWN) {
        jogar_carta(jogo, 1);                      // Tenta jogar a carta 1
        keys[ALLEGRO_KEY_2] &= ~GAME_KEY_DOWN;     // Trava tecla
        *redraw = 1;                               // Avisa o main para redesenhar a tela
    }

    // Carta 3
    if (keys[ALLEGRO_KEY_3] & GAME_KEY_DOWN) {
        jogar_carta(jogo, 2);                      // Tenta jogar a carta 2
        keys[ALLEGRO_KEY_3] &= ~GAME_KEY_DOWN;     // Trava tecla
        *redraw = 1;                               // Avisa o main para redesenhar a tela
    }

    // Carta 4
    if (keys[ALLEGRO_KEY_4] & GAME_KEY_DOWN) {
        jogar_carta(jogo, 3);                      // Tenta jogar a carta 3
        keys[ALLEGRO_KEY_4] &= ~GAME_KEY_DOWN;     // Trava tecla
        *redraw = 1;                               // Avisa o main para redesenhar a tela
    }

    // Carta 5
    if (keys[ALLEGRO_KEY_5] & GAME_KEY_DOWN) {
        jogar_carta(jogo, 4);                      // Tenta jogar a carta 4
        keys[ALLEGRO_KEY_5] &= ~GAME_KEY_DOWN;     // Trava tecla
        *redraw = 1;                               // Avisa o main para redesenhar a tecla
    }

    //PASSAR TURNO (usar a tecla ENTER ou a tecla ESPAÇO)

    if ((keys[ALLEGRO_KEY_SPACE] & GAME_KEY_DOWN) || (keys[ALLEGRO_KEY_ENTER] & GAME_KEY_DOWN)) {
        passar_turno(jogo);
        
        // Trava as teclas para não passar 50 turnos de uma vez
        keys[ALLEGRO_KEY_SPACE] &= ~GAME_KEY_DOWN;
        keys[ALLEGRO_KEY_ENTER] &= ~GAME_KEY_DOWN;
        
        *redraw = 1; // Redesenha a tela com as novas cartas e vida atualizada
    }

    // --- AQUI VOCÊ ADICIONARÁ ENTER, ESPAÇO, ESC DEPOIS ---
}