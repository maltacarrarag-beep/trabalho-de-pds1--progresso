#include "input.h"
#include "logica.h"

void processar_input(EstadoJogo *jogo, unsigned char *keys, int *redraw) {
   
    // NAVEGAÇÃO (Esquerda / Direita)
    
    // Seta Esquerda: Diminui o índice
    if (keys[ALLEGRO_KEY_LEFT] & GAME_KEY_DOWN) {
        if (jogo->carta_selecionada > 0) {
            jogo->carta_selecionada--;
            *redraw = 1;
        }
        keys[ALLEGRO_KEY_LEFT] &= ~GAME_KEY_DOWN;         // Trava para não correr muito rápido
    }

    // Seta Direita: Aumenta o índice
    if (keys[ALLEGRO_KEY_RIGHT] & GAME_KEY_DOWN) {
        if (jogo->carta_selecionada < jogo->jogador.qtd_mao - 1) {
            jogo->carta_selecionada++;
            *redraw = 1;
        }
        keys[ALLEGRO_KEY_RIGHT] &= ~GAME_KEY_DOWN;        // Trava para não correr muito rápido
    }

    // --- AÇÃO (Enter ou Espaço para JOGAR a carta selecionada) ---
    // Nota: O enunciado diz Enter para confirmar e Espaço para "matar todos" (cheat),   !!!!!!!!!!!!!!!!!
    // mas vamos usar Enter para jogar carta por enquanto.
    if (keys[ALLEGRO_KEY_ENTER] & GAME_KEY_DOWN) {
        
        // Joga a carta que estiver marcada no índice 'carta_selecionada'
        jogar_carta(jogo, jogo->carta_selecionada);
        
        // Ajuste de segurança: se joguei a última carta da mão, o cursor deve voltar um pra trás
        if (jogo->carta_selecionada >= jogo->jogador.qtd_mao && jogo->jogador.qtd_mao > 0) {
            jogo->carta_selecionada = jogo->jogador.qtd_mao - 1;
        }
        
        keys[ALLEGRO_KEY_ENTER] &= ~GAME_KEY_DOWN;
        *redraw = 1;
    }

    // PASSAR TURNO (Tecla E ou TAB, já que Enter agora joga carta) 
    // Vamos usar 'E' de "End Turn" ou 'T' de Turno para não confundir com o Enter
    if (keys[ALLEGRO_KEY_E] & GAME_KEY_DOWN) {
        passar_turno(jogo);
        jogo->carta_selecionada = 0; // Reseta cursor pro inicio
        keys[ALLEGRO_KEY_E] &= ~GAME_KEY_DOWN;
        *redraw = 1;
    }

    // --- AQUI VOCÊ ADICIONARÁ ENTER, ESPAÇO, ESC DEPOIS ---
}