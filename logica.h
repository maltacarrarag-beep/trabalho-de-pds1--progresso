#ifndef LOGICA_H
#define LOGICA_H

#include "structs.h"

// Protótipos das funções (Avisar que elas existem)
void embaralhar(Carta *vetor, int tamanho);                // Para embaralhar as cartas
void comprar_cartas(Jogador *j, int quantidade);           // Para comprar as cartas
void descartar_carta(Jogador *j, int indice_na_mao);       // Para descartar as cartas
void jogar_carta(EstadoJogo *jogo, int indice_mao);        // Para jogar as cartas durante o jogo
void passar_turno(EstadoJogo *jogo);                       // Para tornar o jogo infinito (não durar 1 turno só)
void inicializar_jogo(EstadoJogo *jogo);                   // Para começar o jogo

#endif