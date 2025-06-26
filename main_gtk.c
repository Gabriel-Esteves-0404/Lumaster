#include <gtk/gtk.h>
#include <string.h>
#include "usuarios.h"
#include "viagensmain.h"
#include "gui_gerenciador_usuarios.h"

GtkWidget *janela_principal; // Janela principal da aplicação
GtkWidget *pilha_telas; // Pilha de telas para navegação

// Widgets da Tela de Login
GtkWidget *entrada_usuario_login;
GtkWidget *entrada_senha_login;
GtkWidget *rotulo_status_login;
GtkWidget *botao_ir_cadastro_login;
GtkWidget *botao_esqueci_senha_login;

// Widgets da Tela de Cadastro
GtkWidget *entrada_usuario_cadastro;
GtkWidget *entrada_senha_cadastro;
GtkWidget *entrada_dica_cadastro;
GtkWidget *radio_permissao_admin_cadastro;
GtkWidget *radio_permissao_convidado_cadastro;
GtkWidget *entrada_senha_admin_cadastro;
GtkWidget *rotulo_senha_admin_cadastro;
GtkWidget *rotulo_status_cadastro;

// Widgets dos Menus Iniciais (Admin/Convidado)
GtkWidget *rotulo_usuario_atual_admin_inicio;
GtkWidget *rotulo_usuario_atual_convidado_inicio;

// Widgets da Tela de Gerenciamento de Usuários
GtkWidget *visualizador_texto_lista_usuarios;
GtkTextBuffer *buffer_lista_usuarios;
GtkWidget *entrada_usuario_remover;
GtkWidget *rotulo_status_remover;
GtkWidget *entrada_usuario_original_editar;
GtkWidget *entrada_novo_usuario_editar;
GtkWidget *entrada_nova_senha_editar;
GtkWidget *entrada_nova_dica_editar;
GtkWidget *rotulo_status_editar;
GtkWidget *entrada_novo_usuario_admin;
GtkWidget *entrada_nova_senha_admin;
GtkWidget *entrada_nova_dica_admin;
GtkWidget *radio_permissao_admin_novo_usuario_admin;
GtkWidget *radio_permissao_convidado_novo_usuario_admin;
GtkWidget *entrada_senha_admin_novo_usuario_admin;
GtkWidget *rotulo_senha_admin_novo_usuario_admin;
GtkWidget *rotulo_status_novo_usuario_admin;

// Protótipos de Funções Auxiliares
void mostrar_dialogo_mensagem(GtkWindow *pai, GtkMessageType tipo, const char *titulo, const char *mensagem);
void atualizar_exibicao_lista_usuarios();
gboolean existe_admin(); // Verifica se existe um usuário administrador
static void on_permissao_cadastro_alternada(GtkToggleButton *botao, gpointer dados_usuario);
static void on_permissao_novo_usuario_admin_alternada(GtkToggleButton *botao, gpointer dados_usuario);

// Callbacks para funcionalidades placeholder (em construção)
static void on_gerenciamento_financas_clicado(GtkButton *botao, gpointer dados_usuario) {
    mostrar_dialogo_mensagem(GTK_WINDOW(janela_principal), GTK_MESSAGE_INFO, "Gerenciamento de Finanças", "Funcionalidade de Finanças em construção!");
}

/*static void on_gerenciamento_alunos_clicado(GtkButton *botao, gpointer dados_usuario) {
    mostrar_dialogo_mensagem(GTK_WINDOW(janela_principal), GTK_MESSAGE_INFO, "Gerenciamento de Alunos", "Funcionalidade de Alunos em construção!");
}*/

/*static void on_gerenciamento_viagens_clicado(GtkButton *botao, gpointer dados_usuario) {
    mostrar_dialogo_mensagem(GTK_WINDOW(janela_principal), GTK_MESSAGE_INFO, "Gerenciamento de Viagens", "Funcionalidade de Viagens em construção!");
}*/

// Callbacks Específicos do Gerenciamento de Usuários
void on_gerenciamento_usuarios_clicado(GtkButton *botao, gpointer dados_usuario) {
    gtk_stack_set_visible_child_name(GTK_STACK(pilha_telas), "tela_gerenciamento_usuarios");
    atualizar_exibicao_lista_usuarios();
}

static void on_voltar_para_inicio_clicado(GtkButton *botao, gpointer dados_usuario) {
    if (usuario_logado_idx != -1 && usuarios[usuario_logado_idx].permissao_usuario == 1) {
        gtk_stack_set_visible_child_name(GTK_STACK(pilha_telas), "inicio_admin");
    } else {
        gtk_stack_set_visible_child_name(GTK_STACK(pilha_telas), "inicio_convidado");
    }
}

static void on_esqueci_senha_clicado(GtkButton *botao, gpointer dados_usuario) {
    const char *usuario = gtk_entry_get_text(GTK_ENTRY(entrada_usuario_login));
    char buffer_mensagem[256];
    int usuario_encontrado = 0;

    for (int i = 0; i < qntd_usuarios; i++) {
        if (strcmp(usuario, usuarios[i].nome_usuario) == 0) {
            snprintf(buffer_mensagem, sizeof(buffer_mensagem), "A dica para '%s' é: %s", usuarios[i].nome_usuario, usuarios[i].dica_senha);
            usuario_encontrado = 1;
            break;
        }
    }

    if (!usuario_encontrado) {
        snprintf(buffer_mensagem, sizeof(buffer_mensagem), "Usuário '%s' não encontrado.", usuario);
    }
    mostrar_dialogo_mensagem(GTK_WINDOW(janela_principal), GTK_MESSAGE_INFO, "Recuperação de Senha", buffer_mensagem);
}

/**
 * @brief Aplica uma string CSS a um widget GTK.
 * @param widget O widget a ser estilizado.
 * @param string_css A string CSS a ser aplicada.
 */
void aplicar_css_ao_widget(GtkWidget *widget, const char *string_css) {
    GtkCssProvider *provedor = gtk_css_provider_new();
    gtk_css_provider_load_from_data(provedor, string_css, -1, NULL);
    GtkStyleContext *contexto = gtk_widget_get_style_context(widget);
    gtk_style_context_add_provider(contexto, GTK_STYLE_PROVIDER(provedor), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    g_object_unref(provedor);
}

/**
 * @brief Exibe um diálogo de mensagem padrão.
 * @param pai Janela pai do diálogo.
 * @param tipo Tipo de mensagem (GTK_MESSAGE_INFO, GTK_MESSAGE_ERROR, etc.).
 * @param titulo Título do diálogo.
 * @param mensagem Conteúdo da mensagem.
 */
void mostrar_dialogo_mensagem(GtkWindow *pai, GtkMessageType tipo, const char *titulo, const char *mensagem) {
    GtkWidget *dialogo = gtk_message_dialog_new(pai,
                                                 GTK_DIALOG_DESTROY_WITH_PARENT,
                                                 tipo,
                                                 GTK_BUTTONS_OK,
                                                 "%s", mensagem);
    gtk_window_set_title(GTK_WINDOW(dialogo), titulo);
    gtk_dialog_run(GTK_DIALOG(dialogo));
    gtk_widget_destroy(dialogo);
}

/**
 * @brief Atualiza o conteúdo da lista de usuários no TextView.
 */
void atualizar_exibicao_lista_usuarios() {
    char string_lista_usuarios[4096];
    listarUsuarios_gui(string_lista_usuarios, sizeof(string_lista_usuarios));
    gtk_text_buffer_set_text(buffer_lista_usuarios, string_lista_usuarios, -1);
}

/**
 * @brief Verifica se existe pelo menos um usuário com permissão de administrador.
 * @return TRUE se existir um admin, FALSE caso contrário.
 */
gboolean existe_admin() {
    for (int i = 0; i < qntd_usuarios; i++) {
        if (usuarios[i].permissao_usuario == 1) {
            return TRUE;
        }
    }
    return FALSE;
}

// --- Funções de Callback de Ações do Usuário ---

static void on_login_clicado(GtkButton *botao, gpointer dados_usuario) {
    const char *usuario = gtk_entry_get_text(GTK_ENTRY(entrada_usuario_login));
    const char *senha = gtk_entry_get_text(GTK_ENTRY(entrada_senha_login));
    char mensagem[256];

    if (logar_gui(usuario, senha, mensagem)) {
        gtk_label_set_text(GTK_LABEL(rotulo_status_login), mensagem);
        gtk_entry_set_text(GTK_ENTRY(entrada_usuario_login), "");
        gtk_entry_set_text(GTK_ENTRY(entrada_senha_login), "");

        char info_usuario[100];
        snprintf(info_usuario, sizeof(info_usuario), "Usuário Logado: %s (%s)",
                 usuarios[usuario_logado_idx].nome_usuario,
                 usuarios[usuario_logado_idx].permissao_usuario == 1 ? "Admin" : "Convidado");

        if (usuarios[usuario_logado_idx].permissao_usuario == 1) { // Admin
            mainViagens(0, NULL);
        } else { // Convidado
            mainViagens(0, NULL);
        }

    } else {
        gtk_label_set_text(GTK_LABEL(rotulo_status_login), mensagem);
    }
}

static void on_ir_para_cadastro_clicado(GtkButton *botao, gpointer dados_usuario) {
    gtk_stack_set_visible_child_name(GTK_STACK(pilha_telas), "tela_cadastro");
    gtk_label_set_text(GTK_LABEL(rotulo_status_cadastro), "");
    gtk_entry_set_text(GTK_ENTRY(entrada_usuario_cadastro), "");
    gtk_entry_set_text(GTK_ENTRY(entrada_senha_cadastro), "");
    gtk_entry_set_text(GTK_ENTRY(entrada_dica_cadastro), "");
    gtk_entry_set_text(GTK_ENTRY(entrada_senha_admin_cadastro), "");

    if (qntd_usuarios == 0) {
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radio_permissao_admin_cadastro), TRUE);
        gtk_widget_set_sensitive(radio_permissao_convidado_cadastro, FALSE);
        gtk_widget_set_visible(rotulo_senha_admin_cadastro, FALSE);
        gtk_widget_set_visible(entrada_senha_admin_cadastro, FALSE);
    } else {
        gtk_widget_set_sensitive(radio_permissao_convidado_cadastro, TRUE);
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radio_permissao_convidado_cadastro), TRUE);
        on_permissao_cadastro_alternada(GTK_TOGGLE_BUTTON(radio_permissao_convidado_cadastro), NULL);
    }
}

static void on_registrar_clicado(GtkButton *botao, gpointer dados_usuario) {
    const char *usuario = gtk_entry_get_text(GTK_ENTRY(entrada_usuario_cadastro));
    const char *senha = gtk_entry_get_text(GTK_ENTRY(entrada_senha_cadastro));
    const char *dica = gtk_entry_get_text(GTK_ENTRY(entrada_dica_cadastro));
    int permissao = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radio_permissao_admin_cadastro)) ? 1 : 0;
    const char *senha_admin = gtk_entry_get_text(GTK_ENTRY(entrada_senha_admin_cadastro));
    char mensagem[256];

    if (registrarUsuario(usuario, senha, dica, permissao, senha_admin, mensagem)) {
        gtk_label_set_text(GTK_LABEL(rotulo_status_cadastro), mensagem);
        gtk_entry_set_text(GTK_ENTRY(entrada_usuario_cadastro), "");
        gtk_entry_set_text(GTK_ENTRY(entrada_senha_cadastro), "");
        gtk_entry_set_text(GTK_ENTRY(entrada_dica_cadastro), "");
        gtk_entry_set_text(GTK_ENTRY(entrada_senha_admin_cadastro), "");

        if (existe_admin()) {
            gtk_widget_set_visible(botao_ir_cadastro_login, FALSE);
        }
    } else {
        gtk_label_set_text(GTK_LABEL(rotulo_status_cadastro), mensagem);
    }
}

// Callback para os botões de rádio de permissão no cadastro
static void on_permissao_cadastro_alternada(GtkToggleButton *botao, gpointer dados_usuario) {
    if (gtk_toggle_button_get_active(botao)) {
        if (botao == GTK_TOGGLE_BUTTON(radio_permissao_admin_cadastro)) {
            if (existe_admin()) {
                gtk_widget_set_visible(rotulo_senha_admin_cadastro, TRUE);
                gtk_widget_set_visible(entrada_senha_admin_cadastro, TRUE);
            } else {
                gtk_widget_set_visible(rotulo_senha_admin_cadastro, FALSE);
                gtk_widget_set_visible(entrada_senha_admin_cadastro, FALSE);
            }
        } else { // Convidado selecionado
            gtk_widget_set_visible(rotulo_senha_admin_cadastro, FALSE);
            gtk_widget_set_visible(entrada_senha_admin_cadastro, FALSE);
        }
    }
}

static void on_registrar_novo_usuario_admin_clicado(GtkButton *botao, gpointer dados_usuario) {
    const char *usuario = gtk_entry_get_text(GTK_ENTRY(entrada_novo_usuario_admin));
    const char *senha = gtk_entry_get_text(GTK_ENTRY(entrada_nova_senha_admin));
    const char *dica = gtk_entry_get_text(GTK_ENTRY(entrada_nova_dica_admin));
    int permissao = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radio_permissao_admin_novo_usuario_admin)) ? 1 : 0;
    const char *senha_admin = gtk_entry_get_text(GTK_ENTRY(entrada_senha_admin_novo_usuario_admin));
    char mensagem[256];

    if (registrarUsuario(usuario, senha, dica, permissao, senha_admin, mensagem)) {
        gtk_label_set_text(GTK_LABEL(rotulo_status_novo_usuario_admin), mensagem);
        gtk_entry_set_text(GTK_ENTRY(entrada_novo_usuario_admin), "");
        gtk_entry_set_text(GTK_ENTRY(entrada_nova_senha_admin), "");
        gtk_entry_set_text(GTK_ENTRY(entrada_nova_dica_admin), "");
        gtk_entry_set_text(GTK_ENTRY(entrada_senha_admin_novo_usuario_admin), "");
        atualizar_exibicao_lista_usuarios();
    } else {
        gtk_label_set_text(GTK_LABEL(rotulo_status_novo_usuario_admin), mensagem);
    }
}

// Callback para os botões de rádio de permissão na criação de novo usuário pelo admin
static void on_permissao_novo_usuario_admin_alternada(GtkToggleButton *botao, gpointer dados_usuario) {
    if (gtk_toggle_button_get_active(botao)) {
        if (botao == GTK_TOGGLE_BUTTON(radio_permissao_admin_novo_usuario_admin)) {
            gtk_widget_set_visible(rotulo_senha_admin_novo_usuario_admin, TRUE);
            gtk_widget_set_visible(entrada_senha_admin_novo_usuario_admin, TRUE);
        } else {
            gtk_widget_set_visible(rotulo_senha_admin_novo_usuario_admin, FALSE);
            gtk_widget_set_visible(entrada_senha_admin_novo_usuario_admin, FALSE);
        }
    }
}

static void on_voltar_para_login_clicado(GtkButton *botao, gpointer dados_usuario) {
    usuario_logado_idx = -1;
    gtk_stack_set_visible_child_name(GTK_STACK(pilha_telas), "tela_login");
    gtk_label_set_text(GTK_LABEL(rotulo_status_login), "Você foi desconectado.");

    if (existe_admin()) {
        gtk_widget_set_visible(botao_ir_cadastro_login, FALSE);
    } else {
        gtk_widget_set_visible(botao_ir_cadastro_login, TRUE);
    }
}

static void on_logout_clicado(GtkButton *botao, gpointer dados_usuario) {
    g_print("Agora era pra chamar o shutdown do lumaster\n");
    void Lumaster_Shutdown();
    usuario_logado_idx = -1;
    if (rotulo_usuario_atual_admin_inicio) {
        gtk_label_set_text(GTK_LABEL(rotulo_usuario_atual_admin_inicio), "Usuário Logado: Nenhum");
    }
    if (rotulo_usuario_atual_convidado_inicio) {
        gtk_label_set_text(GTK_LABEL(rotulo_usuario_atual_convidado_inicio), "Usuário Logado: Nenhum");
    }
    gtk_stack_set_visible_child_name(GTK_STACK(pilha_telas), "tela_login");
    gtk_label_set_text(GTK_LABEL(rotulo_status_login), "Você foi desconectado.");

    if (existe_admin()) {
        gtk_widget_set_visible(botao_ir_cadastro_login, FALSE);
    } else {
        gtk_widget_set_visible(botao_ir_cadastro_login, TRUE);
    }
}

static void on_remover_usuario_clicado(GtkButton *botao, gpointer dados_usuario) {
    const char *usuario_a_remover = gtk_entry_get_text(GTK_ENTRY(entrada_usuario_remover));
    char mensagem[256];

    if (removerUsuario_gui(usuario_a_remover, mensagem)) {
        gtk_label_set_text(GTK_LABEL(rotulo_status_remover), mensagem);
        gtk_entry_set_text(GTK_ENTRY(entrada_usuario_remover), "");
        atualizar_exibicao_lista_usuarios();
        if (!existe_admin()) {
            gtk_widget_set_visible(botao_ir_cadastro_login, TRUE);
        }
    } else {
        gtk_label_set_text(GTK_LABEL(rotulo_status_remover), mensagem);
    }
}

static void on_editar_usuario_clicado(GtkButton *botao, gpointer dados_usuario) {
    const char *usuario_original = gtk_entry_get_text(GTK_ENTRY(entrada_usuario_original_editar));
    const char *novo_usuario = gtk_entry_get_text(GTK_ENTRY(entrada_novo_usuario_editar));
    const char *nova_senha = gtk_entry_get_text(GTK_ENTRY(entrada_nova_senha_editar));
    const char *nova_dica = gtk_entry_get_text(GTK_ENTRY(entrada_nova_dica_editar));
    char mensagem[256];

    if (editarUsuario_gui(usuario_original, novo_usuario, nova_senha, nova_dica, mensagem)) {
        gtk_label_set_text(GTK_LABEL(rotulo_status_editar), mensagem);
        gtk_entry_set_text(GTK_ENTRY(entrada_usuario_original_editar), "");
        gtk_entry_set_text(GTK_ENTRY(entrada_novo_usuario_editar), "");
        gtk_entry_set_text(GTK_ENTRY(entrada_nova_senha_editar), "");
        gtk_entry_set_text(GTK_ENTRY(entrada_nova_dica_editar), "");
        atualizar_exibicao_lista_usuarios();
    } else {
        gtk_label_set_text(GTK_LABEL(rotulo_status_editar), mensagem);
    }
}

static void on_exportar_csv_clicado(GtkButton *botao, gpointer dados_usuario) {
    char mensagem[256];
    if (exportarCSV_gui(mensagem)) {
        mostrar_dialogo_mensagem(GTK_WINDOW(janela_principal), GTK_MESSAGE_INFO, "Exportar CSV", mensagem);
    } else {
        mostrar_dialogo_mensagem(GTK_WINDOW(janela_principal), GTK_MESSAGE_ERROR, "Exportar CSV", mensagem);
    }
}

static void on_listar_usuarios_clicado(GtkButton *botao, gpointer dados_usuario) {
    atualizar_exibicao_lista_usuarios();
    mostrar_dialogo_mensagem(GTK_WINDOW(janela_principal), GTK_MESSAGE_INFO, "Lista de Usuários", "A lista de usuários foi atualizada.");
}

// --- Funções de Construção da Interface ---

/**
 * @brief Cria e configura a tela de login.
 * @return O widget da tela de login.
 */
GtkWidget *criar_tela_login() {
    GtkWidget *grade = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grade), 10);
    gtk_grid_set_column_spacing(GTK_GRID(grade), 10);
    gtk_widget_set_halign(grade, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(grade, GTK_ALIGN_CENTER);

    GtkWidget *rotulo = gtk_label_new("Login do Usuário");
    aplicar_css_ao_widget(rotulo, "label { font-size: 20px; font-weight: bold; }");
    gtk_grid_attach(GTK_GRID(grade), rotulo, 0, 0, 2, 1);

    gtk_grid_attach(GTK_GRID(grade), gtk_label_new("Usuário:"), 0, 1, 1, 1);
    entrada_usuario_login = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(grade), entrada_usuario_login, 1, 1, 1, 1);

    gtk_grid_attach(GTK_GRID(grade), gtk_label_new("Senha:"), 0, 2, 1, 1);
    entrada_senha_login = gtk_entry_new();
    gtk_entry_set_visibility(GTK_ENTRY(entrada_senha_login), FALSE);
    gtk_entry_set_invisible_char(GTK_ENTRY(entrada_senha_login), '*');
    gtk_grid_attach(GTK_GRID(grade), entrada_senha_login, 1, 2, 1, 1);

    GtkWidget *botao_login = gtk_button_new_with_label("Entrar");
    g_signal_connect(botao_login, "clicked", G_CALLBACK(on_login_clicado), NULL);
    gtk_grid_attach(GTK_GRID(grade), botao_login, 0, 3, 2, 1);

    botao_ir_cadastro_login = gtk_button_new_with_label("Cadastrar novo usuário");
    g_signal_connect(botao_ir_cadastro_login, "clicked", G_CALLBACK(on_ir_para_cadastro_clicado), NULL);
    gtk_grid_attach(GTK_GRID(grade), botao_ir_cadastro_login, 0, 4, 2, 1);

    botao_esqueci_senha_login = gtk_button_new_with_label("Esqueci minha senha");
    g_signal_connect(botao_esqueci_senha_login, "clicked", G_CALLBACK(on_esqueci_senha_clicado), NULL);
    gtk_grid_attach(GTK_GRID(grade), botao_esqueci_senha_login, 0, 5, 2, 1);

    rotulo_status_login = gtk_label_new("");
    gtk_widget_set_halign(rotulo_status_login, GTK_ALIGN_CENTER);
    gtk_grid_attach(GTK_GRID(grade), rotulo_status_login, 0, 6, 2, 1);

    if (existe_admin()) {
        gtk_widget_set_visible(botao_ir_cadastro_login, FALSE);
    } else {
        gtk_widget_set_visible(botao_ir_cadastro_login, TRUE);
    }

    return grade;
}

/**
 * @brief Cria e configura a tela de cadastro de novo usuário.
 * @return O widget da tela de cadastro.
 */
GtkWidget *criar_tela_cadastro() {
    GtkWidget *grade = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grade), 10);
    gtk_grid_set_column_spacing(GTK_GRID(grade), 10);
    gtk_widget_set_halign(grade, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(grade, GTK_ALIGN_CENTER);

    GtkWidget *rotulo = gtk_label_new("Cadastro de Novo Usuário");
    aplicar_css_ao_widget(rotulo, "label { font-size: 20px; font-weight: bold; }");
    gtk_grid_attach(GTK_GRID(grade), rotulo, 0, 0, 2, 1);

    gtk_grid_attach(GTK_GRID(grade), gtk_label_new("Usuário:"), 0, 1, 1, 1);
    entrada_usuario_cadastro = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(grade), entrada_usuario_cadastro, 1, 1, 1, 1);

    gtk_grid_attach(GTK_GRID(grade), gtk_label_new("Senha:"), 0, 2, 1, 1);
    entrada_senha_cadastro = gtk_entry_new();
    gtk_entry_set_visibility(GTK_ENTRY(entrada_senha_cadastro), FALSE);
    gtk_entry_set_invisible_char(GTK_ENTRY(entrada_senha_cadastro), '*');
    gtk_grid_attach(GTK_GRID(grade), entrada_senha_cadastro, 1, 2, 1, 1);

    gtk_grid_attach(GTK_GRID(grade), gtk_label_new("Dica (para senha):"), 0, 3, 1, 1);
    entrada_dica_cadastro = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(grade), entrada_dica_cadastro, 1, 3, 1, 1);

    GtkWidget *rotulo_permissao = gtk_label_new("Permissão:");
    gtk_grid_attach(GTK_GRID(grade), rotulo_permissao, 0, 4, 1, 1);

    GtkWidget *caixa_permissao = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    radio_permissao_convidado_cadastro = gtk_radio_button_new_with_label(NULL, "Convidado");
    radio_permissao_admin_cadastro = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(radio_permissao_convidado_cadastro), "Admin");
    gtk_box_pack_start(GTK_BOX(caixa_permissao), radio_permissao_convidado_cadastro, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(caixa_permissao), radio_permissao_admin_cadastro, FALSE, FALSE, 0);
    gtk_grid_attach(GTK_GRID(grade), caixa_permissao, 1, 4, 1, 1);

    g_signal_connect(radio_permissao_admin_cadastro, "toggled", G_CALLBACK(on_permissao_cadastro_alternada), NULL);
    g_signal_connect(radio_permissao_convidado_cadastro, "toggled", G_CALLBACK(on_permissao_cadastro_alternada), NULL);

    rotulo_senha_admin_cadastro = gtk_label_new("Senha Admin (para autorizar):");
    gtk_grid_attach(GTK_GRID(grade), rotulo_senha_admin_cadastro, 0, 5, 1, 1);
    entrada_senha_admin_cadastro = gtk_entry_new();
    gtk_entry_set_visibility(GTK_ENTRY(entrada_senha_admin_cadastro), FALSE);
    gtk_entry_set_invisible_char(GTK_ENTRY(entrada_senha_admin_cadastro), '*');
    gtk_grid_attach(GTK_GRID(grade), entrada_senha_admin_cadastro, 1, 5, 1, 1);
    gtk_widget_set_visible(rotulo_senha_admin_cadastro, FALSE);
    gtk_widget_set_visible(entrada_senha_admin_cadastro, FALSE);

    GtkWidget *botao_registrar = gtk_button_new_with_label("Registrar");
    g_signal_connect(botao_registrar, "clicked", G_CALLBACK(on_registrar_clicado), NULL);
    gtk_grid_attach(GTK_GRID(grade), botao_registrar, 0, 6, 2, 1);

    GtkWidget *botao_voltar = gtk_button_new_with_label("Voltar para Login");
    g_signal_connect(botao_voltar, "clicked", G_CALLBACK(on_voltar_para_login_clicado), NULL);
    gtk_grid_attach(GTK_GRID(grade), botao_voltar, 0, 7, 2, 1);

    rotulo_status_cadastro = gtk_label_new("");
    gtk_widget_set_halign(rotulo_status_cadastro, GTK_ALIGN_CENTER);
    gtk_grid_attach(GTK_GRID(grade), rotulo_status_cadastro, 0, 8, 2, 1);

    return grade;
}

/**
 * @brief Cria e configura a tela inicial para administradores.
 * @return O widget da tela de início do administrador.
 */
GtkWidget *criar_tela_inicio_admin() {
    GtkWidget *caixa_principal_v = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_set_border_width(GTK_CONTAINER(caixa_principal_v), 10);

    rotulo_usuario_atual_admin_inicio = gtk_label_new("Usuário Logado: ");
    aplicar_css_ao_widget(rotulo_usuario_atual_admin_inicio, "label { font-style: italic; font-size: 12px; }");
    gtk_box_pack_start(GTK_BOX(caixa_principal_v), rotulo_usuario_atual_admin_inicio, FALSE, FALSE, 0);

    GtkWidget *rotulo_menu = gtk_label_new("Menu do Administrador");
    aplicar_css_ao_widget(rotulo_menu, "label { font-size: 20px; font-weight: bold; }");
    gtk_box_pack_start(GTK_BOX(caixa_principal_v), rotulo_menu, FALSE, FALSE, 0);

    GtkWidget *grade = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grade), 10);
    gtk_grid_set_column_spacing(GTK_GRID(grade), 10);
    gtk_widget_set_halign(grade, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(grade, GTK_ALIGN_CENTER);

    //GtkWidget *botao_gerenciamento_usuarios = gtk_button_new_with_label("Gerenciamento de Usuários");
    //g_signal_connect(botao_gerenciamento_usuarios, "clicked", G_CALLBACK(on_gerenciamento_usuarios_clicado), NULL);
    //gtk_grid_attach(GTK_GRID(grade), botao_gerenciamento_usuarios, 0, 0, 1, 1);

    GtkWidget *botao_financas = gtk_button_new_with_label("Gerenciamento de Finanças");
    g_signal_connect(botao_financas, "clicked", G_CALLBACK(on_gerenciamento_financas_clicado), NULL);
    gtk_grid_attach(GTK_GRID(grade), botao_financas, 1, 0, 1, 1);

    /*GtkWidget *botao_alunos = gtk_button_new_with_label("Gerenciamento de Alunos");
    g_signal_connect(botao_alunos, "clicked", G_CALLBACK(on_gerenciamento_alunos_clicado), NULL);
    gtk_grid_attach(GTK_GRID(grade), botao_alunos, 0, 1, 1, 1);*/

    /*GtkWidget *botao_viagens = gtk_button_new_with_label("Gerenciamento de Viagens");
    g_signal_connect(botao_viagens, "clicked", G_CALLBACK(on_gerenciamento_viagens_clicado), NULL);
    gtk_grid_attach(GTK_GRID(grade), botao_viagens, 1, 1, 1, 1);*/

    gtk_box_pack_start(GTK_BOX(caixa_principal_v), grade, TRUE, TRUE, 0);

    GtkWidget *botao_sair = gtk_button_new_with_label("Sair do Usuário");
    g_signal_connect(botao_sair, "clicked", G_CALLBACK(on_logout_clicado), NULL);
    gtk_box_pack_start(GTK_BOX(caixa_principal_v), botao_sair, FALSE, FALSE, 0);

    return caixa_principal_v;
}

/**
 * @brief Cria e configura a tela inicial para convidados.
 * @return O widget da tela de início do convidado.
 */
GtkWidget *criar_tela_inicio_convidado() {
    GtkWidget *caixa_principal_v = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_set_border_width(GTK_CONTAINER(caixa_principal_v), 10);

    rotulo_usuario_atual_convidado_inicio = gtk_label_new("Usuário Logado: ");
    aplicar_css_ao_widget(rotulo_usuario_atual_convidado_inicio, "label { font-style: italic; font-size: 12px; }");
    gtk_box_pack_start(GTK_BOX(caixa_principal_v), rotulo_usuario_atual_convidado_inicio, FALSE, FALSE, 0);

    GtkWidget *rotulo_menu = gtk_label_new("Menu do Convidado (Acesso Limitado)");
    aplicar_css_ao_widget(rotulo_menu, "label { font-size: 20px; font-weight: bold; }");
    gtk_box_pack_start(GTK_BOX(caixa_principal_v), rotulo_menu, FALSE, FALSE, 0);



    GtkWidget *botao_sair = gtk_button_new_with_label("Sair do Usuário");
    g_signal_connect(botao_sair, "clicked", G_CALLBACK(on_logout_clicado), NULL);
    gtk_box_pack_start(GTK_BOX(caixa_principal_v), botao_sair, FALSE, FALSE, 0);

    return caixa_principal_v;
}

/**
 * @brief Cria e configura a tela de gerenciamento de usuários (acessível por admin).
 * @return O widget da tela de gerenciamento de usuários.
 */
GtkWidget *criar_tela_gerenciamento_usuarios() {
    GtkWidget *caixa_principal_v = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_set_border_width(GTK_CONTAINER(caixa_principal_v), 10);

    GtkWidget *rotulo = gtk_label_new("Gerenciamento de Usuários");
    aplicar_css_ao_widget(rotulo, "label { font-size: 20px; font-weight: bold; }");
    gtk_box_pack_start(GTK_BOX(caixa_principal_v), rotulo, FALSE, FALSE, 0);

    GtkWidget *caderno = gtk_notebook_new();
    gtk_box_pack_start(GTK_BOX(caixa_principal_v), caderno, TRUE, TRUE, 0);

    // --- Aba 1: Lista de Usuários ---
    GtkWidget *caixa_lista_usuarios_v = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_set_border_width(GTK_CONTAINER(caixa_lista_usuarios_v), 10);
    visualizador_texto_lista_usuarios = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(visualizador_texto_lista_usuarios), FALSE);
    gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(visualizador_texto_lista_usuarios), FALSE);
    buffer_lista_usuarios = gtk_text_view_get_buffer(GTK_TEXT_VIEW(visualizador_texto_lista_usuarios));
    GtkWidget *window_rolagem = gtk_scrolled_window_new(NULL, NULL);
    gtk_container_add(GTK_CONTAINER(window_rolagem), visualizador_texto_lista_usuarios);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(window_rolagem),
                                   GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_box_pack_start(GTK_BOX(caixa_lista_usuarios_v), window_rolagem, TRUE, TRUE, 0);

    GtkWidget *botao_lista = gtk_button_new_with_label("Atualizar Lista de Usuários");
    g_signal_connect(botao_lista, "clicked", G_CALLBACK(on_listar_usuarios_clicado), NULL);
    gtk_box_pack_start(GTK_BOX(caixa_lista_usuarios_v), botao_lista, FALSE, FALSE, 0);

    gtk_notebook_append_page(GTK_NOTEBOOK(caderno), caixa_lista_usuarios_v, gtk_label_new("Lista de Usuários"));

    // --- Aba 2: Remover Usuário ---
    GtkWidget *grade_remover = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grade_remover), 10);
    gtk_grid_set_column_spacing(GTK_GRID(grade_remover), 10);
    gtk_container_set_border_width(GTK_CONTAINER(grade_remover), 10);

    gtk_grid_attach(GTK_GRID(grade_remover), gtk_label_new("Nome do Usuário a Remover:"), 0, 0, 1, 1);
    entrada_usuario_remover = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(grade_remover), entrada_usuario_remover, 1, 0, 1, 1);

    GtkWidget *botao_remover = gtk_button_new_with_label("Remover Usuário");
    g_signal_connect(botao_remover, "clicked", G_CALLBACK(on_remover_usuario_clicado), NULL);
    gtk_grid_attach(GTK_GRID(grade_remover), botao_remover, 0, 1, 2, 1);

    rotulo_status_remover = gtk_label_new("");
    gtk_widget_set_halign(rotulo_status_remover, GTK_ALIGN_CENTER);
    gtk_grid_attach(GTK_GRID(grade_remover), rotulo_status_remover, 0, 2, 2, 1);

    gtk_notebook_append_page(GTK_NOTEBOOK(caderno), grade_remover, gtk_label_new("Remover Usuário"));

    // --- Aba 3: Editar Usuário ---
    GtkWidget *grade_editar = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grade_editar), 10);
    gtk_grid_set_column_spacing(GTK_GRID(grade_editar), 10);
    gtk_container_set_border_width(GTK_CONTAINER(grade_editar), 10);

    gtk_grid_attach(GTK_GRID(grade_editar), gtk_label_new("Nome Original do Usuário:"), 0, 0, 1, 1);
    entrada_usuario_original_editar = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(grade_editar), entrada_usuario_original_editar, 1, 0, 1, 1);

    gtk_grid_attach(GTK_GRID(grade_editar), gtk_label_new("Novo Nome de Usuário:"), 0, 1, 1, 1);
    entrada_novo_usuario_editar = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(grade_editar), entrada_novo_usuario_editar, 1, 1, 1, 1);

    gtk_grid_attach(GTK_GRID(grade_editar), gtk_label_new("Nova Senha:"), 0, 2, 1, 1);
    entrada_nova_senha_editar = gtk_entry_new();
    gtk_entry_set_visibility(GTK_ENTRY(entrada_nova_senha_editar), FALSE);
    gtk_entry_set_invisible_char(GTK_ENTRY(entrada_nova_senha_editar), '*');
    gtk_grid_attach(GTK_GRID(grade_editar), entrada_nova_senha_editar, 1, 2, 1, 1);

    gtk_grid_attach(GTK_GRID(grade_editar), gtk_label_new("Nova Dica:"), 0, 3, 1, 1);
    entrada_nova_dica_editar = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(grade_editar), entrada_nova_dica_editar, 1, 3, 1, 1);

    GtkWidget *botao_editar = gtk_button_new_with_label("Salvar Edição");
    g_signal_connect(botao_editar, "clicked", G_CALLBACK(on_editar_usuario_clicado), NULL);
    gtk_grid_attach(GTK_GRID(grade_editar), botao_editar, 0, 4, 2, 1);

    rotulo_status_editar = gtk_label_new("");
    gtk_widget_set_halign(rotulo_status_editar, GTK_ALIGN_CENTER);
    gtk_grid_attach(GTK_GRID(grade_editar), rotulo_status_editar, 0, 5, 2, 1);

    gtk_notebook_append_page(GTK_NOTEBOOK(caderno), grade_editar, gtk_label_new("Editar Usuário"));

    // --- Aba 4: Criar Novo Usuário (Admin) ---
    GtkWidget *grade_novo_usuario = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grade_novo_usuario), 10);
    gtk_grid_set_column_spacing(GTK_GRID(grade_novo_usuario), 10);
    gtk_container_set_border_width(GTK_CONTAINER(grade_novo_usuario), 10);

    gtk_grid_attach(GTK_GRID(grade_novo_usuario), gtk_label_new("Nome de Usuário:"), 0, 0, 1, 1);
    entrada_novo_usuario_admin = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(grade_novo_usuario), entrada_novo_usuario_admin, 1, 0, 1, 1);

    gtk_grid_attach(GTK_GRID(grade_novo_usuario), gtk_label_new("Senha:"), 0, 1, 1, 1);
    entrada_nova_senha_admin = gtk_entry_new();
    gtk_entry_set_visibility(GTK_ENTRY(entrada_nova_senha_admin), FALSE);
    gtk_entry_set_invisible_char(GTK_ENTRY(entrada_nova_senha_admin), '*');
    gtk_grid_attach(GTK_GRID(grade_novo_usuario), entrada_nova_senha_admin, 1, 1, 1, 1);

    gtk_grid_attach(GTK_GRID(grade_novo_usuario), gtk_label_new("Dica (para senha):"), 0, 2, 1, 1);
    entrada_nova_dica_admin = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(grade_novo_usuario), entrada_nova_dica_admin, 1, 2, 1, 1);

    GtkWidget *rotulo_permissao_novo_usuario = gtk_label_new("Permissão:");
    gtk_grid_attach(GTK_GRID(grade_novo_usuario), rotulo_permissao_novo_usuario, 0, 3, 1, 1);

    GtkWidget *caixa_permissao_novo_usuario = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    radio_permissao_convidado_novo_usuario_admin = gtk_radio_button_new_with_label(NULL, "Convidado");
    radio_permissao_admin_novo_usuario_admin = gtk_radio_button_new_with_label_from_widget(
        GTK_RADIO_BUTTON(radio_permissao_convidado_novo_usuario_admin), "Admin");
    gtk_box_pack_start(GTK_BOX(caixa_permissao_novo_usuario), radio_permissao_convidado_novo_usuario_admin, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(caixa_permissao_novo_usuario), radio_permissao_admin_novo_usuario_admin, FALSE, FALSE, 0);
    gtk_grid_attach(GTK_GRID(grade_novo_usuario), caixa_permissao_novo_usuario, 1, 3, 1, 1);

    g_signal_connect(radio_permissao_admin_novo_usuario_admin, "toggled", G_CALLBACK(on_permissao_novo_usuario_admin_alternada), NULL);
    g_signal_connect(radio_permissao_convidado_novo_usuario_admin, "toggled", G_CALLBACK(on_permissao_novo_usuario_admin_alternada), NULL);

    rotulo_senha_admin_novo_usuario_admin = gtk_label_new("Senha Admin (para autorizar):");
    gtk_grid_attach(GTK_GRID(grade_novo_usuario), rotulo_senha_admin_novo_usuario_admin, 0, 4, 1, 1);
    entrada_senha_admin_novo_usuario_admin = gtk_entry_new();
    gtk_entry_set_visibility(GTK_ENTRY(entrada_senha_admin_novo_usuario_admin), FALSE);
    gtk_entry_set_invisible_char(GTK_ENTRY(entrada_senha_admin_novo_usuario_admin), '*');
    gtk_grid_attach(GTK_GRID(grade_novo_usuario), entrada_senha_admin_novo_usuario_admin, 1, 4, 1, 1);
    gtk_widget_set_visible(rotulo_senha_admin_novo_usuario_admin, FALSE);
    gtk_widget_set_visible(entrada_senha_admin_novo_usuario_admin, FALSE);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radio_permissao_convidado_novo_usuario_admin), TRUE);

    GtkWidget *botao_registrar_admin = gtk_button_new_with_label("Criar Novo Usuário");
    g_signal_connect(botao_registrar_admin, "clicked", G_CALLBACK(on_registrar_novo_usuario_admin_clicado), NULL);
    gtk_grid_attach(GTK_GRID(grade_novo_usuario), botao_registrar_admin, 0, 5, 2, 1);

    rotulo_status_novo_usuario_admin = gtk_label_new("");
    gtk_widget_set_halign(rotulo_status_novo_usuario_admin, GTK_ALIGN_CENTER);
    gtk_grid_attach(GTK_GRID(grade_novo_usuario), rotulo_status_novo_usuario_admin, 0, 6, 2, 1);

    gtk_notebook_append_page(GTK_NOTEBOOK(caderno), grade_novo_usuario, gtk_label_new("Criar Usuário"));

    // Controles Comuns (Exportar, Sair e Voltar para Início)
    GtkWidget *caixa_controles_comuns = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_set_border_width(GTK_CONTAINER(caixa_controles_comuns), 10);

    GtkWidget *botao_exportar_csv = gtk_button_new_with_label("Exportar Usuários para CSV");
    g_signal_connect(botao_exportar_csv, "clicked", G_CALLBACK(on_exportar_csv_clicado), NULL);
    gtk_box_pack_start(GTK_BOX(caixa_controles_comuns), botao_exportar_csv, FALSE, FALSE, 0);

    GtkWidget *botao_voltar_um = gtk_button_new_with_label("Voltar para Página Inicial");
    g_signal_connect(botao_voltar_um, "clicked", G_CALLBACK(on_voltar_para_inicio_clicado), NULL);
    gtk_box_pack_start(GTK_BOX(caixa_controles_comuns), botao_voltar_um, FALSE, FALSE, 0);

    GtkWidget *botao_sair_um = gtk_button_new_with_label("Sair do Usuário");
    g_signal_connect(botao_sair_um, "clicked", G_CALLBACK(on_logout_clicado), NULL);
    gtk_box_pack_start(GTK_BOX(caixa_controles_comuns), botao_sair_um, FALSE, FALSE, 0);

    gtk_box_pack_start(GTK_BOX(caixa_principal_v), caixa_controles_comuns, FALSE, FALSE, 0);

    return caixa_principal_v;
}

// --- Função Principal da Aplicação ---
static void ativar(GtkApplication *app, gpointer dados_usuario) {
    carregarUsuarios(); // Carrega usuários na inicialização

    janela_principal = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(janela_principal), "Gerenciador de Usuários");
    gtk_window_set_default_size(GTK_WINDOW(janela_principal), 800, 600);
    gtk_container_set_border_width(GTK_CONTAINER(janela_principal), 10);

    pilha_telas = gtk_stack_new();
    gtk_stack_set_transition_type(GTK_STACK(pilha_telas), GTK_STACK_TRANSITION_TYPE_SLIDE_LEFT_RIGHT);
    gtk_container_add(GTK_CONTAINER(janela_principal), pilha_telas);

    // Adiciona as telas à stack
    GtkWidget *tela_login = criar_tela_login();
    gtk_stack_add_named(GTK_STACK(pilha_telas), tela_login, "tela_login");

    GtkWidget *tela_cadastro = criar_tela_cadastro();
    gtk_stack_add_named(GTK_STACK(pilha_telas), tela_cadastro, "tela_cadastro");

    GtkWidget *inicio_admin = criar_tela_inicio_admin();
    gtk_stack_add_named(GTK_STACK(pilha_telas), inicio_admin, "inicio_admin");

    GtkWidget *inicio_convidado = criar_tela_inicio_convidado();
    gtk_stack_add_named(GTK_STACK(pilha_telas), inicio_convidado, "inicio_convidado");

    GtkWidget *tela_gerenciamento_usuarios = criar_tela_gerenciamento_usuarios();
    gtk_stack_add_named(GTK_STACK(pilha_telas), tela_gerenciamento_usuarios, "tela_gerenciamento_usuarios");

    gtk_stack_set_visible_child_name(GTK_STACK(pilha_telas), "tela_login");

    gtk_widget_show_all(janela_principal);
}

int main(int argc, char **argv) {
    GtkApplication *app;
    int status;

    app = gtk_application_new("org.gtk.example.user_manager", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(ativar), NULL);
    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    salvarUsuarios(); // Salva usuários ao fechar a aplicação

    return status;
}