#include "logica.h"
#include "constants.h"
#include <stdio.h>
#include <stdlib.h> 

// FUNÇÕES AUXILIARES

// FUNÇÃO PARA EMBARALHAR (Mistura o vetor)
void embaralhar(Carta *vetor, int tamanho) {
    for (int i = tamanho - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        Carta temp = vetor[i];
        vetor[i] = vetor[j];
        vetor[j] = temp;
    }
}

//FUNÇÃO CRIAR CARTA: função auxiliar que volta nas variáveis da estrutura carta para criá-la no main
Carta criar_carta(tipoCarta tipo, int custo, int efeito) {
    Carta c;
    c.tipo = tipo;
    c.custo = custo;
    c.efeito = efeito;
    c.id = 0; 
    return c;
}

// FUNÇÕES PRINCIPAIS

// FUNÇÃO DE COMPRAR (Move do Baralho -> Mão)
void comprar_cartas(Jogador *j, int quantidade) {
  for (int k = 0; k < quantidade; k++) {
    if (j->qtd_mao >= MAX_CARTAS_MAO) {                  // Mão cheia --> mais do que o máximo permitido(10)
      printf("Mão cheia! Não pode comprar mais.\n");
      return;
    }

    if (j->qtd_baralho == 0) {                           // Se o baralho acabou...
      if (j->qtd_descarte == 0) return;                  // ...e o descarte também, não tem o que fazer (RETORNA).
            
      // Senão, recicla o descarte:
      printf("Embaralhando descarte...\n");
      for (int i = 0; i < j->qtd_descarte; i++) {
        j->baralho[i] = j->descarte[i];                  // Move de volta pro baralho
      }
      j->qtd_baralho = j->qtd_descarte;                  // Atualiza quantidade
      j->qtd_descarte = 0;                               // Zera descarte
            
      embaralhar(j->baralho, j->qtd_baralho);            // Mistura tudo
    }

    // A Mágica do Saque:
    j->qtd_baralho--;                                    // Diminui o topo da pilha
    Carta carta_comprada = j->baralho[j->qtd_baralho];   // Pega a carta

    j->mao[j->qtd_mao] = carta_comprada;                 // Põe na mão
    j->qtd_mao++;                                        // Aumenta a mão
  }
}

// FUNÇÃO DE DESCARTAR
void descartar_carta(Jogador *j, int indice_na_mao) {
    j->descarte[j->qtd_descarte] = j->mao[indice_na_mao];
    j->qtd_descarte++;

    for (int i = indice_na_mao; i < j->qtd_mao - 1; i++) {
        j->mao[i] = j->mao[i + 1];
    }
    j->qtd_mao--;
}

// FUNÇÃO DE JOGAR CARTA DA MÃO     
void jogar_carta(EstadoJogo *jogo, int indice_mao) {
    if (indice_mao >= jogo->jogador.qtd_mao) return;
    
    Carta c = jogo->jogador.mao[indice_mao];

    // Verifica Energia
    if (jogo->jogador.energia < c.custo) {
        printf("Sem energia!\n");
        return;
    }

    // Aplica Efeito
    if (c.tipo == ataque) {
        if (jogo->qtd_inimigos_vivos > 0) {
            int dano = c.efeito;
            jogo->inimigos[0].entidade.vida_atual -= dano;
            if (jogo->inimigos[0].entidade.vida_atual < 0) 
                jogo->inimigos[0].entidade.vida_atual = 0;
            
            // Verifica morte do inimigo aqui mesmo, ou numa função separada depois
            if (jogo->inimigos[0].entidade.vida_atual == 0) {
                 // jogo->qtd_inimigos_vivos--; // Lógica de morte simples
            }
        }
    } 
    else if (c.tipo == defesa) {
        jogo->jogador.entidade.escudo += c.efeito;
    }

    jogo->jogador.energia -= c.custo;
    descartar_carta(&jogo->jogador, indice_mao);
}

// FUNÇÃO DE COMEÇAR O JOGO
void inicializar_jogo(EstadoJogo *jogo){

  // 1. Configurar Jogador (inicializar os dados do jogador)
  jogo->jogador.entidade.vida_max = 100;
  jogo->jogador.entidade.vida_atual = 100;
  jogo->jogador.entidade.escudo = 0;
  jogo->jogador.energia = 3; 

  // 2. Criar Baralho (5 Ataques, 5 Defesas)
  jogo->jogador.qtd_baralho = 0;

  // Ataques (Custo 1, Dano 6)
  for(int i = 0; i < 5; i++) {
    jogo->jogador.baralho[i] = criar_carta(ataque, 1, 6);
    jogo->jogador.qtd_baralho++;
  }
  // Defesas (Custo 1, Escudo 5) 
  for(int i = 5; i < 10; i++) {
    jogo->jogador.baralho[i] = criar_carta(defesa, 1, 5);
    jogo->jogador.qtd_baralho++;
  }

  jogo->jogador.qtd_mao = 0;
  jogo->jogador.qtd_descarte = 0;

  // 3. Configurar o PRIMEIRO inimigo

  jogo->qtd_inimigos_vivos = 1;                  // Garante que o inimigo existe
  // Status do inimigo FRACO
  jogo->inimigos[0].entidade.vida_max = 20;      
  jogo->inimigos[0].entidade.vida_atual = 20;
  jogo->inimigos[0].entidade.escudo = 0;
  jogo->inimigos[0].tipo = 0;

  // IA do inimigo: ciclo de ações dele
  jogo->inimigos[0].acoes[0] = criar_carta(ataque, 0, 5);      // Ação 0: ataque fraco --> dano 5
  jogo->inimigos[0].acoes[1] = criar_carta(defesa, 0, 5);      // Ação 1: defesa --> escudo 5

  jogo->inimigos[0].qtd_acoes = 2; // O ciclo dele tem 2 passos
  jogo->inimigos[0].indice_acao_atual = 0; // Começa pelo passo 0

  // 4. Estado Geral
  jogo->combates_vencidos = 0;
  jogo->turno_atual = 0; // começa no turno do do jogador

  // 5. Chamada da função EMBARALHAR: mistura o deck recém criado
  embaralhar(jogo->jogador.baralho, jogo->jogador.qtd_baralho);

  // 6. Chamada da função COMPRA_CARTAS: compra as 5 cartas iniciais
  comprar_cartas(&jogo->jogador, 5);

  printf("Inicializacao completa! Cartas na mao: %d\n", jogo->jogador.qtd_mao);

}

void passar_turno(EstadoJogo *jogo){

    // 1. FIM DO 1o TURNO DO JOGADOR --> descartar a mão atual

    while (jogo->jogador.qtd_mao > 0) {
        descartar_carta(&jogo->jogador, 0); // Descarta sempre a primeira até esvaziar
    }

    // 2. TURNO DO INIMIGO

    // Processamento de todos os inimigos vivos
    if (jogo->qtd_inimigos_vivos > 0) {
        Inimigo *ini = &jogo->inimigos[0];                          // Focando no primeiro inimigo por enquanto

        // a) Zera o escudo do inimigo 
        ini->entidade.escudo = 0;

        // b) Executa a ação prevista
        Carta acao = ini->acoes[ini->indice_acao_atual];
        
        if (acao.tipo == ataque) {
            // O inimigo ataca o JOGADOR
            int dano = acao.efeito;
            
            // Lógica de Escudo do Jogador
            int dano_no_escudo = dano;
            if (dano_no_escudo > jogo->jogador.entidade.escudo) {
                dano_no_escudo = jogo->jogador.entidade.escudo;
            }
            
            jogo->jogador.entidade.escudo -= dano_no_escudo;        // Reduz escudo
            int dano_na_vida = dano - dano_no_escudo;               // O que sobrou vai na vida
            
            jogo->jogador.entidade.vida_atual -= dano_na_vida;
            
            // Segurança para vida não ficar negativa
            if (jogo->jogador.entidade.vida_atual < 0) jogo->jogador.entidade.vida_atual = 0;
            
            printf("Inimigo atacou! Dano: %d (Vida Restante: %d)\n", dano, jogo->jogador.entidade.vida_atual);
        } 
        else if (acao.tipo == defesa) {
            // Inimigo ganha escudo
            ini->entidade.escudo += acao.efeito;
            printf("Inimigo se defendeu! Escudo: %d\n", ini->entidade.escudo);
        }

        // c) Prepara a PRÓXIMA ação (ciclo)
        ini->indice_acao_atual++;
        // Se chegou no fim do vetor de ações, volta para o 0
        if (ini->indice_acao_atual >= ini->qtd_acoes) {
            ini->indice_acao_atual = 0;
        }
    }

    // 3. NOVO TURNO DO JOGADOR

    // a) Zera escudo do jogador
    jogo->jogador.entidade.escudo = 0;

    // b) Restaura Energia
    jogo->jogador.energia = 3;

    // c) Compra nova mão
    comprar_cartas(&jogo->jogador, 5);
    
    printf("NOVO TURNO INICIADO\n");
}
