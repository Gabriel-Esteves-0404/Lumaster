#ifndef GUI_GERENCIAR_PAGAMENTOS_H
#define GUI_GERENCIAR_PAGAMENTOS_H

#include <gtk/gtk.h>
#include "alunos_gerenciador.h"
#include "gerenciador_viagens.h"
#include "viagensmain.h"
#include "utils.h"

extern GtkWidget *pagina_gerenciar_pagamentos;
extern GtkListBox *listbox_alunos_pagamento_ref;

// Protótipos
void construir_pagina_gerenciar_pagamentos();
void carregar_lista_alunos_para_pagamento(GtkListBox *listbox_alunos);
void exibir_detalhes_pagamento_aluno(Aluno *aluno);
void limpar_detalhes_pagamento_aluno();
void gui_gerenciar_pagamentos_recarregar_alunos_e_limpar_detalhes();

// NOVO: Protótipo para o callback do filtro de CPF
void on_filter_cpf_changed(GtkEntry *entry, gpointer user_data);

#endif // GUI_GERENCIAR_PAGAMENTOS_H
