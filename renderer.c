#include "renderer.h"

#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "structs.h"
#include "constants.h"
#include "utils.h"

void DrawScaledText(ALLEGRO_FONT* font, ALLEGRO_COLOR color, float x, float y,
                    float xscale, float yscale, int alignment,
                    const char* text) {
  ALLEGRO_TRANSFORM transform;
  al_identity_transform(&transform);  // Start with an identity transform
  al_scale_transform(
      &transform, xscale,
      yscale);  // Apply scaling (e.g., sx=2.0, sy=2.0 for double size)
  al_use_transform(&transform);  // Use the transform for subsequent drawing

  al_draw_text(font, color, x, y, alignment, text);
  // al_draw_text(font, color, x, y, alignment, text);
  al_identity_transform(&transform);
  al_use_transform(&transform);  // Use the transform for subsequent drawing
}

void DrawCenteredScaledText(ALLEGRO_FONT* font, ALLEGRO_COLOR color, float x,
                            float y, float xscale, float yscale, char* text) {
  DrawScaledText(font, color, x, y, xscale, yscale, ALLEGRO_ALIGN_CENTRE, text);
}

void FillRenderer(Renderer* renderer) {
  al_set_new_display_option(ALLEGRO_SAMPLE_BUFFERS, 1, ALLEGRO_SUGGEST);
  al_set_new_display_option(ALLEGRO_SAMPLES, 8, ALLEGRO_SUGGEST);
  al_set_new_bitmap_flags(ALLEGRO_MIN_LINEAR | ALLEGRO_MAG_LINEAR);

  renderer->display = al_create_display(DISPLAY_WIDTH, DISPLAY_HEIGHT);
  must_init(renderer->display, "display");

  renderer->display_buffer =
      al_create_bitmap(DISPLAY_BUFFER_WIDTH, DISPLAY_BUFFER_HEIGHT);
  must_init(renderer->display_buffer, "display buffer");

  renderer->font = al_create_builtin_font();
  must_init(renderer->font, "font");
}

void ClearRenderer(Renderer* renderer) {
  al_destroy_display(renderer->display);
  al_destroy_bitmap(renderer->display_buffer);
  al_destroy_font(renderer->font);
}

void RenderBackground(Renderer* renderer) {
  al_clear_to_color(al_map_rgb(30, 30, 30));    //pinta todo o fundo de cinza escuro
}

void RenderDeck(Renderer* renderer, int x_left, int y_top) {
  ALLEGRO_BITMAP* prev_bmp_target = al_get_target_bitmap();

  ALLEGRO_BITMAP* deck_bitmap = al_create_bitmap(DECK_WIDTH, DECK_HEIGHT);
  al_set_target_bitmap(deck_bitmap);

  al_draw_filled_rounded_rectangle(0, 0, DECK_WIDTH, DECK_HEIGHT, 10, 0,
                                   al_map_rgb(255, 255, 255));
  al_set_target_bitmap(prev_bmp_target);

  al_draw_scaled_bitmap(deck_bitmap, 0, 0, DECK_WIDTH, DECK_HEIGHT, x_left,
                        y_top, DECK_WIDTH, DECK_HEIGHT, 0);
  al_destroy_bitmap(deck_bitmap);
}

void RenderHealthBar(float x, float y, int atual, int max, int escudo, ALLEGRO_FONT* font) {

  // A barra será desenhada em Y + 105 
  float y_bar = y + 105;
  
  // Fundo cinza da barra
  al_draw_filled_rectangle(x, y_bar, x + 100, y_bar + 15, al_map_rgb(50, 50, 50));
    
  // Cálculo da porcentagem de vida
  float porcentagem = (float)atual / (float)max;
  if (porcentagem < 0) porcentagem = 0;
    
  // Barra Colorida (Verde se for player/vida cheia, Vermelha se estiver baixa, etc)
  // Vamos usar Vermelho padrão para vida
  float largura_vida = 100.0 * porcentagem;
  al_draw_filled_rectangle(x, y_bar, x + largura_vida, y_bar + 15, al_map_rgb(200, 50, 50));
    
  // Borda da barra
  al_draw_rectangle(x, y_bar, x + 100, y_bar + 15, al_map_rgb(255, 255, 255), 2);

  // Se tiver escudo, desenha um numero extra
  char txt_status[30];
  if (escudo > 0) sprintf(txt_status, "%d/%d [+%d]", atual, max, escudo);
  else sprintf(txt_status, "%d/%d", atual, max);
    
  DrawCenteredScaledText(font, al_map_rgb(255,255,255), x + 50, y_bar + 20, 1.0, 1.0, txt_status);

}

void RenderCard(const Renderer* renderer, int x_left, int y_top) {
  ALLEGRO_BITMAP* card_bitmap = al_create_bitmap(CARD_WIDTH, CARD_HEIGHT);
  al_set_target_bitmap(card_bitmap);

  al_draw_filled_rounded_rectangle(0, 0, CARD_WIDTH, CARD_HEIGHT, 5, 5,
                                   al_map_rgb(255, 255, 255));
  al_draw_rounded_rectangle(0, 0, CARD_WIDTH, CARD_HEIGHT, 5, 5,
                            al_map_rgb(255, 0, 0), 2);

  float xscale = 2, yscale = 2;

  ALLEGRO_COLOR color = al_map_rgb(0, 0, 0);

  char text[10] = "";
  xscale = 3, yscale = 3;
  sprintf(text, "Oi");

  DrawScaledText(renderer->font, color, (CARD_WIDTH * 0.5) / xscale,
                 (CARD_HEIGHT * 0.3) / yscale, xscale, yscale,
                 ALLEGRO_ALIGN_LEFT, text);

  al_set_target_bitmap(renderer->display_buffer);
  al_draw_scaled_bitmap(card_bitmap, 0, 0, CARD_WIDTH, CARD_HEIGHT, x_left,
                        y_top, CARD_WIDTH, CARD_HEIGHT, 0);

  al_destroy_bitmap(card_bitmap);
}

void RenderPlayerHand(Renderer* renderer) {
  float start_x = HAND_BEGIN_X; 
  float start_y = HAND_BEGIN_Y;
  float espacamento = 130; 

  for (int i = 0; i < renderer->jogo->jogador.qtd_mao; i++) {
    Carta c = renderer->jogo->jogador.mao[i];
        
    float x = start_x + (i * espacamento);
    float y = start_y;

    
    // Se esta é a carta selecionada, desenha ela mais pra cima e com uma borda amarela
    int selecionada = (i == renderer->jogo->carta_selecionada);
    if (selecionada) {
      y -= 30; // Sobe a carta
    }

    // Cor baseada no tipo
    ALLEGRO_COLOR cor;
    if (c.tipo == ataque) cor = al_map_rgb(200, 50, 50);      // VERMELHO
    else if (c.tipo == defesa) cor = al_map_rgb(50, 50, 200); // AZUL
    else cor = al_map_rgb(100, 100, 100);                     // CINZA

    // Desenha Carta (retângulo)
    al_draw_filled_rectangle(x, y, x + 100, y + 150, cor);

    // Borda: se selecionada --> AMARELA / se não --> BRANCA
    if (selecionada) {
      al_draw_rectangle(x, y, x + 100, y + 150, al_map_rgb(255, 255, 0), 4);        // Borda amarela
    } else {
        al_draw_rectangle(x, y, x + 100, y + 150, al_map_rgb(255, 255, 255), 2);    // Borda branca
      }

    // Escrver o Tipo da Carta (ATAQUE / DEFESA / ESPECIAL)
    char txt_tipo[20];
    if (c.tipo == ataque) {
      sprintf(txt_tipo, "ATAQUE");
    } else if (c.tipo == defesa) {
      sprintf(txt_tipo, "DEFESA");
    } else {
      sprintf(txt_tipo, "ESPECIAL");
    }

    // Desenha centralizado e logo abaixo do topo
    al_draw_text(renderer->font, al_map_rgb(255, 255, 255), x + 50, y + 25, ALLEGRO_ALIGN_CENTER, txt_tipo);

    // Texto Custo (amarelo no canto)
    char txt_custo[10];
    sprintf(txt_custo, "%d", c.custo);
    al_draw_text(renderer->font, al_map_rgb(255, 255, 0), x + 5, y + 5, 0, txt_custo);

    // Texto Valor (grande no meio)
    char txt_valor[20];
    if (c.tipo == ataque) sprintf(txt_valor, "Dano: %d", c.efeito);
    else if (c.tipo == defesa) sprintf(txt_valor, "Escudo: %d", c.efeito);
    else sprintf(txt_valor, "%d", c.efeito);
    al_draw_text(renderer->font, al_map_rgb(255, 255, 255), x + 50, y + 70, ALLEGRO_ALIGN_CENTER, txt_valor);
  }
}

void RenderEnemies(Renderer* renderer) {

  // Para a posição não depender de números, mas da posição de certas coisas
  float start_x = 800;  
  float ey = ENEMY_BEGIN_Y; 
  float espacamento = 200;

  for (int i = 0; i < MAX_INIMIGOS; i++) {
    // Verifica se o inimigo[i] está vivo
    if (renderer->jogo->inimigos[i].entidade.vida_atual > 0) {
      Inimigo *ini = &renderer->jogo->inimigos[i];
          
      // Posição 
      float ex = start_x + (i * espacamento);                      // aumenta 200 a cada inimigo a mais

      // O "quadrado vermelho"
      al_draw_filled_rectangle(ex, ey, ex + 100, ey + 100, al_map_rgb(200, 0, 0));
      al_draw_rectangle(ex, ey, ex + 100, ey + 100, al_map_rgb(255, 255, 255), 2);

      // Barra de vida
      RenderHealthBar(ex, ey, ini->entidade.vida_atual, ini->entidade.vida_max, ini->entidade.escudo, renderer->font);

      // Intenção do Inimigo (o que ele vai fazer?)
      char texto_acao[64];
      Carta acao = ini->acoes[ini->indice_acao_atual];
          
      if (acao.tipo == ataque) {
        // Se for atacar, mostra o dano em Vermelho Claro
        sprintf(texto_acao, "ATACAR: %d", acao.efeito);
        DrawCenteredScaledText(renderer->font, al_map_rgb(255, 100, 100), ex + 50, ey - 30, 1.2, 1.2, texto_acao);
      } else if (acao.tipo == defesa) {
        // Se for defender, mostra o escudo em Azul Claro
        sprintf(texto_acao, "DEFENDER: %d", acao.efeito);
        DrawCenteredScaledText(renderer->font, al_map_rgb(100, 100, 255), ex + 50, ey - 30, 1.2, 1.2, texto_acao);
      } 
    }
  }
}

void RenderPlayer(Renderer * renderer){
  float px = PLAYER_BEGIN_X;
  float py = PLAYER_BEGIN_Y;
    
  al_draw_filled_rectangle(px, py, px + 100, py + 100, al_map_rgb(0, 200, 0));

  // Desenha a barra da vida
  RenderHealthBar(px, py, renderer->jogo->jogador.entidade.vida_atual, 
                  renderer->jogo->jogador.entidade.vida_max, 
                  renderer->jogo->jogador.entidade.escudo,
                  renderer->font);
    
  // Texto da Energia
  char texto_energia[64];
  sprintf(texto_energia, "Energia: %d/3", renderer->jogo->jogador.energia);
  al_draw_text(renderer->font, al_map_rgb(255, 255, 0), px, py - 20, 0, texto_energia);
}

void RenderEnergy(Renderer* renderer) {}

void Render(Renderer* renderer) {
  // 1. Prepara o buffer para desenhar
  al_set_target_bitmap(renderer->display_buffer);

  // SEGURANÇA: Se o jogo não foi conectado no main, desenha só o fundo e sai
  if (renderer->jogo == NULL) {
    // Copia o buffer para a tela e sai
    al_set_target_backbuffer(renderer->display);
    al_draw_scaled_bitmap(renderer->display_buffer, 0, 0, DISPLAY_BUFFER_WIDTH,
                        DISPLAY_BUFFER_HEIGHT, 0, 0, DISPLAY_WIDTH,
                        DISPLAY_HEIGHT, 0);
    al_flip_display();
    return;
  }

  // Limpa a tela pintando de cinza
  RenderBackground(renderer);

  // 2. DESENHAR JOGADOR (Quadrado VERDE)
  RenderPlayer(renderer);            // Desenha o jogador

  // 3. DESENHAR INIMIGO (Quadrado VERMELHO)
  RenderEnemies(renderer);           // Desenha o inimigo

  // 4. DESENHAR A MÃO DO JOGADOR
  RenderPlayerHand(renderer);        // Desenha as cartas                 

  // 5. FINALIZAÇÃO DO BUFFER 
  // Troca o alvo de volta para a tela real
  al_set_target_backbuffer(renderer->display);

  // Desenha o buffer esticado na tela inteira
  al_draw_scaled_bitmap(renderer->display_buffer, 0, 0, DISPLAY_BUFFER_WIDTH,
                      DISPLAY_BUFFER_HEIGHT, 0, 0, DISPLAY_WIDTH,
                      DISPLAY_HEIGHT, 0);

  // Mostra tudo
  al_flip_display();
}