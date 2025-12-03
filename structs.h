#ifndef STRUCTS_H
#define STRUCTS_H

typedef enum {ataque, defesa, especial} tipoCarta;   // 3 tipos de carta


// 1) ESTRUTURA DA CARTA 
typedef struct Carta{
    tipoCarta tipo;             // Ataque, Defesa ou Especial
    int custo;                  // Custo de energia (entre 0 e 3)
    int efeito;                 // Pontos atribuidos ao dano causado (ataque) ou ao escudo (defesa)
    int id;                     // Para identificar as cartas melhor
} Carta;

// 2) ESTRUTURA DA ENTIDADE
typedef struct Entidade{        // Mais fácil de entender, pode ser o jogador ou o inimigo
    int vida_atual;             // Vida a qual é inicializada na entidade
    int vida_max;               // Limite até onde a entidade pode se curar
    int escudo;                 // Recurso a mais de proteção da entidade
} Entidade;

// 2.1) ESTRUTURA DO JOGADOR
typedef struct Jogador{
    Entidade entidade;          // Inlcui a vida e o escudo
    int energia;                // 3 no início de cada turno
    Carta baralho[50];
    int qtd_baralho;
    Carta mao[10];
    int qtd_mao;
    Carta descarte[50];
    int qtd_descarte;
} Jogador;

//2.2) ESTRUTURA DO INIMIGO
typedef struct Inimigo{
    Entidade entidade;
    int tipo;                   // Fraco = 0 OU Forte = 1
    //IA do inimigo
    Carta acoes[5];             // 5 --> número de ações máximo "seguro"
    int qtd_acoes;
    int indice_acao_atual;      // Qual ação será usada nesse turno
} Inimigo;

// 3) ESTRUTURA DO COMBATE DO JOGO
typedef struct EstadoJogo{      //IMPORTANTÍSSIMA! Para salvar os dados do jogo em determinado estado/momento
    Jogador jogador;            // 1 jogador (seu personagem) por combate
    Inimigo inimigos[2];        // Máximo de 2 inimigos por combate / 5% de chance de ser forte e max 1 inimigo forte
    int qtd_inimigos_vivos;     // Se for 1, o programa lê  somente inimigos[0] / Se for 2, serão lidos inimigos[0] e [1]
    int turno_atual;            // Vez de quem jogar: 0 = vez do jogador / 1 = vez do inimigo
    int combates_vencidos;      // Quando for = 10, a tela de vitória será exibida (começa em 0)
    int carta_selecionada;      // Índice da carta que a setinha está apontando na hora de escolher qual será usada (0 a qtd_mao - 1)
    // int selecionando_carta;     // 0 = Escolhendo carta, 1 = Escolhendo alvo (inimigo)
    // int alvo_selecionado;       // Qual inimigo a mira está apontando (0 ou 1)
} EstadoJogo;

#endif