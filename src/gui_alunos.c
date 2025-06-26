#include "gui_alunos.h"
#include "utils.h" 

// Variável global para a página de cadastro de aluno
GtkWidget *pagina_cadastro_aluno = NULL; 

// Variáveis estáticas para guardar o contexto da viagem/turma
static int current_id_viagem_cadastro_aluno = -1;
static char current_nome_turma_cadastro_aluno[MAX_NOME];

// Widgets do formulário de cadastro de aluno
static GtkEntry *entry_aluno_nome;
static GtkEntry *entry_aluno_rg;
static GtkEntry *entry_aluno_cpf;
static GtkEntry *entry_aluno_nascimento; 

// Widgets para pagamento
static GtkSpinButton *spin_aluno_data_dia_pagamento;
static GtkSpinButton *spin_aluno_data_mes_pagamento;
static GtkSpinButton *spin_aluno_data_ano_pagamento;
static GtkCheckButton *check_aluno_pagamento_credito;
static GtkSpinButton *spin_aluno_valor_entrada;
static GtkCheckButton *check_aluno_com_entrada; 

static GtkLabel *label_aluno_contexto_viagem;
static GtkLabel *label_aluno_contexto_turma;

// --- FUNÇÕES AUXILIARES E CALLBACKS ---

// Declaração antecipada
static void on_aluno_com_entrada_toggled(GtkToggleButton *toggle_button, gpointer user_data);

// Limpa o formulário de cadastro de aluno
void limpar_formulario_cadastro_aluno() {
    gtk_entry_set_text(entry_aluno_nome, "");
    gtk_entry_set_text(entry_aluno_rg, "");
    gtk_entry_set_text(entry_aluno_cpf, "");
    gtk_entry_set_text(entry_aluno_nascimento, "");
    
    gtk_spin_button_set_value(spin_aluno_data_dia_pagamento, 1);
    gtk_spin_button_set_value(spin_aluno_data_mes_pagamento, 1);
    gtk_spin_button_set_value(spin_aluno_data_ano_pagamento, 2025); 
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check_aluno_pagamento_credito), FALSE);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check_aluno_com_entrada), FALSE); 
    gtk_spin_button_set_value(spin_aluno_valor_entrada, 0.00);
    gtk_widget_set_sensitive(GTK_WIDGET(spin_aluno_valor_entrada), FALSE); 

    gtk_label_set_text(label_aluno_contexto_viagem, "Viagem: (N/A)");
    gtk_label_set_text(label_aluno_contexto_turma, "Turma: (N/A)");
    current_id_viagem_cadastro_aluno = -1;
    memset(current_nome_turma_cadastro_aluno, 0, sizeof(current_nome_turma_cadastro_aluno));
    g_print("DEBUG: Formulário de cadastro de aluno limpo. Contexto resetado.\n");
}

// Callback para o botão Salvar Aluno
static void on_salvar_aluno_clicked(GtkButton *btn, gpointer user_data) {
    const char *nome = gtk_entry_get_text(entry_aluno_nome);
    const char *rg = gtk_entry_get_text(entry_aluno_rg);
    const char *cpf_digitado = gtk_entry_get_text(entry_aluno_cpf); 
    const char *nascimento = gtk_entry_get_text(entry_aluno_nascimento);

    struct tm data_inicio_pagamento_aluno = {0};
    data_inicio_pagamento_aluno.tm_mday = gtk_spin_button_get_value_as_int(spin_aluno_data_dia_pagamento);
    data_inicio_pagamento_aluno.tm_mon = gtk_spin_button_get_value_as_int(spin_aluno_data_mes_pagamento) - 1;
    data_inicio_pagamento_aluno.tm_year = gtk_spin_button_get_value_as_int(spin_aluno_data_ano_pagamento) - 1900;
    mktime(&data_inicio_pagamento_aluno); 

    int is_credito = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(check_aluno_pagamento_credito));
    double valor_entrada = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(check_aluno_com_entrada)) ? gtk_spin_button_get_value(spin_aluno_valor_entrada) : 0.0;

    g_print("DEBUG: Tentando cadastrar aluno. Viagem ID: %d, Turma: '%s'\n",
            current_id_viagem_cadastro_aluno, current_nome_turma_cadastro_aluno);

    if (strlen(nome) == 0 || strlen(rg) == 0 || strlen(cpf_digitado) == 0 || strlen(nascimento) == 0) {
        mostrar_dialogo_aviso(NULL, "Campos Obrigatórios", "Por favor, preencha todos os campos do aluno (Nome, RG, CPF, Nascimento).");
        return;
    }
    if (mktime(&data_inicio_pagamento_aluno) == (time_t)-1) { 
        mostrar_dialogo_aviso(NULL, "Data de Início do Pagamento Inválida", "Por favor, insira uma data de início do pagamento válida.");
        return;
    }

    char* cpf_limpo = limpar_string_numerica(cpf_digitado);
    if (cpf_limpo == NULL || strlen(cpf_limpo) != 11) {
        mostrar_dialogo_aviso(NULL, "CPF Inválido", "O CPF deve conter exatamente 11 dígitos numéricos.");
        g_free(cpf_limpo); 
        return;
    }

    Aluno* aluno_existente_na_viagem_turma = buscar_aluno_por_cpf_e_viagem_turma_ptr(
                                                cpf_limpo, 
                                                current_id_viagem_cadastro_aluno, 
                                                current_nome_turma_cadastro_aluno);
    
    if (aluno_existente_na_viagem_turma != NULL) {
        mostrar_dialogo_aviso(NULL, "Aluno Duplicado na Turma", 
                                g_strdup_printf("Já existe um aluno cadastrado com este CPF (%s) "
                                                "NESTA VIAGEM E TURMA:\nNome: %s", 
                                                aluno_existente_na_viagem_turma->cpf, 
                                                aluno_existente_na_viagem_turma->nome));
        g_free(cpf_limpo); 
        return;
    }

    if (current_id_viagem_cadastro_aluno == -1 || strlen(current_nome_turma_cadastro_aluno) == 0 ||
        current_id_viagem_cadastro_aluno >= MAX_VIAGENS || !viagens[current_id_viagem_cadastro_aluno].ativa) {
        mostrar_dialogo_erro(NULL, "Erro de Contexto", "Não foi possível identificar a viagem/turma para este cadastro de aluno. Por favor, selecione novamente a viagem e turma.");
        g_print("ERRO: Contexto de viagem/turma inválido ao tentar salvar aluno.\n");
        g_free(cpf_limpo); 
        return;
    }

    gboolean turma_existe_na_viagem = FALSE;
    Viagem *v = &viagens[current_id_viagem_cadastro_aluno];
    for(int i = 0; i < v->num_turmas_cadastradas; i++){
        if(v->turmas[i].ativa && strcmp(v->turmas[i].nome_turma, current_nome_turma_cadastro_aluno) == 0){
            turma_existe_na_viagem = TRUE;
            break;
        }
    }

    if(!turma_existe_na_viagem){
        mostrar_dialogo_erro(NULL, "Erro de Contexto", "A turma especificada não foi encontrada ou está inativa na viagem selecionada. Por favor, selecione novamente.");
        g_print("ERRO: Turma '%s' não existe na viagem %d.\n", current_nome_turma_cadastro_aluno, current_id_viagem_cadastro_aluno);
        g_free(cpf_limpo); 
        return;
    }

    int aluno_id = adicionar_aluno(nome, rg, cpf_limpo, nascimento, 
                                    current_id_viagem_cadastro_aluno, 
                                    current_nome_turma_cadastro_aluno,
                                    data_inicio_pagamento_aluno, 
                                    is_credito,
                                    valor_entrada);

    if (aluno_id != -1) {
        mostrar_dialogo_info(NULL, "Cadastro de Aluno", 
                                g_strdup_printf("Aluno '%s' cadastrado com sucesso para a turma '%s' da viagem '%s'!\n"
                                                "Total de parcelas geradas: %d",
                                                nome,
                                                current_nome_turma_cadastro_aluno,
                                                viagens[current_id_viagem_cadastro_aluno].nome_viagem,
                                                alunos[aluno_id].num_parcelas_cadastradas));
        
        limpar_formulario_cadastro_aluno(); 
        mudar_para_pagina("edicao_viagem_page"); 
    } else {
        mostrar_dialogo_erro(NULL, "Erro de Cadastro", "Não foi possível cadastrar o aluno. Limite de alunos atingido ou erro interno.");
    }
    
    g_free(cpf_limpo); 
}

// Callback para habilitar/desabilitar o campo de valor de entrada
static void on_aluno_com_entrada_toggled(GtkToggleButton *toggle_button, gpointer user_data) {
    gboolean active = gtk_toggle_button_get_active(toggle_button);
    gtk_widget_set_sensitive(GTK_WIDGET(spin_aluno_valor_entrada), active);
}

// Exibe o formulário de cadastro de aluno, pré-preenchendo com dados da viagem/turma
void exibir_cadastro_aluno_para_viagem_turma(int id_viagem, const char *nome_turma) {
    g_print("DEBUG: Chamado exibir_cadastro_aluno_para_viagem_turma para Viagem ID: %d, Turma: '%s'\n", id_viagem, nome_turma);

    if (id_viagem < 0 || id_viagem >= MAX_VIAGENS || !viagens[id_viagem].ativa) {
        mostrar_dialogo_erro(NULL, "Erro", "Viagem inválida ou inativa para cadastro de aluno.");
        g_print("ERRO: ID de viagem inválido ou inativo em exibir_cadastro_aluno_para_viagem_turma.\n");
        return;
    }

    limpar_formulario_cadastro_aluno();

    current_id_viagem_cadastro_aluno = id_viagem;
    strncpy(current_nome_turma_cadastro_aluno, nome_turma, MAX_NOME - 1);
    current_nome_turma_cadastro_aluno[MAX_NOME - 1] = '\0';

    time_t now = time(NULL);
    struct tm *local_now = localtime(&now);
    gtk_spin_button_set_value(spin_aluno_data_dia_pagamento, local_now->tm_mday);
    gtk_spin_button_set_value(spin_aluno_data_mes_pagamento, local_now->tm_mon + 1);
    gtk_spin_button_set_value(spin_aluno_data_ano_pagamento, local_now->tm_year + 1900);

    gtk_label_set_text(label_aluno_contexto_viagem, 
                         g_strdup_printf("Viagem: %s", viagens[id_viagem].nome_viagem));
    gtk_label_set_text(label_aluno_contexto_turma, 
                         g_strdup_printf("Turma: %s", nome_turma));

    mudar_para_pagina("cadastro_aluno_page");
}

// --- CONSTRUÇÃO DA PÁGINA DE CADASTRO DE ALUNO ---
void construir_pagina_cadastro_aluno() {
    pagina_cadastro_aluno = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_set_border_width(GTK_CONTAINER(pagina_cadastro_aluno), 20);

    GtkWidget *label_titulo = gtk_label_new("<span size='x-large' weight='bold'>Cadastrar Aluno</span>");
    gtk_label_set_use_markup(GTK_LABEL(label_titulo), TRUE);
    gtk_label_set_xalign(GTK_LABEL(label_titulo), 0);
    gtk_box_pack_start(GTK_BOX(pagina_cadastro_aluno), label_titulo, FALSE, FALSE, 10);

    // Frame para melhor organização visual do contexto
    GtkWidget *frame_contexto = gtk_frame_new("Contexto de Cadastro");
    GtkWidget *vbox_contexto = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_set_border_width(GTK_CONTAINER(vbox_contexto), 5);
    gtk_container_add(GTK_CONTAINER(frame_contexto), vbox_contexto);

    label_aluno_contexto_viagem = GTK_LABEL(gtk_label_new("Viagem: (N/A)")); 
    gtk_label_set_xalign(GTK_LABEL(label_aluno_contexto_viagem), 0);
    gtk_box_pack_start(GTK_BOX(vbox_contexto), GTK_WIDGET(label_aluno_contexto_viagem), FALSE, FALSE, 0);

    label_aluno_contexto_turma = GTK_LABEL(gtk_label_new("Turma: (N/A)")); 
    gtk_label_set_xalign(GTK_LABEL(label_aluno_contexto_turma), 0);
    gtk_box_pack_start(GTK_BOX(vbox_contexto), GTK_WIDGET(label_aluno_contexto_turma), FALSE, FALSE, 0);

    gtk_box_pack_start(GTK_BOX(pagina_cadastro_aluno), frame_contexto, FALSE, FALSE, 10);

    // Grid para organizar os campos do formulário
    GtkWidget *grid_form = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid_form), 5);
    gtk_grid_set_column_spacing(GTK_GRID(grid_form), 10);
    gtk_container_set_border_width(GTK_CONTAINER(grid_form), 10);
    gtk_box_pack_start(GTK_BOX(pagina_cadastro_aluno), grid_form, FALSE, FALSE, 0);

    int row = 0;
    gtk_grid_attach(GTK_GRID(grid_form), gtk_label_new("Nome Completo:"), 0, row, 1, 1);
    entry_aluno_nome = GTK_ENTRY(gtk_entry_new());
    gtk_grid_attach(GTK_GRID(grid_form), GTK_WIDGET(entry_aluno_nome), 1, row++, 1, 1);

    gtk_grid_attach(GTK_GRID(grid_form), gtk_label_new("RG:"), 0, row, 1, 1);
    entry_aluno_rg = GTK_ENTRY(gtk_entry_new());
    gtk_grid_attach(GTK_GRID(grid_form), GTK_WIDGET(entry_aluno_rg), 1, row++, 1, 1);

    gtk_grid_attach(GTK_GRID(grid_form), gtk_label_new("CPF:"), 0, row, 1, 1);
    entry_aluno_cpf = GTK_ENTRY(gtk_entry_new());
    gtk_grid_attach(GTK_GRID(grid_form), GTK_WIDGET(entry_aluno_cpf), 1, row++, 1, 1);

    gtk_grid_attach(GTK_GRID(grid_form), gtk_label_new("Data de Nascimento (DD/MM/AAAA):"), 0, row, 1, 1);
    entry_aluno_nascimento = GTK_ENTRY(gtk_entry_new());
    gtk_entry_set_placeholder_text(entry_aluno_nascimento, "Ex: 01/01/2000");
    gtk_grid_attach(GTK_GRID(grid_form), GTK_WIDGET(entry_aluno_nascimento), 1, row++, 1, 1);

    // Novos campos de pagamento
    GtkWidget *frame_pagamento = gtk_frame_new("Detalhes de Pagamento");
    gtk_widget_set_margin_top(frame_pagamento, 10);
    GtkWidget *grid_pagamento = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid_pagamento), 5);
    gtk_grid_set_column_spacing(GTK_GRID(grid_pagamento), 10);
    gtk_container_set_border_width(GTK_CONTAINER(grid_pagamento), 10);
    gtk_container_add(GTK_CONTAINER(frame_pagamento), grid_pagamento);
    gtk_grid_attach(GTK_GRID(grid_form), frame_pagamento, 0, row++, 2, 1); 

    int payment_row = 0; 
    GtkWidget *hbox_data_pagamento = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_box_pack_start(GTK_BOX(hbox_data_pagamento), gtk_label_new("Data Início Pagamento (DD/MM/AAAA):"), FALSE, FALSE, 0);
    spin_aluno_data_dia_pagamento = criar_spin_button_inteiro(1, 31, 1);
    spin_aluno_data_mes_pagamento = criar_spin_button_inteiro(1, 12, 1);
    spin_aluno_data_ano_pagamento = criar_spin_button_inteiro(1900, 2100, 1);
    gtk_box_pack_start(GTK_BOX(hbox_data_pagamento), GTK_WIDGET(spin_aluno_data_dia_pagamento), FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(hbox_data_pagamento), gtk_label_new("/"), FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(hbox_data_pagamento), GTK_WIDGET(spin_aluno_data_mes_pagamento), FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(hbox_data_pagamento), gtk_label_new("/"), FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(hbox_data_pagamento), GTK_WIDGET(spin_aluno_data_ano_pagamento), FALSE, FALSE, 0);
    gtk_grid_attach(GTK_GRID(grid_pagamento), hbox_data_pagamento, 0, payment_row++, 2, 1);

    check_aluno_pagamento_credito = GTK_CHECK_BUTTON(gtk_check_button_new_with_label("Pagamento por Cartão de Crédito (Quitação automática)"));
    gtk_grid_attach(GTK_GRID(grid_pagamento), GTK_WIDGET(check_aluno_pagamento_credito), 0, payment_row++, 2, 1);

    check_aluno_com_entrada = GTK_CHECK_BUTTON(gtk_check_button_new_with_label("Incluir Valor de Entrada?"));
    gtk_grid_attach(GTK_GRID(grid_pagamento), GTK_WIDGET(check_aluno_com_entrada), 0, payment_row, 1, 1);
    g_signal_connect(check_aluno_com_entrada, "toggled", G_CALLBACK(on_aluno_com_entrada_toggled), NULL);

    spin_aluno_valor_entrada = criar_spin_button_monetario(0.00, 100000.00, 0.01);
    gtk_widget_set_sensitive(GTK_WIDGET(spin_aluno_valor_entrada), FALSE); 
    gtk_grid_attach(GTK_GRID(grid_pagamento), gtk_label_new("Valor da Entrada (R$):"), 1, payment_row, 1, 1);
    gtk_grid_attach(GTK_GRID(grid_pagamento), GTK_WIDGET(spin_aluno_valor_entrada), 2, payment_row++, 1, 1);

    // Botões de Ação
    GtkWidget *hbox_buttons = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_widget_set_halign(hbox_buttons, GTK_ALIGN_END);
    gtk_box_pack_end(GTK_BOX(pagina_cadastro_aluno), hbox_buttons, FALSE, FALSE, 10); 

    GtkWidget *btn_salvar_aluno = gtk_button_new_with_label("Cadastrar Aluno");
    g_signal_connect(btn_salvar_aluno, "clicked", G_CALLBACK(on_salvar_aluno_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(hbox_buttons), btn_salvar_aluno, FALSE, FALSE, 0);

    GtkWidget *btn_voltar = gtk_button_new_with_label("Voltar");
    g_signal_connect_swapped(btn_voltar, "clicked", G_CALLBACK(limpar_formulario_cadastro_aluno), NULL);
    g_signal_connect(btn_voltar, "clicked", G_CALLBACK(mudar_para_pagina), (gpointer)"edicao_viagem_page"); 
    gtk_box_pack_start(GTK_BOX(hbox_buttons), btn_voltar, FALSE, FALSE, 0);

    gtk_widget_show_all(pagina_cadastro_aluno);
}