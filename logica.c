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

            //Segurança pra não ficar negativo
            if (jogo->inimigos[0].entidade.vida_atual < 0) 
                jogo->inimigos[0].entidade.vida_atual = 0;
            
            // DETECTAR VITÓRIA --> verifica morte do inimigo 
            if (jogo->inimigos[0].entidade.vida_atual == 0) {
                printf("Inimigo Derrotado!\n");
                jogo->qtd_inimigos_vivos--;

                // Se todos morreram (nesse caso só tem 1 por enquanto)
                if (jogo->qtd_inimigos_vivos == 0) {
                    printf(">>> VITORIA NO COMBATE! <<<\n");
                    avancar_nivel(jogo);                              // Chama a função nova
                    return;                                           // Sai da função pra não dar erro
                }
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

  // 3. Configurar os Inimigos

  jogo->qtd_inimigos_vivos = 2;                                // Garante que 2 inimigos existam para o primeiro combate

  //INIMIGO 1 (FRACO ou FORTE)
  gerar_inimigos(&jogo->inimigos[0]);

  //INIMIGO 2 (Se o 1o for FORTE, o 2o é FRACO. Caso contrário, ele pode ser FRACO ou FORTE)
  gerar_inimigos(&jogo->inimigos[1]);

  // Segurança --> por azar os dois forem fortes (tipo = 1)
  if (jogo->inimigos[0].tipo == 1 && jogo->inimigos[1].tipo == 1) {
      jogo->inimigos[1].tipo = 0;
      jogo->inimigos[1].entidade.vida_max = 30;                // Valor seguro de fraco
      jogo->inimigos[1].entidade.vida_atual = 30;
  }

  // 4. Estado Geral
  jogo->combates_vencidos = 0;
  jogo->turno_atual = 0; // começa no turno do do jogador

  // 5. Chamada da função EMBARALHAR: mistura o deck recém criado
  embaralhar(jogo->jogador.baralho, jogo->jogador.qtd_baralho);

  // 6. Chamada da função COMPRA_CARTAS: compra as 5 cartas iniciais
  comprar_cartas(&jogo->jogador, 5);

  jogo->carta_selecionada = 0;                                 // Começa selecionando a primeira carta

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
            
            jogo->jogador.entidade.vida_atual -= dano_na_vida;      // Vida atual recebe o dano
            
            // Segurança para vida não ficar negativa
            if (jogo->jogador.entidade.vida_atual < 0) jogo->jogador.entidade.vida_atual = 0;

            // DETECTAR GAME OVER
            if (jogo->jogador.entidade.vida_atual == 0) {
                printf("\nGAME OVER\nVoce morreu!\nReiniciando o jogo...\n");
                inicializar_jogo(jogo);                             // Reseta tudo do zero
                return;                                             // Para tudo
            }
            
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

void avancar_nivel(EstadoJogo *jogo){
    jogo->combates_vencidos++;                                    // Atualiza o n de combates vencidos após o avanço de nível
    printf("PREPARANDO NIVEL %d\n", jogo->combates_vencidos + 1); // Printa o n do nível atual

    // 1. Cura um pouco o jogador (Recompensa de 10 de vida) --> voltar com a vida máxima(para iniciar o novo combate)
    jogo->jogador.entidade.vida_atual += 10;
    if (jogo->jogador.entidade.vida_atual > jogo->jogador.entidade.vida_max)
        jogo->jogador.entidade.vida_atual = jogo->jogador.entidade.vida_max;

    // 2. Reseta energia e zera escudo do jogador
    jogo->jogador.energia = 3;
    jogo->jogador.entidade.escudo = 0;
    
    // 3. Limpa a mão atual para comprar novas cartas limpas
    while (jogo->jogador.qtd_mao > 0) {
        descartar_carta(&jogo->jogador, 0);
    }
    // Compra 5 cartas novas para começar o turno bombando
    comprar_cartas(&jogo->jogador, 5);

    // 4. Cria Novo Inimigo (Escalando Dificuldade)
    jogo->qtd_inimigos_vivos = 2;
    
    gerar_inimigos(&jogo->inimigos[0]);
    gerar_inimigos(&jogo->inimigos[1]);
    
    // A mesma verificação de segurança (pra não ter 2 FORTES)
    if (jogo->inimigos[0].tipo == 1 && jogo->inimigos[1].tipo == 1) {
         jogo->inimigos[1].tipo = 0;
         jogo->inimigos[1].entidade.vida_max = 30;
         jogo->inimigos[1].entidade.vida_atual = 30;
    }

    printf("Novos inimigos surgiram!\n");
}

// Função auxiliar para criar cartas de ação aleatórias
Carta criar_acao_inimigo(int custo_max) {
    
    // 1. Sorteia Tipo (50% Ataque / 50% Defesa)
    tipoCarta tipo = (rand() % 2 == 0) ? ataque : defesa; 
    
    // 2. Sorteia Custo (0 até o máximo permitido pela força do inimigo)
    int custo = rand() % (custo_max + 1); 
    
    // 3. Calcula o Efeito 
    int efeito = 0;
    
    switch (custo) {
        case 0:
            // Custo 0 --> efeito entre 1 e 5
            efeito = 1 + (rand() % 5); 
            break;
        case 1:
            // Custo 1 --> efeito entre 5 e 10
            efeito = 5 + (rand() % 6); 
            break;
        case 2:
            // Custo 2 --> efeito entre 10 e 15
            efeito = 10 + (rand() % 6); 
            break;
        case 3:
            // Custo 3 --> efeito entre 15 e 30
            efeito = 15 + (rand() % 16); 
            break;
        default:
            efeito = 1; // Segurança
            break;
    }

    return criar_carta(tipo, custo, efeito);
}

void gerar_inimigos(Inimigo *ini) {

    // 1. Sorteio do Tipo: 5% de chance de ser FORTE
    int sorteio = rand() % 100;                          // Se nesse sorteio der um número menor que 5, o inimigo é FORTE 
    
    if (sorteio < 5) {
        // INIMIGO FORTE
        ini->tipo = 1;                                   // 1 = Forte
        
        ini->entidade.vida_max = 40 + (rand() % 61);     // Vida entre 40 e 100
    
        ini->qtd_acoes = 2 + (rand() % 2);               // IA: 2 ou 3 ações
    } else {
        // INIMIGO FRACO
        ini->tipo = 0;                                   // 0 = Fraco
        
        ini->entidade.vida_max = 10 + (rand() % 21);     // Vida: Entre 10 e 30
    
        ini->qtd_acoes = 1 + (rand() % 2);               // IA: 1 ou 2 ações
    }

    // Configurações Comuns
    ini->entidade.vida_atual = ini->entidade.vida_max;
    ini->entidade.escudo = 0;
    ini->indice_acao_atual = 0;

    // Gerar as ações da IA
    for (int i = 0; i < ini->qtd_acoes; i++) {
        // Forte --> pode ter cartas de custo maior (até 3)
        // Fraco --> só custo baixo (até 1)
        int forca_max = (ini->tipo == 1) ? 3 : 1;
        ini->acoes[i] = criar_acao_inimigo(forca_max);
    }
}
