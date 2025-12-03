#ifndef _RENDERER_H_
#define _RENDERER_H_

#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_image.h>

#include "structs.h"

typedef struct {
  ALLEGRO_DISPLAY* display;
  ALLEGRO_BITMAP* display_buffer;

  ALLEGRO_FONT* font;

  EstadoJogo *jogo;               // Pra ficar igual do .c --> dar certo na compilação

} Renderer;

void FillRenderer(Renderer* renderer);
void Render(Renderer* renderer);
void ClearRenderer(Renderer* renderer);

#endif