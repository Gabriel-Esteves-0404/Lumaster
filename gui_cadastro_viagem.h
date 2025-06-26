#ifndef GUI_CADASTRO_VIAGEM_H
#define GUI_CADASTRO_VIAGEM_H

#include <gtk/gtk.h>
#include "gerenciador_viagens.h"
#include "viagensmain.h"
#include "alunos_gerenciador.h" // Se houver necessidade de acesso direto aqui

// Variável global para a página de cadastro (APENAS DECLARAÇÃO EXTERNA)
extern GtkWidget *pagina_cadastro_viagem;

// --- Protótipos das funções de construção e manipulação da GUI ---

// Função principal para construir a página de cadastro
void construir_pagina_cadastro_viagem();

// Função para limpar/resetar o formulário de cadastro
void limpar_formulario_cadastro_viagem();

// Função para abrir o diálogo de cadastro de turma
void abrir_dialogo_cadastro_turma(GtkWindow *parent_window);

// Callbacks para gerenciamento de visibilidade e dados
void on_guia_local_toggled(GtkToggleButton *toggle_button, gpointer user_data);
void on_viagem_extra_toggled(GtkToggleButton *toggle_button, gpointer user_data);
void on_dias_viagem_changed(GtkSpinButton *spin_button, gpointer user_data);

// Callbacks para controlar a visibilidade dos campos de alimentação por dia
void on_quant_almocos_alunos_changed(GtkSpinButton *spin_button, gpointer user_data);
void on_quant_jantares_alunos_changed(GtkSpinButton *spin_button, gpointer user_data);
void on_quant_almocos_profs_changed(GtkSpinButton *spin_button, gpointer user_data);
void on_quant_jantares_profs_changed(GtkSpinButton *spin_button, gpointer user_data);

#endif // GUI_CADASTRO_VIAGEM_H