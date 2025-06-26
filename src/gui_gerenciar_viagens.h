#ifndef GUI_GERENCIAR_VIAGENS_H
#define GUI_GERENCIAR_VIAGENS_H

#include <gtk/gtk.h>
#include "gerenciador_viagens.h"
#include "viagensmain.h"
#include "utils.h"
#include "gui_cadastro_viagem.h"
#include "gui_edicao_viagem.h" // Incluir para carregar_dados_para_edicao

void construir_pagina_gerenciar_viagens();


static void on_adicionar_viagem_na_lista_clicked(GtkButton *btn, gpointer user_data);


void on_selecionar_viagem_do_item_lista(GtkButton *btn, gpointer user_data);


void recarregar_lista_viagens(GtkListBox *list_box);

#endif