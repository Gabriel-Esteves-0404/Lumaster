#ifndef GUI_ALUNOS_H
#define GUI_ALUNOS_H

#include <gtk/gtk.h>
#include "gerenciador_viagens.h"
#include "alunos_gerenciador.h"
#include "viagensmain.h"
#include "utils.h"

// Variáveis globais para as páginas de alunos
extern GtkWidget *pagina_cadastro_aluno;
extern GtkWidget *pagina_gerenciar_alunos_geral;

// Protótipos das funções da GUI de Alunos
void construir_pagina_cadastro_aluno();
void exibir_cadastro_aluno_para_viagem_turma(int id_viagem, const char *nome_turma);
void limpar_formulario_cadastro_aluno();

#endif // GUI_ALUNOS_H