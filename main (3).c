//Nome:Lu�sa Silveira - Matricula:2210875
//Nome: Lucas Lucena - matr�cula 2010796

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TAM 128
#define BYTE 8 // n�mero de bits em um byte

typedef struct NoFreq {
    unsigned char simbolo;
    int frequencia;
    struct NoFreq* proximo;
} NoFreq;

typedef struct {
    NoFreq* primeiro;
    int tamanho;
} FREQUENCIA;

typedef struct NoTrie {
    char caractere;
    int frequencia;
    struct NoTrie* esquerda;
    struct NoTrie* direita;
} NoTrie;

typedef struct {
    char caractere;
    unsigned int codigo;
    int tamanho; 
} CaractereCodigo;

// Fun��es para frequ�ncia
int* criaVetorFrequencia(FILE* arqTexto, int vetor[]) {
    int c;
    for (int i = 0; i < TAM; i++) {
        vetor[i] = 0;
    }
    while ((c = fgetc(arqTexto)) != EOF) {
        vetor[c]++;
    }
    return vetor;
}

FREQUENCIA* inicializa_lista_frequencia() {
    FREQUENCIA* lista = (FREQUENCIA*)malloc(sizeof(FREQUENCIA));
    if (lista == NULL) {
        printf("Erro ao alocar mem�ria.\n");
        exit(1);
    }
    lista->primeiro = NULL;
    lista->tamanho = 0;
    return lista;
}

void insereOrdenado(FREQUENCIA* lista, NoFreq* no) {
    if (lista->primeiro == NULL || no->frequencia < lista->primeiro->frequencia) {
        no->proximo = lista->primeiro;
        lista->primeiro = no;
    }
    else {
        NoFreq* anterior = lista->primeiro;
        NoFreq* atual = lista->primeiro->proximo;
        while (atual != NULL && atual->frequencia < no->frequencia) {
            anterior = atual;
            atual = atual->proximo;
        }
        no->proximo = atual;
        anterior->proximo = no;
    }
    lista->tamanho++;
}

void criar_lista_frequecia(int vetor[], FREQUENCIA* lista) {
    for (int i = 0; i < TAM; i++) {
        if (vetor[i] > 0) {
            NoFreq* aux = (NoFreq*)malloc(sizeof(NoFreq));
            if (aux == NULL) {
                printf("Erro ao alocar mem�ria.\n");
                exit(1);
            }
            aux->simbolo = i;
            aux->frequencia = vetor[i];
            aux->proximo = NULL;
            insereOrdenado(lista, aux);
        }
    }

    // Adicionando manualmente o caractere '!' com frequ�ncia 0 - serve como eot
    NoFreq* eot = (NoFreq*)malloc(sizeof(NoFreq));
    if (eot == NULL) {
        printf("Erro ao alocar mem�ria.\n");
        exit(1);
    }
    eot->simbolo = '!';
    eot->frequencia = 0;
    eot->proximo = NULL;
    insereOrdenado(lista, eot);
}

void liberaLista(FREQUENCIA* lista) {
    NoFreq* atual = lista->primeiro;
    while (atual != NULL) {
        NoFreq* temp = atual;
        atual = atual->proximo;
        free(temp);
    }
    free(lista);
}

void imprime_frequencia(FREQUENCIA* lista) {
    NoFreq* atual = lista->primeiro;
    while (atual != NULL) {
        printf("'%c'- %d\n", atual->simbolo, atual->frequencia);
        atual = atual->proximo;
    }
}

// Fun��es para �rvore de Huffman
NoTrie* criarNoTrie(char caractere, int frequencia) {
    NoTrie* novoNo = (NoTrie*)malloc(sizeof(NoTrie));
    novoNo->caractere = caractere;
    novoNo->frequencia = frequencia;
    novoNo->esquerda = NULL;
    novoNo->direita = NULL;
    return novoNo;
}


NoTrie* construirArvoreHuffman(FREQUENCIA* lista_frequencia) {
    int tamanho = lista_frequencia->tamanho;
    NoTrie** fila = (NoTrie**)malloc(tamanho * sizeof(NoTrie*));
    if (fila == NULL) {
        printf("Erro ao alocar memoria.\n");
        exit(1);
    }

    // Preenchimento inicial da fila com os n�s da lista de frequ�ncia

    NoFreq* atual = lista_frequencia->primeiro;
    for (int i = 0; i < tamanho; ++i) {
        fila[i] = criarNoTrie(atual->simbolo, atual->frequencia);
        atual = atual->proximo;
    }

    // Constru��o da �rvore de Huffman
    for (int i = 0; i < tamanho - 1; ++i) {
        int indiceMenor1 = 0, indiceMenor2 = 1;
        for (int j = 2; j < tamanho - i; ++j) {
            if (fila[j]->frequencia < fila[indiceMenor1]->frequencia) {
                indiceMenor2 = indiceMenor1;
                indiceMenor1 = j;
            }
            else if (fila[j]->frequencia < fila[indiceMenor2]->frequencia) {
                indiceMenor2 = j;
            }
        }

        NoTrie* novoPai = criarNoTrie('\0', fila[indiceMenor1]->frequencia + fila[indiceMenor2]->frequencia);
        novoPai->esquerda = fila[indiceMenor1];
        novoPai->direita = fila[indiceMenor2];
        fila[indiceMenor1] = novoPai;
        fila[indiceMenor2] = fila[tamanho - i - 1];
    }

    NoTrie* raiz = fila[0];
    free(fila);
    return raiz;
}


void geraCodigoRecursivo(NoTrie* raiz, unsigned int codigo, int tamanho, CaractereCodigo caracteresCodificados[], int* indiceCaracteresCodificados) {
    if (raiz == NULL)
        return;

    if (raiz->caractere != '\0') {
        caracteresCodificados[*indiceCaracteresCodificados].caractere = raiz->caractere;
        caracteresCodificados[*indiceCaracteresCodificados].codigo = codigo;
        caracteresCodificados[*indiceCaracteresCodificados].tamanho = tamanho;
        ++(*indiceCaracteresCodificados);
        return;
    }

    geraCodigoRecursivo(raiz->esquerda, codigo << 1, tamanho + 1, caracteresCodificados, indiceCaracteresCodificados);

    geraCodigoRecursivo(raiz->direita, (codigo << 1) | 1, tamanho + 1, caracteresCodificados, indiceCaracteresCodificados);
}

void codificarHuffman(NoTrie* raiz, CaractereCodigo caracteresCodificados[],int* indiceCaracteresCodificados) {
    unsigned int codigo = 0;
    int tamanho = 0;
    geraCodigoRecursivo(raiz, codigo, tamanho, caracteresCodificados,indiceCaracteresCodificados);
}


void compacta(FILE* arqTexto, FILE* arqBin, CaractereCodigo* v, FREQUENCIA* freq) {
    unsigned int byte = 0; // vai ser o nosso buffer
    int contbits = 0;

    //adicionando a tabela de codigo no arquivo
    int tamanhoStruct = freq->tamanho;
    fwrite(&tamanhoStruct, sizeof(int), 1, arqBin);

    for (int i = 0; i < tamanhoStruct; i++) {
        fwrite(&(v[i].caractere), sizeof(char), 1, arqBin);
        fwrite(&(v[i].codigo), sizeof(unsigned int), 1, arqBin);
        fwrite(&(v[i].tamanho), sizeof(int), 1, arqBin);
    }

    while (!feof(arqTexto)) {   // lemos o arquivo
        unsigned char letra = fgetc(arqTexto);  // pegamos a letra 
        for (int i = 0; i < tamanhoStruct; i++) {   // percorremos o vetor at� achar um s�mbolo que corresponde a letra 
                
            if (v[i].caractere == letra) {  // quando encontra, adiciona ao byte na posi��o correspondente 
                int tamanho = v[i].tamanho;
                contbits += tamanho;
                byte = byte << tamanho;
                byte |= (int)v[i].codigo;

                while (contbits >= BYTE) {  // se o byte estiver cheio, com 8 ou mais bits, escreve. Isso se mant�m at� o contbits for menor que 8, n�o estiver cheio

                    unsigned int novoCod = (byte >> (contbits - BYTE)) & 0xFF;
                    fwrite(&novoCod, 1, 1, arqBin);
                    contbits -= BYTE; // reduz de 8 o contbits
                }

            }
        }
    }

    // escrevendo o EOT (!), fazemos um processo semelhante ao anterior, mas precisamos percorrer o arquivo para saber o c�digo do "!"
    unsigned char eot = '!';
    for (int i = 0; i < TAM; i++) {
        if (v[i].caractere == eot) {
            int tamanho = v[i].tamanho;
            contbits += tamanho;
            byte = byte << tamanho;
            byte |= v[i].codigo; // Adiciona o c�digo do EOT ao byte
            while (contbits > 0) {
                unsigned int novoCod = byte >> (contbits - BYTE);
                novoCod &= 0xFF;
                if (contbits < BYTE) {
                    novoCod &= (0xFF >> (BYTE - contbits));
                }
                fwrite(&novoCod, 1, 1, arqBin);
                contbits -= BYTE;
            }
        }
    }
}

void descompacta(FILE* arqBin, FILE* arqTexto) {
    unsigned int buffer = 0;
    int contbits = 0;
    unsigned char hexa;
    int total_caracteres;

    //lendo a tabela de codigo
    fread(&total_caracteres, sizeof(int), 1, arqBin);
    CaractereCodigo* traduz = (CaractereCodigo*)malloc(total_caracteres * sizeof(CaractereCodigo));
    for (int i = 0; i < total_caracteres; i++) {
        fread(&(traduz[i].caractere), 1, 1, arqBin);
        fread(&(traduz[i].codigo), sizeof(unsigned int), 1, arqBin);
        fread(&(traduz[i].tamanho), sizeof(int), 1, arqBin);
    }

    while (!feof(arqBin)) { // Enquanto n�o chegarmos ao final do arquivo bin�rio
        fread(&hexa, 1, 1, arqBin); // L� um byte do arquivo bin�rio

        for (int i = 7; i >= 0; i--) {
            unsigned int novoCod = 0;
            novoCod |= (hexa >> i) & 1; // Extrai um bit do byte
            buffer |= (novoCod); // Adiciona o bit lido ao buffer
            contbits++;

            for (int j = 0; j < total_caracteres; j++) {
                if (traduz[j].codigo == buffer && contbits == traduz[j].tamanho) {
                    if (traduz[j].caractere == '!') // EOT
                        return;
                    fputc(traduz[j].caractere, arqTexto);
                    buffer= 0;
                    contbits = 0;
                }
            }
            buffer <<= 1;
        }
    }
}

void imprimeBinario(unsigned int num, int tamanho) {
    for (int i = tamanho - 1; i >= 0; i--) {
        printf("%d", (num >> i) & 1);
    }
}

int main(void) {
    FILE* arqTexto;
    arqTexto = fopen("texto.txt", "r");
    if (arqTexto == NULL) {
        printf("Erro ao abrir o arquivo.");
        return 1;
    }

    FILE* arqBin;
    arqBin = fopen("texto_bin.bin", "wb");
    if (arqBin == NULL) {
        printf("Erro ao criar o arquivo bin�rio.");
        exit(1);
    }

    // Cria��o do vetor de frequ�ncia e contagem das frequ�ncias
    int vetorFrequencia[TAM];
    criaVetorFrequencia(arqTexto, vetorFrequencia);
    fclose(arqTexto);

    // Inicializa��o da lista de frequ�ncia e preenchimento
    FREQUENCIA* lista_frequencia = inicializa_lista_frequencia();
    criar_lista_frequecia(vetorFrequencia, lista_frequencia);

    // imprimindo a lista de frequencia
    printf("Lista de Frequencia:\n");
    imprime_frequencia(lista_frequencia);
    
    // Constru��o da �rvore de Huffman
    NoTrie* raizHuffman = construirArvoreHuffman(lista_frequencia);

    // Gera��o dos c�digos de Huffman
    CaractereCodigo caracteresCodificados[TAM];
    int indiceCaracteresCodificados = 0;
    codificarHuffman(raizHuffman, caracteresCodificados,&indiceCaracteresCodificados);

    // Impress�o dos c�digos de Huffman em hexadecimal e  binario
    printf("\nCodificacao de Huffman:\n");
    for (int i = 0; i < indiceCaracteresCodificados; ++i) {
        printf("'%c'- %u (", caracteresCodificados[i].caractere, caracteresCodificados[i].codigo);
        imprimeBinario(caracteresCodificados[i].codigo, caracteresCodificados[i].tamanho);
        printf(") - %d\n", caracteresCodificados[i].tamanho);
    }

    // Compacta��o do arquivo
    arqTexto = fopen("texto.txt", "r");
    if (arqTexto == NULL) {
        printf("Erro ao abrir o arquivo.");
        exit(1);
    }
    compacta(arqTexto, arqBin, caracteresCodificados, lista_frequencia);
    fclose(arqBin);

    // Descompacta��o do arquivo
    arqBin = fopen("texto_bin.bin", "rb");
    FILE* arqTextoOut = fopen("texto_descompactado.txt", "w");
    if (arqBin == NULL || arqTextoOut == NULL) {
        printf("Erro ao abrir os arquivos.");
        exit(1);
    }

    descompacta(arqBin, arqTextoOut);

    fclose(arqBin);
    fclose(arqTextoOut);
    liberaLista(lista_frequencia);

    return 0;
}