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

void RenderBackground(Renderer* renderer) {
  al_clear_to_color(al_map_rgb(0, 0, 0));
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

void RenderHealthBar(float x_begin, float x_end, float y_down_left,
                     ALLEGRO_FONT* font) {
  float mid_y = y_down_left - (HEALTH_BAR_HEIGHT * 0.78);

  al_draw_filled_rounded_rectangle(
      x_begin - HEALTH_BAR_BACKGROUND_EXTRA,
      y_down_left - HEALTH_BAR_BACKGROUND_EXTRA,
      x_end + HEALTH_BAR_BACKGROUND_EXTRA,
      y_down_left - HEALTH_BAR_HEIGHT + HEALTH_BAR_BACKGROUND_EXTRA,
      HEALTH_BAR_RX, HEALTH_BAR_RY, al_map_rgb(255, 255, 255));
  char text[100] = "";
  sprintf(text, "Vida?");
  float x_scale = 2.0, y_scale = 2.0;
  DrawScaledText(font, al_map_rgb(0, 0, 0), (x_begin + x_end) / 2.0 / x_scale,
                 mid_y / y_scale, x_scale, y_scale, ALLEGRO_ALIGN_CENTRE, text);
}

void RenderCreature(const Renderer* renderer, int begin_x, int mid_y,
                    int width) {
  al_draw_filled_circle(begin_x + width / 2.0, mid_y, width,
                        al_map_rgb(255, 255, 255));
  float x_end = begin_x + width;

  float health_bar_y = mid_y + width + 20;
  RenderHealthBar(begin_x, x_end, health_bar_y, renderer->font);

  // Suggestion: render enemy intents here.
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
  RenderCard(renderer, HAND_BEGIN_X, HAND_BEGIN_Y);
}

void RenderEnemies(Renderer* renderer) {}

void RenderEnergy(Renderer* renderer) {}

void Render(Renderer* renderer) {
  // 1. Prepara o buffer para desenhar
  al_set_target_bitmap(renderer->display_buffer);
    
  // Pinta o fundo de cinza escuro
  al_clear_to_color(al_map_rgb(30, 30, 30));

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

  // 2. DESENHAR JOGADOR (Quadrado VERDE)
  float px = PLAYER_BEGIN_X;
  float py = PLAYER_BEGIN_Y;
    
  al_draw_filled_rectangle(px, py, px + 100, py + 100, al_map_rgb(0, 200, 0));
    
  // Texto da Vida do Jogador
  char texto_vida[64];
  sprintf(texto_vida, "HP: %d/%d [Escudo: %d]", 
          renderer->jogo->jogador.entidade.vida_atual,       // Representa a vida atual na fração / VIDA_MAX
          renderer->jogo->jogador.entidade.vida_max,         // Representa a vida máxima como denominador da fração da vida
          renderer->jogo->jogador.entidade.escudo);          // Representa o escudo do player ao lado do hp
  al_draw_text(renderer->font, al_map_rgb(255, 255, 255), px, py - 20, 0, texto_vida);
    
  // Texto da Energia
  char texto_energia[64];
  sprintf(texto_energia, "Energia: %d/3", renderer->jogo->jogador.energia);
  al_draw_text(renderer->font, al_map_rgb(255, 255, 0), px, py + 110, 0, texto_energia);


  // 3. DESENHAR INIMIGO (Quadrado VERMELHO)
  // Verifica se tem inimigo vivo
  if (renderer->jogo->qtd_inimigos_vivos > 0) {
    Inimigo *ini = &renderer->jogo->inimigos[0];
        
    // Posição (Defina ENEMY_BEGIN_X no constants.h ou use 900 fixo aqui)
    float ex = 900; 
    float ey = PLAYER_BEGIN_Y; 

    // O "quadrado vermelho"
    al_draw_filled_rectangle(ex, ey, ex + 100, ey + 100, al_map_rgb(200, 0, 0));

    // Vida do Inimigo
    char texto_vida[64];
    if (ini->entidade.escudo > 0) {
      sprintf(texto_vida, "HP: %d/%d [Escudo: %d]", 
              ini->entidade.vida_atual,                      // Representa a vida atual na fração / VIDA_MAX
              ini->entidade.vida_max,                        // Representa a vida máxima como denominador da fração da vida
              ini->entidade.escudo);                         // Representa o escudo do inimigo ao lado do hp

    } else {                                                 // Caso o inimigo NÃO TENHA ESCUDO, só aparece o HP
      sprintf(texto_vida, "HP: %d/%d", 
              ini->entidade.vida_atual, 
              ini->entidade.vida_max);
    }
    al_draw_text(renderer->font, al_map_rgb(255, 255, 255), ex, ey - 20, 0, texto_vida);

    // Intenção do Inimigo (o que ele vai fazer?)
    char texto_acao[64];
    Carta acao = ini->acoes[ini->indice_acao_atual];
        
    if (acao.tipo == ataque) {
      // Se for atacar, mostra o dano em Vermelho Claro
      sprintf(texto_acao, "!!! VAI ATACAR: %d !!!", acao.efeito);
      al_draw_text(renderer->font, al_map_rgb(255, 100, 100), ex, ey - 40, 0, texto_acao);
    } else if (acao.tipo == defesa) {
      // Se for defender, mostra o escudo em Azul Claro
      sprintf(texto_acao, "(Vai Defender: %d)", acao.efeito);
      al_draw_text(renderer->font, al_map_rgb(100, 100, 255), ex, ey - 40, 0, texto_acao);
    } else {
      // Caso tenha algum outro tipo
      sprintf(texto_acao, "(Preparando...)");
      al_draw_text(renderer->font, al_map_rgb(200, 200, 200), ex, ey - 40, 0, texto_acao);
    }
  }

  // 4. DESENHAR A MÃO DO JOGADOR
  float start_x = HAND_BEGIN_X; 
  float start_y = HAND_BEGIN_Y;
  float espacamento = 130; 

  for (int i = 0; i < renderer->jogo->jogador.qtd_mao; i++) {
      Carta c = renderer->jogo->jogador.mao[i];
        
      float x = start_x + (i * espacamento);
      float y = start_y;

      // Cor baseada no tipo
      ALLEGRO_COLOR cor;
      if (c.tipo == ataque) cor = al_map_rgb(200, 50, 50);      // VERMELHO
      else if (c.tipo == defesa) cor = al_map_rgb(50, 50, 200); // AZUL
      else cor = al_map_rgb(100, 100, 100);                     // CINZA

      // Desenha Carta
      al_draw_filled_rectangle(x, y, x + 100, y + 150, cor);
      al_draw_rectangle(x, y, x + 100, y + 150, al_map_rgb(255, 255, 255), 2);

      // Texto Custo
      char txt_custo[10];
      sprintf(txt_custo, "%d", c.custo);
      al_draw_text(renderer->font, al_map_rgb(255, 255, 0), x + 5, y + 5, 0, txt_custo);

      // Texto Valor
      char txt_valor[10];
      sprintf(txt_valor, "%d", c.efeito);
      al_draw_text(renderer->font, al_map_rgb(255, 255, 255), x + 50, y + 70, ALLEGRO_ALIGN_CENTER, txt_valor);

      // Tecla de Atalho
      char txt_tecla[10];
      sprintf(txt_tecla, "[%d]", i + 1);
      al_draw_text(renderer->font, al_map_rgb(200, 200, 200), x + 50, y + 130, ALLEGRO_ALIGN_CENTER, txt_tecla);
  }                       

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

void ClearRenderer(Renderer* renderer) {
  al_destroy_display(renderer->display);
  al_destroy_bitmap(renderer->display_buffer);
  al_destroy_font(renderer->font);
}