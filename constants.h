#ifndef _CONSTANTS_H_
#define _CONSTANTS_H_

// Novas variáveis para
#define MAX_CARTAS_MAO 10
#define MAX_CARTAS_DECK 50
#define MAX_ACOES_INIMIGO 5
#define MAX_INIMIGOS 2

#define GAME_KEY_SEEN 1
#define GAME_KEY_DOWN 2

//define o tam da tela
#define DISPLAY_BUFFER_WIDTH 1280.0     
#define DISPLAY_BUFFER_HEIGHT 720.0

#define DISPLAY_SCALE 1.0
#define DISPLAY_WIDTH DISPLAY_SCALE* DISPLAY_BUFFER_WIDTH
#define DISPLAY_HEIGHT DISPLAY_SCALE* DISPLAY_BUFFER_HEIGHT

#define DRAW_DECK_X 15
#define DRAW_DECK_Y 910

#define DECK_WIDTH 80
#define DECK_HEIGHT 80

//coordenadas onde o jogador comeca
#define PLAYER_BEGIN_X 200
#define PLAYER_BEGIN_Y 350
#define PLAYER_RADIUS 80

//coordenadas onde o inimigo comeca
#define ENEMY_BEGIN_X 1000
#define ENEMY_BEGIN_Y 350

#define HEALTH_BAR_HEIGHT 20
#define HEALTH_BAR_BACKGROUND_EXTRA 2
#define HEALTH_BAR_RX 3
#define HEALTH_BAR_RY 3

//posição da mão
#define HAND_BEGIN_X 100
#define HAND_BEGIN_Y 500

//tamanho das cartas
#define CARD_WIDTH 120
#define CARD_HEIGHT 180

// You might want to use this.
typedef enum {
  MOVE_LEFT = 0,
  MOVE_RIGHT = 1,
} CursorMovementDirection;

#endif