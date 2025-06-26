#ifndef GERENCIADOR_VIAGENS_H
#define GERENCIADOR_VIAGENS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h> 

// --- DEFINIÇÕES GLOBAIS ---
#define MAX_NOME 100
#define MAX_DESCRICAO_GASTO 100
#define MAX_ITENS_OUTROS 5
#define MAX_TURMAS_POR_VIAGEM 10
#define MAX_VIAGENS 20
#define MAX_DIAS_VIAGEM 30
#define MAX_ALUNOS_POR_TURMA 100
#define NOME_ARQUIVO_DADOS "viagens_data.bin"


// --- ESTRUTURAS DE DADOS ---

// Estrutura para gastos adicionais
typedef struct GastoAdicional {
    char descricao[MAX_DESCRICAO_GASTO];
    double valor; 
    int ativa;
} GastoAdicional;

// Estrutura para Turma
typedef struct Turma {
    char nome_turma[MAX_NOME];
    int alunos;
    int numprofessores;
    int ativa;
} Turma;

// Estrutura para Viagem
typedef struct Viagem {
    char nome_viagem[MAX_NOME];
    char nome_escola[MAX_NOME];
    Turma turmas[MAX_TURMAS_POR_VIAGEM];
    int num_turmas_cadastradas;
    struct tm data_da_viagem; 
    char destino[50];
    char viagemextra_nome[50];
    char opcao_viagem_extra; 
    char opcao_guia_local;   
    int dias;
    int motoristas;
    int convidados;
    int numguias_viagem;
    double onibus; 
    double hotel; 
    double guialocalvalor; 
    double actes_por_professor; 
    double pulseiras; 
    double etiquetas; 
    double viagemextra_valor; 
    double guia_viagem_valor_total; 
    double despesas_guia_viagem; 
    double despesas_motorista; 
    double despesas_luciana; 
    double taxa_apoio_escola; 
    double farmacia; 
    double gasto_extra_geral; 
    double emergencia; 
    double poupanca_lumaster; 
    double seguro_viagem; 
    int quant_almocos_alunos_total_dias;
    int quant_jantares_alunos_total_dias;
    double val_jantar_alunos_dias[MAX_DIAS_VIAGEM]; 
    int quant_almocos_profs_total_dias;
    int quant_jantares_profs_total_dias;
    double val_almo_alunos_dias[MAX_DIAS_VIAGEM]; 
    double val_almo_profs_dias[MAX_DIAS_VIAGEM]; 
    double val_jantares_profs_dias[MAX_DIAS_VIAGEM]; 
    double val_servico_bordo; 
    double val_alimentacao_extra; 
    GastoAdicional outros_custos_diversos[MAX_ITENS_OUTROS];
    int num_outros_custos_diversos;
    double percentual_lucro_desejado; 
    double totalPorAluno; 
    int minQteAlunos; 

    int ativa; 
} Viagem;

// --- DECLARAÇÃO DE VARIÁVEIS GLOBAIS ---
extern Viagem viagens[MAX_VIAGENS];
extern int num_total_viagens_sistema;


// --- PROTÓTIPOS DAS FUNÇÕES CORE/BACKEND (sem interação direta com GUI) ---

// Funções de manipulação de dados
int adicionar_viagem_ao_sistema(Viagem* nova_viagem);
int atualizar_viagem_no_sistema(int index_viagem, const Viagem* dados_atualizados);
int remover_viagem_do_sistema(int index_viagem); 

// Funções de cálculo
void calcular_e_definir_custo_final_aluno(Viagem* v);
void calcular_custos_base(const Viagem* v, double* p_total_base, double* p_custo_aluno_base, double* p_total_custos_principais, double* p_total_custos_pessoas, double* p_total_custos_diversos, double* p_total_custos_alimentacao); 

// Funções auxiliares (backend)
int get_total_alunos_em_turmas(const Viagem* v); 
int get_total_professores(const Viagem* v);

// Funções de arquivo
void salvar_dados();
void carregar_dados();
void exportar_planilha_para_csv(int index_viagem);

#endif // GERENCIADOR_VIAGENS_H