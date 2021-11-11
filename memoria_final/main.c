/* Daniel Guimaraes - 1910462 */

# include <stdio.h>
# include <stdlib.h>
# include <math.h>
# include <string.h>

# define TRUE 1
# define FALSE 0
# define bool unsigned char

/* algoritmo */
# define ALG_FIFO 0
# define ALG_NRU 1
# define ALG_LFU 2

/* flags para o algoritmo */
# define FLAG_REF 1
# define FLAG_MOD 2
# define FLAG_NULL 0

/* flags de erro */
# define ERROR_INVALID_CALL -1
# define ERROR_UNDEFINED_PG_MM -2
# define ERROR_NO_MEMORY -3
# define ERROR_EMPTY_QUEUE -4
# define ERROR_FULL_QUEUE -5
# define ERROR_NO_FILE -6

/* definicao de variaveis */
# define alg_t unsigned int
# define pgsize_t unsigned int
# define mmsize_t unsigned int
# define flagpag_t unsigned short
# define tempo_t unsigned long
# define pagid_t unsigned int
# define endereco_t unsigned int
# define quantidade_t unsigned int

/* definicao de elemento e lista, para uma lista encadeada */
typedef struct elemento {
    struct elemento *prev;         // elemento anterior na lista encadeada
    struct elemento *next;         // elemento sucessor na lista encadeada
    pagid_t x;                      // conteudo
} Elemento;

typedef struct lista {
    Elemento *inicio;
    int quantidade;
} Lista;

void listaPop(Elemento *elemento);
void listaPush(Lista *lista, pagid_t x);
void listaRemove(Lista *lista);
Lista *listaCriar();

/* definicao de uma pagina para uma tabela */
typedef struct pagina {
    pagid_t meuid;          // o id da propria pagina
    tempo_t ultimaref;      // tempo da ultima referencia
    bool presente;          // se esta na memoria
    bool modificada;        // TRUE se foi modificada
    flagpag_t flagpag;      // FLAG_NULL, FLAG_REF OU FLAG_MOD
    quantidade_t quantidadeUsadas;      // quantidade de vez que foi executada
} Pagina;

/* definicao de fila, para o algoritmo FIFO*/
typedef struct fila {
    Pagina **lista;
    long ini;
    long n;
    long max;
} Fila;

void filaRemove(Fila *fila);
Fila *filaCria(long size);
bool filaVazia(Fila *f);
bool filaCheia(Fila *f);
Pagina *filaPop(Fila *f);
void filaPush(Fila *fila, Pagina *pag);


/* definicao da memoria fisica do computador. Explicado mais abaixo */
typedef struct memoria {
    long paginasMax;
    long paginasAtual;
    Lista* paginas;
} Memoria;

/* definicao do resultado contendo os dados do simulador */
typedef struct resultado {
    long pageFaults;
    long pageWrites;
} Resultado;


alg_t alg;                      // algoritmo de escolha
pgsize_t pgsize;                // tamanho da pagina
mmsize_t mmsize;                // tamanho da memoria
char *filename;                 // nome do arquivo de log
endereco_t addrDeslocamento;    // bits de deslocamento
tempo_t time;                   // tempo do processador (simulado)
Pagina *tabela;                 // tabela de paginas
Resultado resultado;            // resultado da simulacao

/*
    Para a implementacao do algoritmo ALG_FIFO segunda chance,
    alem da tabela sera necessario uma fila com as paginas da memoria
*/
Fila *filaFIFO;             // fila para o ALG_FIFO

/*
    Para a implementacao do algoritmo ALG_NRU E ALG_LFU/LRU, 
    alem da tabela sera necessario uma lista contendo as paginas na memoria.
    Essa lista esta dentro da estrutura memoria, que so eh alocada se precisar se usada
*/
Memoria memoria;                

/* atualiza o resultado de acordo com a pagina saindo da memoria */
void contabilizaResultado(Pagina pagSaida) {
    resultado.pageFaults++;
    if (pagSaida.modificada == TRUE) {
        resultado.pageWrites++;
    }

    // reseta a pagina
    tabela[pagSaida.meuid].presente = FALSE;
    tabela[pagSaida.meuid].modificada = FALSE;

    return;
}

/*
    Retorna o numero da pagina a partir de um endereco
*/
pagid_t getNumeroPagina(endereco_t endereco) {
    return endereco >> addrDeslocamento;
}

/*
    ALGORITMO FIFO SEGUNDA CHANCE
    Substitui e coloca a pagina recebida no lugar de outra
    atraves do algoritmo FIFO de segunda chance
*/
void algoritmoFIFO(Pagina *pagNova) {
    Pagina *pag;
    while(TRUE) {
        pag = filaPop(filaFIFO);            // pega a primeira da fila
        if (pag->flagpag & FLAG_REF) {      // se ela foi referenciada
            pag->flagpag = FLAG_NULL;       // apaga-se a flag
            filaPush(filaFIFO, pag);        // coloca no final da fila
        } else {
            filaPush(filaFIFO, pagNova);    // efetua a substituicao
            contabilizaResultado(*pag);
            break;
        }
    }
}

/*
    ALGORITMO NRU (NAO RECENTEMENTE UTILIZADO)
    Retira uma pagina pelo algortimo NRU e coloca no lugar
    uma nova pagina.
*/
void algoritmoNRU(Pagina *pagNova) {
    // ordem de prioridade:
    //      se encontrou uma FLAG_NULL, ja retorna
    //      se encontrou uma FLAG_MOD mas nao FLAG_REF, guarda pois se nao tiver nenhuma acima, retorna essa
    //      se encontrou uma FLAG_REF mas nao FLAG_MOD, guarda pois se nao tiver nenhuma acima, retorna essa
    //      se encontrou uma FLAG_REF    e    FLAG_MOD, guarda pois se nao tiver nenhuma acima, retorna essa
    pagid_t temp;

    Elemento *pagMod = NULL;
    Elemento *pagRef = NULL;
    Elemento *pagModRef = NULL;
    Elemento *pagFinal = NULL;

    Elemento *e = memoria.paginas->inicio;
    Pagina p;

    while (e != NULL) {    // andando em toda a lista
        temp = e->x;        // id da pagina
        p = tabela[temp];   // pagina
        if (p.flagpag == FLAG_NULL) {
            pagFinal = e;
            break;
        } else if (p.flagpag & FLAG_MOD) {
            if (p.flagpag & FLAG_REF) {
                pagModRef = e;
            } else {
                pagMod = e;
            }
        } else {
            pagRef = e;
        }
        e = e->next;
    }

    if (pagFinal != NULL) {
        contabilizaResultado(tabela[pagFinal->x]);
        listaPop(pagFinal);
    } else if (pagMod != NULL) {
        contabilizaResultado(tabela[pagMod->x]);
        listaPop(pagMod);
    } else if (pagRef != NULL) {
        contabilizaResultado(tabela[pagRef->x]);
        listaPop(pagRef);
    } else {
        contabilizaResultado(tabela[pagModRef->x]);
        listaPop(pagModRef);
    }
    listaPush(memoria.paginas, pagNova->meuid);
    return;
}

/*
    ALGORITMO LFU (MENOS FREQUENTEMENTE UTILIZADO)
    Retorna uma pagina pelo algoritmo LFU
*/
void algoritmoLFU(Pagina *pagNova) {
    // basta encontrar o menor numero na lista
    Elemento *e = memoria.paginas->inicio;
    Elemento *eFinal = NULL;
    Pagina pFinal;
    pagid_t temp;
    Pagina p;
    while (e != NULL) {
        p = tabela[e->x];
        if (eFinal == NULL || p.quantidadeUsadas <= pFinal.quantidadeUsadas) {
            pFinal = p;
            eFinal = e;
        }
        e = e->next;
    }

    listaPop(eFinal);
    listaPush(memoria.paginas, pagNova->meuid);
    contabilizaResultado(pFinal);
    return;
}

/*
    Caso LFU: zera a quantidade para todos as paginas na fila
    Caso NRU: reinicia as flags de R e M
*/
void interrupcaoRelogio() {
    if (alg == ALG_FIFO) {
        return; // FIFO nao muda nada
    }

    Elemento *e = memoria.paginas->inicio;
    pagid_t p;
    while (e != NULL) {
        p = e->x;
        if (alg == ALG_LFU) {
            tabela[p].quantidadeUsadas = 0;
        } else {
            tabela[p].flagpag = FLAG_NULL;
        }
        e = e->next;
    }
    return;
}

void adicionaMemoria(Pagina *pag) {
    switch (alg)
    {
    case ALG_FIFO: // adiciona na fila somente
        filaPush(filaFIFO, pag);
        break;
    
    case ALG_LFU: // adiciona na memoria
        listaPush(memoria.paginas, pag->meuid);
        break;

    case ALG_NRU:
        listaPush(memoria.paginas, pag->meuid);
        break;

    default:
        break;
    }
    return;
}

/*
    Processa uma acesso a memoria de acordo com o acesso
*/
void processaAcesso(endereco_t endereco, char read_write) {
    pagid_t numPag = getNumeroPagina(endereco);
    
    // acessando a tabela
    Pagina *pag = &tabela[numPag];

    // garantindo que o id esta escrito na pagina (no inicio nao esta)
    pag->meuid = numPag;

    // verificando se ela esta na memoria fisica
    if (pag->presente == FALSE) {
        // verificando se tem espaco na memoria
        if (memoria.paginasAtual < memoria.paginasMax) {
            // adicinando na memoria
            pag->presente = TRUE;
            adicionaMemoria(pag);
            memoria.paginasAtual++;
        } else {
            // page-fault!
            if (alg == ALG_NRU) {
                algoritmoNRU(pag);
            } else if (alg == ALG_FIFO) {
                algoritmoFIFO(pag);
            } else {
                algoritmoLFU(pag);
            }
        }
    }
    // contando os acessos aquela pagina
    pag->quantidadeUsadas++;

    // setando read e write
    if (read_write == 'R') {
        pag->flagpag = pag->flagpag | FLAG_REF;
    } else {
        pag->modificada = TRUE;
        pag->flagpag = pag->flagpag | FLAG_MOD | FLAG_REF;
    }

    // setando o tempo da leitura
    pag->ultimaref = time;

    // aumenta o tempo do processamento
    time++;
    if (time % 10000 == 0) {
        interrupcaoRelogio();
    }
    return;
}

/*
    Executa a simulacao
*/
void loopProcessador() {
    // abrindo o arquivo
    FILE *arquivo = fopen(filename, "r");
    if (arquivo == NULL) {
        printf("Arquivo nao encontrado.\n");
        exit(ERROR_NO_MEMORY);
    }
    // iniciando o loop
    endereco_t addr;
    char rw;
    while (fscanf(arquivo, "%x %c ", &addr, &rw) > 0) {
        processaAcesso(addr, rw);
    }

    // mostrando quantas paginas estao na memoria fisica
    //printf("aa %lu\n", memoria.paginas->quantidade);

    // fechando tudo
    fclose(arquivo);
    free(tabela);
    if (alg == ALG_FIFO) {
        filaRemove(filaFIFO);
    } else {
        listaRemove(memoria.paginas);
    }
    return;
}

/* ==== FUNCOES DE CONFIG ==== */

/*
    CONFIG
    Determina o deslocalemento necessario para obter o numero da pagina
    a partir do endereco
*/
void setAddrDeslocamento() {
    addrDeslocamento = 10 + (int)log2(pgsize);
}

/*
    CONFIG
    Prepara as variaveis do resultado
*/
void prepararResultado() {
    resultado.pageFaults = 0;
    resultado.pageWrites = 0;
    return;
}

/*
    CONFIG
    Configura a memoria do simulador
*/
void prepararMemoria() {
    memoria.paginasAtual = 0;
    memoria.paginasMax = mmsize * 1000 / pgsize;
    if (alg != ALG_FIFO) {
        memoria.paginas = listaCriar();
    }
    return;
}

/*
    CONFIG
    Configura a fila do simulador, caso precise
*/
void prepararFila() {
    filaFIFO = filaCria(mmsize * 1000 / pgsize);
}

/*
    CONFIG
    Cria a tabela de paginas
*/
void alocarTabela() {
    long tam = (long) pow(2, 32 - addrDeslocamento);         // aloca o maximo
    tabela = (Pagina *)malloc(sizeof(Pagina) * tam);
    if (tabela == NULL) {
        printf("erro de memoria alocando a tabela\n");
        exit(ERROR_NO_MEMORY);
    }
    for (int i = 0; i < tam; i++) {
        tabela[i].presente = FALSE;
        tabela[i].modificada = FALSE;
        tabela[i].flagpag = FLAG_NULL;
    }
    return;
}

/*
    Configura todas as variaveis globais
    Roda todas as funcoes de CONFIG.
*/
void setConfig(char *argv[]) {
    /* argv -> sim-virtual ALG FILE PGSIZE(KB) MMSIZE(MB) */
    // definindo o arquivo
    filename = argv[2];

    // definindo o algoritmo
    if (strcmp(argv[1], "FIFO") == 0) {
        alg = ALG_FIFO;
    } else if (strcmp(argv[1], "NRU") == 0) {
        alg = ALG_NRU;
    } else {
        alg = ALG_LFU;
    }

    // definindo o tamanho da pagina
    pgsize = atoi(argv[3]);

    // definindo o tamanho
    mmsize = atoi(argv[4]);

    // rodando as outras configuracoes
    setAddrDeslocamento();
    alocarTabela();
    prepararMemoria();
    if (alg == ALG_FIFO) { prepararFila(); }

    time = 0;
    return;
}


int main(int argc, char *argv[]) {    
    
    if (argc != 5) {
        printf("Entrada invalida!\n");
        printf("Os argumentos devem estar na forma ALG FILE PGSIZE MMSIZE.\n");
        exit(ERROR_INVALID_CALL);
    }

    printf("Configurando o simulador\n");
    // configura a memoria virtual com os parametros
    setConfig(argv);

    printf("Arquivo de entrada: %s\n", filename);
    printf("Tamanho da memoria: %d MB\n", mmsize);
    printf("Tamanho das paginas: %d KB\n", pgsize);
    printf("Algoritmo de substituicao: %s\n", argv[1]);

    printf("Executando o simulador...\n");
    // roda a simulacao
    loopProcessador();

    printf("Finalizado!\n");
    // exibe as respostas

    printf("Time: %lu\n", time);
    printf("Numero de page-faults: %lu\n", resultado.pageFaults);
    printf("Numero de page-writes: %lu\n", resultado.pageWrites);

    return 0;
}

/* ESTRUTURA DA FILA */

void filaRemove(Fila *fila) {
    free(fila->lista);
    free(fila);
}

Fila *filaCria(long size) {
    Fila *f;
    f = (Fila *)malloc(sizeof(Fila));
    if (f == NULL) {
        printf("Erro na criacao de fila\n");
        exit(ERROR_NO_MEMORY);
    }
    f->lista = (Pagina **)malloc(sizeof(Pagina *) * size);
    if (f->lista == NULL) {
        printf("Erro na criacao de fila\n");
        exit(ERROR_NO_MEMORY);
    }

    f->n = 0;
    f->ini = 0;
    f->max = size;
    return f;
}

bool filaVazia(Fila *f) {
    return f->n == 0;
}

bool filaCheia(Fila *f) {
    return f->n == f->max;
}

Pagina *filaPop(Fila *fila) {
    Pagina *p = fila->lista[fila->ini];
    fila->ini = (fila->ini + 1) % fila->max;
    fila->n--;
    return p;
}

void filaPush(Fila *fila, Pagina *pag) {
    if (filaCheia(fila)) {
        printf("Fila cheia\n");
        exit(ERROR_FULL_QUEUE);
    }
    fila->lista[(fila->ini + fila->n) % fila->max] = pag;
    fila->n++;
    return;
}


/* ESTRUTURA DA LISTA ENCADEADA */

void listaPop(Elemento *elemento) {
    Elemento *p = elemento->prev;
    Elemento *n = elemento->next;
    if (p == NULL) {    // elemento era primeiro da lista
        memoria.paginas->inicio = n;
    } else {            // remocao normal
        p->next = n;
    }

    if (n != NULL) {    // se elemento era ultimo da lista, faz nada
        n->prev = p;   
    }
    free(elemento);
    return;
}

void listaPush(Lista *lista, pagid_t x) {
    Elemento *new = (Elemento *)malloc(sizeof(Elemento));
    if (new == NULL) {
        printf("Erro de memoria no elemento\n");
        exit(ERROR_NO_MEMORY);
    }
    // coloca no inicio da fila
    new->x = x;
    new->prev = NULL;
    new->next = lista->inicio;
    lista->inicio = new;
    lista->quantidade++;
    return;
}

void listaRemove(Lista *lista) {
    Elemento *e = lista->inicio;
    Elemento *next;
    while (e != NULL) {
        next = e->next;
        free(e);
        e = next;
    }
    free(lista);
    return;
}

Lista *listaCriar() {
    Lista *l = (Lista *) malloc (sizeof(Lista));
    if (l == NULL) {
        printf("Erro de memoria na criacao da lista\n");
        exit(ERROR_NO_MEMORY);
    }
    l->quantidade = 0;
    l->inicio = NULL;
} 