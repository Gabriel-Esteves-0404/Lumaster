#ifndef ALUNO_GERENCIADOR_H
#define ALUNO_GERENCIADOR_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h> // Para struct tm
#include "gerenciador_viagens.h" // Para acessar a struct Viagem e MAX_NOME
#include "utils.h" // Incluído para toLowerStr_util e contemPalavras_util

#define MAX_ALUNOS 1000
#define MAX_PARCELAS_ALUNO 36 // Limite máximo de parcelas por aluno
#define NOME_ARQUIVO_ALUNOS "alunos_data.bin"


typedef struct {
    struct tm data_vencimento;
    double valor;
    int quitada;
    int ativa;
} Parcela;

typedef struct {
    int id_aluno;
    char nome[100];
    char rg[20];
    char cpf[20];
    char nascimento[11]; // DD/MM/AAAA
    struct tm data_inicio_pagamento; // NOVO: Data de início de pagamento do aluno
    int id_viagem;
    char nome_turma[MAX_NOME];
    int ativo; // 1 = ativo, 0 = removido/inativo

    Parcela parcelas[MAX_PARCELAS_ALUNO]; // Array fixo de parcelas para o aluno
    int num_parcelas_cadastradas;          // Quantidade de parcelas ativas cadastradas
} Aluno;

typedef enum {
    PAGAMENTO_EM_DIA,
    PAGAMENTO_ATRASADO,
    PAGAMENTO_QUITADO
} StatusPagamento;

StatusPagamento verificar_status_pagamento_aluno(const Aluno *aluno);


extern Aluno alunos[MAX_ALUNOS];
extern int num_total_alunos_sistema;

// --- PROTÓTIPOS DAS FUNÇÕES CORE/BACKEND (sem interação direta com GUI) ---

void inativar_alunos_por_viagem(int id_viagem);

// Funções de manipulação de dados
// Atualizar protótipo da função adicionar_aluno com os novos parâmetros
int adicionar_aluno(const char* nome, const char* rg, const char* cpf, const char* nascimento,
                    int id_viagem, const char* nome_turma, const struct tm data_inicio_pagamento,
                    int is_credito, double valor_entrada); // double para valor_entrada

int remover_aluno_por_cpf(const char* cpf);
int alterar_dados_aluno_por_cpf(const char* cpf, const char* novo_nome, const char* novo_rg, const char* novo_nascimento);
Aluno* buscar_aluno_por_cpf_ptr(const char* cpf); // Retorna ponteiro para o aluno encontrado ou NULL
Aluno* buscar_aluno_por_nome_ptr(const char* nome_busca); // Retorna ponteiro para o primeiro aluno encontrado ou NULL

// Funções de listagem de dados (para serem consumidas pela GUI ou console)
int contar_alunos_na_turma(int id_viagem, const char* nome_turma);
void listar_todos_alunos_backend(void (*callback_mostrar_aluno)(const Aluno*, const char*));
void listar_alunos_por_viagem_backend(int index_viagem, void (*callback_mostrar_aluno)(const Aluno*, const char*));

// Funções de arquivo
void salvar_dados_alunos();
void carregar_dados_alunos();
void exportar_todos_alunos_viagem_csv(const Viagem* v, int index_viagem);
void exportar_alunos_por_turma_csv(const Viagem* v, int index_viagem);
void exportar_planilha_embarque_por_turma_csv(int index_viagem);

// NOVO: Protótipo da função para exportar planilha de seguro viagem
void exportar_seguro_viagem_por_turma_csv(int index_viagem);

// Funções para manipulação de parcelas (protótipos)
void gerar_parcelas_aluno(Aluno *aluno, double valor_total_viagem,
                          const struct tm *data_viagem, int is_credito, double valor_entrada);

Aluno* buscar_aluno_por_cpf_e_viagem_turma_ptr(const char* cpf, int id_viagem, const char* nome_turma);

#endif // ALUNO_GERENCIADOR_H