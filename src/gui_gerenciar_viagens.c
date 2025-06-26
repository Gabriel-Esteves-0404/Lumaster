#include "gui_gerenciar_viagens.h"

GtkWidget *pagina_gerenciar_viagens = NULL; // ÚNICA DEFINIÇÃO

static void on_adicionar_viagem_na_lista_clicked(GtkButton *btn, gpointer user_data) {
    g_print("Botão 'ADICIONAR VIAGEM' clicado. Redirecionando para o formulário de cadastro.\n");
    limpar_formulario_cadastro_viagem(); // Garante que o formulário esteja limpo
    mudar_para_pagina("cadastro_viagem_page");
}

// Callback para selecionar viagem de um item da lista
void on_selecionar_viagem_do_item_lista(GtkButton *btn, gpointer user_data) {
    int index_viagem = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(btn), "viagem_index"));
    if (index_viagem >= 0 && index_viagem < MAX_VIAGENS && viagens[index_viagem].ativa) {
        viagem_selecionada_gui_idx = index_viagem;
        atualizar_status_viagem_selecionada();
        g_print("Viagem '%s' selecionada da lista.\n", viagens[index_viagem].nome_viagem);

        // Carrega os dados da viagem selecionada na página de edição
        carregar_dados_para_edicao(viagem_selecionada_gui_idx);
        // Muda para a página de edição da viagem
        mudar_para_pagina("edicao_viagem_page");

    } else {
        mostrar_dialogo_aviso(GTK_WINDOW(gtk_widget_get_ancestor(GTK_WIDGET(btn), GTK_TYPE_WINDOW)),
                              "Viagem Inativa",
                              g_strdup_printf("A viagem '%s' está inativa e não pode ser selecionada para operações.",
                                              viagens[index_viagem].nome_viagem));
    }
}

// Recarrega os itens da GtkListBox de viagens na página de gerenciamento
// AGORA LISTARÁ APENAS AS VIAGENS ATIVAS
void recarregar_lista_viagens(GtkListBox *list_box) {
    g_print("DEBUG: recarregar_lista_viagens chamada para listar apenas as viagens ATIVAS.\n");
    if (list_box == NULL) {
        g_print("DEBUG: ERRO: list_box é NULL em recarregar_lista_viagens.\n");
        return;
    }

    GList *children, *iter;
    children = gtk_container_get_children(GTK_CONTAINER(list_box));
    for (iter = children; iter != NULL; iter = g_list_next(iter)) {
        gtk_widget_destroy(GTK_WIDGET(iter->data));
    }
    g_list_free(children);
    g_print("DEBUG: Conteúdo da list_box limpo.\n");

    int count_active = 0; // Contador para viagens ativas adicionadas
    for (int i = 0; i < MAX_VIAGENS; ++i) {
        // MODIFICAÇÃO: Apenas adiciona a viagem se ela estiver ativa
        if (viagens[i].ativa) { 
            g_print("DEBUG: Tentando adicionar viagem %d: %s - Ativa: %d\n", i, viagens[i].nome_viagem, viagens[i].ativa);

            GtkWidget *row_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
            gtk_container_set_border_width(GTK_CONTAINER(row_box), 5);

            GtkWidget *label_viagem = gtk_label_new(NULL);
            char texto_viagem[256];

            if (viagens[i].data_da_viagem.tm_year > 0) { // Verifica se a data foi definida (ano > 1900)
                snprintf(texto_viagem, sizeof(texto_viagem),
                         "%s - %s - %02d/%02d/%04d", // Remove "(INATIVA)" daqui
                         viagens[i].nome_viagem,
                         viagens[i].nome_escola,
                         viagens[i].data_da_viagem.tm_mday,
                         viagens[i].data_da_viagem.tm_mon + 1,
                         viagens[i].data_da_viagem.tm_year + 1900);
            } else {
                snprintf(texto_viagem, sizeof(texto_viagem),
                         "<b>%s</b> - %s - Data Indefinida", // Remove "(INATIVA)" daqui
                         viagens[i].nome_viagem,
                         viagens[i].nome_escola);
            }

            gchar *escaped_text_for_markup = g_markup_escape_text(texto_viagem, -1);
            gchar *final_markup_string = g_strdup_printf("<span foreground=\"black\">%s</span>", // Sempre preto se for ativa
                                                         escaped_text_for_markup);

            gtk_label_set_markup(GTK_LABEL(label_viagem), final_markup_string);
            g_free(escaped_text_for_markup);
            g_free(final_markup_string);

            gtk_label_set_xalign(GTK_LABEL(label_viagem), 0);
            gtk_box_pack_start(GTK_BOX(row_box), label_viagem, TRUE, TRUE, 0);

            GtkWidget *btn_selecionar_item = gtk_button_new_with_label("Selecionar");
            g_object_set_data(G_OBJECT(btn_selecionar_item), "viagem_index", GINT_TO_POINTER(i));
            g_signal_connect(btn_selecionar_item, "clicked", G_CALLBACK(on_selecionar_viagem_do_item_lista), NULL);
            gtk_box_pack_end(GTK_BOX(row_box), btn_selecionar_item, FALSE, FALSE, 0);

            GtkWidget *row = gtk_list_box_row_new();
            gtk_container_add(GTK_CONTAINER(row), row_box);
            gtk_list_box_insert(GTK_LIST_BOX(list_box), row, -1);
            gtk_widget_show_all(row);
            count_active++; // Incrementa o contador apenas para viagens ativas
        } else {
             g_print("DEBUG: Viagem %d '%s' está INATIVA. Não será adicionada à lista visível.\n", i, viagens[i].nome_viagem);
        }
    }
    if (count_active == 0) { // Verifica o contador de viagens ativas
        g_print("DEBUG: Nenhuma viagem ATIVA encontrada. Exibindo mensagem.\n");
        GtkWidget *empty_label = gtk_label_new("Nenhuma viagem ativa cadastrada no sistema.");
        GtkWidget *row = gtk_list_box_row_new();
        gtk_container_add(GTK_CONTAINER(row), empty_label);
        gtk_list_box_insert(GTK_LIST_BOX(list_box), row, -1);
        gtk_widget_show_all(row);
    }
    g_print("DEBUG: %d viagens ATIVAS adicionadas à list_box.\n", count_active);
}


void construir_pagina_gerenciar_viagens() {
    pagina_gerenciar_viagens = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_set_border_width(GTK_CONTAINER(pagina_gerenciar_viagens), 20);

    GtkWidget *header_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    /*GtkWidget *btn_voltar_header = gtk_button_new_from_icon_name("go-previous-symbolic", GTK_ICON_SIZE_LARGE_TOOLBAR);
    gtk_button_set_relief(GTK_BUTTON(btn_voltar_header), GTK_RELIEF_NONE);
    g_signal_connect(btn_voltar_header, "clicked", G_CALLBACK(mudar_para_pagina), (gpointer)"menu_principal_page");
    gtk_box_pack_start(GTK_BOX(header_box), btn_voltar_header, FALSE, FALSE, 0);*/

    GtkWidget *label_titulo = gtk_label_new("<span size='x-large' weight='bold'>Gerenciamento de Viagens Ativas</span>"); // Título ajustado
    gtk_label_set_use_markup(GTK_LABEL(label_titulo), TRUE);
    gtk_label_set_xalign(GTK_LABEL(label_titulo), 0);
    gtk_box_pack_start(GTK_BOX(header_box), label_titulo, TRUE, TRUE, 0);

    GtkWidget *btn_adicionar_viagem = gtk_button_new_with_label("ADICIONAR VIAGEM");
    gtk_widget_set_size_request(btn_adicionar_viagem, 150, 40);
    gtk_widget_set_halign(btn_adicionar_viagem, GTK_ALIGN_END);
    g_signal_connect(btn_adicionar_viagem, "clicked", G_CALLBACK(on_adicionar_viagem_na_lista_clicked), NULL);
    gtk_box_pack_end(GTK_BOX(header_box), btn_adicionar_viagem, FALSE, FALSE, 0);

    gtk_box_pack_start(GTK_BOX(pagina_gerenciar_viagens), header_box, FALSE, FALSE, 10);

    GtkWidget *scrolled_window_list = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window_list), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
    gtk_widget_set_vexpand(scrolled_window_list, TRUE);

    GtkListBox *list_box_viagens = GTK_LIST_BOX(gtk_list_box_new());
    gtk_list_box_set_selection_mode(list_box_viagens, GTK_SELECTION_NONE);
    gtk_container_add(GTK_CONTAINER(scrolled_window_list), GTK_WIDGET(list_box_viagens));

    g_object_set_data(G_OBJECT(pagina_gerenciar_viagens), "list_box_viagens", list_box_viagens);

    gtk_box_pack_start(GTK_BOX(pagina_gerenciar_viagens), scrolled_window_list, TRUE, TRUE, 0);

    gtk_widget_show_all(pagina_gerenciar_viagens);
}