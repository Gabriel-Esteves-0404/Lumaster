#ifndef GUI_EDICAO_VIAGEM_H
#define GUI_EDICAO_VIAGEM_H

#include <gtk/gtk.h>
#include "gerenciador_viagens.h" // Para a struct Viagem
#include "viagensmain.h"         // Para mudar_para_pagina, app_global, etc.
#include "utils.h"               // Para funções de diálogo

// Variável global para a página de edição (APENAS DECLARAÇÃO EXTERNA)
extern GtkWidget *pagina_edicao_viagem;

// NOVO: Arrays de GtkWidget* para os rótulos dos campos de alimentação por dia
extern GtkWidget *label_val_almo_alunos_dias[MAX_DIAS_VIAGEM];
extern GtkWidget *label_val_jantar_alunos_dias[MAX_DIAS_VIAGEM];
extern GtkWidget *label_val_almo_profs_dias[MAX_DIAS_VIAGEM];
extern GtkWidget *label_val_jantares_profs_dias[MAX_DIAS_VIAGEM];


// --- Protótipos das funções de construção e manipulação da GUI ---

// Função principal para construir a página de edição
void construir_pagina_edicao_viagem();

// Função para carregar os dados de uma viagem específica na página de edição
void carregar_dados_para_edicao(int index_viagem);

// Funções para limpar/resetar o formulário de edição
void limpar_formulario_edicao_viagem();

#endif // GUI_EDICAO_VIAGEM_H
