#include <gtk/gtk.h>
#include "gerenciador_viagens.h"
#include "alunos_gerenciador.h"
#include "utils.h"
#include "gui_cadastro_viagem.h"
#include "gui_edicao_viagem.h"
#include "gui_alunos.h"       
#include "gui_gerenciar_pagamentos.h"
#include "viagensmain.h"
#include "gui_gerenciar_viagens.h"
#include "gui_gerenciador_usuarios.h"

// --- DEFINIÇÃO DAS VARIÁVEIS GLOBAIS ---
Viagem viagens[MAX_VIAGENS];
int num_total_viagens_sistema = 0;

Aluno alunos[MAX_ALUNOS];
int num_total_alunos_sistema = 0;

// Variáveis globais da GUI
GtkApplication *app_global = NULL;
GtkStack *stack_paginas = NULL;
GtkLabel *label_viagem_selecionada_status = NULL;
int viagem_selecionada_gui_idx = -1; 
GtkWidget *pagina_menu_principal = NULL;
GtkWidget *pagina_detalhes_edicao_viagem = NULL;
GtkWidget *pagina_gerenciar_alunos_geral = NULL;


// --- FUNÇÕES DE CONSTRUÇÃO DAS PÁGINAS ---
void construir_pagina_menu_principal();
void construir_pagina_cadastro_aluno(); 
void construir_pagina_gerenciar_pagamentos(); 


// --- FUNÇÕES DE ATUALIZAÇÃO DA UI ---
void mudar_para_pagina(const char *nome_pagina) {
    if (stack_paginas && nome_pagina) {
        gtk_stack_set_visible_child_name(stack_paginas, nome_pagina);

        if (strcmp(nome_pagina, "gerenciar_viagens_page") == 0) {
            g_print("DEBUG: Chamada para recarregar lista na página de gerenciamento de viagens.\n");
            if (pagina_gerenciar_viagens) {
                GtkListBox *list_box = GTK_LIST_BOX(g_object_get_data(G_OBJECT(pagina_gerenciar_viagens), "list_box_viagens"));
                if (list_box) {
                    recarregar_lista_viagens(list_box);
                } else {
                    g_print("DEBUG: ERRO: list_box_viagens é NULL em g_object_get_data da pagina_gerenciar_viagens.\n");
                }
            } else {
                g_print("DEBUG: ERRO: pagina_gerenciar_viagens é NULL ao tentar recarregar lista.\n");
            }
        } else if (strcmp(nome_pagina, "gerenciar_pagamentos_page") == 0) {
            g_print("DEBUG: Chamada para recarregar lista de alunos na página de gerenciamento de pagamentos.\n");
            gui_gerenciar_pagamentos_recarregar_alunos_e_limpar_detalhes();
        }
    }
}

void atualizar_status_viagem_selecionada() {
    if (label_viagem_selecionada_status) {
        if (viagem_selecionada_gui_idx != -1 && viagens[viagem_selecionada_gui_idx].ativa) {
            char status_text[256];
            snprintf(status_text, sizeof(status_text),
                     "<span foreground=\"darkgreen\" weight=\"bold\">&lt;&lt;&lt; Viagem Selecionada: %s - %s &gt;&gt;&gt;</span>",
                     viagens[viagem_selecionada_gui_idx].nome_viagem,
                     viagens[viagem_selecionada_gui_idx].nome_escola);

            gchar *utf8_status_text = g_locale_to_utf8(status_text, -1, NULL, NULL, NULL);
            if (utf8_status_text) {
                gtk_label_set_markup(GTK_LABEL(label_viagem_selecionada_status), utf8_status_text);
                g_free(utf8_status_text);
            } else {
                gtk_label_set_text(GTK_LABEL(label_viagem_selecionada_status), "Erro de codificação no status da viagem.");
            }

        } else {
            gchar *utf8_no_viagem_selected = NULL;
            utf8_no_viagem_selected = g_locale_to_utf8("<span foreground=\"darkred\" weight=\"bold\">&lt;&lt;&lt; Nenhuma viagem selecionada &gt;&gt;&gt;</span>", -1, NULL, NULL, NULL);

            if (utf8_no_viagem_selected) {
                gtk_label_set_markup(GTK_LABEL(label_viagem_selecionada_status), utf8_no_viagem_selected);
                g_free(utf8_no_viagem_selected);
            } else {
                gtk_label_set_text(GTK_LABEL(label_viagem_selecionada_status), "Nenhuma viagem selecionada (erro de codificação).");
            }
        }
    }
}


// --- CALLBACKS ---
static void on_cadastrar_viagem_clicked(GtkButton *btn, gpointer user_data) {
    g_print("Botão 'Gerenciar Viagens' clicado. Redirecionando para Gerenciamento de Viagens.\n");
    mudar_para_pagina("gerenciar_viagens_page");
}

//Botão Gerenciar Pagamentos
static void on_gerenciar_pagamentos_clicked(GtkButton *btn, gpointer user_data) {
    g_print("Botão 'Gerenciar Pagamentos' clicado. Redirecionando.\n");
    mudar_para_pagina("gerenciar_pagamentos_page");
}


//Botão Exportar Planilha Seguro Viagem
static void on_exportar_planilha_seguro_viagem_clicked(GtkButton *btn, gpointer user_data) {
    if (viagem_selecionada_gui_idx != -1 && viagens[viagem_selecionada_gui_idx].ativa) {
        g_print("Botão 'Exportar Planilha Seguro Viagem' clicado para %s.\n", viagens[viagem_selecionada_gui_idx].nome_viagem);
        exportar_seguro_viagem_por_turma_csv(viagem_selecionada_gui_idx);
        mostrar_dialogo_info(GTK_WINDOW(gtk_widget_get_ancestor(GTK_WIDGET(btn), GTK_TYPE_WINDOW)),
                             "Exportação Concluída",
                             "As planilhas de seguro viagem (uma por turma, com alunos qualificados) foram exportadas para arquivos CSV! Verifique a pasta do executável.");
    } else {
        mostrar_dialogo_aviso(GTK_WINDOW(gtk_widget_get_ancestor(GTK_WIDGET(btn), GTK_TYPE_WINDOW)),
                              "Aviso",
                              "Nenhuma viagem selecionada para exportar a planilha de seguro viagem.");
    }
}

//Botão Exportar Planilha Embarque
static void on_exportar_planilha_embarque_clicked(GtkButton *btn, gpointer user_data) {
    if (viagem_selecionada_gui_idx != -1 && viagens[viagem_selecionada_gui_idx].ativa) {
        g_print("Botão 'Exportar Planilha de Embarque' clicado para %s.\n", viagens[viagem_selecionada_gui_idx].nome_viagem);
        exportar_planilha_embarque_por_turma_csv(viagem_selecionada_gui_idx);
        mostrar_dialogo_info(GTK_WINDOW(gtk_widget_get_ancestor(GTK_WIDGET(btn), GTK_TYPE_WINDOW)),
                             "Exportação Concluída",
                             "As planilhas de embarque (uma por turma) foram exportadas para arquivos CSV! Verifique a pasta do executável.");
    } else {
        mostrar_dialogo_aviso(GTK_WINDOW(gtk_widget_get_ancestor(GTK_WIDGET(btn), GTK_TYPE_WINDOW)),
                              "Aviso",
                              "Nenhuma viagem selecionada para exportar a planilha de embarque.");
    }
}

//Botão Excluir viagem selecionada
static void on_excluir_viagem_selecionada_clicked(GtkButton *btn, gpointer user_data) {
    if (viagem_selecionada_gui_idx != -1 && viagens[viagem_selecionada_gui_idx].ativa) {
        g_print("Botão 'Excluir Viagem Selecionada' clicado para %s.\n", viagens[viagem_selecionada_gui_idx].nome_viagem);
        if (mostrar_dialogo_confirmacao(GTK_WINDOW(gtk_widget_get_ancestor(GTK_WIDGET(btn), GTK_TYPE_WINDOW)),
                                       "Confirmar Exclusão",
                                       g_strdup_printf("Tem certeza que deseja EXCLUIR a viagem '%s'?\nEsta ação não pode ser desfeita.",
                                                       viagens[viagem_selecionada_gui_idx].nome_viagem))) {
            if (remover_viagem_do_sistema(viagem_selecionada_gui_idx)) {
                mostrar_dialogo_info(GTK_WINDOW(gtk_widget_get_ancestor(GTK_WIDGET(btn), GTK_TYPE_WINDOW)),
                                     "Viagem Removida",
                                     "Viagem excluída com sucesso.");
                viagem_selecionada_gui_idx = -1; // Deseleciona após a exclusão
                atualizar_status_viagem_selecionada();
                mudar_para_pagina("gerenciar_viagens_page"); // Recarrega a lista
            } else {
                mostrar_dialogo_erro(GTK_WINDOW(gtk_widget_get_ancestor(GTK_WIDGET(btn), GTK_TYPE_WINDOW)),
                                     "Erro",
                                     "Não foi possível excluir a viagem. Verifique se ela está ativa ou existe.");
            }
        } else {
            g_print("Exclusão cancelada.\n");
        }
    } else {
        mostrar_dialogo_aviso(GTK_WINDOW(gtk_widget_get_ancestor(GTK_WIDGET(btn), GTK_TYPE_WINDOW)),
                               "Aviso",
                               "Nenhuma viagem selecionada para exclusão.");
    }
}

//Botão porta planilhas de custo csv
static void on_exportar_planilha_csv_clicked(GtkButton *btn, gpointer user_data) {
    if (viagem_selecionada_gui_idx != -1 && viagens[viagem_selecionada_gui_idx].ativa) {
        g_print("Botão 'Exportar Planilha Custos (CSV)' clicado para %s.\n", viagens[viagem_selecionada_gui_idx].nome_viagem);
        exportar_planilha_para_csv(viagem_selecionada_gui_idx);
        mostrar_dialogo_info(GTK_WINDOW(gtk_widget_get_ancestor(GTK_WIDGET(btn), GTK_TYPE_WINDOW)),
                             "Exportação Concluída",
                             "Planilha de custos exportada com sucesso para um arquivo CSV!");
    } else {
        mostrar_dialogo_aviso(GTK_WINDOW(gtk_widget_get_ancestor(GTK_WIDGET(btn), GTK_TYPE_WINDOW)),
                               "Aviso",
                               "Nenhuma viagem selecionada para exportar a planilha.");
    }
}

//Botão para tirar a viagem do contexto.
static void on_deselecionar_viagem_clicked(GtkButton *btn, gpointer user_data) {
    if (viagem_selecionada_gui_idx != -1) {
        g_print("Botão 'Deselecionar Viagem' clicado. Viagem '%s' deselecionada.\n", viagens[viagem_selecionada_gui_idx].nome_viagem);
        viagem_selecionada_gui_idx = -1;
        atualizar_status_viagem_selecionada();
        mostrar_dialogo_info(GTK_WINDOW(gtk_widget_get_ancestor(GTK_WIDGET(btn), GTK_TYPE_WINDOW)),
                             "Informação",
                             "Viagem deselecionada com sucesso.");
    } else {
        mostrar_dialogo_info(GTK_WINDOW(gtk_widget_get_ancestor(GTK_WIDGET(btn), GTK_TYPE_WINDOW)),
                             "Informação",
                             "Nenhuma viagem está selecionada.");
    }
}

static void on_exportar_planilha_seguro_viagem_clicked(GtkButton *btn, gpointer user_data);

static void on_exportar_planilha_embarque_clicked(GtkButton *btn, gpointer user_data);

void on_application_shutdown(GtkApplication* app, gpointer user_data) {
    g_print("DEBUG: Sinal de shutdown da aplicação recebido. Salvando dados...\n");
    salvar_dados();
    salvar_dados_alunos();
    g_print("DEBUG: Dados salvos. Aplicação encerrada.\n");
}

void Lumaster_Shutdown() {
    void on_application_shutdown(GtkApplication* app, gpointer user_data);
    g_print("DEBUG: Chamada para Lumaster_Shutdown. Iniciando processo de encerramento...\n");
    if (app_global) {
        g_application_quit(G_APPLICATION(app_global));
    } else {
        g_print("ERRO: app_global é NULL. Não foi possível encerrar a aplicação via GtkApplication.\n");
    }
}

// --- FUNÇÃO PRINCIPAL DE ATIVAÇÃO DA APLICAÇÃO GTK (CONSTRUÇÃO DA INTERFACE) ---
static void iniciarLumaster(GtkApplication* app, gpointer user_data) {
    app_global = app;

    GtkWidget *janela;
    GtkWidget *grid_principal;
    GtkWidget *box_menu_lateral;
    GtkWidget *scrolled_window_menu;
    GtkWidget *box_status_viagem_painel;

    // 1. Criação da Janela Principal
    janela = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(janela), "Sistema de Gestão de Viagens Lumaster");
    gtk_window_set_default_size(GTK_WINDOW(janela), 1200, 800);
    gtk_container_set_border_width(GTK_CONTAINER(janela), 10);

    // 2. Layout Principal com GtkGrid
    grid_principal = gtk_grid_new();
    gtk_container_add(GTK_CONTAINER(janela), grid_principal);
    gtk_grid_set_row_spacing(GTK_GRID(grid_principal), 5);
    gtk_grid_set_column_spacing(GTK_GRID(grid_principal), 10);

    // 3. Área do Menu Lateral (Coluna 0 do Grid Principal)
    scrolled_window_menu = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window_menu), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
    gtk_widget_set_hexpand(scrolled_window_menu, FALSE);
    gtk_widget_set_vexpand(scrolled_window_menu, TRUE);
    gtk_widget_set_size_request(scrolled_window_menu, 250, -1);

    box_menu_lateral = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_add(GTK_CONTAINER(scrolled_window_menu), box_menu_lateral);
    gtk_widget_set_margin_start(box_menu_lateral, 10);
    gtk_widget_set_margin_end(box_menu_lateral, 10);
    gtk_widget_set_margin_top(box_menu_lateral, 10);
    gtk_widget_set_margin_bottom(box_menu_lateral, 10);
    gtk_grid_attach(GTK_GRID(grid_principal), scrolled_window_menu, 0, 0, 1, 2);

    // --- Botões e Títulos no Menu Lateral ---
    GtkWidget *btn;
    GtkWidget *label_secao;

    // Seção: Opções do Sistema
    label_secao = gtk_label_new("<b>Opções do Sistema</b>");
    gtk_label_set_use_markup(GTK_LABEL(label_secao), TRUE);
    gtk_box_pack_start(GTK_BOX(box_menu_lateral), label_secao, FALSE, FALSE, 15);

    btn = gtk_button_new_with_label("Gerenciar Viagens");
    g_signal_connect(btn, "clicked", G_CALLBACK(on_cadastrar_viagem_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(box_menu_lateral), btn, FALSE, FALSE, 0);

    // NOVO BOTÃO: Gerenciar Pagamentos
    btn = gtk_button_new_with_label("Gerenciar Pagamentos");
    g_signal_connect(btn, "clicked", G_CALLBACK(on_gerenciar_pagamentos_clicked), NULL); // Conecta o novo callback
    gtk_box_pack_start(GTK_BOX(box_menu_lateral), btn, FALSE, FALSE, 0); // Adiciona ao menu

    btn = gtk_button_new_with_label("Gerenciamento de Usuários");
    g_signal_connect(btn, "clicked", G_CALLBACK(on_gerenciamento_usuarios_clicado), NULL);
    gtk_box_pack_start(GTK_BOX(box_menu_lateral),  btn, FALSE, FALSE, 0);

    gtk_box_pack_start(GTK_BOX(box_menu_lateral), gtk_separator_new(GTK_ORIENTATION_HORIZONTAL), FALSE, FALSE, 10);

    // Seção: Opções para Viagem Acessada
    label_secao = gtk_label_new("<b>Opções da Viagem Selecionada</b>");
    gtk_label_set_use_markup(GTK_LABEL(label_secao), TRUE);
    gtk_box_pack_start(GTK_BOX(box_menu_lateral), label_secao, FALSE, FALSE, 15);

    btn = gtk_button_new_with_label("Excluir Viagem Selecionada");
    g_signal_connect(btn, "clicked", G_CALLBACK(on_excluir_viagem_selecionada_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(box_menu_lateral), btn, FALSE, FALSE, 0);

    //btoes de exportação

    btn = gtk_button_new_with_label("Exportar Planilha Custos (CSV)");
    g_signal_connect(btn, "clicked", G_CALLBACK(on_exportar_planilha_csv_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(box_menu_lateral), btn, FALSE, FALSE, 0);

    btn = gtk_button_new_with_label("Exportar Planilha Seguro Viagem (CSV)");
    g_signal_connect(btn, "clicked", G_CALLBACK(on_exportar_planilha_seguro_viagem_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(box_menu_lateral), btn, FALSE, FALSE, 0);

    btn = gtk_button_new_with_label("Exportar Planilha de Embarque (CSV)");
    g_signal_connect(btn, "clicked", G_CALLBACK(on_exportar_planilha_embarque_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(box_menu_lateral), btn, FALSE, FALSE, 0);

    gtk_box_pack_start(GTK_BOX(box_menu_lateral), gtk_separator_new(GTK_ORIENTATION_HORIZONTAL), FALSE, FALSE, 10);

    // Seção: Outras Opções
    label_secao = gtk_label_new("<b>Outras Opções</b>");
    gtk_label_set_use_markup(GTK_LABEL(label_secao), TRUE);
    gtk_box_pack_start(GTK_BOX(box_menu_lateral), label_secao, FALSE, FALSE, 15);

    btn = gtk_button_new_with_label("Deselecionar Viagem Atual");
    g_signal_connect(btn, "clicked", G_CALLBACK(on_deselecionar_viagem_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(box_menu_lateral), btn, FALSE, FALSE, 0);

    gtk_box_pack_start(GTK_BOX(box_menu_lateral), gtk_separator_new(GTK_ORIENTATION_HORIZONTAL), FALSE, FALSE, 20);
    

    stack_paginas = GTK_STACK(gtk_stack_new());
    gtk_stack_set_transition_type(GTK_STACK(stack_paginas), GTK_STACK_TRANSITION_TYPE_SLIDE_LEFT_RIGHT);
    gtk_widget_set_hexpand(GTK_WIDGET(stack_paginas), TRUE);
    gtk_widget_set_vexpand(GTK_WIDGET(stack_paginas), TRUE);

    gtk_grid_attach(GTK_GRID(grid_principal), GTK_WIDGET(stack_paginas), 1, 0, 1, 1);

    // --- Construção e Adição das Páginas ao GtkStack ---
    construir_pagina_menu_principal();
    gtk_stack_add_named(GTK_STACK(stack_paginas), pagina_menu_principal, "menu_principal_page");

    construir_pagina_gerenciar_viagens();
    gtk_stack_add_named(GTK_STACK(stack_paginas), pagina_gerenciar_viagens, "gerenciar_viagens_page");

    construir_pagina_cadastro_viagem();
    gtk_stack_add_named(GTK_STACK(stack_paginas), pagina_cadastro_viagem, "cadastro_viagem_page");

    construir_pagina_edicao_viagem(); // Adiciona a nova página de Edição de Viagem ao stack
    gtk_stack_add_named(GTK_STACK(stack_paginas), pagina_edicao_viagem, "edicao_viagem_page");

    construir_pagina_cadastro_aluno(); // NOVO: Adiciona a página de cadastro de aluno
    gtk_stack_add_named(GTK_STACK(stack_paginas), pagina_cadastro_aluno, "cadastro_aluno_page");

    // Adiciona a página de gerenciamento de pagamentos
    construir_pagina_gerenciar_pagamentos();
    gtk_stack_add_named(GTK_STACK(stack_paginas), pagina_gerenciar_pagamentos, "gerenciar_pagamentos_page");

    // Define a página inicial
    gtk_stack_set_visible_child_name(GTK_STACK(stack_paginas), "menu_principal_page");

    // 5. Área de Status da Viagem Selecionada (Coluna 1, Linha 1 do Grid Principal - abaixo do GtkStack)
    box_status_viagem_painel = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_widget_set_halign(box_status_viagem_painel, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(box_status_viagem_painel, GTK_ALIGN_END);

    label_viagem_selecionada_status = GTK_LABEL(gtk_label_new(NULL));
    gtk_label_set_use_markup(GTK_LABEL(label_viagem_selecionada_status), TRUE);
    gtk_box_pack_start(GTK_BOX(box_status_viagem_painel), GTK_WIDGET(label_viagem_selecionada_status), FALSE, FALSE, 0);

    gtk_grid_attach(GTK_GRID(grid_principal), box_status_viagem_painel, 1, 1, 1, 1);

    carregar_dados();
    carregar_dados_alunos();

    atualizar_status_viagem_selecionada(); // Atualiza o status inicial

    gtk_widget_show_all(janela);
}

//Feito para integrar com o login:
int mainViagens(int argc, char *argv[]) {
    int status;
    app_global = gtk_application_new("org.lumaster.gerenciadorviagens", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app_global, "activate", G_CALLBACK(iniciarLumaster), NULL);
    g_signal_connect(app_global, "shutdown", G_CALLBACK(on_application_shutdown), NULL);
    status = g_application_run(G_APPLICATION(app_global), argc, argv);
    g_object_unref(app_global);
    return status;
}

// Abre a primeira tela do sistema, depois do login:
void construir_pagina_menu_principal() {
    pagina_menu_principal = gtk_box_new(GTK_ORIENTATION_VERTICAL, 20);
    gtk_container_set_border_width(GTK_CONTAINER(pagina_menu_principal), 50);
    gtk_widget_set_halign(pagina_menu_principal, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(pagina_menu_principal, GTK_ALIGN_CENTER);

    GtkWidget *img_bem_vindo = gtk_image_new_from_icon_name("document-send-symbolic", GTK_ICON_SIZE_DIALOG);
    gtk_box_pack_start(GTK_BOX(pagina_menu_principal), img_bem_vindo, FALSE, FALSE, 0);

    GtkWidget *label_titulo = gtk_label_new("<span size='xx-large' weight='bold'>Bem-vindo ao Sistema de Gestão de Viagens Lumaster</span>");
    gtk_label_set_use_markup(GTK_LABEL(label_titulo), TRUE);
    gtk_label_set_xalign(GTK_LABEL(label_titulo), 0);
    gtk_box_pack_start(GTK_BOX(pagina_menu_principal), label_titulo, FALSE, FALSE, 0);

    GtkWidget *label_instrucoes = gtk_label_new(
        "Este sistema permite gerenciar viagens escolares, turmas e alunos de forma eficiente.\n"
        "Use o menu lateral para navegar entre as diferentes funcionalidades."
    );
    gtk_label_set_line_wrap(GTK_LABEL(label_instrucoes), TRUE);
    gtk_label_set_justify(GTK_LABEL(label_instrucoes), GTK_JUSTIFY_CENTER);
    gtk_box_pack_start(GTK_BOX(pagina_menu_principal), label_instrucoes, FALSE, FALSE, 0);

    gtk_widget_show_all(pagina_menu_principal);
}