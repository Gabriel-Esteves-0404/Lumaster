#include "gerenciador_viagens.h"
#include "alunos_gerenciador.h"
#include "viagensmain.h"
#include "utils.h"
#include "gui_cadastro_viagem.h"
#include <time.h>

#define MAX_ALUNOS_POR_TURMA 100

GtkWidget *pagina_cadastro_viagem = NULL;

static Viagem viagem_em_cadastro; // Dados da viagem em cadastro

static Turma *turmas_em_cadastro[MAX_TURMAS_POR_VIAGEM];
static int num_turmas_em_cadastro = 0;

static void atualizar_listbox_turmas();

static GtkSpinButton *spin_data_dia;
static GtkSpinButton *spin_data_mes;
static GtkSpinButton *spin_data_ano;

static GtkEntry *entry_nome_viagem;
static GtkEntry *entry_nome_escola;
static GtkEntry *entry_destino;
static GtkSpinButton *spin_dias;
static GtkSpinButton *spin_num_guias_viagem;
static GtkCheckButton *check_opcao_guia_local;
static GtkSpinButton *spin_guia_local_valor;
static GtkSpinButton *spin_convidados;
static GtkSpinButton *spin_motoristas;
static GtkCheckButton *check_opcao_viagem_extra;
static GtkEntry *entry_viagem_extra_nome;
static GtkSpinButton *spin_viagem_extra_valor;
static GtkSpinButton *spin_min_qte_alunos;

static GtkSpinButton *spin_onibus;
static GtkSpinButton *spin_hotel;
static GtkSpinButton *spin_actes_por_professor;
static GtkSpinButton *spin_pulseiras;
static GtkSpinButton *spin_etiquetas;

static GtkSpinButton *spin_guia_viagem_valor_total;
static GtkSpinButton *spin_despesas_guia_viagem;
static GtkSpinButton *spin_despesas_motorista;
static GtkSpinButton *spin_despesas_luciana;
static GtkSpinButton *spin_taxa_apoio_escola;

static GtkSpinButton *spin_farmacia;
static GtkSpinButton *spin_gasto_extra_geral;
static GtkSpinButton *spin_emergencia;
static GtkSpinButton *spin_poupanca_lumaster;
static GtkSpinButton *spin_seguro_viagem;
static GtkListBox *listbox_outros_custos;

static GtkSpinButton *spin_val_almo_alunos_dias[MAX_DIAS_VIAGEM];
static GtkSpinButton *spin_val_jantar_alunos_dias[MAX_DIAS_VIAGEM];
static GtkSpinButton *spin_val_almo_profs_dias[MAX_DIAS_VIAGEM];
static GtkSpinButton *spin_val_jantares_profs_dias[MAX_DIAS_VIAGEM];

static GtkSpinButton *spin_quant_almocos_alunos_total_dias;
static GtkSpinButton *spin_quant_jantares_alunos_total_dias;
static GtkSpinButton *spin_quant_almocos_profs_total_dias;
static GtkSpinButton *spin_quant_jantares_profs_total_dias;
static GtkSpinButton *spin_val_servico_bordo;
static GtkSpinButton *spin_val_alimentacao_extra;

static GtkSpinButton *spin_percentual_lucro_desejado;
static GtkLabel *label_custo_total_base;
static GtkLabel *label_custo_aluno_base;
static GtkLabel *label_lucro_calculado;
static GtkLabel *label_custo_final_total;
static GtkLabel *label_custo_final_por_aluno;

static GtkWidget *grid_custos_alimentacao;
static GtkListBox *listbox_turmas_cadastradas;

void on_guia_local_toggled(GtkToggleButton *toggle_button, gpointer user_data) {
    gboolean active = gtk_toggle_button_get_active(toggle_button);
    gtk_widget_set_sensitive(GTK_WIDGET(spin_guia_local_valor), active);
}

void on_viagem_extra_toggled(GtkToggleButton *toggle_button, gpointer user_data) {
    gboolean active = gtk_toggle_button_get_active(toggle_button);
    gtk_widget_set_sensitive(GTK_WIDGET(entry_viagem_extra_nome), active);
    gtk_widget_set_sensitive(GTK_WIDGET(spin_viagem_extra_valor), active);
}

void on_quant_almocos_alunos_changed(GtkSpinButton *spin_button, gpointer user_data) {
    GtkGrid *grid = GTK_GRID(user_data);
    int num_dias = gtk_spin_button_get_value_as_int(spin_button);
    int base_row = 1;

    for (int i = 0; i < MAX_DIAS_VIAGEM; i++) {
        gboolean visible = (i < num_dias);
        if (spin_val_almo_alunos_dias[i]) {
            gtk_widget_set_visible(gtk_grid_get_child_at(GTK_GRID(grid), 0, base_row + i), visible);
            gtk_widget_set_visible(GTK_WIDGET(spin_val_almo_alunos_dias[i]), visible);
        }
    }
}

void on_quant_jantares_alunos_changed(GtkSpinButton *spin_button, gpointer user_data) {
    GtkGrid *grid = GTK_GRID(user_data);
    int num_dias = gtk_spin_button_get_value_as_int(spin_button);
    int base_row = (1 + MAX_DIAS_VIAGEM + 1);
    
    for (int i = 0; i < MAX_DIAS_VIAGEM; i++) {
        gboolean visible = (i < num_dias);
        if (spin_val_jantar_alunos_dias[i]) {
            gtk_widget_set_visible(gtk_grid_get_child_at(GTK_GRID(grid), 0, base_row + i), visible);
            gtk_widget_set_visible(GTK_WIDGET(spin_val_jantar_alunos_dias[i]), visible);
        }
    }
}

void on_quant_almocos_profs_changed(GtkSpinButton *spin_button, gpointer user_data) {
    GtkGrid *grid = GTK_GRID(user_data);
    int num_dias = gtk_spin_button_get_value_as_int(spin_button);
    int base_row = (1 + MAX_DIAS_VIAGEM + 1) + (1 + MAX_DIAS_VIAGEM + 1);
    
    for (int i = 0; i < MAX_DIAS_VIAGEM; i++) {
        gboolean visible = (i < num_dias);
        if (spin_val_almo_profs_dias[i]) {
            gtk_widget_set_visible(gtk_grid_get_child_at(GTK_GRID(grid), 0, base_row + i), visible);
            gtk_widget_set_visible(GTK_WIDGET(spin_val_almo_profs_dias[i]), visible);
        }
    }
}

void on_quant_jantares_profs_changed(GtkSpinButton *spin_button, gpointer user_data) {
    GtkGrid *grid = GTK_GRID(user_data);
    int num_dias = gtk_spin_button_get_value_as_int(spin_button);
    int base_row = (1 + MAX_DIAS_VIAGEM + 1) + (1 + MAX_DIAS_VIAGEM + 1) + (1 + MAX_DIAS_VIAGEM + 1);
    
    for (int i = 0; i < MAX_DIAS_VIAGEM; i++) {
        gboolean visible = (i < num_dias);
        if (spin_val_jantares_profs_dias[i]) {
            gtk_widget_set_visible(gtk_grid_get_child_at(GTK_GRID(grid), 0, base_row + i), visible);
            gtk_widget_set_visible(GTK_WIDGET(spin_val_jantares_profs_dias[i]), visible);
        }
    }
}

void on_dias_viagem_changed(GtkSpinButton *spin_button, gpointer user_data) {
    int num_dias = gtk_spin_button_get_value_as_int(spin_button);

    gtk_adjustment_set_upper(gtk_spin_button_get_adjustment(spin_quant_almocos_alunos_total_dias), num_dias);
    gtk_adjustment_set_upper(gtk_spin_button_get_adjustment(spin_quant_jantares_alunos_total_dias), num_dias);
    gtk_adjustment_set_upper(gtk_spin_button_get_adjustment(spin_quant_almocos_profs_total_dias), num_dias);
    gtk_adjustment_set_upper(gtk_spin_button_get_adjustment(spin_quant_jantares_profs_total_dias), num_dias);

    if (gtk_spin_button_get_value_as_int(spin_quant_almocos_alunos_total_dias) > num_dias) {
        gtk_spin_button_set_value(spin_quant_almocos_alunos_total_dias, num_dias);
    } else {
        on_quant_almocos_alunos_changed(spin_quant_almocos_alunos_total_dias, user_data);
    }

    if (gtk_spin_button_get_value_as_int(spin_quant_jantares_alunos_total_dias) > num_dias) {
        gtk_spin_button_set_value(spin_quant_jantares_alunos_total_dias, num_dias);
    } else {
        on_quant_jantares_alunos_changed(spin_quant_jantares_alunos_total_dias, user_data);
    }

    if (gtk_spin_button_get_value_as_int(spin_quant_almocos_profs_total_dias) > num_dias) {
        gtk_spin_button_set_value(spin_quant_almocos_profs_total_dias, num_dias);
    } else {
        on_quant_almocos_profs_changed(spin_quant_almocos_profs_total_dias, user_data);
    }

    if (gtk_spin_button_get_value_as_int(spin_quant_jantares_profs_total_dias) > num_dias) {
        gtk_spin_button_set_value(spin_quant_jantares_profs_total_dias, num_dias);
    } else {
        on_quant_jantares_profs_changed(spin_quant_jantares_profs_total_dias, user_data);
    }
}

static void on_calcular_lucro_changed(GtkSpinButton *spin_button, gpointer user_data) {
    strncpy(viagem_em_cadastro.nome_viagem, gtk_entry_get_text(entry_nome_viagem), MAX_NOME - 1);
    viagem_em_cadastro.nome_viagem[MAX_NOME - 1] = '\0';
    strncpy(viagem_em_cadastro.nome_escola, gtk_entry_get_text(entry_nome_escola), MAX_NOME - 1);
    viagem_em_cadastro.nome_escola[MAX_NOME - 1] = '\0';
    strncpy(viagem_em_cadastro.destino, gtk_entry_get_text(entry_destino), 49);
    viagem_em_cadastro.destino[49] = '\0';

    viagem_em_cadastro.data_da_viagem.tm_mday = gtk_spin_button_get_value_as_int(spin_data_dia);
    viagem_em_cadastro.data_da_viagem.tm_mon = gtk_spin_button_get_value_as_int(spin_data_mes) - 1;
    viagem_em_cadastro.data_da_viagem.tm_year = gtk_spin_button_get_value_as_int(spin_data_ano) - 1900;
    
    viagem_em_cadastro.dias = gtk_spin_button_get_value_as_int(spin_dias);
    viagem_em_cadastro.numguias_viagem = gtk_spin_button_get_value_as_int(spin_num_guias_viagem);
    viagem_em_cadastro.opcao_guia_local = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(check_opcao_guia_local)) ? 'S' : 'N';
    viagem_em_cadastro.guialocalvalor = gtk_spin_button_get_value(spin_guia_local_valor);
    viagem_em_cadastro.convidados = gtk_spin_button_get_value_as_int(spin_convidados);
    viagem_em_cadastro.motoristas = gtk_spin_button_get_value_as_int(spin_motoristas);
    viagem_em_cadastro.opcao_viagem_extra = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(check_opcao_viagem_extra)) ? 'S' : 'N';
    strncpy(viagem_em_cadastro.viagemextra_nome, gtk_entry_get_text(entry_viagem_extra_nome), 49);
    viagem_em_cadastro.viagemextra_nome[49] = '\0';
    viagem_em_cadastro.viagemextra_valor = gtk_spin_button_get_value(spin_viagem_extra_valor);
    viagem_em_cadastro.minQteAlunos = gtk_spin_button_get_value_as_int(spin_min_qte_alunos);

    viagem_em_cadastro.onibus = gtk_spin_button_get_value(spin_onibus);
    viagem_em_cadastro.hotel = gtk_spin_button_get_value(spin_hotel);
    viagem_em_cadastro.actes_por_professor = gtk_spin_button_get_value(spin_actes_por_professor);
    viagem_em_cadastro.pulseiras = gtk_spin_button_get_value(spin_pulseiras);
    viagem_em_cadastro.etiquetas = gtk_spin_button_get_value(spin_etiquetas);

    viagem_em_cadastro.guia_viagem_valor_total = gtk_spin_button_get_value(spin_guia_viagem_valor_total);
    viagem_em_cadastro.despesas_guia_viagem = gtk_spin_button_get_value(spin_despesas_guia_viagem);
    viagem_em_cadastro.despesas_motorista = gtk_spin_button_get_value(spin_despesas_motorista);
    viagem_em_cadastro.despesas_luciana = gtk_spin_button_get_value(spin_despesas_luciana);
    viagem_em_cadastro.taxa_apoio_escola = gtk_spin_button_get_value(spin_taxa_apoio_escola);

    viagem_em_cadastro.farmacia = gtk_spin_button_get_value(spin_farmacia);
    viagem_em_cadastro.gasto_extra_geral = gtk_spin_button_get_value(spin_gasto_extra_geral);
    viagem_em_cadastro.emergencia = gtk_spin_button_get_value(spin_emergencia);
    viagem_em_cadastro.poupanca_lumaster = gtk_spin_button_get_value(spin_poupanca_lumaster);
    viagem_em_cadastro.seguro_viagem = gtk_spin_button_get_value(spin_seguro_viagem);

    viagem_em_cadastro.num_outros_custos_diversos = 0;
    GList *children_other_costs = gtk_container_get_children(GTK_CONTAINER(listbox_outros_custos));
    GList *iter_other_costs = NULL;
    for (iter_other_costs = children_other_costs; iter_other_costs != NULL; iter_other_costs = g_list_next(iter_other_costs)) {
        GtkWidget *row_widget = GTK_WIDGET(iter_other_costs->data);
        GtkEntry *entry_desc = GTK_ENTRY(g_object_get_data(G_OBJECT(row_widget), "entry_desc"));
        GtkSpinButton *spin_val = GTK_SPIN_BUTTON(g_object_get_data(G_OBJECT(row_widget), "spin_valor"));

        if (entry_desc && spin_val && viagem_em_cadastro.num_outros_custos_diversos < MAX_ITENS_OUTROS) {
            strncpy(viagem_em_cadastro.outros_custos_diversos[viagem_em_cadastro.num_outros_custos_diversos].descricao,
                    gtk_entry_get_text(entry_desc), MAX_DESCRICAO_GASTO - 1);
            viagem_em_cadastro.outros_custos_diversos[viagem_em_cadastro.num_outros_custos_diversos].descricao[MAX_DESCRICAO_GASTO - 1] = '\0';
            viagem_em_cadastro.outros_custos_diversos[viagem_em_cadastro.num_outros_custos_diversos].valor = gtk_spin_button_get_value(spin_val);
            viagem_em_cadastro.outros_custos_diversos[viagem_em_cadastro.num_outros_custos_diversos].ativa = 1;
            viagem_em_cadastro.num_outros_custos_diversos++;
        }
    }
    g_list_free(children_other_costs);

    viagem_em_cadastro.quant_almocos_alunos_total_dias = gtk_spin_button_get_value_as_int(spin_quant_almocos_alunos_total_dias);
    for (int i = 0; i < viagem_em_cadastro.quant_almocos_alunos_total_dias && i < MAX_DIAS_VIAGEM; ++i) {
        viagem_em_cadastro.val_almo_alunos_dias[i] = gtk_spin_button_get_value(spin_val_almo_alunos_dias[i]);
    }
    viagem_em_cadastro.quant_jantares_alunos_total_dias = gtk_spin_button_get_value_as_int(spin_quant_jantares_alunos_total_dias);
    for (int i = 0; i < viagem_em_cadastro.quant_jantares_alunos_total_dias && i < MAX_DIAS_VIAGEM; ++i) {
        viagem_em_cadastro.val_jantar_alunos_dias[i] = gtk_spin_button_get_value(spin_val_jantar_alunos_dias[i]);
    }
    viagem_em_cadastro.quant_almocos_profs_total_dias = gtk_spin_button_get_value_as_int(spin_quant_almocos_profs_total_dias);
    for (int i = 0; i < viagem_em_cadastro.quant_almocos_profs_total_dias && i < MAX_DIAS_VIAGEM; ++i) {
        viagem_em_cadastro.val_almo_profs_dias[i] = gtk_spin_button_get_value(spin_val_almo_profs_dias[i]);
    }
    viagem_em_cadastro.quant_jantares_profs_total_dias = gtk_spin_button_get_value_as_int(spin_quant_jantares_profs_total_dias);
    for (int i = 0; i < viagem_em_cadastro.quant_jantares_profs_total_dias && i < MAX_DIAS_VIAGEM; ++i) {
        viagem_em_cadastro.val_jantares_profs_dias[i] = gtk_spin_button_get_value(spin_val_jantares_profs_dias[i]);
    }
    viagem_em_cadastro.val_servico_bordo = gtk_spin_button_get_value(spin_val_servico_bordo);
    viagem_em_cadastro.val_alimentacao_extra = gtk_spin_button_get_value(spin_val_alimentacao_extra);

    viagem_em_cadastro.percentual_lucro_desejado = gtk_spin_button_get_value(spin_percentual_lucro_desejado);

    viagem_em_cadastro.num_turmas_cadastradas = 0;
    for (int i = 0; i < num_turmas_em_cadastro; i++) {
        if (turmas_em_cadastro[i]) {
            viagem_em_cadastro.turmas[viagem_em_cadastro.num_turmas_cadastradas] = *turmas_em_cadastro[i];
            viagem_em_cadastro.turmas[viagem_em_cadastro.num_turmas_cadastradas].ativa = 1;
            viagem_em_cadastro.num_turmas_cadastradas++;
        }
    }

    double total_base_calculado, custo_aluno_base_simulacao, dummy1, dummy2, dummy3, dummy4;
    calcular_custos_base(&viagem_em_cadastro, &total_base_calculado, &custo_aluno_base_simulacao, &dummy1, &dummy2, &dummy3, &dummy4);

    double lucro_perc = viagem_em_cadastro.percentual_lucro_desejado;
    double lucro_total = total_base_calculado * (lucro_perc / 100.0);
    double total_final_simulacao = total_base_calculado + lucro_total;
    double custo_final_aluno_simulacao = (viagem_em_cadastro.minQteAlunos > 0) ? total_final_simulacao / viagem_em_cadastro.minQteAlunos : total_final_simulacao;

    viagem_em_cadastro.totalPorAluno = custo_final_aluno_simulacao;

    char buffer[100];
    snprintf(buffer, sizeof(buffer), "R$ %.2f", total_base_calculado);
    gtk_label_set_text(label_custo_total_base, buffer);

    if (viagem_em_cadastro.minQteAlunos > 0) {
        snprintf(buffer, sizeof(buffer), "R$ %.2f", custo_aluno_base_simulacao);
        gtk_label_set_text(label_custo_aluno_base, buffer);
    } else {
        gtk_label_set_text(label_custo_aluno_base, "N/A (Min. Alunos = 0)");
    }
    
    snprintf(buffer, sizeof(buffer), "R$ %.2f (%.2f%%)", lucro_total, lucro_perc);
    gtk_label_set_text(label_lucro_calculado, buffer);

    snprintf(buffer, sizeof(buffer), "R$ %.2f", total_final_simulacao);
    gtk_label_set_text(label_custo_final_total, buffer);

    if (viagem_em_cadastro.minQteAlunos > 0) {
        snprintf(buffer, sizeof(buffer), "R$ %.2f", viagem_em_cadastro.totalPorAluno);
        gtk_label_set_text(label_custo_final_por_aluno, buffer);
    } else {
        gtk_label_set_text(label_custo_final_por_aluno, "N/A (Min. Alunos = 0)");
    }
}

static void on_remover_turma_clicked(GtkButton *btn, gpointer user_data) {
    int turma_index_to_remove = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(btn), "turma_index"));
    GtkWidget *parent_window = gtk_widget_get_ancestor(GTK_WIDGET(btn), GTK_TYPE_WINDOW);

    if (mostrar_dialogo_confirmacao(GTK_WINDOW(parent_window), "Confirmar Remoção", "Tem certeza que deseja remover esta turma?")) {
        if (turma_index_to_remove >= 0 && turma_index_to_remove < num_turmas_em_cadastro) {
            if (turmas_em_cadastro[turma_index_to_remove]) {
                g_free(turmas_em_cadastro[turma_index_to_remove]);
                turmas_em_cadastro[turma_index_to_remove] = NULL;
            }

            for (int i = turma_index_to_remove; i < num_turmas_em_cadastro - 1; i++) {
                turmas_em_cadastro[i] = turmas_em_cadastro[i + 1];
            }
            turmas_em_cadastro[num_turmas_em_cadastro - 1] = NULL;
            num_turmas_em_cadastro--;

            atualizar_listbox_turmas();
            mostrar_dialogo_info(GTK_WINDOW(parent_window), "Turma Removida", "Turma removida com sucesso.");
        }
    }
}

static void atualizar_listbox_turmas() {
    GList *children_turmas, *iter_turmas;
    children_turmas = gtk_container_get_children(GTK_CONTAINER(listbox_turmas_cadastradas));
    for (iter_turmas = children_turmas; iter_turmas != NULL; iter_turmas = g_list_next(iter_turmas)) {
        gtk_widget_destroy(GTK_WIDGET(iter_turmas->data));
    }
    g_list_free(children_turmas);

    if (num_turmas_em_cadastro == 0) {
        GtkWidget *label = gtk_label_new("Nenhuma turma cadastrada para esta viagem.");
        gtk_list_box_insert(listbox_turmas_cadastradas, label, -1);
        gtk_widget_show(label);
    } else {
        for (int i = 0; i < num_turmas_em_cadastro; i++) {
            Turma *t = turmas_em_cadastro[i];
            if (t) {
                char turma_info[256];
                snprintf(turma_info, sizeof(turma_info),
                         "<b>%s</b>: %d alunos, %d professores",
                         t->nome_turma, t->alunos, t->numprofessores);

                GtkWidget *row_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
                GtkWidget *label = gtk_label_new(NULL);
                gtk_label_set_markup(GTK_LABEL(label), turma_info);
                gtk_label_set_xalign(GTK_LABEL(label), 0);
                gtk_box_pack_start(GTK_BOX(row_box), label, TRUE, TRUE, 0);

                GtkWidget *btn_remover_turma = gtk_button_new_with_label("Remover");
                g_object_set_data(G_OBJECT(btn_remover_turma), "turma_index", GINT_TO_POINTER(i));
                g_signal_connect(btn_remover_turma, "clicked", G_CALLBACK(on_remover_turma_clicked), NULL);
                gtk_box_pack_end(GTK_BOX(row_box), btn_remover_turma, FALSE, FALSE, 0);

                GtkWidget *row = gtk_list_box_row_new();
                gtk_container_add(GTK_CONTAINER(row), row_box);
                gtk_list_box_insert(listbox_turmas_cadastradas, row, -1);
                gtk_widget_show_all(row);
            }
        }
    }
}

void abrir_dialogo_cadastro_turma(GtkWindow *parent_window) {
    if (num_turmas_em_cadastro >= MAX_TURMAS_POR_VIAGEM) {
        mostrar_dialogo_aviso(parent_window, "Limite de Turmas", "Esta viagem já atingiu o número máximo de turmas permitidas.");
        return;
    }

    GtkWidget *dialog = gtk_dialog_new_with_buttons("Adicionar Nova Turma",
                                                    parent_window,
                                                    GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                                                    "_Adicionar", GTK_RESPONSE_ACCEPT,
                                                    "_Cancelar", GTK_RESPONSE_CANCEL,
                                                    NULL);
    gtk_window_set_default_size(GTK_WINDOW(dialog), 300, 200);

    GtkWidget *content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 10);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 5);
    gtk_container_set_border_width(GTK_CONTAINER(grid), 10);
    gtk_container_add(GTK_CONTAINER(content_area), grid);

    GtkWidget *label_nome_turma = gtk_label_new("Nome da Turma:");
    GtkWidget *entry_nome_turma = gtk_entry_new();
    GtkWidget *label_professores = gtk_label_new("Quant. Professores:");
    GtkSpinButton *spin_professores = GTK_SPIN_BUTTON(gtk_spin_button_new_with_range(0, 50, 1));

    gtk_grid_attach(GTK_GRID(grid), label_nome_turma, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), entry_nome_turma, 1, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), label_professores, 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), GTK_WIDGET(spin_professores), 1, 1, 1, 1);

    gtk_widget_show_all(dialog);

    gint response = gtk_dialog_run(GTK_DIALOG(dialog));

    if (response == GTK_RESPONSE_ACCEPT) {
        const char *nome_turma_digitado = gtk_entry_get_text(GTK_ENTRY(entry_nome_turma));
        if (strlen(nome_turma_digitado) == 0) {
            mostrar_dialogo_aviso(parent_window, "Campo Obrigatório", "Por favor, digite o nome da turma.");
            gtk_widget_destroy(dialog);
            return;
        }

        gboolean turma_ja_existe = FALSE;
        for (int i = 0; i < num_turmas_em_cadastro; i++) {
            if (turmas_em_cadastro[i] && turmas_em_cadastro[i]->ativa && strcmp(turmas_em_cadastro[i]->nome_turma, nome_turma_digitado) == 0) {
                turma_ja_existe = TRUE;
                break;
            }
        }
        if (turma_ja_existe) {
            mostrar_dialogo_aviso(parent_window, "Turma Duplicada", "Já existe uma turma com este nome. Escolha outro nome.");
            gtk_widget_destroy(dialog);
            return;
        }

        int new_turma_idx = num_turmas_em_cadastro;

        turmas_em_cadastro[new_turma_idx] = g_malloc(sizeof(Turma));
        Turma *new_turma = turmas_em_cadastro[new_turma_idx];

        if (new_turma) {
            strncpy(new_turma->nome_turma, nome_turma_digitado, MAX_NOME - 1);
            new_turma->nome_turma[MAX_NOME - 1] = '\0';
            new_turma->alunos = 0;
            new_turma->numprofessores = gtk_spin_button_get_value_as_int(spin_professores);
            new_turma->ativa = 1;

            num_turmas_em_cadastro++;
            atualizar_listbox_turmas();
            mostrar_dialogo_info(parent_window, "Turma Adicionada", "Turma adicionada com sucesso ao cadastro da viagem!");
        } else {
            mostrar_dialogo_erro(parent_window, "Erro", "Falha ao alocar memória para a turma.");
        }
    }

    gtk_widget_destroy(dialog);
}

static void on_adicionar_turma_clicked(GtkButton *btn, gpointer user_data) {
    GtkWindow *parent_window = GTK_WINDOW(gtk_widget_get_ancestor(GTK_WIDGET(btn), GTK_TYPE_WINDOW));
    abrir_dialogo_cadastro_turma(parent_window);
}

static void on_add_outro_custo_clicked(GtkButton *btn, gpointer user_data) {
    GList *current_children = gtk_container_get_children(GTK_CONTAINER(listbox_outros_custos));
    guint num_current_children = g_list_length(current_children);
    g_list_free(current_children);

    if (num_current_children >= MAX_ITENS_OUTROS) {
        mostrar_dialogo_aviso(GTK_WINDOW(gtk_widget_get_ancestor(GTK_WIDGET(btn), GTK_TYPE_WINDOW)),
                              "Limite Atingido", "Número máximo de outros custos adicionais atingido.");
        return;
    }

    GtkWidget *row_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_widget_set_margin_bottom(row_box, 5);

    GtkWidget *label_desc = gtk_label_new("Descrição:");
    gtk_box_pack_start(GTK_BOX(row_box), label_desc, FALSE, FALSE, 0);

    GtkEntry *entry_desc = GTK_ENTRY(gtk_entry_new());
    gtk_entry_set_placeholder_text(entry_desc, "Ex: Entrada Parque");
    gtk_box_pack_start(GTK_BOX(row_box), GTK_WIDGET(entry_desc), TRUE, TRUE, 0);

    GtkWidget *label_valor = gtk_label_new("Valor (R$):");
    gtk_box_pack_start(GTK_BOX(row_box), label_valor, FALSE, FALSE, 0);

    GtkSpinButton *spin_valor = GTK_SPIN_BUTTON(gtk_spin_button_new_with_range(0.00, 100000.00, 0.01));
    gtk_box_pack_start(GTK_BOX(row_box), GTK_WIDGET(spin_valor), FALSE, FALSE, 0);

    GtkWidget *btn_remove = gtk_button_new_from_icon_name("list-remove-symbolic", GTK_ICON_SIZE_SMALL_TOOLBAR);
    gtk_button_set_relief(GTK_BUTTON(btn_remove), GTK_RELIEF_NONE);
    gtk_box_pack_end(GTK_BOX(row_box), btn_remove, FALSE, FALSE, 0);

    g_object_set_data(G_OBJECT(row_box), "entry_desc", entry_desc);
    g_object_set_data(G_OBJECT(row_box), "spin_valor", spin_valor);

    g_signal_connect_swapped(btn_remove, "clicked", G_CALLBACK(gtk_widget_destroy), row_box);
    g_signal_connect(spin_valor, "value-changed", G_CALLBACK(on_calcular_lucro_changed), NULL);
    g_signal_connect(entry_desc, "changed", G_CALLBACK(on_calcular_lucro_changed), NULL);

    gtk_list_box_insert(listbox_outros_custos, row_box, -1);
    gtk_widget_show_all(row_box);
}

static void on_salvar_nova_viagem_clicked(GtkButton *btn, gpointer user_data) {
    g_print("Botão 'Salvar Nova Viagem' clicado.\n");

    GList *children_temp = NULL;
    GList *iter_temp = NULL;

    if (strlen(gtk_entry_get_text(entry_nome_viagem)) == 0 ||
        strlen(gtk_entry_get_text(entry_nome_escola)) == 0 ||
        strlen(gtk_entry_get_text(entry_destino)) == 0) {
        mostrar_dialogo_aviso(NULL, "Campos Obrigatórios", "Por favor, preencha o Nome da Viagem, Nome da Escola e Destino.");
        return;
    }
    
    strncpy(viagem_em_cadastro.nome_viagem, gtk_entry_get_text(entry_nome_viagem), MAX_NOME - 1);
    viagem_em_cadastro.nome_viagem[MAX_NOME - 1] = '\0';
    strncpy(viagem_em_cadastro.nome_escola, gtk_entry_get_text(entry_nome_escola), MAX_NOME - 1);
    viagem_em_cadastro.nome_escola[MAX_NOME - 1] = '\0';
    strncpy(viagem_em_cadastro.destino, gtk_entry_get_text(entry_destino), 49);
    viagem_em_cadastro.destino[49] = '\0';

    viagem_em_cadastro.dias = gtk_spin_button_get_value_as_int(spin_dias);
    viagem_em_cadastro.numguias_viagem = gtk_spin_button_get_value_as_int(spin_num_guias_viagem);
    viagem_em_cadastro.opcao_guia_local = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(check_opcao_guia_local)) ? 'S' : 'N';
    viagem_em_cadastro.guialocalvalor = gtk_spin_button_get_value(spin_guia_local_valor);
    viagem_em_cadastro.convidados = gtk_spin_button_get_value_as_int(spin_convidados);
    viagem_em_cadastro.motoristas = gtk_spin_button_get_value_as_int(spin_motoristas);
    viagem_em_cadastro.opcao_viagem_extra = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(check_opcao_viagem_extra)) ? 'S' : 'N';
    strncpy(viagem_em_cadastro.viagemextra_nome, gtk_entry_get_text(entry_viagem_extra_nome), 49);
    viagem_em_cadastro.viagemextra_nome[49] = '\0';
    viagem_em_cadastro.viagemextra_valor = gtk_spin_button_get_value(spin_viagem_extra_valor);
    viagem_em_cadastro.minQteAlunos = gtk_spin_button_get_value_as_int(spin_min_qte_alunos);

    viagem_em_cadastro.data_da_viagem.tm_mday = gtk_spin_button_get_value_as_int(spin_data_dia);
    viagem_em_cadastro.data_da_viagem.tm_mon = gtk_spin_button_get_value_as_int(spin_data_mes) - 1;
    viagem_em_cadastro.data_da_viagem.tm_year = gtk_spin_button_get_value_as_int(spin_data_ano) - 1900;
    
    viagem_em_cadastro.onibus = gtk_spin_button_get_value(spin_onibus);
    viagem_em_cadastro.hotel = gtk_spin_button_get_value(spin_hotel);
    viagem_em_cadastro.actes_por_professor = gtk_spin_button_get_value(spin_actes_por_professor);
    viagem_em_cadastro.pulseiras = gtk_spin_button_get_value(spin_pulseiras);
    viagem_em_cadastro.etiquetas = gtk_spin_button_get_value(spin_etiquetas);

    viagem_em_cadastro.guia_viagem_valor_total = gtk_spin_button_get_value(spin_guia_viagem_valor_total);
    viagem_em_cadastro.despesas_guia_viagem = gtk_spin_button_get_value(spin_despesas_guia_viagem);
    viagem_em_cadastro.despesas_motorista = gtk_spin_button_get_value(spin_despesas_motorista);
    viagem_em_cadastro.despesas_luciana = gtk_spin_button_get_value(spin_despesas_luciana);
    viagem_em_cadastro.taxa_apoio_escola = gtk_spin_button_get_value(spin_taxa_apoio_escola);

    viagem_em_cadastro.farmacia = gtk_spin_button_get_value(spin_farmacia);
    viagem_em_cadastro.gasto_extra_geral = gtk_spin_button_get_value(spin_gasto_extra_geral);
    viagem_em_cadastro.emergencia = gtk_spin_button_get_value(spin_emergencia);
    viagem_em_cadastro.poupanca_lumaster = gtk_spin_button_get_value(spin_poupanca_lumaster);
    viagem_em_cadastro.seguro_viagem = gtk_spin_button_get_value(spin_seguro_viagem);

    viagem_em_cadastro.num_outros_custos_diversos = 0;
    children_temp = gtk_container_get_children(GTK_CONTAINER(listbox_outros_custos));
    for (iter_temp = children_temp; iter_temp != NULL; iter_temp = g_list_next(iter_temp)) {
        GtkWidget *row_widget = GTK_WIDGET(iter_temp->data);
        GtkEntry *entry_desc = GTK_ENTRY(g_object_get_data(G_OBJECT(row_widget), "entry_desc"));
        GtkSpinButton *spin_val = GTK_SPIN_BUTTON(g_object_get_data(G_OBJECT(row_widget), "spin_valor"));

        if (entry_desc && spin_val && viagem_em_cadastro.num_outros_custos_diversos < MAX_ITENS_OUTROS) {
            strncpy(viagem_em_cadastro.outros_custos_diversos[viagem_em_cadastro.num_outros_custos_diversos].descricao,
                    gtk_entry_get_text(entry_desc), MAX_DESCRICAO_GASTO - 1);
            viagem_em_cadastro.outros_custos_diversos[viagem_em_cadastro.num_outros_custos_diversos].descricao[MAX_DESCRICAO_GASTO - 1] = '\0';
            viagem_em_cadastro.outros_custos_diversos[viagem_em_cadastro.num_outros_custos_diversos].valor = gtk_spin_button_get_value(spin_val);
            viagem_em_cadastro.outros_custos_diversos[viagem_em_cadastro.num_outros_custos_diversos].ativa = 1;
            viagem_em_cadastro.num_outros_custos_diversos++;
        }
    }
    g_list_free(children_temp);

    viagem_em_cadastro.quant_almocos_alunos_total_dias = gtk_spin_button_get_value_as_int(spin_quant_almocos_alunos_total_dias);
    for (int i = 0; i < viagem_em_cadastro.quant_almocos_alunos_total_dias && i < MAX_DIAS_VIAGEM; ++i) {
        viagem_em_cadastro.val_almo_alunos_dias[i] = gtk_spin_button_get_value(spin_val_almo_alunos_dias[i]);
    }
    viagem_em_cadastro.quant_jantares_alunos_total_dias = gtk_spin_button_get_value_as_int(spin_quant_jantares_alunos_total_dias);
    for (int i = 0; i < viagem_em_cadastro.quant_jantares_alunos_total_dias && i < MAX_DIAS_VIAGEM; ++i) {
        viagem_em_cadastro.val_jantar_alunos_dias[i] = gtk_spin_button_get_value(spin_val_jantar_alunos_dias[i]);
    }
    viagem_em_cadastro.quant_almocos_profs_total_dias = gtk_spin_button_get_value_as_int(spin_quant_almocos_profs_total_dias);
    for (int i = 0; i < viagem_em_cadastro.quant_almocos_profs_total_dias && i < MAX_DIAS_VIAGEM; ++i) {
        viagem_em_cadastro.val_almo_profs_dias[i] = gtk_spin_button_get_value(spin_val_almo_profs_dias[i]);
    }
    viagem_em_cadastro.quant_jantares_profs_total_dias = gtk_spin_button_get_value_as_int(spin_quant_jantares_profs_total_dias);
    for (int i = 0; i < viagem_em_cadastro.quant_jantares_profs_total_dias && i < MAX_DIAS_VIAGEM; ++i) {
        viagem_em_cadastro.val_jantares_profs_dias[i] = gtk_spin_button_get_value(spin_val_jantares_profs_dias[i]);
    }
    viagem_em_cadastro.val_servico_bordo = gtk_spin_button_get_value(spin_val_servico_bordo);
    viagem_em_cadastro.val_alimentacao_extra = gtk_spin_button_get_value(spin_val_alimentacao_extra);

    viagem_em_cadastro.percentual_lucro_desejado = gtk_spin_button_get_value(spin_percentual_lucro_desejado);

    viagem_em_cadastro.num_turmas_cadastradas = 0;
    for (int i = 0; i < num_turmas_em_cadastro; i++) {
        if (turmas_em_cadastro[i]) {
            viagem_em_cadastro.turmas[viagem_em_cadastro.num_turmas_cadastradas] = *turmas_em_cadastro[i];
            viagem_em_cadastro.turmas[viagem_em_cadastro.num_turmas_cadastradas].ativa = 1;
            viagem_em_cadastro.num_turmas_cadastradas++;
        }
    }

    viagem_em_cadastro.ativa = 1;

    int added_idx = adicionar_viagem_ao_sistema(&viagem_em_cadastro);

    if (added_idx != -1) {
        for (int i = 0; i < num_turmas_em_cadastro; i++) {
            if (turmas_em_cadastro[i]) {
                g_free(turmas_em_cadastro[i]);
                turmas_em_cadastro[i] = NULL;
            }
        }
        num_turmas_em_cadastro = 0;

        char msg[512];
        snprintf(msg, sizeof(msg), "Viagem '%s' cadastrada com sucesso!\nCusto por aluno (base %d): R$%.2f",
                 viagem_em_cadastro.nome_viagem, viagens[added_idx].minQteAlunos, viagens[added_idx].totalPorAluno);
        mostrar_dialogo_info(NULL, "Cadastro Concluído", msg);

        limpar_formulario_cadastro_viagem();
        memset(&viagem_em_cadastro, 0, sizeof(Viagem));
        
        mudar_para_pagina("gerenciar_viagens_page");
    } else {
        mostrar_dialogo_erro(NULL, "Erro de Cadastro", "Não foi possível cadastrar a viagem. Verifique o limite de viagens no sistema.");
    }
}

void limpar_formulario_cadastro_viagem() {
    gtk_entry_set_text(entry_nome_viagem, "");
    gtk_entry_set_text(entry_nome_escola, "");
    gtk_entry_set_text(entry_destino, "");

    gtk_spin_button_set_value(spin_dias, 1);
    gtk_spin_button_set_value(spin_num_guias_viagem, 0);
    
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check_opcao_guia_local), FALSE);
    gtk_spin_button_set_value(spin_guia_local_valor, 0.00);
    gtk_widget_set_sensitive(GTK_WIDGET(spin_guia_local_valor), FALSE);

    gtk_spin_button_set_value(spin_convidados, 0);
    gtk_spin_button_set_value(spin_motoristas, 0);

    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check_opcao_viagem_extra), FALSE);
    gtk_entry_set_text(entry_viagem_extra_nome, "");
    gtk_spin_button_set_value(spin_viagem_extra_valor, 0.00);
    gtk_widget_set_sensitive(GTK_WIDGET(entry_viagem_extra_nome), FALSE);
    gtk_widget_set_sensitive(GTK_WIDGET(spin_viagem_extra_valor), FALSE);

    gtk_spin_button_set_value(spin_min_qte_alunos, 1);

    time_t rawtime;
    struct tm *info;
    time(&rawtime);
    info = localtime(&rawtime);

    gtk_spin_button_set_value(spin_data_dia, info->tm_mday);
    gtk_spin_button_set_value(spin_data_mes, info->tm_mon + 1);
    gtk_spin_button_set_value(spin_data_ano, info->tm_year + 1900);

    gtk_spin_button_set_value(spin_onibus, 0.00);
    gtk_spin_button_set_value(spin_hotel, 0.00);
    gtk_spin_button_set_value(spin_actes_por_professor, 0.00);
    gtk_spin_button_set_value(spin_pulseiras, 0.00);
    gtk_spin_button_set_value(spin_etiquetas, 0.00);

    gtk_spin_button_set_value(spin_guia_viagem_valor_total, 0.00);
    gtk_spin_button_set_value(spin_despesas_guia_viagem, 0.00);
    gtk_spin_button_set_value(spin_despesas_motorista, 0.00);
    gtk_spin_button_set_value(spin_despesas_luciana, 0.00);
    gtk_spin_button_set_value(spin_taxa_apoio_escola, 0.00);

    gtk_spin_button_set_value(spin_farmacia, 0.00);
    gtk_spin_button_set_value(spin_gasto_extra_geral, 0.00);
    gtk_spin_button_set_value(spin_emergencia, 0.00);
    gtk_spin_button_set_value(spin_poupanca_lumaster, 0.00);
    gtk_spin_button_set_value(spin_seguro_viagem, 0.00);

    GList *children_temp, *iter_temp;
    children_temp = gtk_container_get_children(GTK_CONTAINER(listbox_outros_custos));
    for (iter_temp = children_temp; iter_temp != NULL; iter_temp = g_list_next(iter_temp)) {
        gtk_widget_destroy(GTK_WIDGET(iter_temp->data));
    }
    g_list_free(children_temp);

    gtk_spin_button_set_value(spin_quant_almocos_alunos_total_dias, 0);
    gtk_spin_button_set_value(spin_quant_jantares_alunos_total_dias, 0);
    gtk_spin_button_set_value(spin_quant_almocos_profs_total_dias, 0);
    gtk_spin_button_set_value(spin_quant_jantares_profs_total_dias, 0);
    
    for (int i = 0; i < MAX_DIAS_VIAGEM; i++) {
        if (spin_val_almo_alunos_dias[i]) gtk_spin_button_set_value(spin_val_almo_alunos_dias[i], 0.00);
        if (spin_val_jantar_alunos_dias[i]) gtk_spin_button_set_value(spin_val_jantar_alunos_dias[i], 0.00);
        if (spin_val_almo_profs_dias[i]) gtk_spin_button_set_value(spin_val_almo_profs_dias[i], 0.00);
        if (spin_val_jantares_profs_dias[i]) gtk_spin_button_set_value(spin_val_jantares_profs_dias[i], 0.00);
    }
    gtk_spin_button_set_value(spin_val_servico_bordo, 0.00);
    gtk_spin_button_set_value(spin_val_alimentacao_extra, 0.00);

    gtk_spin_button_set_value(spin_percentual_lucro_desejado, 0.00);
    gtk_label_set_text(label_custo_total_base, "R$ 0.00");
    gtk_label_set_text(label_custo_aluno_base, "R$ 0.00");
    gtk_label_set_text(label_lucro_calculado, "R$ 0.00 (0.00%)");
    gtk_label_set_text(label_custo_final_total, "R$ 0.00");
    gtk_label_set_text(label_custo_final_por_aluno, "R$ 0.00");

    for (int i = 0; i < num_turmas_em_cadastro; i++) {
        if (turmas_em_cadastro[i]) {
            g_free(turmas_em_cadastro[i]);
            turmas_em_cadastro[i] = NULL;
        }
    }
    num_turmas_em_cadastro = 0;
    atualizar_listbox_turmas();
}

void construir_pagina_cadastro_viagem() {
    pagina_cadastro_viagem = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_set_border_width(GTK_CONTAINER(pagina_cadastro_viagem), 20);

    GtkWidget *label_titulo = gtk_label_new("<span size='x-large' weight='bold'>Cadastro de Nova Viagem</span>");
    gtk_label_set_use_markup(GTK_LABEL(label_titulo), TRUE);
    gtk_label_set_xalign(GTK_LABEL(label_titulo), 0);
    gtk_box_pack_start(GTK_BOX(pagina_cadastro_viagem), label_titulo, FALSE, FALSE, 10);

    GtkWidget *notebook = gtk_notebook_new();
    gtk_box_pack_start(GTK_BOX(pagina_cadastro_viagem), notebook, TRUE, TRUE, 0);

    GtkWidget *scrolled_win_basico = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_win_basico), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
    GtkWidget *grid_basico = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid_basico), 5);
    gtk_grid_set_column_spacing(GTK_GRID(grid_basico), 10);
    gtk_container_set_border_width(GTK_CONTAINER(grid_basico), 10);
    gtk_container_add(GTK_CONTAINER(scrolled_win_basico), grid_basico);

    int row = 0;
    gtk_grid_attach(GTK_GRID(grid_basico), gtk_label_new("Nome da Viagem:"), 0, row, 1, 1);
    entry_nome_viagem = GTK_ENTRY(gtk_entry_new());
    g_signal_connect(entry_nome_viagem, "changed", G_CALLBACK(on_calcular_lucro_changed), NULL);
    gtk_grid_attach(GTK_GRID(grid_basico), GTK_WIDGET(entry_nome_viagem), 1, row++, 1, 1);

    gtk_grid_attach(GTK_GRID(grid_basico), gtk_label_new("Nome da Escola:"), 0, row, 1, 1);
    entry_nome_escola = GTK_ENTRY(gtk_entry_new());
    g_signal_connect(entry_nome_escola, "changed", G_CALLBACK(on_calcular_lucro_changed), NULL);
    gtk_grid_attach(GTK_GRID(grid_basico), GTK_WIDGET(entry_nome_escola), 1, row++, 1, 1);

    gtk_grid_attach(GTK_GRID(grid_basico), gtk_label_new("Destino:"), 0, row, 1, 1);
    entry_destino = GTK_ENTRY(gtk_entry_new());
    g_signal_connect(entry_destino, "changed", G_CALLBACK(on_calcular_lucro_changed), NULL);
    gtk_grid_attach(GTK_GRID(grid_basico), GTK_WIDGET(entry_destino), 1, row++, 1, 1);

    GtkWidget *hbox_data = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_box_pack_start(GTK_BOX(hbox_data), gtk_label_new("Data da Viagem (DD/MM/AAAA):"), FALSE, FALSE, 0);
    spin_data_dia = criar_spin_button_inteiro(1, 31, 1);
    spin_data_mes = criar_spin_button_inteiro(1, 12, 1);
    spin_data_ano = criar_spin_button_inteiro(1900, 2100, 1);
    g_signal_connect(spin_data_dia, "value-changed", G_CALLBACK(on_calcular_lucro_changed), NULL);
    g_signal_connect(spin_data_mes, "value-changed", G_CALLBACK(on_calcular_lucro_changed), NULL);
    g_signal_connect(spin_data_ano, "value-changed", G_CALLBACK(on_calcular_lucro_changed), NULL);
    gtk_box_pack_start(GTK_BOX(hbox_data), GTK_WIDGET(spin_data_dia), FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(hbox_data), gtk_label_new("/"), FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(hbox_data), GTK_WIDGET(spin_data_mes), FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(hbox_data), gtk_label_new("/"), FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(hbox_data), GTK_WIDGET(spin_data_ano), FALSE, FALSE, 0);
    gtk_grid_attach(GTK_GRID(grid_basico), hbox_data, 0, row++, 2, 1);

    gtk_grid_attach(GTK_GRID(grid_basico), gtk_label_new("Dias de Viagem:"), 0, row, 1, 1);
    spin_dias = GTK_SPIN_BUTTON(gtk_spin_button_new_with_range(1, MAX_DIAS_VIAGEM, 1));
    g_signal_connect(spin_dias, "value-changed", G_CALLBACK(on_dias_viagem_changed), grid_custos_alimentacao);
    g_signal_connect(spin_dias, "value-changed", G_CALLBACK(on_calcular_lucro_changed), NULL);
    gtk_grid_attach(GTK_GRID(grid_basico), GTK_WIDGET(spin_dias), 1, row++, 1, 1);

    gtk_grid_attach(GTK_GRID(grid_basico), gtk_label_new("Número de Guias (não locais):"), 0, row, 1, 1);
    spin_num_guias_viagem = GTK_SPIN_BUTTON(gtk_spin_button_new_with_range(0, 10, 1));
    g_signal_connect(spin_num_guias_viagem, "value-changed", G_CALLBACK(on_calcular_lucro_changed), NULL);
    gtk_grid_attach(GTK_GRID(grid_basico), GTK_WIDGET(spin_num_guias_viagem), 1, row++, 1, 1);

    check_opcao_guia_local = GTK_CHECK_BUTTON(gtk_check_button_new_with_label("Terá Guia Local?"));
    gtk_grid_attach(GTK_GRID(grid_basico), GTK_WIDGET(check_opcao_guia_local), 0, row, 1, 1);
    spin_guia_local_valor = GTK_SPIN_BUTTON(gtk_spin_button_new_with_range(0.00, 10000.00, 0.01));
    gtk_spin_button_set_value(spin_guia_local_valor, 0.00);
    gtk_widget_set_sensitive(GTK_WIDGET(spin_guia_local_valor), FALSE);
    g_signal_connect(check_opcao_guia_local, "toggled", G_CALLBACK(on_guia_local_toggled), NULL);
    g_signal_connect(spin_guia_local_valor, "value-changed", G_CALLBACK(on_calcular_lucro_changed), NULL);
    gtk_grid_attach(GTK_GRID(grid_basico), gtk_label_new("Valor (R$):"), 1, row, 1, 1);
    gtk_grid_attach(GTK_GRID(grid_basico), GTK_WIDGET(spin_guia_local_valor), 2, row++, 1, 1);

    gtk_grid_attach(GTK_GRID(grid_basico), gtk_label_new("Número de Convidados:"), 0, row, 1, 1);
    spin_convidados = GTK_SPIN_BUTTON(gtk_spin_button_new_with_range(0, 10, 1));
    g_signal_connect(spin_convidados, "value-changed", G_CALLBACK(on_calcular_lucro_changed), NULL);
    gtk_grid_attach(GTK_GRID(grid_basico), GTK_WIDGET(spin_convidados), 1, row++, 1, 1);

    gtk_grid_attach(GTK_GRID(grid_basico), gtk_label_new("Número de Motoristas:"), 0, row, 1, 1);
    spin_motoristas = GTK_SPIN_BUTTON(gtk_spin_button_new_with_range(0, 5, 1));
    g_signal_connect(spin_motoristas, "value-changed", G_CALLBACK(on_calcular_lucro_changed), NULL);
    gtk_grid_attach(GTK_GRID(grid_basico), GTK_WIDGET(spin_motoristas), 1, row++, 1, 1);

    check_opcao_viagem_extra = GTK_CHECK_BUTTON(gtk_check_button_new_with_label("Terá Passeio Extra?"));
    gtk_grid_attach(GTK_GRID(grid_basico), GTK_WIDGET(check_opcao_viagem_extra), 0, row, 1, 1);
    entry_viagem_extra_nome = GTK_ENTRY(gtk_entry_new());
    gtk_entry_set_placeholder_text(entry_viagem_extra_nome, "Nome do Passeio Extra");
    gtk_widget_set_sensitive(GTK_WIDGET(entry_viagem_extra_nome), FALSE);
    gtk_grid_attach(GTK_GRID(grid_basico), GTK_WIDGET(entry_viagem_extra_nome), 1, row, 1, 1);
    spin_viagem_extra_valor = GTK_SPIN_BUTTON(gtk_spin_button_new_with_range(0.00, 50000.00, 0.01));
    gtk_spin_button_set_value(spin_viagem_extra_valor, 0.00);
    gtk_widget_set_sensitive(GTK_WIDGET(spin_viagem_extra_valor), FALSE);
    g_signal_connect(check_opcao_viagem_extra, "toggled", G_CALLBACK(on_viagem_extra_toggled), NULL);
    g_signal_connect(spin_viagem_extra_valor, "value-changed", G_CALLBACK(on_calcular_lucro_changed), NULL);
    gtk_grid_attach(GTK_GRID(grid_basico), gtk_label_new("Valor (R$):"), 2, row, 1, 1);
    gtk_grid_attach(GTK_GRID(grid_basico), GTK_WIDGET(spin_viagem_extra_valor), 3, row++, 1, 1);

    gtk_grid_attach(GTK_GRID(grid_basico), gtk_label_new("Min. Alunos para Preço:"), 0, row, 1, 1);
    spin_min_qte_alunos = GTK_SPIN_BUTTON(gtk_spin_button_new_with_range(0, 500, 1));
    gtk_spin_button_set_value(spin_min_qte_alunos, 1);
    g_signal_connect(spin_min_qte_alunos, "value-changed", G_CALLBACK(on_calcular_lucro_changed), NULL);
    gtk_grid_attach(GTK_GRID(grid_basico), GTK_WIDGET(spin_min_qte_alunos), 1, row++, 1, 1);

    GtkWidget *frame_turmas = gtk_frame_new("Turmas Cadastradas (Temporário)");
    gtk_widget_set_margin_top(frame_turmas, 15);
    gtk_grid_attach(GTK_GRID(grid_basico), frame_turmas, 0, row++, 4, 1);

    GtkWidget *vbox_turmas = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_set_border_width(GTK_CONTAINER(vbox_turmas), 5);
    gtk_container_add(GTK_CONTAINER(frame_turmas), vbox_turmas);

    listbox_turmas_cadastradas = GTK_LIST_BOX(gtk_list_box_new());
    gtk_list_box_set_selection_mode(listbox_turmas_cadastradas, GTK_SELECTION_NONE);
    gtk_box_pack_start(GTK_BOX(vbox_turmas), GTK_WIDGET(listbox_turmas_cadastradas), TRUE, TRUE, 0);

    GtkWidget *btn_add_turma = gtk_button_new_with_label("Adicionar Turma");
    gtk_box_pack_start(GTK_BOX(vbox_turmas), btn_add_turma, FALSE, FALSE, 0);
    g_signal_connect(btn_add_turma, "clicked", G_CALLBACK(on_adicionar_turma_clicked), NULL);
    
    atualizar_listbox_turmas();
    memset(&viagem_em_cadastro, 0, sizeof(Viagem));
    num_turmas_em_cadastro = 0;

    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), scrolled_win_basico, gtk_label_new("Detalhes Básicos"));

    GtkWidget *scrolled_win_custos_principais = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_win_custos_principais), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
    GtkWidget *grid_custos_principais = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid_custos_principais), 5);
    gtk_grid_set_column_spacing(GTK_GRID(grid_custos_principais), 10);
    gtk_container_set_border_width(GTK_CONTAINER(grid_custos_principais), 10);
    gtk_container_add(GTK_CONTAINER(scrolled_win_custos_principais), grid_custos_principais);

    row = 0;
    gtk_grid_attach(GTK_GRID(grid_custos_principais), gtk_label_new("Valor do Ônibus (R$):"), 0, row, 1, 1);
    spin_onibus = GTK_SPIN_BUTTON(gtk_spin_button_new_with_range(0.00, 1000000.00, 0.01));
    g_signal_connect(spin_onibus, "value-changed", G_CALLBACK(on_calcular_lucro_changed), NULL);
    gtk_grid_attach(GTK_GRID(grid_custos_principais), GTK_WIDGET(spin_onibus), 1, row++, 1, 1);

    gtk_grid_attach(GTK_GRID(grid_custos_principais), gtk_label_new("Valor do Hotel (R$):"), 0, row, 1, 1);
    spin_hotel = GTK_SPIN_BUTTON(gtk_spin_button_new_with_range(0.00, 2000000.00, 0.01));
    g_signal_connect(spin_hotel, "value-changed", G_CALLBACK(on_calcular_lucro_changed), NULL);
    gtk_grid_attach(GTK_GRID(grid_custos_principais), GTK_WIDGET(spin_hotel), 1, row++, 1, 1);

    gtk_grid_attach(GTK_GRID(grid_custos_principais), gtk_label_new("Taxa ACTES por Professor (R$):"), 0, row, 1, 1);
    spin_actes_por_professor = GTK_SPIN_BUTTON(gtk_spin_button_new_with_range(0.00, 10000.00, 0.01));
    g_signal_connect(spin_actes_por_professor, "value-changed", G_CALLBACK(on_calcular_lucro_changed), NULL);
    gtk_grid_attach(GTK_GRID(grid_custos_principais), GTK_WIDGET(spin_actes_por_professor), 1, row++, 1, 1);

    gtk_grid_attach(GTK_GRID(grid_custos_principais), gtk_label_new("Valor das Pulseiras (R$):"), 0, row, 1, 1);
    spin_pulseiras = GTK_SPIN_BUTTON(gtk_spin_button_new_with_range(0.00, 50000.00, 0.01));
    g_signal_connect(spin_pulseiras, "value-changed", G_CALLBACK(on_calcular_lucro_changed), NULL);
    gtk_grid_attach(GTK_GRID(grid_custos_principais), GTK_WIDGET(spin_pulseiras), 1, row++, 1, 1);

    gtk_grid_attach(GTK_GRID(grid_custos_principais), gtk_label_new("Valor das Etiquetas (R$):"), 0, row, 1, 1);
    spin_etiquetas = GTK_SPIN_BUTTON(gtk_spin_button_new_with_range(0.00, 50000.00, 0.01));
    g_signal_connect(spin_etiquetas, "value-changed", G_CALLBACK(on_calcular_lucro_changed), NULL);
    gtk_grid_attach(GTK_GRID(grid_custos_principais), GTK_WIDGET(spin_etiquetas), 1, row++, 1, 1);

    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), scrolled_win_custos_principais, gtk_label_new("Custos Principais"));

    GtkWidget *scrolled_win_custos_pessoas = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_win_custos_pessoas), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
    GtkWidget *grid_custos_pessoas = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid_custos_pessoas), 5);
    gtk_grid_set_column_spacing(GTK_GRID(grid_custos_pessoas), 10);
    gtk_container_set_border_width(GTK_CONTAINER(grid_custos_pessoas), 10);
    gtk_container_add(GTK_CONTAINER(scrolled_win_custos_pessoas), grid_custos_pessoas);

    row = 0;
    gtk_grid_attach(GTK_GRID(grid_custos_pessoas), gtk_label_new("Valor do Guia da Viagem (Total R$):"), 0, row, 1, 1);
    spin_guia_viagem_valor_total = GTK_SPIN_BUTTON(gtk_spin_button_new_with_range(0.00, 200000.00, 0.01));
    g_signal_connect(spin_guia_viagem_valor_total, "value-changed", G_CALLBACK(on_calcular_lucro_changed), NULL);
    gtk_grid_attach(GTK_GRID(grid_custos_pessoas), GTK_WIDGET(spin_guia_viagem_valor_total), 1, row++, 1, 1);

    gtk_grid_attach(GTK_GRID(grid_custos_pessoas), gtk_label_new("Despesas dos Guias (R$):"), 0, row, 1, 1);
    spin_despesas_guia_viagem = GTK_SPIN_BUTTON(gtk_spin_button_new_with_range(0.00, 50000.00, 0.01));
    g_signal_connect(spin_despesas_guia_viagem, "value-changed", G_CALLBACK(on_calcular_lucro_changed), NULL);
    gtk_grid_attach(GTK_GRID(grid_custos_pessoas), GTK_WIDGET(spin_despesas_guia_viagem), 1, row++, 1, 1);

    gtk_grid_attach(GTK_GRID(grid_custos_pessoas), gtk_label_new("Despesas dos Motoristas (R$):"), 0, row, 1, 1);
    spin_despesas_motorista = GTK_SPIN_BUTTON(gtk_spin_button_new_with_range(0.00, 50000.00, 0.01));
    g_signal_connect(spin_despesas_motorista, "value-changed", G_CALLBACK(on_calcular_lucro_changed), NULL);
    gtk_grid_attach(GTK_GRID(grid_custos_pessoas), GTK_WIDGET(spin_despesas_motorista), 1, row++, 1, 1);

    gtk_grid_attach(GTK_GRID(grid_custos_pessoas), gtk_label_new("Despesas da Luciana (R$):"), 0, row, 1, 1);
    spin_despesas_luciana = GTK_SPIN_BUTTON(gtk_spin_button_new_with_range(0.00, 50000.00, 0.01));
    g_signal_connect(spin_despesas_luciana, "value-changed", G_CALLBACK(on_calcular_lucro_changed), NULL);
    gtk_grid_attach(GTK_GRID(grid_custos_pessoas), GTK_WIDGET(spin_despesas_luciana), 1, row++, 1, 1);

    gtk_grid_attach(GTK_GRID(grid_custos_pessoas), gtk_label_new("Taxa de Apoio da Escola (R$):"), 0, row, 1, 1);
    spin_taxa_apoio_escola = GTK_SPIN_BUTTON(gtk_spin_button_new_with_range(0.00, 100000.00, 0.01));
    g_signal_connect(spin_taxa_apoio_escola, "value-changed", G_CALLBACK(on_calcular_lucro_changed), NULL);
    gtk_grid_attach(GTK_GRID(grid_custos_pessoas), GTK_WIDGET(spin_taxa_apoio_escola), 1, row++, 1, 1);

    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), scrolled_win_custos_pessoas, gtk_label_new("Custos Pessoas"));

    GtkWidget *scrolled_win_custos_diversos = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_win_custos_diversos), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
    GtkWidget *grid_custos_diversos = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid_custos_diversos), 5);
    gtk_grid_set_column_spacing(GTK_GRID(grid_custos_diversos), 10);
    gtk_container_set_border_width(GTK_CONTAINER(grid_custos_diversos), 10);
    gtk_container_add(GTK_CONTAINER(scrolled_win_custos_diversos), grid_custos_diversos);

    row = 0;
    gtk_grid_attach(GTK_GRID(grid_custos_diversos), gtk_label_new("Gastos com Farmácia (R$):"), 0, row, 1, 1);
    spin_farmacia = GTK_SPIN_BUTTON(gtk_spin_button_new_with_range(0.00, 20000.00, 0.01));
    g_signal_connect(spin_farmacia, "value-changed", G_CALLBACK(on_calcular_lucro_changed), NULL);
    gtk_grid_attach(GTK_GRID(grid_custos_diversos), GTK_WIDGET(spin_farmacia), 1, row++, 1, 1);

    gtk_grid_attach(GTK_GRID(grid_custos_diversos), gtk_label_new("Gastos Extras Gerais (R$):"), 0, row, 1, 1);
    spin_gasto_extra_geral = GTK_SPIN_BUTTON(gtk_spin_button_new_with_range(0.00, 50000.00, 0.01));
    g_signal_connect(spin_gasto_extra_geral, "value-changed", G_CALLBACK(on_calcular_lucro_changed), NULL);
    gtk_grid_attach(GTK_GRID(grid_custos_diversos), GTK_WIDGET(spin_gasto_extra_geral), 1, row++, 1, 1);

    gtk_grid_attach(GTK_GRID(grid_custos_diversos), gtk_label_new("Valor para Emergência (R$):"), 0, row, 1, 1);
    spin_emergencia = GTK_SPIN_BUTTON(gtk_spin_button_new_with_range(0.00, 100000.00, 0.01));
    g_signal_connect(spin_emergencia, "value-changed", G_CALLBACK(on_calcular_lucro_changed), NULL);
    gtk_grid_attach(GTK_GRID(grid_custos_diversos), GTK_WIDGET(spin_emergencia), 1, row++, 1, 1);

    gtk_grid_attach(GTK_GRID(grid_custos_diversos), gtk_label_new("Poupança Lumaster (R$):"), 0, row, 1, 1);
    spin_poupanca_lumaster = GTK_SPIN_BUTTON(gtk_spin_button_new_with_range(0.00, 100000.00, 0.01));
    g_signal_connect(spin_poupanca_lumaster, "value-changed", G_CALLBACK(on_calcular_lucro_changed), NULL);
    gtk_grid_attach(GTK_GRID(grid_custos_diversos), GTK_WIDGET(spin_poupanca_lumaster), 1, row++, 1, 1);

    gtk_grid_attach(GTK_GRID(grid_custos_diversos), gtk_label_new("Seguro Viagem (R$):"), 0, row, 1, 1);
    spin_seguro_viagem = GTK_SPIN_BUTTON(gtk_spin_button_new_with_range(0.00, 200000.00, 0.01));
    g_signal_connect(spin_seguro_viagem, "value-changed", G_CALLBACK(on_calcular_lucro_changed), NULL);
    gtk_grid_attach(GTK_GRID(grid_custos_diversos), GTK_WIDGET(spin_seguro_viagem), 1, row++, 1, 1);

    GtkWidget *frame_outros_custos = gtk_frame_new("Outros Custos Adicionais");
    gtk_widget_set_margin_top(frame_outros_custos, 15);
    gtk_grid_attach(GTK_GRID(grid_custos_diversos), frame_outros_custos, 0, row++, 2, 1);

    GtkWidget *vbox_outros_custos = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_set_border_width(GTK_CONTAINER(vbox_outros_custos), 5);
    gtk_container_add(GTK_CONTAINER(frame_outros_custos), vbox_outros_custos);

    listbox_outros_custos = GTK_LIST_BOX(gtk_list_box_new());
    gtk_list_box_set_selection_mode(listbox_outros_custos, GTK_SELECTION_NONE);
    gtk_box_pack_start(GTK_BOX(vbox_outros_custos), GTK_WIDGET(listbox_outros_custos), TRUE, TRUE, 0);

    GtkWidget *btn_add_outro_custo = gtk_button_new_with_label("Adicionar Outro Custo");
    gtk_box_pack_start(GTK_BOX(vbox_outros_custos), btn_add_outro_custo, FALSE, FALSE, 0);
    g_signal_connect(btn_add_outro_custo, "clicked", G_CALLBACK(on_add_outro_custo_clicked), NULL);

    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), scrolled_win_custos_diversos, gtk_label_new("Custos Diversos"));

    grid_custos_alimentacao = gtk_grid_new();
    GtkWidget *local_scrolled_win_custos_alimentacao = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(local_scrolled_win_custos_alimentacao), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);

    gtk_grid_set_row_spacing(GTK_GRID(grid_custos_alimentacao), 5);
    gtk_grid_set_column_spacing(GTK_GRID(grid_custos_alimentacao), 10);
    gtk_container_set_border_width(GTK_CONTAINER(grid_custos_alimentacao), 10);
    gtk_container_add(GTK_CONTAINER(local_scrolled_win_custos_alimentacao), grid_custos_alimentacao);

    row = 0;
    gtk_grid_attach(GTK_GRID(grid_custos_alimentacao), gtk_label_new("Dias com Almoço Alunos:"), 0, row, 1, 1);
    spin_quant_almocos_alunos_total_dias = GTK_SPIN_BUTTON(gtk_spin_button_new_with_range(0, MAX_DIAS_VIAGEM, 1));
    g_signal_connect(spin_quant_almocos_alunos_total_dias, "value-changed", G_CALLBACK(on_quant_almocos_alunos_changed), grid_custos_alimentacao);
    g_signal_connect(spin_quant_almocos_alunos_total_dias, "value-changed", G_CALLBACK(on_calcular_lucro_changed), NULL);
    gtk_grid_attach(GTK_GRID(grid_custos_alimentacao), GTK_WIDGET(spin_quant_almocos_alunos_total_dias), 1, row++, 1, 1);
    for (int i = 0; i < MAX_DIAS_VIAGEM; i++) {
        char label_text[50];
        snprintf(label_text, sizeof(label_text), "Valor Almoço Alunos Dia %d (R$):", i + 1);
        gtk_grid_attach(GTK_GRID(grid_custos_alimentacao), gtk_label_new(label_text), 0, row, 1, 1);
        spin_val_almo_alunos_dias[i] = GTK_SPIN_BUTTON(gtk_spin_button_new_with_range(0.00, 500.00, 0.01));
        g_signal_connect(spin_val_almo_alunos_dias[i], "value-changed", G_CALLBACK(on_calcular_lucro_changed), NULL);
        gtk_grid_attach(GTK_GRID(grid_custos_alimentacao), GTK_WIDGET(spin_val_almo_alunos_dias[i]), 1, row++, 1, 1);
        gtk_widget_set_visible(gtk_grid_get_child_at(GTK_GRID(grid_custos_alimentacao), 0, row - 1), FALSE);
        gtk_widget_set_visible(GTK_WIDGET(spin_val_almo_alunos_dias[i]), FALSE);
    }

    row++;
    gtk_grid_attach(GTK_GRID(grid_custos_alimentacao), gtk_label_new("Dias com Jantar Alunos:"), 0, row, 1, 1);
    spin_quant_jantares_alunos_total_dias = GTK_SPIN_BUTTON(gtk_spin_button_new_with_range(0, MAX_DIAS_VIAGEM, 1));
    g_signal_connect(spin_quant_jantares_alunos_total_dias, "value-changed", G_CALLBACK(on_quant_jantares_alunos_changed), grid_custos_alimentacao);
    gtk_grid_attach(GTK_GRID(grid_custos_alimentacao), GTK_WIDGET(spin_quant_jantares_alunos_total_dias), 1, row++, 1, 1);
    for (int i = 0; i < MAX_DIAS_VIAGEM; i++) {
        char label_text[50];
        snprintf(label_text, sizeof(label_text), "Valor Jantar Alunos Dia %d (R$):", i + 1);
        gtk_grid_attach(GTK_GRID(grid_custos_alimentacao), gtk_label_new(label_text), 0, row, 1, 1);
        spin_val_jantar_alunos_dias[i] = GTK_SPIN_BUTTON(gtk_spin_button_new_with_range(0.00, 500.00, 0.01));
        g_signal_connect(spin_val_jantar_alunos_dias[i], "value-changed", G_CALLBACK(on_calcular_lucro_changed), NULL);
        gtk_grid_attach(GTK_GRID(grid_custos_alimentacao), GTK_WIDGET(spin_val_jantar_alunos_dias[i]), 1, row++, 1, 1);
        gtk_widget_set_visible(gtk_grid_get_child_at(GTK_GRID(grid_custos_alimentacao), 0, row - 1), FALSE);
        gtk_widget_set_visible(GTK_WIDGET(spin_val_jantar_alunos_dias[i]), FALSE);
    }

    row++;
    gtk_grid_attach(GTK_GRID(grid_custos_alimentacao), gtk_label_new("Dias com Almoço Professores:"), 0, row, 1, 1);
    spin_quant_almocos_profs_total_dias = GTK_SPIN_BUTTON(gtk_spin_button_new_with_range(0, MAX_DIAS_VIAGEM, 1));
    g_signal_connect(spin_quant_almocos_profs_total_dias, "value-changed", G_CALLBACK(on_quant_almocos_profs_changed), grid_custos_alimentacao);
    gtk_grid_attach(GTK_GRID(grid_custos_alimentacao), GTK_WIDGET(spin_quant_almocos_profs_total_dias), 1, row++, 1, 1);
    for (int i = 0; i < MAX_DIAS_VIAGEM; i++) {
        char label_text[50];
        snprintf(label_text, sizeof(label_text), "Valor Almoço Profs Dia %d (R$):", i + 1);
        gtk_grid_attach(GTK_GRID(grid_custos_alimentacao), gtk_label_new(label_text), 0, row, 1, 1);
        spin_val_almo_profs_dias[i] = GTK_SPIN_BUTTON(gtk_spin_button_new_with_range(0.00, 500.00, 0.01));
        g_signal_connect(spin_val_almo_profs_dias[i], "value-changed", G_CALLBACK(on_calcular_lucro_changed), NULL);
        gtk_grid_attach(GTK_GRID(grid_custos_alimentacao), GTK_WIDGET(spin_val_almo_profs_dias[i]), 1, row++, 1, 1);
        gtk_widget_set_visible(gtk_grid_get_child_at(GTK_GRID(grid_custos_alimentacao), 0, row - 1), FALSE);
        gtk_widget_set_visible(GTK_WIDGET(spin_val_almo_profs_dias[i]), FALSE);
    }

    row++;
    gtk_grid_attach(GTK_GRID(grid_custos_alimentacao), gtk_label_new("Dias com Jantar Professores:"), 0, row, 1, 1);
    spin_quant_jantares_profs_total_dias = GTK_SPIN_BUTTON(gtk_spin_button_new_with_range(0, MAX_DIAS_VIAGEM, 1));
    g_signal_connect(spin_quant_jantares_profs_total_dias, "value-changed", G_CALLBACK(on_quant_jantares_profs_changed), grid_custos_alimentacao);
    gtk_grid_attach(GTK_GRID(grid_custos_alimentacao), GTK_WIDGET(spin_quant_jantares_profs_total_dias), 1, row++, 1, 1);
    for (int i = 0; i < MAX_DIAS_VIAGEM; i++) {
        char label_text[50];
        snprintf(label_text, sizeof(label_text), "Valor Jantar Profs Dia %d (R$):", i + 1);
        gtk_grid_attach(GTK_GRID(grid_custos_alimentacao), gtk_label_new(label_text), 0, row, 1, 1);
        spin_val_jantares_profs_dias[i] = GTK_SPIN_BUTTON(gtk_spin_button_new_with_range(0.00, 500.00, 0.01));
        g_signal_connect(spin_val_jantares_profs_dias[i], "value-changed", G_CALLBACK(on_calcular_lucro_changed), NULL);
        gtk_grid_attach(GTK_GRID(grid_custos_alimentacao), GTK_WIDGET(spin_val_jantares_profs_dias[i]), 1, row++, 1, 1);
        gtk_widget_set_visible(gtk_grid_get_child_at(GTK_GRID(grid_custos_alimentacao), 0, row - 1), FALSE);
        gtk_widget_set_visible(GTK_WIDGET(spin_val_jantares_profs_dias[i]), FALSE);
    }

    row++;
    gtk_grid_attach(GTK_GRID(grid_custos_alimentacao), gtk_label_new("Custos Serviço de Bordo (R$):"), 0, row, 1, 1);
    spin_val_servico_bordo = GTK_SPIN_BUTTON(gtk_spin_button_new_with_range(0.00, 50000.00, 0.01));
    g_signal_connect(spin_val_servico_bordo, "value-changed", G_CALLBACK(on_calcular_lucro_changed), NULL);
    gtk_grid_attach(GTK_GRID(grid_custos_alimentacao), GTK_WIDGET(spin_val_servico_bordo), 1, row++, 1, 1);

    gtk_grid_attach(GTK_GRID(grid_custos_alimentacao), gtk_label_new("Outros Gastos Alimentação (R$):"), 0, row, 1, 1);
    spin_val_alimentacao_extra = GTK_SPIN_BUTTON(gtk_spin_button_new_with_range(0.00, 50000.00, 0.01));
    g_signal_connect(spin_val_alimentacao_extra, "value-changed", G_CALLBACK(on_calcular_lucro_changed), NULL);
    gtk_grid_attach(GTK_GRID(grid_custos_alimentacao), GTK_WIDGET(spin_val_alimentacao_extra), 1, row++, 1, 1);

    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), local_scrolled_win_custos_alimentacao, gtk_label_new("Alimentação"));

    GtkWidget *scrolled_win_lucro = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_win_lucro), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
    GtkWidget *grid_lucro = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid_lucro), 5);
    gtk_grid_set_column_spacing(GTK_GRID(grid_lucro), 10);
    gtk_container_set_border_width(GTK_CONTAINER(grid_lucro), 10);
    gtk_container_add(GTK_CONTAINER(scrolled_win_lucro), grid_lucro);

    row = 0;
    gtk_grid_attach(GTK_GRID(grid_lucro), gtk_label_new("Custo Total Base (sem lucro):"), 0, row, 1, 1);
    label_custo_total_base = GTK_LABEL(gtk_label_new("R$ 0.00"));
    gtk_label_set_xalign(label_custo_total_base, 0); gtk_grid_attach(GTK_GRID(grid_lucro), GTK_WIDGET(label_custo_total_base), 1, row++, 1, 1);

    gtk_grid_attach(GTK_GRID(grid_lucro), gtk_label_new("Custo Base por Aluno:"), 0, row, 1, 1);
    label_custo_aluno_base = GTK_LABEL(gtk_label_new("R$ 0.00"));
    gtk_label_set_xalign(label_custo_aluno_base, 0); gtk_grid_attach(GTK_GRID(grid_lucro), GTK_WIDGET(label_custo_aluno_base), 1, row++, 1, 1);

    gtk_grid_attach(GTK_GRID(grid_lucro), gtk_label_new("Porcentagem de Lucro Desejada (%):"), 0, row, 1, 1);
    spin_percentual_lucro_desejado = GTK_SPIN_BUTTON(gtk_spin_button_new_with_range(0.00, 100.00, 0.01));
    gtk_spin_button_set_value(spin_percentual_lucro_desejado, 0.00);
    g_signal_connect(spin_percentual_lucro_desejado, "value-changed", G_CALLBACK(on_calcular_lucro_changed), NULL);
    gtk_grid_attach(GTK_GRID(grid_lucro), GTK_WIDGET(spin_percentual_lucro_desejado), 1, row++, 1, 1);

    gtk_grid_attach(GTK_GRID(grid_lucro), gtk_label_new("Lucro Calculado:"), 0, row, 1, 1);
    label_lucro_calculado = GTK_LABEL(gtk_label_new("R$ 0.00 (0.00%)"));
    gtk_label_set_xalign(label_lucro_calculado, 0); gtk_grid_attach(GTK_GRID(grid_lucro), GTK_WIDGET(label_lucro_calculado), 1, row++, 1, 1);

    gtk_grid_attach(GTK_GRID(grid_lucro), gtk_label_new("Custo Final Total:"), 0, row, 1, 1);
    label_custo_final_total = GTK_LABEL(gtk_label_new("R$ 0.00"));
    gtk_label_set_xalign(label_custo_final_total, 0); gtk_grid_attach(GTK_GRID(grid_lucro), GTK_WIDGET(label_custo_final_total), 1, row++, 1, 1);

    gtk_grid_attach(GTK_GRID(grid_lucro), gtk_label_new("Custo Final por Aluno:"), 0, row, 1, 1);
    label_custo_final_por_aluno = GTK_LABEL(gtk_label_new("R$ 0.00"));
    gtk_label_set_xalign(label_custo_final_por_aluno, 0); gtk_grid_attach(GTK_GRID(grid_lucro), GTK_WIDGET(label_custo_final_por_aluno), 1, row++, 1, 1);

    GtkWidget *btn_salvar_viagem = gtk_button_new_with_label("Salvar Nova Viagem");
    gtk_widget_set_halign(btn_salvar_viagem, GTK_ALIGN_CENTER);
    g_signal_connect(btn_salvar_viagem, "clicked", G_CALLBACK(on_salvar_nova_viagem_clicked), NULL);
    gtk_grid_attach(GTK_GRID(grid_lucro), btn_salvar_viagem, 0, row++, 2, 1);

    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), scrolled_win_lucro, gtk_label_new("Lucro"));

    GtkWidget *btn_voltar_cadastro = gtk_button_new_with_label("Voltar ao Menu Principal");
    gtk_widget_set_halign(btn_voltar_cadastro, GTK_ALIGN_END);
    g_signal_connect_swapped(btn_voltar_cadastro, "clicked", G_CALLBACK(limpar_formulario_cadastro_viagem), NULL);
    g_signal_connect(btn_voltar_cadastro, "clicked", G_CALLBACK(mudar_para_pagina), (gpointer)"menu_principal_page");
    gtk_box_pack_end(GTK_BOX(pagina_cadastro_viagem), btn_voltar_cadastro, FALSE, FALSE, 10);

    gtk_widget_show_all(pagina_cadastro_viagem);

    limpar_formulario_cadastro_viagem();
}