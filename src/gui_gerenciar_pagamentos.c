#include "gui_gerenciar_pagamentos.h"
#include <stdio.h> // Para snprintf
#include <string.h> // Para memset

GtkWidget *pagina_gerenciar_pagamentos = NULL; // ÚNICA DEFINIÇÃO
GtkListBox *listbox_alunos_pagamento_ref = NULL; // Variável global para referenciar o listbox de alunos

// Widgets para a seção de detalhes do aluno/parcela
static GtkLabel *label_pagamento_aluno_nome;
static GtkLabel *label_pagamento_aluno_cpf;
static GtkLabel *label_pagamento_aluno_viagem_turma;
static GtkLabel *label_pagamento_aluno_valor_total_viagem;
static GtkListBox *listbox_pagamento_parcelas;
static GtkWidget *frame_detalhes_aluno_pagamento; // Para habilitar/desabilitar


// Variável para guardar o aluno atualmente selecionado na lista de pagamentos
static Aluno *aluno_selecionado_pagamento = NULL;
static GtkEntry *entry_pagamento_filter_cpf;

// --- FUNÇÕES AUXILIARES E CALLBACKS ---

// Limpa os campos de detalhes do aluno/parcelas
void limpar_detalhes_pagamento_aluno() {
    gtk_label_set_text(label_pagamento_aluno_nome, "Nome: ");
    gtk_label_set_text(label_pagamento_aluno_cpf, "CPF: ");
    gtk_label_set_text(label_pagamento_aluno_viagem_turma, "Viagem/Turma: ");
    gtk_label_set_text(label_pagamento_aluno_valor_total_viagem, "Valor Total da Viagem: R$ 0.00");

    GList *children, *iter;
    children = gtk_container_get_children(GTK_CONTAINER(listbox_pagamento_parcelas));
    for (iter = children; iter != NULL; iter = g_list_next(iter)) {
        gtk_widget_destroy(GTK_WIDGET(iter->data));
    }
    g_list_free(children);

    gtk_widget_set_sensitive(frame_detalhes_aluno_pagamento, FALSE); // Desabilita o frame
    aluno_selecionado_pagamento = NULL; // Ninguém selecionado
}

// Callback para o toggle button de quitação da parcela
static void on_quitar_parcela_toggled(GtkToggleButton *toggle_button, gpointer user_data) {
    Parcela *parcela = (Parcela *)user_data;
    if (parcela) {
        parcela->quitada = gtk_toggle_button_get_active(toggle_button);
        salvar_dados_alunos(); // Salva a alteração na parcela

        // Atualiza o texto do botão (opcional, pode ser feito via CSS ou ícone)
        // char btn_text[50];
        // snprintf(btn_text, sizeof(btn_text), "%s", parcela->quitada ? "Quitada" : "Não Quitada");
        // gtk_button_set_label(GTK_BUTTON(toggle_button), btn_text);

        // Atualiza a exibição da parcela para refletir a mudança de estado (cor, etc.)
        GtkWidget *row_box = gtk_widget_get_parent(GTK_WIDGET(toggle_button));
        GtkWidget *label_parcela_info = GTK_WIDGET(g_object_get_data(G_OBJECT(row_box), "parcela_info_label"));
        if (label_parcela_info) {
            char parcela_info_markup[256];
            char data_venc_str[15];
            strftime(data_venc_str, sizeof(data_venc_str), "%d/%m/%Y", &parcela->data_vencimento);

            snprintf(parcela_info_markup, sizeof(parcela_info_markup),
                     "<span %s>Vencimento: %s | Valor: R$%.2f %s</span>",
                     parcela->quitada ? "foreground='green' weight='bold'" : "foreground='red' weight='bold'",
                     data_venc_str, parcela->valor, parcela->quitada ? "" : " (PENDENTE)");
            gtk_label_set_markup(GTK_LABEL(label_parcela_info), parcela_info_markup);
        }

        //mostrar_dialogo_info(NULL, "Status da Parcela",
                             //g_strdup_printf("Parcela alterada para %s!", parcela->quitada ? "QUITADA" : "NÃO QUITADA"));
    }
}

// Exibe os detalhes de pagamento do aluno selecionado
void exibir_detalhes_pagamento_aluno(Aluno *aluno) {
    if (!aluno) return;

    aluno_selecionado_pagamento = aluno; // Armazena o aluno selecionado

    gtk_widget_set_sensitive(frame_detalhes_aluno_pagamento, TRUE); // Habilita o frame

    char buffer[256];
    char nascimento_str[12];
    strncpy(nascimento_str, aluno->nascimento, sizeof(nascimento_str));
    nascimento_str[sizeof(nascimento_str) - 1] = '\0';

    gtk_label_set_text(label_pagamento_aluno_nome, g_strdup_printf("Nome: %s", aluno->nome));
    gtk_label_set_text(label_pagamento_aluno_cpf, g_strdup_printf("CPF: %s", aluno->cpf));

    // Buscar nome da viagem e turma
    char viagem_nome[MAX_NOME] = "Viagem Indefinida";
    double valor_total_viagem = 0.0;
    if (aluno->id_viagem >= 0 && aluno->id_viagem < MAX_VIAGENS && viagens[aluno->id_viagem].ativa) {
        strncpy(viagem_nome, viagens[aluno->id_viagem].nome_viagem, MAX_NOME - 1);
        viagem_nome[MAX_NOME - 1] = '\0';
        valor_total_viagem = viagens[aluno->id_viagem].totalPorAluno; // Pega o valor da viagem
    }
    gtk_label_set_text(label_pagamento_aluno_viagem_turma,
                       g_strdup_printf("Viagem: %s / Turma: %s", viagem_nome, aluno->nome_turma));
    gtk_label_set_text(label_pagamento_aluno_valor_total_viagem,
                       g_strdup_printf("Valor Total da Viagem: R$%.2f", valor_total_viagem));

    // Limpa a lista de parcelas e recarrega
    GList *children, *iter;
    children = gtk_container_get_children(GTK_CONTAINER(listbox_pagamento_parcelas));
    for (iter = children; iter != NULL; iter = g_list_next(iter)) {
        gtk_widget_destroy(GTK_WIDGET(iter->data));
    }
    g_list_free(children);

    if (aluno->num_parcelas_cadastradas == 0) {
        GtkWidget *label = gtk_label_new("Nenhuma parcela cadastrada para este aluno.");
        gtk_list_box_insert(listbox_pagamento_parcelas, label, -1);
        gtk_widget_show(label);
    } else {
        for (int i = 0; i < aluno->num_parcelas_cadastradas; i++) {
            Parcela *p = &aluno->parcelas[i];
            if (p->ativa) {
                GtkWidget *row_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
                gtk_container_set_border_width(GTK_CONTAINER(row_box), 5);

                char parcela_info_markup[256];
                char data_venc_str[15];
                strftime(data_venc_str, sizeof(data_venc_str), "%d/%m/%Y", &p->data_vencimento);

                snprintf(parcela_info_markup, sizeof(parcela_info_markup),
                         "<span %s>Parcela %d: Vencimento: %s | Valor: R$%.2f %s</span>",
                         p->quitada ? "foreground='green' weight='bold'" : "foreground='red' weight='bold'",
                         i + 1, data_venc_str, p->valor, p->quitada ? "" : " (PENDENTE)");

                GtkWidget *label_parcela_info = gtk_label_new(NULL);
                gtk_label_set_markup(GTK_LABEL(label_parcela_info), parcela_info_markup);
                gtk_label_set_xalign(GTK_LABEL(label_parcela_info), 0);
                gtk_box_pack_start(GTK_BOX(row_box), label_parcela_info, TRUE, TRUE, 0);
                g_object_set_data(G_OBJECT(row_box), "parcela_info_label", label_parcela_info); // Store label for update

                GtkWidget *toggle_quitado = gtk_check_button_new_with_label(p->quitada ? "Quitada" : "Não Quitada");
                gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(toggle_quitado), p->quitada);
                // Conecta o callback passando o ponteiro para a parcela
                g_signal_connect(toggle_quitado, "toggled", G_CALLBACK(on_quitar_parcela_toggled), p);
                gtk_box_pack_end(GTK_BOX(row_box), toggle_quitado, FALSE, FALSE, 0);

                GtkWidget *row = gtk_list_box_row_new();
                gtk_container_add(GTK_CONTAINER(row), row_box);
                gtk_list_box_insert(listbox_pagamento_parcelas, row, -1);
                gtk_widget_show_all(row);
            }
        }
    }
}

// Callback para quando um aluno é selecionado na lista (listbox_alunos_pagamento)
static void on_aluno_pagamento_row_selected(GtkListBox *list_box, GtkListBoxRow *row, gpointer user_data) {
    if (row) {
        int index_aluno = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(row), "aluno_index"));
        if (index_aluno >= 0 && index_aluno < MAX_ALUNOS && alunos[index_aluno].ativo) {
            exibir_detalhes_pagamento_aluno(&alunos[index_aluno]);
        } else {
            limpar_detalhes_pagamento_aluno();
        }
    } else {
        limpar_detalhes_pagamento_aluno();
    }
}

// Carrega todos os alunos na GtkListBox para gerenciamento de pagamento
void carregar_lista_alunos_para_pagamento(GtkListBox *listbox_alunos) {
    g_print("DEBUG: carregar_lista_alunos_para_pagamento chamada.\n");
    if (!listbox_alunos) return;

    // Limpa o listbox atual para recarregar os alunos
    GList *children, *iter;
    children = gtk_container_get_children(GTK_CONTAINER(listbox_alunos));
    for (iter = children; iter != NULL; iter = g_list_next(iter)) {
        gtk_widget_destroy(GTK_WIDGET(iter->data));
    }
    g_list_free(children);

    // --- Obtém o texto do filtro de CPF (e o limpa) ---
    const char *filter_cpf_raw = gtk_entry_get_text(entry_pagamento_filter_cpf);
    char *filter_cpf_cleaned = NULL;
    if (filter_cpf_raw && strlen(filter_cpf_raw) > 0) {
        // Usa sua função utilitária para manter apenas dígitos no CPF
        filter_cpf_cleaned = limpar_string_numerica(filter_cpf_raw);
    } else {
        // Se não há filtro, usa uma string vazia para corresponder a todos os CPFs
        filter_cpf_cleaned = g_strdup("");
    }

    int count = 0; // Contador de alunos exibidos
    for (int i = 0; i < MAX_ALUNOS; ++i) {
        // Processa apenas alunos ativos
        if (alunos[i].ativo) {
            // --- Aplica o filtro de CPF ---
            if (strlen(filter_cpf_cleaned) > 0) {
                // Se o CPF do aluno NÃO contém a string limpa do filtro, pula este aluno
                if (strstr(alunos[i].cpf, filter_cpf_cleaned) == NULL) {
                    continue;
                }
            }

            // --- Determina o status de pagamento para coloração ---
            // Chama a função que verifica se o aluno está em dia, atrasado ou quitado
            StatusPagamento status = verificar_status_pagamento_aluno(&alunos[i]);
            const char *color = "black"; // Cor padrão para "Em Dia"

            if (status == PAGAMENTO_ATRASADO) {
                color = "red"; // Alunos com parcelas atrasadas ficam vermelhos
            } else if (status == PAGAMENTO_QUITADO) {
                color = "green"; // Alunos com todas as parcelas quitadas ficam verdes
            }

            // Cria a caixa horizontal para o conteúdo da linha
            GtkWidget *row_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
            gtk_container_set_border_width(GTK_CONTAINER(row_box), 5);

            // Cria o rótulo para as informações do aluno
            GtkWidget *label_aluno_info = gtk_label_new(NULL);
            char aluno_info_text[256];

            // Busca os nomes da viagem e turma para exibir
            char viagem_nome[MAX_NOME] = "Viagem Indefinida";
            char turma_nome[MAX_NOME] = "Turma Indefinida";
            if (alunos[i].id_viagem >= 0 && alunos[i].id_viagem < MAX_VIAGENS && viagens[alunos[i].id_viagem].ativa) {
                strncpy(viagem_nome, viagens[alunos[i].id_viagem].nome_viagem, MAX_NOME - 1);
                viagem_nome[MAX_NOME - 1] = '\0';
                strncpy(turma_nome, alunos[i].nome_turma, MAX_NOME - 1);
                turma_nome[MAX_NOME - 1] = '\0';
            }

            // Formata o texto das informações do aluno
            snprintf(aluno_info_text, sizeof(aluno_info_text),
                        "%s - CPF: %s | Viagem: %s / Turma: %s",
                        alunos[i].nome, alunos[i].cpf, viagem_nome, turma_nome);

            // Escapa o texto para Pango Markup e aplica a cor desejada
            gchar *escaped_text = g_markup_escape_text(aluno_info_text, -1);
            gchar *final_markup_string = g_strdup_printf("<span foreground=\"%s\">%s</span>", color, escaped_text);

            // Define o texto no rótulo com o markup de cor
            gtk_label_set_markup(GTK_LABEL(label_aluno_info), final_markup_string);
            g_free(escaped_text);     // Libera a memória do texto escapado
            g_free(final_markup_string); // Libera a memória da string final com markup

            gtk_label_set_xalign(GTK_LABEL(label_aluno_info), 0);
            gtk_box_pack_start(GTK_BOX(row_box), label_aluno_info, TRUE, TRUE, 0);

            // Cria a linha da lista (GtkListBoxRow) e adiciona a caixa de conteúdo
            GtkWidget *row = gtk_list_box_row_new();
            gtk_container_add(GTK_CONTAINER(row), row_box);
            // Armazena o índice do aluno na linha para uso no callback de seleção
            g_object_set_data(G_OBJECT(row), "aluno_index", GINT_TO_POINTER(i));
            
            // Insere a linha no listbox e a torna visível
            gtk_list_box_insert(listbox_alunos, row, -1);
            gtk_widget_show_all(row);
            count++; // Incrementa o contador de alunos exibidos
        }
    }

    // --- Exibe uma mensagem se nenhum aluno for encontrado ---
    if (count == 0) {
        GtkWidget *empty_label;
        if (strlen(filter_cpf_cleaned) > 0) {
            empty_label = gtk_label_new("Nenhum aluno encontrado com o CPF informado.");
        } else {
            empty_label = gtk_label_new("Nenhum aluno cadastrado no sistema.");
        }
        GtkWidget *row = gtk_list_box_row_new();
        gtk_container_add(GTK_CONTAINER(row), empty_label);
        gtk_list_box_insert(listbox_alunos, row, -1);
        gtk_widget_show_all(row);
    }

    g_free(filter_cpf_cleaned); // Libera a memória alocada para o CPF limpo
}


// --- CONSTRUÇÃO DA PÁGINA DE GERENCIAMENTO DE PAGAMENTOS ---
void construir_pagina_gerenciar_pagamentos() {
    pagina_gerenciar_pagamentos = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_set_border_width(GTK_CONTAINER(pagina_gerenciar_pagamentos), 20);

    GtkWidget *label_titulo = gtk_label_new("<span size='x-large' weight='bold'>Gerenciamento de Pagamentos</span>");
    gtk_label_set_use_markup(GTK_LABEL(label_titulo), TRUE);
    gtk_label_set_xalign(GTK_LABEL(label_titulo), 0);
    gtk_box_pack_start(GTK_BOX(pagina_gerenciar_pagamentos), label_titulo, FALSE, FALSE, 10);

    // Layout principal da página: HBox com lista de alunos à esquerda e detalhes à direita
    GtkWidget *hbox_main = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 15);
    gtk_box_pack_start(GTK_BOX(pagina_gerenciar_pagamentos), hbox_main, TRUE, TRUE, 0);

    // --- Seção Esquerda: Lista de Alunos ---
    GtkWidget *vbox_alunos_list = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_widget_set_hexpand(vbox_alunos_list, TRUE);
    gtk_widget_set_size_request(vbox_alunos_list, 400, -1);

    GtkWidget *label_alunos_list = gtk_label_new("<b>Alunos Cadastrados</b>");
    gtk_label_set_use_markup(GTK_LABEL(label_alunos_list), TRUE);
    gtk_label_set_xalign(GTK_LABEL(label_alunos_list), 0);
    gtk_box_pack_start(GTK_BOX(vbox_alunos_list), label_alunos_list, FALSE, FALSE, 0);

    // NOVO: Campo de filtro por CPF
    GtkWidget *hbox_filter = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_box_pack_start(GTK_BOX(hbox_filter), gtk_label_new("Filtrar por CPF:"), FALSE, FALSE, 0);
    entry_pagamento_filter_cpf = GTK_ENTRY(gtk_entry_new());
    gtk_entry_set_placeholder_text(entry_pagamento_filter_cpf, "Digite o CPF (somente números)");
    // Conectar o sinal 'changed' para aplicar o filtro dinamicamente
    g_signal_connect(entry_pagamento_filter_cpf, "changed", G_CALLBACK(on_filter_cpf_changed), NULL);
    gtk_box_pack_start(GTK_BOX(hbox_filter), GTK_WIDGET(entry_pagamento_filter_cpf), TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(vbox_alunos_list), hbox_filter, FALSE, FALSE, 5); // Adicionar ao vbox da lista de alunos

    GtkWidget *scrolled_window_alunos = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window_alunos), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
    gtk_widget_set_vexpand(scrolled_window_alunos, TRUE);

    GtkListBox *listbox_alunos_pagamento = GTK_LIST_BOX(gtk_list_box_new());
    gtk_list_box_set_selection_mode(listbox_alunos_pagamento, GTK_SELECTION_SINGLE); // Apenas um aluno por vez
    g_signal_connect(listbox_alunos_pagamento, "row-activated", G_CALLBACK(on_aluno_pagamento_row_selected), NULL);
    gtk_container_add(GTK_CONTAINER(scrolled_window_alunos), GTK_WIDGET(listbox_alunos_pagamento));
    gtk_box_pack_start(GTK_BOX(vbox_alunos_list), scrolled_window_alunos, TRUE, TRUE, 0);

    gtk_box_pack_start(GTK_BOX(hbox_main), vbox_alunos_list, TRUE, TRUE, 0);

    // Armazenar a referência global para o listbox de alunos de pagamento
    listbox_alunos_pagamento_ref = listbox_alunos_pagamento;


    // --- Seção Direita: Detalhes do Aluno e Parcelas ---
    frame_detalhes_aluno_pagamento = gtk_frame_new("Detalhes do Aluno e Parcelas");
    gtk_widget_set_hexpand(frame_detalhes_aluno_pagamento, TRUE);
    gtk_widget_set_sensitive(frame_detalhes_aluno_pagamento, FALSE); // Começa desabilitado

    GtkWidget *vbox_detalhes = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_set_border_width(GTK_CONTAINER(vbox_detalhes), 10);
    gtk_container_add(GTK_CONTAINER(frame_detalhes_aluno_pagamento), vbox_detalhes);

    label_pagamento_aluno_nome = GTK_LABEL(gtk_label_new("Nome: "));
    gtk_label_set_xalign(GTK_LABEL(label_pagamento_aluno_nome), 0);
    gtk_box_pack_start(GTK_BOX(vbox_detalhes), GTK_WIDGET(label_pagamento_aluno_nome), FALSE, FALSE, 0);

    label_pagamento_aluno_cpf = GTK_LABEL(gtk_label_new("CPF: "));
    gtk_label_set_xalign(GTK_LABEL(label_pagamento_aluno_cpf), 0);
    gtk_box_pack_start(GTK_BOX(vbox_detalhes), GTK_WIDGET(label_pagamento_aluno_cpf), FALSE, FALSE, 0);

    label_pagamento_aluno_viagem_turma = GTK_LABEL(gtk_label_new("Viagem/Turma: "));
    gtk_label_set_xalign(GTK_LABEL(label_pagamento_aluno_viagem_turma), 0);
    gtk_box_pack_start(GTK_BOX(vbox_detalhes), GTK_WIDGET(label_pagamento_aluno_viagem_turma), FALSE, FALSE, 0);

    label_pagamento_aluno_valor_total_viagem = GTK_LABEL(gtk_label_new("Valor Total da Viagem: R$ 0.00"));
    gtk_label_set_xalign(GTK_LABEL(label_pagamento_aluno_valor_total_viagem), 0);
    gtk_box_pack_start(GTK_BOX(vbox_detalhes), GTK_WIDGET(label_pagamento_aluno_valor_total_viagem), FALSE, FALSE, 0);

    GtkWidget *label_parcelas_title = gtk_label_new("<b>Parcelas:</b>");
    gtk_label_set_use_markup(GTK_LABEL(label_parcelas_title), TRUE);
    gtk_label_set_xalign(GTK_LABEL(label_parcelas_title), 0);
    gtk_box_pack_start(GTK_BOX(vbox_detalhes), label_parcelas_title, FALSE, FALSE, 10);

    GtkWidget *scrolled_window_parcelas = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window_parcelas), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
    gtk_widget_set_vexpand(scrolled_window_parcelas, TRUE);

    listbox_pagamento_parcelas = GTK_LIST_BOX(gtk_list_box_new());
    gtk_list_box_set_selection_mode(listbox_pagamento_parcelas, GTK_SELECTION_NONE);
    gtk_container_add(GTK_CONTAINER(scrolled_window_parcelas), GTK_WIDGET(listbox_pagamento_parcelas));
    gtk_box_pack_start(GTK_BOX(vbox_detalhes), scrolled_window_parcelas, TRUE, TRUE, 0);

    gtk_box_pack_start(GTK_BOX(hbox_main), frame_detalhes_aluno_pagamento, TRUE, TRUE, 0);


    // Botão Voltar
    /*GtkWidget *btn_voltar = gtk_button_new_with_label("Voltar ao Menu Principal");
    gtk_widget_set_halign(btn_voltar, GTK_ALIGN_END);
    g_signal_connect(btn_voltar, "clicked", G_CALLBACK(limpar_detalhes_pagamento_aluno), NULL); // Limpa ao voltar
    g_signal_connect(btn_voltar, "clicked", G_CALLBACK(mudar_para_pagina), (gpointer)"menu_principal_page");
    gtk_box_pack_end(GTK_BOX(pagina_gerenciar_pagamentos), btn_voltar, FALSE, FALSE, 10);*/

    gtk_widget_show_all(pagina_gerenciar_pagamentos);

    // Carregar a lista de alunos inicialmente
    // carregar_lista_alunos_para_pagamento(listbox_alunos_pagamento); // Comentado, será chamado por gui_gerenciar_pagamentos_recarregar_alunos_e_limpar_detalhes()
    // limpar_detalhes_pagamento_aluno(); // Comentado, será chamado por gui_gerenciar_pagamentos_recarregar_alunos_e_limpar_detalhes()
}

// Função auxiliar para ser chamada do main.c para recarregar a lista e limpar detalhes
void gui_gerenciar_pagamentos_recarregar_alunos_e_limpar_detalhes() {
    if (listbox_alunos_pagamento_ref) {
        carregar_lista_alunos_para_pagamento(listbox_alunos_pagamento_ref);
        limpar_detalhes_pagamento_aluno();
    } else {
        g_print("ERRO: listbox_alunos_pagamento_ref é NULL em gui_gerenciar_pagamentos_recarregar_alunos_e_limpar_detalhes.\n");
    }
}

void on_filter_cpf_changed(GtkEntry *entry, gpointer user_data) {
    gui_gerenciar_pagamentos_recarregar_alunos_e_limpar_detalhes();
}
