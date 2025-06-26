#ifndef VIAGENS_MAIN_H
#define VIAGENS_MAIN_H

#include <gtk/gtk.h>
#include "gerenciador_viagens.h" // Para a struct Viagem (se precisar de acesso direto)
#include "gui_gerenciar_pagamentos.h" // Incluir para protótipo da função de recarga

extern GtkApplication *app_global;
extern GtkStack *stack_paginas;
extern GtkLabel *label_viagem_selecionada_status;
extern int viagem_selecionada_gui_idx;


extern GtkWidget *pagina_gerenciar_viagens; // Declarado aqui, definido em gui_gerenciar_viagens.c
extern GtkWidget *pagina_gerenciar_pagamentos; // Declarado aqui, definido em gui_gerenciar_pagamentos.c

int mainViagens(int argc, char *argv[]);

void mudar_para_pagina(const char *nome_pagina);
void atualizar_status_viagem_selecionada();

void recarregar_lista_viagens(GtkListBox *list_box); // De gui_gerenciar_viagens.c

#endif