#include "gui_edicao_viagem.h"
#include "alunos_gerenciador.h"
#include "gui_alunos.h"
#include <time.h>  
#include <string.h>

#define MAX_ALUNOS_POR_TURMA 100

// Variável global para a página de edição (ÚNICA DEFINIÇÃO)
GtkWidget *pagina_edicao_viagem = NULL;

// Variável para armazenar o índice da viagem que está sendo editada
static int index_viagem_em_edicao = -1;

// Variável temporária para armazenar dados da viagem durante a edição
static Viagem viagem_em_edicao;

// Turmas temporárias para a edição (copiadas da viagem original)
static Turma *turmas_em_edicao[MAX_TURMAS_POR_VIAGEM];
static int num_turmas_em_edicao = 0;

// Widgets para a data da viagem (agora somente leitura na edição)
static GtkSpinButton *spin_edicao_data_dia;
static GtkSpinButton *spin_edicao_data_mes;
static GtkSpinButton *spin_edicao_data_ano;

// Declarações dos widgets estáticos globais
static GtkEntry *entry_edicao_nome_viagem;
static GtkEntry *entry_edicao_nome_escola;
static GtkEntry *entry_edicao_destino;
static GtkSpinButton *spin_edicao_dias;
static GtkSpinButton *spin_edicao_num_guias_viagem;
static GtkCheckButton *check_edicao_opcao_guia_local;
static GtkSpinButton *spin_edicao_guia_local_valor;
static GtkSpinButton *spin_edicao_convidados;
static GtkSpinButton *spin_edicao_motoristas;
static GtkCheckButton *check_edicao_opcao_viagem_extra;
static GtkEntry *entry_edicao_viagem_extra_nome;
static GtkSpinButton *spin_edicao_viagem_extra_valor;
static GtkSpinButton *spin_edicao_min_qte_alunos;

// Custos Principais
static GtkSpinButton *spin_edicao_onibus;
static GtkSpinButton *spin_edicao_hotel;
static GtkSpinButton *spin_edicao_actes_por_professor;
static GtkSpinButton *spin_edicao_pulseiras;
static GtkSpinButton *spin_edicao_etiquetas;

// Custos com Pessoas
static GtkSpinButton *spin_edicao_guia_viagem_valor_total;
static GtkSpinButton *spin_edicao_despesas_guia_viagem;
static GtkSpinButton *spin_edicao_despesas_motorista;
static GtkSpinButton *spin_edicao_despesas_luciana;
static GtkSpinButton *spin_edicao_taxa_apoio_escola;

// Custos Diversos
static GtkSpinButton *spin_edicao_farmacia;
static GtkSpinButton *spin_edicao_gasto_extra_geral;
static GtkSpinButton *spin_edicao_emergencia;
static GtkSpinButton *spin_edicao_poupanca_lumaster;
static GtkSpinButton *spin_edicao_seguro_viagem;
static GtkListBox *listbox_edicao_outros_custos;

// Custos com Alimentação
static GtkSpinButton *spin_edicao_val_almo_alunos_dias[MAX_DIAS_VIAGEM];
static GtkSpinButton *spin_edicao_val_jantar_alunos_dias[MAX_DIAS_VIAGEM];
static GtkSpinButton *spin_edicao_val_almo_profs_dias[MAX_DIAS_VIAGEM];
static GtkSpinButton *spin_edicao_val_jantares_profs_dias[MAX_DIAS_VIAGEM];

static GtkSpinButton *spin_edicao_quant_almocos_alunos_total_dias;
static GtkSpinButton *spin_edicao_quant_jantares_alunos_total_dias;
static GtkSpinButton *spin_edicao_quant_almocos_profs_total_dias;
static GtkSpinButton *spin_edicao_quant_jantares_profs_total_dias;
static GtkSpinButton *spin_edicao_val_servico_bordo;
static GtkSpinButton *spin_edicao_val_alimentacao_extra;

// Add this declaration for the grid used in alimentação tab
static GtkWidget *grid_edicao_custos_alimentacao;

// Definição de Lucro
static GtkSpinButton *spin_edicao_percentual_lucro_desejado;
static GtkLabel *label_edicao_custo_total_base;
static GtkLabel *label_edicao_custo_aluno_base;
static GtkLabel *label_edicao_lucro_calculado;
static GtkLabel *label_edicao_custo_final_total;
static GtkLabel *label_edicao_custo_final_por_aluno; // Exibe o totalPorAluno
static GtkListBox *listbox_edicao_turmas_cadastradas;

// NOVO: Variável estática para o botão "Adicionar Outro Custo"
static GtkWidget *btn_add_outro_custo_edicao;

// NEW: Variáveis estáticas para os rótulos de "Custo por aluno" e "Data Viagem" (não editáveis)
static GtkLabel *label_custo_por_aluno_display;
static GtkLabel *label_data_viagem_display;


// --- FUNÇÕES AUXILIARES E CALLBACKS ---

// Forward declarations para funções estáticas internas
static void atualizar_listbox_edicao_turmas();
static void on_calcular_lucro_edicao_changed(GtkSpinButton *spin_button, gpointer user_data);
static void on_remover_edicao_turma_clicked(GtkButton *btn, gpointer user_data);
static void on_adicionar_edicao_turma_clicked(GtkButton *btn, gpointer user_data);
static void on_add_edicao_outro_custo_clicked(GtkButton *btn, gpointer user_data);
static void on_salvar_edicao_viagem_clicked(GtkButton *btn, gpointer user_data);

// Estas funções de "changed" agora apenas ajustam a visibilidade e não disparam recalculo
static void on_edicao_dias_viagem_changed(GtkSpinButton *spin_button, gpointer user_data);
static void on_edicao_quant_almocos_alunos_changed(GtkSpinButton *spin_button, gpointer user_data);
static void on_edicao_quant_jantares_alunos_changed(GtkSpinButton *spin_button, gpointer user_data);
static void on_edicao_quant_almocos_profs_changed(GtkSpinButton *spin_button, gpointer user_data);
static void on_edicao_quant_jantares_profs_changed(GtkSpinButton *spin_button, gpointer user_data);
static void on_edicao_guia_local_toggled(GtkToggleButton *toggle_button, gpointer user_data);
static void on_edicao_viagem_extra_toggled(GtkToggleButton *toggle_button, gpointer user_data);

// Callback para o botão "Adicionar Aluno" na lista de turmas
static void on_adicionar_aluno_a_turma_clicked(GtkButton *btn, gpointer user_data) {
    int id_viagem_contexto = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(btn), "id_viagem"));
    const char *nome_turma_contexto = (const char *)g_object_get_data(G_OBJECT(btn), "nome_turma");

    g_print("DEBUG: Botão 'Adicionar Aluno' clicado para Viagem ID: %d, Turma: %s\n", id_viagem_contexto, nome_turma_contexto);

    // Atualiza os campos editáveis (Nome, Escola, Destino) na struct global `viagens`
    strncpy(viagens[id_viagem_contexto].nome_viagem, gtk_entry_get_text(entry_edicao_nome_viagem), MAX_NOME - 1);
    viagens[id_viagem_contexto].nome_viagem[MAX_NOME - 1] = '\0';
    strncpy(viagens[id_viagem_contexto].nome_escola, gtk_entry_get_text(entry_edicao_nome_escola), MAX_NOME - 1);
    viagens[id_viagem_contexto].nome_escola[MAX_NOME - 1] = '\0';
    strncpy(viagens[id_viagem_contexto].destino, gtk_entry_get_text(entry_edicao_destino), 49);
    viagens[id_viagem_contexto].destino[49] = '\0';

    // Garante que as turmas no array global `viagens` reflitam o que está na tela de edição.
    viagens[id_viagem_contexto].num_turmas_cadastradas = 0;
    for (int i = 0; i < num_turmas_em_edicao; i++) {
        if (turmas_em_edicao[i] && turmas_em_edicao[i]->ativa) {
            viagens[id_viagem_contexto].turmas[viagens[id_viagem_contexto].num_turmas_cadastradas] = *turmas_em_edicao[i];
            viagens[id_viagem_contexto].turmas[viagens[id_viagem_contexto].num_turmas_cadastradas].ativa = 1;
            viagens[id_viagem_contexto].turmas[viagens[id_viagem_contexto].num_turmas_cadastradas].alunos = turmas_em_edicao[i]->alunos;
            viagens[id_viagem_contexto].turmas[viagens[id_viagem_contexto].num_turmas_cadastradas].numprofessores = turmas_em_edicao[i]->numprofessores;
            viagens[id_viagem_contexto].num_turmas_cadastradas++;
        }
    }

    salvar_dados(); // Salva a struct global 'viagens' com as turmas atualizadas.

    exibir_cadastro_aluno_para_viagem_turma(id_viagem_contexto, nome_turma_contexto);
}

// Implementações das funções auxiliares
static void on_edicao_guia_local_toggled(GtkToggleButton *toggle_button, gpointer user_data) {
    gboolean active = gtk_toggle_button_get_active(toggle_button);
    gtk_widget_set_sensitive(GTK_WIDGET(spin_edicao_guia_local_valor), active);
}

static void on_edicao_viagem_extra_toggled(GtkToggleButton *toggle_button, gpointer user_data) {
    gboolean active = gtk_toggle_button_get_active(toggle_button);
    gtk_widget_set_sensitive(GTK_WIDGET(entry_edicao_viagem_extra_nome), active);
    gtk_widget_set_sensitive(GTK_WIDGET(spin_edicao_viagem_extra_valor), active);
}

static void on_edicao_quant_almocos_alunos_changed(GtkSpinButton *spin_button, gpointer user_data) {
    GtkGrid *grid = GTK_GRID(user_data);
    int num_dias = gtk_spin_button_get_value_as_int(spin_button);
    int base_row = 1;
    for (int i = 0; i < MAX_DIAS_VIAGEM; i++) {
        gboolean visible = (i < num_dias);
        if (spin_edicao_val_almo_alunos_dias[i]) {
            gtk_widget_set_visible(gtk_grid_get_child_at(GTK_GRID(grid), 0, base_row + i), visible);
            gtk_widget_set_visible(GTK_WIDGET(spin_edicao_val_almo_alunos_dias[i]), visible);
        }
    }
}

static void on_edicao_quant_jantares_alunos_changed(GtkSpinButton *spin_button, gpointer user_data) {
    GtkGrid *grid = GTK_GRID(user_data);
    int num_dias = gtk_spin_button_get_value_as_int(spin_button);
    int base_row = (1 + MAX_DIAS_VIAGEM + 1);
    for (int i = 0; i < MAX_DIAS_VIAGEM; i++) {
        gboolean visible = (i < num_dias);
        if (spin_edicao_val_jantar_alunos_dias[i]) {
            gtk_widget_set_visible(gtk_grid_get_child_at(GTK_GRID(grid), 0, base_row + i), visible);
            gtk_widget_set_visible(GTK_WIDGET(spin_edicao_val_jantar_alunos_dias[i]), visible);
        }
    }
}

static void on_edicao_quant_almocos_profs_changed(GtkSpinButton *spin_button, gpointer user_data) {
    GtkGrid *grid = GTK_GRID(user_data);
    int num_dias = gtk_spin_button_get_value_as_int(spin_button);
    int base_row = (1 + MAX_DIAS_VIAGEM + 1) + (1 + MAX_DIAS_VIAGEM + 1);
    for (int i = 0; i < MAX_DIAS_VIAGEM; i++) {
        gboolean visible = (i < num_dias);
        if (spin_edicao_val_almo_profs_dias[i]) {
            gtk_widget_set_visible(gtk_grid_get_child_at(GTK_GRID(grid), 0, base_row + i), visible);
            gtk_widget_set_visible(GTK_WIDGET(spin_edicao_val_almo_profs_dias[i]), visible);
        }
    }
}

static void on_edicao_quant_jantares_profs_changed(GtkSpinButton *spin_button, gpointer user_data) {
    GtkGrid *grid = GTK_GRID(user_data);
    int num_dias = gtk_spin_button_get_value_as_int(spin_button);
    int base_row = (1 + MAX_DIAS_VIAGEM + 1) + (1 + MAX_DIAS_VIAGEM + 1) + (1 + MAX_DIAS_VIAGEM + 1);
    for (int i = 0; i < MAX_DIAS_VIAGEM; i++) {
        gboolean visible = (i < num_dias);
        if (spin_edicao_val_jantares_profs_dias[i]) {
            gtk_widget_set_visible(gtk_grid_get_child_at(GTK_GRID(grid), 0, base_row + i), visible);
            gtk_widget_set_visible(GTK_WIDGET(spin_edicao_val_jantares_profs_dias[i]), visible);
        }
    }
}

static void on_edicao_dias_viagem_changed(GtkSpinButton *spin_button, gpointer user_data) {
    int num_dias = gtk_spin_button_get_value_as_int(spin_button);
    gtk_adjustment_set_upper(gtk_spin_button_get_adjustment(spin_edicao_quant_almocos_alunos_total_dias), num_dias);
    gtk_adjustment_set_upper(gtk_spin_button_get_adjustment(spin_edicao_quant_jantares_alunos_total_dias), num_dias);
    gtk_adjustment_set_upper(gtk_spin_button_get_adjustment(spin_edicao_quant_almocos_profs_total_dias), num_dias);
    gtk_adjustment_set_upper(gtk_spin_button_get_adjustment(spin_edicao_quant_jantares_profs_total_dias), num_dias);

    if (gtk_spin_button_get_value_as_int(spin_edicao_quant_almocos_alunos_total_dias) > num_dias) {
        gtk_spin_button_set_value(spin_edicao_quant_almocos_alunos_total_dias, num_dias);
    } else {
        on_edicao_quant_almocos_alunos_changed(spin_edicao_quant_almocos_alunos_total_dias, user_data);
    }
    if (gtk_spin_button_get_value_as_int(spin_edicao_quant_jantares_alunos_total_dias) > num_dias) {
        gtk_spin_button_set_value(spin_edicao_quant_jantares_alunos_total_dias, num_dias);
    } else {
        on_edicao_quant_jantares_alunos_changed(spin_edicao_quant_jantares_alunos_total_dias, user_data);
    }
    if (gtk_spin_button_get_value_as_int(spin_edicao_quant_almocos_profs_total_dias) > num_dias) {
        gtk_spin_button_set_value(spin_edicao_quant_almocos_profs_total_dias, num_dias);
    } else {
        on_edicao_quant_almocos_profs_changed(spin_edicao_quant_almocos_profs_total_dias, user_data);
    }
    if (gtk_spin_button_get_value_as_int(spin_edicao_quant_jantares_profs_total_dias) > num_dias) {
        gtk_spin_button_set_value(spin_edicao_quant_jantares_profs_total_dias, num_dias);
    } else {
        on_edicao_quant_jantares_profs_changed(spin_edicao_quant_jantares_profs_total_dias, user_data);
    }
}

// Esta função on_calcular_lucro_edicao_changed agora só será chamada na inicialização
// (dentro de carregar_dados_para_edicao) e pelos campos editáveis (Nome, Escola, Destino)
// para atualizar os rótulos de resumo. Ela NÃO recalcula o totalPorAluno.
static void on_calcular_lucro_edicao_changed(GtkSpinButton *spin_button, gpointer user_data) {
    // AQUI, os rótulos de custo total, lucro, etc., continuarão a refletir os valores da
    // viagem_em_edicao (que veio diretamente da struct salva `viagens[index]`),
    // SEM serem recalculados com base nos widgets da GUI (que estarão desativados).

    // Os dados da viagem_em_edicao devem vir preenchidos de carregar_dados_para_edicao()
    // e devem ter o totalPorAluno correto da viagem salva.

    double total_base_calculado, custo_aluno_base_simulacao, dummy1, dummy2, dummy3, dummy4;
    calcular_custos_base(&viagem_em_edicao, &total_base_calculado, &custo_aluno_base_simulacao, &dummy1, &dummy2, &dummy3, &dummy4);

    double lucro_perc = viagem_em_edicao.percentual_lucro_desejado;
    double lucro_total = total_base_calculado * (lucro_perc / 100.0);
    double total_final_simulacao = total_base_calculado + lucro_total;
    
    char buffer[100];
    snprintf(buffer, sizeof(buffer), "R$ %.2f", total_base_calculado);
    gtk_label_set_text(label_edicao_custo_total_base, buffer);

    if (viagem_em_edicao.minQteAlunos > 0) {
        snprintf(buffer, sizeof(buffer), "R$ %.2f", custo_aluno_base_simulacao);
        gtk_label_set_text(label_edicao_custo_aluno_base, buffer);
    } else {
        gtk_label_set_text(label_edicao_custo_aluno_base, "N/A (Min. Alunos = 0)");
    }
    
    snprintf(buffer, sizeof(buffer), "R$ %.2f (%.2f%%)", lucro_total, lucro_perc);
    gtk_label_set_text(label_edicao_lucro_calculado, buffer);

    snprintf(buffer, sizeof(buffer), "R$ %.2f", total_final_simulacao);
    gtk_label_set_text(label_edicao_custo_final_total, buffer);

    if (viagem_em_edicao.minQteAlunos > 0) {
        // Exibe o totalPorAluno que veio da struct carregada
        snprintf(buffer, sizeof(buffer), "R$ %.2f", viagem_em_edicao.totalPorAluno);
        gtk_label_set_text(label_edicao_custo_final_por_aluno, buffer);
    } else {
        gtk_label_set_text(label_edicao_custo_final_por_aluno, "N/A (Min. Alunos = 0)");
    }
}

// --- Callback para remover turma na edição ---
static void on_remover_edicao_turma_clicked(GtkButton *btn, gpointer user_data) {
    int turma_index_to_remove = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(btn), "turma_index"));
    GtkWidget *parent_window = gtk_widget_get_ancestor(GTK_WIDGET(btn), GTK_TYPE_WINDOW);

    if (mostrar_dialogo_confirmacao(GTK_WINDOW(parent_window), "Confirmar Remoção", "Tem certeza que deseja remover esta turma? Isso também inativará os alunos associados a esta turma.")) {
        if (turma_index_to_remove >= 0 && turma_index_to_remove < num_turmas_em_edicao) {
            
            if (turmas_em_edicao[turma_index_to_remove]) {
                turmas_em_edicao[turma_index_to_remove]->ativa = 0; // Marca como inativa na cópia temporária
            }
            
            g_free(turmas_em_edicao[turma_index_to_remove]);
            for (int i = turma_index_to_remove; i < num_turmas_em_edicao - 1; i++) {
                turmas_em_edicao[i] = turmas_em_edicao[i+1];
            }
            turmas_em_edicao[num_turmas_em_edicao - 1] = NULL;
            num_turmas_em_edicao--;

            // Atualiza os campos editáveis (Nome, Escola, Destino) na struct global `viagens`
            strncpy(viagens[index_viagem_em_edicao].nome_viagem, gtk_entry_get_text(entry_edicao_nome_viagem), MAX_NOME - 1);
            viagens[index_viagem_em_edicao].nome_viagem[MAX_NOME - 1] = '\0';
            strncpy(viagens[index_viagem_em_edicao].nome_escola, gtk_entry_get_text(entry_edicao_nome_escola), MAX_NOME - 1);
            viagens[index_viagem_em_edicao].nome_escola[MAX_NOME - 1] = '\0';
            strncpy(viagens[index_viagem_em_edicao].destino, gtk_entry_get_text(entry_edicao_destino), 49);
            viagens[index_viagem_em_edicao].destino[49] = '\0';

            // RECONSTRÓI A LISTA DE TURMAS NO ARRAY GLOBAL `viagens` A PARTIR DO ARRAY TEMPORÁRIO
            viagens[index_viagem_em_edicao].num_turmas_cadastradas = 0;
            for (int i = 0; i < num_turmas_em_edicao; i++) {
                if (turmas_em_edicao[i] && turmas_em_edicao[i]->ativa) {
                    viagens[index_viagem_em_edicao].turmas[viagens[index_viagem_em_edicao].num_turmas_cadastradas] = *turmas_em_edicao[i];
                    viagens[index_viagem_em_edicao].turmas[viagens[index_viagem_em_edicao].num_turmas_cadastradas].ativa = 1;
                    viagens[index_viagem_em_edicao].turmas[viagens[index_viagem_em_edicao].num_turmas_cadastradas].alunos = turmas_em_edicao[i]->alunos;
                    viagens[index_viagem_em_edicao].turmas[viagens[index_viagem_em_edicao].num_turmas_cadastradas].numprofessores = turmas_em_edicao[i]->numprofessores;
                    viagens[index_viagem_em_edicao].num_turmas_cadastradas++;
                }
            }
            // Não inativar alunos da turma aqui, apenas ao "salvar viagem" (ou um botão específico)
            // se essa funcionalidade for necessária e dissociada da edição de turmas.
            // Por enquanto, a lógica `inativar_alunos_por_viagem` é chamada ao REMOVER a VIAGEM.

            salvar_dados(); // Salva as alterações na struct global `viagens`

            mostrar_dialogo_info(GTK_WINDOW(parent_window), "Turma Removida", "Turma removida e viagem atualizada com sucesso.");
            
            atualizar_listbox_edicao_turmas(); // Atualiza a exibição da lista de turmas
        }
    }
}

// --- FUNÇÃO PARA ATUALIZAR O LISTBOX DE TURMAS NA EDIÇÃO ---
static void atualizar_listbox_edicao_turmas() {
    g_print("DEBUG (atualizar_listbox_edicao_turmas): Atualizando lista de turmas na tela de edição.\n");
    GList *children_turmas, *iter_turmas;
    children_turmas = gtk_container_get_children(GTK_CONTAINER(listbox_edicao_turmas_cadastradas));
    for (iter_turmas = children_turmas; iter_turmas != NULL; iter_turmas = g_list_next(iter_turmas)) {
        gtk_widget_destroy(GTK_WIDGET(iter_turmas->data));
    }
    g_list_free(children_turmas);

    if (num_turmas_em_edicao == 0) {
        GtkWidget *label = gtk_label_new("Nenhuma turma cadastrada para esta viagem.");
        gtk_list_box_insert(listbox_edicao_turmas_cadastradas, label, -1);
        gtk_widget_show(label);
    } else {
        for (int i = 0; i < num_turmas_em_edicao; i++) {
            Turma *t = turmas_em_edicao[i];
            if (t && t->ativa) { // Apenas mostra turmas ativas no array temporário
                char turma_info[256];
                snprintf(turma_info, sizeof(turma_info),
                         "<b>%s</b>: %d alunos, %d professores",
                         t->nome_turma, t->alunos, t->numprofessores);

                GtkWidget *row_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
                GtkWidget *label = gtk_label_new(NULL);
                gtk_label_set_markup(GTK_LABEL(label), turma_info);
                gtk_label_set_xalign(GTK_LABEL(label), 0);
                gtk_box_pack_start(GTK_BOX(row_box), label, TRUE, TRUE, 0);

                // Botão "Remover"
                GtkWidget *btn_remover_turma = gtk_button_new_with_label("Remover");
                g_object_set_data(G_OBJECT(btn_remover_turma), "turma_index", GINT_TO_POINTER(i));
                g_signal_connect(btn_remover_turma, "clicked", G_CALLBACK(on_remover_edicao_turma_clicked), NULL);
                gtk_box_pack_end(GTK_BOX(row_box), btn_remover_turma, FALSE, FALSE, 0);

                // Botão "Adicionar Aluno"
                GtkWidget *btn_add_aluno = gtk_button_new_with_label("Adicionar Aluno");
                g_object_set_data(G_OBJECT(btn_add_aluno), "id_viagem", GINT_TO_POINTER(index_viagem_em_edicao));
                g_object_set_data_full(G_OBJECT(btn_add_aluno), "nome_turma", 
                                        g_strdup(t->nome_turma), 
                                        (GDestroyNotify)g_free);

                g_signal_connect(btn_add_aluno, "clicked", G_CALLBACK(on_adicionar_aluno_a_turma_clicked), NULL);
                gtk_box_pack_end(GTK_BOX(row_box), btn_add_aluno, FALSE, FALSE, 0);

                GtkWidget *row = gtk_list_box_row_new();
                gtk_container_add(GTK_CONTAINER(row), row_box);
                gtk_list_box_insert(listbox_edicao_turmas_cadastradas, row, -1);
                gtk_widget_show_all(row);
            }
        }
    }
}

// --- FUNÇÃO PARA DIÁLOGO DE CADASTRO DE TURMA NA EDIÇÃO ---
void abrir_dialogo_edicao_cadastro_turma(GtkWindow *parent_window) {
    if (num_turmas_em_edicao >= MAX_TURMAS_POR_VIAGEM) {
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
        for (int i = 0; i < num_turmas_em_edicao; i++) {
            if (turmas_em_edicao[i] && turmas_em_edicao[i]->ativa && strcmp(turmas_em_edicao[i]->nome_turma, nome_turma_digitado) == 0) {
                turma_ja_existe = TRUE;
                break;
            }
        }
        if (turma_ja_existe) {
            mostrar_dialogo_aviso(parent_window, "Turma Duplicada", "Já existe uma turma com este nome nesta viagem. Escolha outro nome.");
            gtk_widget_destroy(dialog);
            return;
        }

        int new_turma_idx = num_turmas_em_edicao;
        turmas_em_edicao[new_turma_idx] = g_malloc(sizeof(Turma));
        Turma *new_turma = turmas_em_edicao[new_turma_idx];

        if (new_turma) {
            strncpy(new_turma->nome_turma, nome_turma_digitado, MAX_NOME - 1);
            new_turma->nome_turma[MAX_NOME - 1] = '\0';
            new_turma->alunos = 0;
            new_turma->numprofessores = gtk_spin_button_get_value_as_int(spin_professores);
            new_turma->ativa = 1;

            num_turmas_em_edicao++;
            
            // ATUALIZA OS CAMPOS BÁSICOS E AS TURMAS NO ARRAY GLOBAL `viagens`
            strncpy(viagens[index_viagem_em_edicao].nome_viagem, gtk_entry_get_text(entry_edicao_nome_viagem), MAX_NOME - 1);
            viagens[index_viagem_em_edicao].nome_viagem[MAX_NOME - 1] = '\0';
            strncpy(viagens[index_viagem_em_edicao].nome_escola, gtk_entry_get_text(entry_edicao_nome_escola), MAX_NOME - 1);
            viagens[index_viagem_em_edicao].nome_escola[MAX_NOME - 1] = '\0';
            strncpy(viagens[index_viagem_em_edicao].destino, gtk_entry_get_text(entry_edicao_destino), 49);
            viagens[index_viagem_em_edicao].destino[49] = '\0';

            viagens[index_viagem_em_edicao].num_turmas_cadastradas = 0;
            for (int i = 0; i < num_turmas_em_edicao; i++) {
                if (turmas_em_edicao[i] && turmas_em_edicao[i]->ativa) {
                    viagens[index_viagem_em_edicao].turmas[viagens[index_viagem_em_edicao].num_turmas_cadastradas] = *turmas_em_edicao[i];
                    viagens[index_viagem_em_edicao].turmas[viagens[index_viagem_em_edicao].num_turmas_cadastradas].ativa = 1;
                    viagens[index_viagem_em_edicao].turmas[viagens[index_viagem_em_edicao].num_turmas_cadastradas].alunos = turmas_em_edicao[i]->alunos;
                    viagens[index_viagem_em_edicao].turmas[viagens[index_viagem_em_edicao].num_turmas_cadastradas].numprofessores = turmas_em_edicao[i]->numprofessores;
                    viagens[index_viagem_em_edicao].num_turmas_cadastradas++;
                }
            }
            salvar_dados(); // Salva as alterações na struct global `viagens`

            mostrar_dialogo_info(parent_window, "Turma Adicionada", "Turma adicionada e viagem atualizada com sucesso!");
            atualizar_listbox_edicao_turmas(); // Atualiza a exibição da lista de turmas
        } else {
            mostrar_dialogo_erro(parent_window, "Erro", "Falha ao alocar memória para a turma.");
        }
    }
    gtk_widget_destroy(dialog);
}

static void on_adicionar_edicao_turma_clicked(GtkButton *btn, gpointer user_data) {
    GtkWindow *parent_window = GTK_WINDOW(gtk_widget_get_ancestor(GTK_WIDGET(btn), GTK_TYPE_WINDOW));
    abrir_dialogo_edicao_cadastro_turma(parent_window);
}

// A função on_add_edicao_outro_custo_clicked agora está mais para um "visualizador" de custos extras.
static void on_add_edicao_outro_custo_clicked(GtkButton *btn, gpointer user_data) {
    // Esta função será chamada se o botão "Adicionar Outro Custo" for habilitado.
    // Mas na lógica atual, ele está desativado.
    // Mantendo a função, mas ela não terá efeito prático.
    mostrar_dialogo_aviso(NULL, "Funcionalidade Restrita", "A adição de novos custos é restrita na tela de edição para manter a integridade dos dados financeiros da viagem.");
}

// Esta função on_salvar_edicao_viagem_clicked agora salva APENAS NOME, ESCOLA, DESTINO.
static void on_salvar_edicao_viagem_clicked(GtkButton *btn, gpointer user_data) {
    g_print("Botão 'Salvar Edição da Viagem' clicado.\n");

    if (index_viagem_em_edicao == -1) {
        mostrar_dialogo_erro(NULL, "Erro de Edição", "Nenhuma viagem selecionada para edição.");
        return;
    }

    if (strlen(gtk_entry_get_text(entry_edicao_nome_viagem)) == 0 ||
        strlen(gtk_entry_get_text(entry_edicao_nome_escola)) == 0 ||
        strlen(gtk_entry_get_text(entry_edicao_destino)) == 0) {
        mostrar_dialogo_aviso(NULL, "Campos Obrigatórios", "Por favor, preencha o Nome da Viagem, Nome da Escola e Destino.");
        return;
    }

    // Apenas atualiza os campos permitidos na struct global `viagens`
    strncpy(viagens[index_viagem_em_edicao].nome_viagem, gtk_entry_get_text(entry_edicao_nome_viagem), MAX_NOME - 1);
    viagens[index_viagem_em_edicao].nome_viagem[MAX_NOME - 1] = '\0';
    strncpy(viagens[index_viagem_em_edicao].nome_escola, gtk_entry_get_text(entry_edicao_nome_escola), MAX_NOME - 1);
    viagens[index_viagem_em_edicao].nome_escola[MAX_NOME - 1] = '\0';
    strncpy(viagens[index_viagem_em_edicao].destino, gtk_entry_get_text(entry_edicao_destino), 49);
    viagens[index_viagem_em_edicao].destino[49] = '\0';

    // **IMPORTANTE**: Os outros campos (custos, data, lucro, totalPorAluno) NÃO SÃO LIDOS DA GUI AQUI,
    // E, portanto, não sobrescrevem os valores que foram carregados na `viagens[index_viagem_em_edicao]`
    // que já possui os valores corretos da carga inicial.

    salvar_dados(); // Salva a struct global `viagens` (com as alterações nos campos básicos)

    char msg[512];
    snprintf(msg, sizeof(msg), "Viagem '%s' atualizada com sucesso!\nCusto por aluno (inalterado): R$%.2f",
             viagens[index_viagem_em_edicao].nome_viagem, viagens[index_viagem_em_edicao].totalPorAluno);
    mostrar_dialogo_info(NULL, "Edição Concluída", msg);

    index_viagem_em_edicao = -1; // Reseta o índice de edição
    memset(&viagem_em_edicao, 0, sizeof(Viagem)); // Limpa a struct temporária
    
    mudar_para_pagina("gerenciar_viagens_page"); // Volta para a página de gerenciamento
    atualizar_status_viagem_selecionada(); // Atualiza o status caso a viagem editada seja a selecionada
}

// --- FUNÇÃO PARA LIMPAR TODOS OS CAMPOS DO FORMULÁRIO DE EDIÇÃO DE VIAGEM ---
// Esta função é chamada ao entrar na tela de edição apenas para "resetar" os valores visuais iniciais,
// e ao sair da tela para garantir que não haja lixo de dados.
void limpar_formulario_edicao_viagem() {
    gtk_entry_set_text(entry_edicao_nome_viagem, "");
    gtk_entry_set_text(entry_edicao_nome_escola, "");
    gtk_entry_set_text(entry_edicao_destino, "");

    // Resetar rótulos de visualização
    gtk_label_set_text(label_custo_por_aluno_display, "R$ xxx");
    gtk_label_set_text(label_data_viagem_display, "dd/mm/yyyy");


    // Campos de spin button (mesmo que desativados, é bom resetar visualmente)
    gtk_spin_button_set_value(spin_edicao_dias, 1);
    gtk_spin_button_set_value(spin_edicao_num_guias_viagem, 0);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check_edicao_opcao_guia_local), FALSE);
    gtk_spin_button_set_value(spin_edicao_guia_local_valor, 0.00);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check_edicao_opcao_viagem_extra), FALSE);
    gtk_entry_set_text(entry_edicao_viagem_extra_nome, "");
    gtk_spin_button_set_value(spin_edicao_viagem_extra_valor, 0.00);
    gtk_spin_button_set_value(spin_edicao_min_qte_alunos, 1);
    gtk_spin_button_set_value(spin_edicao_data_dia, 1);
    gtk_spin_button_set_value(spin_edicao_data_mes, 1);
    gtk_spin_button_set_value(spin_edicao_data_ano, 2025);

    gtk_spin_button_set_value(spin_edicao_onibus, 0.00);
    gtk_spin_button_set_value(spin_edicao_hotel, 0.00);
    gtk_spin_button_set_value(spin_edicao_actes_por_professor, 0.00);
    gtk_spin_button_set_value(spin_edicao_pulseiras, 0.00);
    gtk_spin_button_set_value(spin_edicao_etiquetas, 0.00);
    gtk_spin_button_set_value(spin_edicao_guia_viagem_valor_total, 0.00);
    gtk_spin_button_set_value(spin_edicao_despesas_guia_viagem, 0.00);
    gtk_spin_button_set_value(spin_edicao_despesas_motorista, 0.00);
    gtk_spin_button_set_value(spin_edicao_despesas_luciana, 0.00);
    gtk_spin_button_set_value(spin_edicao_taxa_apoio_escola, 0.00);
    gtk_spin_button_set_value(spin_edicao_farmacia, 0.00);
    gtk_spin_button_set_value(spin_edicao_gasto_extra_geral, 0.00);
    gtk_spin_button_set_value(spin_edicao_emergencia, 0.00);
    gtk_spin_button_set_value(spin_edicao_poupanca_lumaster, 0.00);
    gtk_spin_button_set_value(spin_edicao_seguro_viagem, 0.00);

    // Limpa e destrói outros custos dinâmicos
    GList *children_temp = gtk_container_get_children(GTK_CONTAINER(listbox_edicao_outros_custos));
    for (GList *iter_temp = children_temp; iter_temp != NULL; iter_temp = g_list_next(iter_temp)) {
        gtk_widget_destroy(GTK_WIDGET(iter_temp->data));
    }
    g_list_free(children_temp);

    gtk_spin_button_set_value(spin_edicao_quant_almocos_alunos_total_dias, 0);
    gtk_spin_button_set_value(spin_edicao_quant_jantares_alunos_total_dias, 0);
    gtk_spin_button_set_value(spin_edicao_quant_almocos_profs_total_dias, 0);
    gtk_spin_button_set_value(spin_edicao_quant_jantares_profs_total_dias, 0);
    
    for (int i = 0; i < MAX_DIAS_VIAGEM; i++) {
        if (spin_edicao_val_almo_alunos_dias[i]) gtk_spin_button_set_value(spin_edicao_val_almo_alunos_dias[i], 0.00);
        if (spin_edicao_val_jantar_alunos_dias[i]) gtk_spin_button_set_value(spin_edicao_val_jantar_alunos_dias[i], 0.00);
        if (spin_edicao_val_almo_profs_dias[i]) gtk_spin_button_set_value(spin_edicao_val_almo_profs_dias[i], 0.00);
        if (spin_edicao_val_jantares_profs_dias[i]) gtk_spin_button_set_value(spin_edicao_val_jantares_profs_dias[i], 0.00);
    }
    gtk_spin_button_set_value(spin_edicao_val_servico_bordo, 0.00);
    gtk_spin_button_set_value(spin_edicao_val_alimentacao_extra, 0.00);

    gtk_spin_button_set_value(spin_edicao_percentual_lucro_desejado, 0.00);
    gtk_label_set_text(label_edicao_custo_total_base, "R$ 0.00");
    gtk_label_set_text(label_edicao_custo_aluno_base, "R$ 0.00");
    gtk_label_set_text(label_edicao_lucro_calculado, "R$ 0.00 (0.00%)");
    gtk_label_set_text(label_edicao_custo_final_total, "R$ 0.00");
    gtk_label_set_text(label_edicao_custo_final_por_aluno, "R$ 0.00");

    for (int i = 0; i < num_turmas_em_edicao; i++) {
        if (turmas_em_edicao[i]) {
            g_free(turmas_em_edicao[i]);
            turmas_em_edicao[i] = NULL;
        }
    }
    num_turmas_em_edicao = 0;
    atualizar_listbox_edicao_turmas(); // Atualiza a exibição da lista de turmas vazia
    index_viagem_em_edicao = -1; // Garante que a viagem em edição seja resetada
    memset(&viagem_em_edicao, 0, sizeof(Viagem)); // Limpa a struct temporária
}

// --- FUNÇÃO PARA CARREGAR OS DADOS DE UMA VIAGEM ESPECÍFICA NA PÁGINA DE EDIÇÃO ---
void carregar_dados_para_edicao(int index) {
    g_print("DEBUG (carregar_dados_para_edicao): Carregando dados para edição da viagem ID: %d\n", index);
    g_print("DEBUG (carregar_dados_para_edicao): Valor totalPorAluno da viagem original: %.2f\n", viagens[index].totalPorAluno);

    if (index < 0 || index >= MAX_VIAGENS || !viagens[index].ativa) {
        mostrar_dialogo_erro(NULL, "Erro", "Viagem inválida para edição.");
        g_print("ERRO (carregar_dados_para_edicao): Viagem inválida ou inativa. ID: %d, Ativa: %d\n", index, viagens[index].ativa);
        return;
    }

    // NÃO CHAMAR limpar_formulario_edicao_viagem() AQUI!
    // Queremos que os dados persistam visualmente ao entrar na tela.
    // O reset visual será feito apenas ao SAIR da tela.

    index_viagem_em_edicao = index;
    // COPIA **TODA** A STRUCT da viagem original para a temporária.
    // Isso é VITAL para que todos os campos, incluindo totalPorAluno e data_da_viagem,
    // sejam transferidos corretamente para `viagem_em_edicao`.
    viagem_em_edicao = viagens[index]; 
    g_print("DEBUG (carregar_dados_para_edicao): Viagem '%s' copiada para edição. totalPorAluno na cópia: %.2f\n", viagem_em_edicao.nome_viagem, viagem_em_edicao.totalPorAluno);

    // Limpa turmas temporárias existentes (após copiar, para não perder dados)
    for (int i = 0; i < num_turmas_em_edicao; i++) {
        if (turmas_em_edicao[i]) {
            g_free(turmas_em_edicao[i]);
            turmas_em_edicao[i] = NULL;
        }
    }
    num_turmas_em_edicao = 0;

    // Copia as turmas da viagem original para as turmas temporárias de edição
    for (int i = 0; i < viagem_em_edicao.num_turmas_cadastradas; i++) {
        if (viagem_em_edicao.turmas[i].ativa) {
            turmas_em_edicao[num_turmas_em_edicao] = g_malloc(sizeof(Turma));
            if (turmas_em_edicao[num_turmas_em_edicao]) {
                *turmas_em_edicao[num_turmas_em_edicao] = viagem_em_edicao.turmas[i];
                num_turmas_em_edicao++;
            }
        }
    }
    g_print("DEBUG (carregar_dados_para_edicao): %d turmas copiadas para edição.\n", num_turmas_em_edicao);


    // --- PREENCHE OS WIDGETS DA GUI E DEFINE SENSIBILIDADE ---

    // Campos EDITÁVEIS
    gtk_entry_set_text(entry_edicao_nome_viagem, viagem_em_edicao.nome_viagem);
    gtk_editable_set_editable(GTK_EDITABLE(entry_edicao_nome_viagem), TRUE);
    gtk_widget_set_sensitive(GTK_WIDGET(entry_edicao_nome_viagem), TRUE);

    gtk_entry_set_text(entry_edicao_nome_escola, viagem_em_edicao.nome_escola);
    gtk_editable_set_editable(GTK_EDITABLE(entry_edicao_nome_escola), TRUE);
    gtk_widget_set_sensitive(GTK_WIDGET(entry_edicao_nome_escola), TRUE);

    gtk_entry_set_text(entry_edicao_destino, viagem_em_edicao.destino);
    gtk_editable_set_editable(GTK_EDITABLE(entry_edicao_destino), TRUE);
    gtk_widget_set_sensitive(GTK_WIDGET(entry_edicao_destino), TRUE);


    // Campos SOMENTE VISUALIZAÇÃO (Labels de exibição)
    char buffer_custo[50];
    snprintf(buffer_custo, sizeof(buffer_custo), "R$ %.2f", viagem_em_edicao.totalPorAluno);
    gtk_label_set_text(label_custo_por_aluno_display, buffer_custo);

    char buffer_data[20];
    strftime(buffer_data, sizeof(buffer_data), "%d/%m/%Y", &viagem_em_edicao.data_da_viagem);
    gtk_label_set_text(label_data_viagem_display, buffer_data);


    // Campos DE CONFIGURAÇÃO (Spin buttons, Check buttons, Entries) - Preencher e DESATIVAR
    gtk_spin_button_set_value(spin_edicao_data_dia, viagem_em_edicao.data_da_viagem.tm_mday);
    gtk_widget_set_sensitive(GTK_WIDGET(spin_edicao_data_dia), FALSE);
    gtk_spin_button_set_value(spin_edicao_data_mes, viagem_em_edicao.data_da_viagem.tm_mon + 1);
    gtk_widget_set_sensitive(GTK_WIDGET(spin_edicao_data_mes), FALSE);
    gtk_spin_button_set_value(spin_edicao_data_ano, viagem_em_edicao.data_da_viagem.tm_year + 1900);
    gtk_widget_set_sensitive(GTK_WIDGET(spin_edicao_data_ano), FALSE);

    gtk_spin_button_set_value(spin_edicao_dias, viagem_em_edicao.dias);
    gtk_widget_set_sensitive(GTK_WIDGET(spin_edicao_dias), FALSE);
    gtk_spin_button_set_value(spin_edicao_num_guias_viagem, viagem_em_edicao.numguias_viagem);
    gtk_widget_set_sensitive(GTK_WIDGET(spin_edicao_num_guias_viagem), FALSE);

    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check_edicao_opcao_guia_local), viagem_em_edicao.opcao_guia_local == 'S');
    gtk_widget_set_sensitive(GTK_WIDGET(check_edicao_opcao_guia_local), FALSE);
    gtk_spin_button_set_value(spin_edicao_guia_local_valor, viagem_em_edicao.guialocalvalor);
    gtk_widget_set_sensitive(GTK_WIDGET(spin_edicao_guia_local_valor), FALSE);

    gtk_spin_button_set_value(spin_edicao_convidados, viagem_em_edicao.convidados);
    gtk_widget_set_sensitive(GTK_WIDGET(spin_edicao_convidados), FALSE);
    gtk_spin_button_set_value(spin_edicao_motoristas, viagem_em_edicao.motoristas);
    gtk_widget_set_sensitive(GTK_WIDGET(spin_edicao_motoristas), FALSE);

    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check_edicao_opcao_viagem_extra), viagem_em_edicao.opcao_viagem_extra == 'S');
    gtk_widget_set_sensitive(GTK_WIDGET(check_edicao_opcao_viagem_extra), FALSE);
    gtk_entry_set_text(entry_edicao_viagem_extra_nome, viagem_em_edicao.viagemextra_nome);
    gtk_editable_set_editable(GTK_EDITABLE(entry_edicao_viagem_extra_nome), FALSE); // Set editable false
    gtk_widget_set_sensitive(GTK_WIDGET(entry_edicao_viagem_extra_nome), FALSE);
    gtk_spin_button_set_value(spin_edicao_viagem_extra_valor, viagem_em_edicao.viagemextra_valor);
    gtk_widget_set_sensitive(GTK_WIDGET(spin_edicao_viagem_extra_valor), FALSE);

    gtk_spin_button_set_value(spin_edicao_min_qte_alunos, viagem_em_edicao.minQteAlunos);
    gtk_widget_set_sensitive(GTK_WIDGET(spin_edicao_min_qte_alunos), FALSE);

    // Custos Principais (Desativar)
    gtk_spin_button_set_value(spin_edicao_onibus, viagem_em_edicao.onibus);
    gtk_widget_set_sensitive(GTK_WIDGET(spin_edicao_onibus), FALSE);
    gtk_spin_button_set_value(spin_edicao_hotel, viagem_em_edicao.hotel);
    gtk_widget_set_sensitive(GTK_WIDGET(spin_edicao_hotel), FALSE);
    gtk_spin_button_set_value(spin_edicao_actes_por_professor, viagem_em_edicao.actes_por_professor);
    gtk_widget_set_sensitive(GTK_WIDGET(spin_edicao_actes_por_professor), FALSE);
    gtk_spin_button_set_value(spin_edicao_pulseiras, viagem_em_edicao.pulseiras);
    gtk_widget_set_sensitive(GTK_WIDGET(spin_edicao_pulseiras), FALSE);
    gtk_spin_button_set_value(spin_edicao_etiquetas, viagem_em_edicao.etiquetas);
    gtk_widget_set_sensitive(GTK_WIDGET(spin_edicao_etiquetas), FALSE);

    // Custos com Pessoas (Desativar)
    gtk_spin_button_set_value(spin_edicao_guia_viagem_valor_total, viagem_em_edicao.guia_viagem_valor_total);
    gtk_widget_set_sensitive(GTK_WIDGET(spin_edicao_guia_viagem_valor_total), FALSE);
    gtk_spin_button_set_value(spin_edicao_despesas_guia_viagem, viagem_em_edicao.despesas_guia_viagem);
    gtk_widget_set_sensitive(GTK_WIDGET(spin_edicao_despesas_guia_viagem), FALSE);
    gtk_spin_button_set_value(spin_edicao_despesas_motorista, viagem_em_edicao.despesas_motorista);
    gtk_widget_set_sensitive(GTK_WIDGET(spin_edicao_despesas_motorista), FALSE);
    gtk_spin_button_set_value(spin_edicao_despesas_luciana, viagem_em_edicao.despesas_luciana);
    gtk_widget_set_sensitive(GTK_WIDGET(spin_edicao_despesas_luciana), FALSE);
    gtk_spin_button_set_value(spin_edicao_taxa_apoio_escola, viagem_em_edicao.taxa_apoio_escola);
    gtk_widget_set_sensitive(GTK_WIDGET(spin_edicao_taxa_apoio_escola), FALSE);

    // Custos Diversos (Desativar)
    gtk_spin_button_set_value(spin_edicao_farmacia, viagem_em_edicao.farmacia);
    gtk_widget_set_sensitive(GTK_WIDGET(spin_edicao_farmacia), FALSE);
    gtk_spin_button_set_value(spin_edicao_gasto_extra_geral, viagem_em_edicao.gasto_extra_geral);
    gtk_widget_set_sensitive(GTK_WIDGET(spin_edicao_gasto_extra_geral), FALSE);
    gtk_spin_button_set_value(spin_edicao_emergencia, viagem_em_edicao.emergencia);
    gtk_widget_set_sensitive(GTK_WIDGET(spin_edicao_emergencia), FALSE);
    gtk_spin_button_set_value(spin_edicao_poupanca_lumaster, viagem_em_edicao.poupanca_lumaster);
    gtk_widget_set_sensitive(GTK_WIDGET(spin_edicao_poupanca_lumaster), FALSE);
    gtk_spin_button_set_value(spin_edicao_seguro_viagem, viagem_em_edicao.seguro_viagem);
    gtk_widget_set_sensitive(GTK_WIDGET(spin_edicao_seguro_viagem), FALSE);

    // Outros Custos Adicionais: Limpar dinamicamente e desativar.
    // Primeiro removemos os anteriores
    GList *children_other_costs_to_destroy = gtk_container_get_children(GTK_CONTAINER(listbox_edicao_outros_custos));
    for (GList *iter = children_other_costs_to_destroy; iter != NULL; iter = g_list_next(iter)) {
        gtk_widget_destroy(GTK_WIDGET(iter->data));
    }
    g_list_free(children_other_costs_to_destroy);

    // Depois adicionamos os da viagem atual como somente leitura
    for (int i = 0; i < viagem_em_edicao.num_outros_custos_diversos; i++) {
        if (viagem_em_edicao.outros_custos_diversos[i].ativa) {
            GtkWidget *row_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
            gtk_widget_set_margin_bottom(row_box, 5);
            GtkWidget *label_desc = gtk_label_new("Descrição:");
            gtk_box_pack_start(GTK_BOX(row_box), label_desc, FALSE, FALSE, 0);
            GtkEntry *entry_desc = GTK_ENTRY(gtk_entry_new());
            gtk_entry_set_text(entry_desc, viagem_em_edicao.outros_custos_diversos[i].descricao);
            gtk_editable_set_editable(GTK_EDITABLE(entry_desc), FALSE); // Somente leitura
            gtk_box_pack_start(GTK_BOX(row_box), GTK_WIDGET(entry_desc), TRUE, TRUE, 0);
            GtkWidget *label_valor = gtk_label_new("Valor (R$):");
            gtk_box_pack_start(GTK_BOX(row_box), label_valor, FALSE, FALSE, 0);
            GtkSpinButton *spin_val = GTK_SPIN_BUTTON(gtk_spin_button_new_with_range(0.00, 100000.00, 0.01));
            gtk_spin_button_set_value(spin_val, viagem_em_edicao.outros_custos_diversos[i].valor);
            gtk_widget_set_sensitive(GTK_WIDGET(spin_val), FALSE); // Somente leitura
            gtk_box_pack_start(GTK_BOX(row_box), GTK_WIDGET(spin_val), FALSE, FALSE, 0);
            GtkWidget *btn_remove = gtk_button_new_from_icon_name("list-remove-symbolic", GTK_ICON_SIZE_SMALL_TOOLBAR);
            gtk_button_set_relief(GTK_BUTTON(btn_remove), GTK_RELIEF_NONE);
            gtk_widget_set_sensitive(GTK_WIDGET(btn_remove), FALSE); // Desativar botão de remover
            gtk_box_pack_end(GTK_BOX(row_box), btn_remove, FALSE, FALSE, 0);

            g_object_set_data(G_OBJECT(row_box), "entry_desc", entry_desc);
            g_object_set_data(G_OBJECT(row_box), "spin_valor", spin_val);

            gtk_list_box_insert(listbox_edicao_outros_custos, row_box, -1);
            gtk_widget_show_all(row_box);
        }
    }
    // Desativar o botão de adicionar outros custos
    gtk_widget_set_sensitive(btn_add_outro_custo_edicao, FALSE);


    gtk_spin_button_set_value(spin_edicao_quant_almocos_alunos_total_dias, viagem_em_edicao.quant_almocos_alunos_total_dias);
    gtk_widget_set_sensitive(GTK_WIDGET(spin_edicao_quant_almocos_alunos_total_dias), FALSE);
    for (int i = 0; i < viagem_em_edicao.quant_almocos_alunos_total_dias && i < MAX_DIAS_VIAGEM; ++i) {
        gtk_spin_button_set_value(spin_edicao_val_almo_alunos_dias[i], viagem_em_edicao.val_almo_alunos_dias[i]);
        gtk_widget_set_sensitive(GTK_WIDGET(spin_edicao_val_almo_alunos_dias[i]), FALSE);
    }
    gtk_spin_button_set_value(spin_edicao_quant_jantares_alunos_total_dias, viagem_em_edicao.quant_jantares_alunos_total_dias);
    gtk_widget_set_sensitive(GTK_WIDGET(spin_edicao_quant_jantares_alunos_total_dias), FALSE);
    for (int i = 0; i < viagem_em_edicao.quant_jantares_alunos_total_dias && i < MAX_DIAS_VIAGEM; ++i) {
        gtk_spin_button_set_value(spin_edicao_val_jantar_alunos_dias[i], viagem_em_edicao.val_jantar_alunos_dias[i]);
        gtk_widget_set_sensitive(GTK_WIDGET(spin_edicao_val_jantar_alunos_dias[i]), FALSE);
    }
    gtk_spin_button_set_value(spin_edicao_quant_almocos_profs_total_dias, viagem_em_edicao.quant_almocos_profs_total_dias);
    gtk_widget_set_sensitive(GTK_WIDGET(spin_edicao_quant_almocos_profs_total_dias), FALSE);
    for (int i = 0; i < viagem_em_edicao.quant_almocos_profs_total_dias && i < MAX_DIAS_VIAGEM; ++i) {
        gtk_spin_button_set_value(spin_edicao_val_almo_profs_dias[i], viagem_em_edicao.val_almo_profs_dias[i]);
        gtk_widget_set_sensitive(GTK_WIDGET(spin_edicao_val_almo_profs_dias[i]), FALSE);
    }
    gtk_spin_button_set_value(spin_edicao_quant_jantares_profs_total_dias, viagem_em_edicao.quant_jantares_profs_total_dias);
    gtk_widget_set_sensitive(GTK_WIDGET(spin_edicao_quant_jantares_profs_total_dias), FALSE);
    for (int i = 0; i < viagem_em_edicao.quant_jantares_profs_total_dias && i < MAX_DIAS_VIAGEM; ++i) {
        gtk_spin_button_set_value(spin_edicao_val_jantares_profs_dias[i], viagem_em_edicao.val_jantares_profs_dias[i]);
        gtk_widget_set_sensitive(GTK_WIDGET(spin_edicao_val_jantares_profs_dias[i]), FALSE);
    }

    gtk_spin_button_set_value(spin_edicao_val_servico_bordo, viagem_em_edicao.val_servico_bordo);
    gtk_widget_set_sensitive(GTK_WIDGET(spin_edicao_val_servico_bordo), FALSE);
    gtk_spin_button_set_value(spin_edicao_val_alimentacao_extra, viagem_em_edicao.val_alimentacao_extra);
    gtk_widget_set_sensitive(GTK_WIDGET(spin_edicao_val_alimentacao_extra), FALSE);
    
    gtk_spin_button_set_value(spin_edicao_percentual_lucro_desejado, viagem_em_edicao.percentual_lucro_desejado);
    gtk_widget_set_sensitive(GTK_WIDGET(spin_edicao_percentual_lucro_desejado), FALSE);

    // Chama os handlers para atualizar a visibilidade dos campos de alimentação (mesmo que desativados)
    on_edicao_dias_viagem_changed(spin_edicao_dias, grid_edicao_custos_alimentacao);
    on_edicao_guia_local_toggled(GTK_TOGGLE_BUTTON(check_edicao_opcao_guia_local), NULL);
    on_edicao_viagem_extra_toggled(GTK_TOGGLE_BUTTON(check_edicao_opcao_viagem_extra), NULL);

    atualizar_listbox_edicao_turmas(); // Atualiza a lista de turmas na tela de edição
    on_calcular_lucro_edicao_changed(NULL, NULL); // Atualiza os rótulos de resumo (apenas exibição)
}

// --- FUNÇÃO PRINCIPAL PARA CONSTRUIR A PÁGINA DE EDIÇÃO DE VIAGEM ---
void construir_pagina_edicao_viagem() {
    pagina_edicao_viagem = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_set_border_width(GTK_CONTAINER(pagina_edicao_viagem), 20);

    GtkWidget *label_titulo = gtk_label_new("<span size='x-large' weight='bold'>Editar Viagem</span>");
    gtk_label_set_use_markup(GTK_LABEL(label_titulo), TRUE);
    gtk_label_set_xalign(GTK_LABEL(label_titulo), 0);
    gtk_box_pack_start(GTK_BOX(pagina_edicao_viagem), label_titulo, FALSE, FALSE, 10);

    // --- Seção de Custo por Aluno e Data da Viagem (Fixos) ---
    GtkWidget *hbox_info_resumo = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 20);
    gtk_widget_set_margin_top(hbox_info_resumo, 10);
    gtk_widget_set_halign(hbox_info_resumo, GTK_ALIGN_CENTER);
    gtk_box_pack_start(GTK_BOX(pagina_edicao_viagem), hbox_info_resumo, FALSE, FALSE, 0);

    GtkWidget *vbox_custo = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    GtkWidget *label_custo_title = gtk_label_new("<b>Custo por aluno:</b>");
    gtk_label_set_use_markup(GTK_LABEL(label_custo_title), TRUE);
    gtk_box_pack_start(GTK_BOX(vbox_custo), label_custo_title, FALSE, FALSE, 0);
    label_custo_por_aluno_display = GTK_LABEL(gtk_label_new("R$ xxx"));
    gtk_box_pack_start(GTK_BOX(vbox_custo), GTK_WIDGET(label_custo_por_aluno_display), FALSE, FALSE, 0);
    GtkWidget *label_nao_editavel_custo = gtk_label_new("");
    gtk_box_pack_start(GTK_BOX(vbox_custo), label_nao_editavel_custo, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(hbox_info_resumo), vbox_custo, FALSE, FALSE, 0);

    GtkWidget *vbox_data = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    GtkWidget *label_data_title = gtk_label_new("<b>Data Viagem:</b>");
    gtk_label_set_use_markup(GTK_LABEL(label_data_title), TRUE);
    gtk_box_pack_start(GTK_BOX(vbox_data), label_data_title, FALSE, FALSE, 0);
    label_data_viagem_display = GTK_LABEL(gtk_label_new("dd/mm/yyyy"));
    gtk_box_pack_start(GTK_BOX(vbox_data), GTK_WIDGET(label_data_viagem_display), FALSE, FALSE, 0);
    GtkWidget *label_nao_editavel_data = gtk_label_new("");
    gtk_box_pack_start(GTK_BOX(vbox_data), label_nao_editavel_data, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(hbox_info_resumo), vbox_data, FALSE, FALSE, 0);


    GtkWidget *notebook = gtk_notebook_new();
    gtk_box_pack_start(GTK_BOX(pagina_edicao_viagem), notebook, TRUE, TRUE, 0);

    // --- ABA 1: DETALHES BÁSICOS ---
    GtkWidget *scrolled_win_basico = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_win_basico), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
    GtkWidget *grid_basico = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid_basico), 5);
    gtk_grid_set_column_spacing(GTK_GRID(grid_basico), 10);
    gtk_container_set_border_width(GTK_CONTAINER(grid_basico), 10);
    gtk_container_add(GTK_CONTAINER(scrolled_win_basico), grid_basico);

    int row = 0;
    gtk_grid_attach(GTK_GRID(grid_basico), gtk_label_new("Nome da Viagem:"), 0, row, 1, 1);
    entry_edicao_nome_viagem = GTK_ENTRY(gtk_entry_new());
    // Apenas conecta para atualização dos rótulos de resumo (via on_calcular_lucro_edicao_changed)
    g_signal_connect(entry_edicao_nome_viagem, "changed", G_CALLBACK(on_calcular_lucro_edicao_changed), NULL);
    gtk_grid_attach(GTK_GRID(grid_basico), GTK_WIDGET(entry_edicao_nome_viagem), 1, row++, 1, 1);

    gtk_grid_attach(GTK_GRID(grid_basico), gtk_label_new("Nome da Escola:"), 0, row, 1, 1);
    entry_edicao_nome_escola = GTK_ENTRY(gtk_entry_new());
    g_signal_connect(entry_edicao_nome_escola, "changed", G_CALLBACK(on_calcular_lucro_edicao_changed), NULL);
    gtk_grid_attach(GTK_GRID(grid_basico), GTK_WIDGET(entry_edicao_nome_escola), 1, row++, 1, 1);

    gtk_grid_attach(GTK_GRID(grid_basico), gtk_label_new("Destino:"), 0, row, 1, 1);
    entry_edicao_destino = GTK_ENTRY(gtk_entry_new());
    g_signal_connect(entry_edicao_destino, "changed", G_CALLBACK(on_calcular_lucro_edicao_changed), NULL);
    gtk_grid_attach(GTK_GRID(grid_basico), GTK_WIDGET(entry_edicao_destino), 1, row++, 1, 1);

    // Campos de Data da Viagem (não editáveis, usando SpinButtons preenchidos e desativados)
    GtkWidget *hbox_data = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_box_pack_start(GTK_BOX(hbox_data), gtk_label_new("Data da Viagem (DD/MM/AAAA):"), FALSE, FALSE, 0);
    spin_edicao_data_dia = criar_spin_button_inteiro(1, 31, 1);
    spin_edicao_data_mes = criar_spin_button_inteiro(1, 12, 1);
    spin_edicao_data_ano = criar_spin_button_inteiro(1900, 2100, 1);
    gtk_box_pack_start(GTK_BOX(hbox_data), GTK_WIDGET(spin_edicao_data_dia), FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(hbox_data), gtk_label_new("/"), FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(hbox_data), GTK_WIDGET(spin_edicao_data_mes), FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(hbox_data), gtk_label_new("/"), FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(hbox_data), GTK_WIDGET(spin_edicao_data_ano), FALSE, FALSE, 0);
    gtk_grid_attach(GTK_GRID(grid_basico), hbox_data, 0, row++, 2, 1);


    gtk_grid_attach(GTK_GRID(grid_basico), gtk_label_new("Dias de Viagem:"), 0, row, 1, 1);
    spin_edicao_dias = GTK_SPIN_BUTTON(gtk_spin_button_new_with_range(1, MAX_DIAS_VIAGEM, 1));
    gtk_grid_attach(GTK_GRID(grid_basico), GTK_WIDGET(spin_edicao_dias), 1, row++, 1, 1);

    gtk_grid_attach(GTK_GRID(grid_basico), gtk_label_new("Número de Guias (não locais):"), 0, row, 1, 1);
    spin_edicao_num_guias_viagem = GTK_SPIN_BUTTON(gtk_spin_button_new_with_range(0, 10, 1));
    gtk_grid_attach(GTK_GRID(grid_basico), GTK_WIDGET(spin_edicao_num_guias_viagem), 1, row++, 1, 1);

    check_edicao_opcao_guia_local = GTK_CHECK_BUTTON(gtk_check_button_new_with_label("Terá Guia Local?"));
    gtk_grid_attach(GTK_GRID(grid_basico), GTK_WIDGET(check_edicao_opcao_guia_local), 0, row, 1, 1);
    spin_edicao_guia_local_valor = GTK_SPIN_BUTTON(gtk_spin_button_new_with_range(0.00, 10000.00, 0.01));
    gtk_spin_button_set_value(spin_edicao_guia_local_valor, 0.00);
    gtk_widget_set_sensitive(GTK_WIDGET(spin_edicao_guia_local_valor), FALSE);
    g_signal_connect(check_edicao_opcao_guia_local, "toggled", G_CALLBACK(on_edicao_guia_local_toggled), NULL);
    gtk_grid_attach(GTK_GRID(grid_basico), gtk_label_new("Valor (R$):"), 1, row, 1, 1);
    gtk_grid_attach(GTK_GRID(grid_basico), GTK_WIDGET(spin_edicao_guia_local_valor), 2, row++, 1, 1);

    gtk_grid_attach(GTK_GRID(grid_basico), gtk_label_new("Número de Convidados:"), 0, row, 1, 1);
    spin_edicao_convidados = GTK_SPIN_BUTTON(gtk_spin_button_new_with_range(0, 10, 1));
    gtk_grid_attach(GTK_GRID(grid_basico), GTK_WIDGET(spin_edicao_convidados), 1, row++, 1, 1);

    gtk_grid_attach(GTK_GRID(grid_basico), gtk_label_new("Número de Motoristas:"), 0, row, 1, 1);
    spin_edicao_motoristas = GTK_SPIN_BUTTON(gtk_spin_button_new_with_range(0, 5, 1));
    gtk_grid_attach(GTK_GRID(grid_basico), GTK_WIDGET(spin_edicao_motoristas), 1, row++, 1, 1);

    check_edicao_opcao_viagem_extra = GTK_CHECK_BUTTON(gtk_check_button_new_with_label("Terá Passeio Extra?"));
    gtk_grid_attach(GTK_GRID(grid_basico), GTK_WIDGET(check_edicao_opcao_viagem_extra), 0, row, 1, 1);
    entry_edicao_viagem_extra_nome = GTK_ENTRY(gtk_entry_new());
    gtk_entry_set_placeholder_text(entry_edicao_viagem_extra_nome, "Nome do Passeio Extra");
    gtk_widget_set_sensitive(GTK_WIDGET(entry_edicao_viagem_extra_nome), FALSE);
    gtk_grid_attach(GTK_GRID(grid_basico), GTK_WIDGET(entry_edicao_viagem_extra_nome), 1, row, 1, 1);
    spin_edicao_viagem_extra_valor = GTK_SPIN_BUTTON(gtk_spin_button_new_with_range(0.00, 50000.00, 0.01));
    gtk_spin_button_set_value(spin_edicao_viagem_extra_valor, 0.00);
    gtk_widget_set_sensitive(GTK_WIDGET(spin_edicao_viagem_extra_valor), FALSE);
    g_signal_connect(check_edicao_opcao_viagem_extra, "toggled", G_CALLBACK(on_edicao_viagem_extra_toggled), NULL);
    gtk_grid_attach(GTK_GRID(grid_basico), gtk_label_new("Valor (R$):"), 2, row, 1, 1);
    gtk_grid_attach(GTK_GRID(grid_basico), GTK_WIDGET(spin_edicao_viagem_extra_valor), 3, row++, 1, 1);

    gtk_grid_attach(GTK_GRID(grid_basico), gtk_label_new("Min. Alunos para Preço:"), 0, row, 1, 1);
    spin_edicao_min_qte_alunos = GTK_SPIN_BUTTON(gtk_spin_button_new_with_range(0, 500, 1));
    gtk_spin_button_set_value(spin_edicao_min_qte_alunos, 1);
    gtk_grid_attach(GTK_GRID(grid_basico), GTK_WIDGET(spin_edicao_min_qte_alunos), 1, row++, 1, 1);

    // --- Seção de Turmas Cadastradas (na Aba 1 ou separada, mas visível) ---
    GtkWidget *frame_turmas = gtk_frame_new("Turmas Cadastradas (Edição)");
    gtk_widget_set_margin_top(frame_turmas, 15);
    gtk_grid_attach(GTK_GRID(grid_basico), frame_turmas, 0, row++, 4, 1);

    GtkWidget *vbox_turmas = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_set_border_width(GTK_CONTAINER(vbox_turmas), 5);
    gtk_container_add(GTK_CONTAINER(frame_turmas), vbox_turmas);

    listbox_edicao_turmas_cadastradas = GTK_LIST_BOX(gtk_list_box_new());
    gtk_list_box_set_selection_mode(listbox_edicao_turmas_cadastradas, GTK_SELECTION_NONE);
    gtk_box_pack_start(GTK_BOX(vbox_turmas), GTK_WIDGET(listbox_edicao_turmas_cadastradas), TRUE, TRUE, 0);

    GtkWidget *btn_add_turma = gtk_button_new_with_label("Adicionar Turma");
    gtk_box_pack_start(GTK_BOX(vbox_turmas), btn_add_turma, FALSE, FALSE, 0);
    g_signal_connect(btn_add_turma, "clicked", G_CALLBACK(abrir_dialogo_edicao_cadastro_turma), NULL);
    
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), scrolled_win_basico, gtk_label_new("Detalhes Básicos"));


    // --- ABA 2: CUSTOS PRINCIPAIS ---
    GtkWidget *scrolled_win_custos_principais = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_win_custos_principais), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
    GtkWidget *grid_custos_principais = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid_custos_principais), 5);
    gtk_grid_set_column_spacing(GTK_GRID(grid_custos_principais), 10);
    gtk_container_set_border_width(GTK_CONTAINER(grid_custos_principais), 10);
    gtk_container_add(GTK_CONTAINER(scrolled_win_custos_principais), grid_custos_principais);

    row = 0;
    gtk_grid_attach(GTK_GRID(grid_custos_principais), gtk_label_new("Valor do Ônibus (R$):"), 0, row, 1, 1);
    spin_edicao_onibus = GTK_SPIN_BUTTON(gtk_spin_button_new_with_range(0.00, 1000000.00, 0.01));
    gtk_grid_attach(GTK_GRID(grid_custos_principais), GTK_WIDGET(spin_edicao_onibus), 1, row++, 1, 1);

    gtk_grid_attach(GTK_GRID(grid_custos_principais), gtk_label_new("Valor do Hotel (R$):"), 0, row, 1, 1);
    spin_edicao_hotel = GTK_SPIN_BUTTON(gtk_spin_button_new_with_range(0.00, 2000000.00, 0.01));
    gtk_grid_attach(GTK_GRID(grid_custos_principais), GTK_WIDGET(spin_edicao_hotel), 1, row++, 1, 1);

    gtk_grid_attach(GTK_GRID(grid_custos_principais), gtk_label_new("Taxa ACTES por Professor (R$):"), 0, row, 1, 1);
    spin_edicao_actes_por_professor = GTK_SPIN_BUTTON(gtk_spin_button_new_with_range(0.00, 10000.00, 0.01));
    gtk_grid_attach(GTK_GRID(grid_custos_principais), GTK_WIDGET(spin_edicao_actes_por_professor), 1, row++, 1, 1);

    gtk_grid_attach(GTK_GRID(grid_custos_principais), gtk_label_new("Valor das Pulseiras (R$):"), 0, row, 1, 1);
    spin_edicao_pulseiras = GTK_SPIN_BUTTON(gtk_spin_button_new_with_range(0.00, 50000.00, 0.01));
    gtk_grid_attach(GTK_GRID(grid_custos_principais), GTK_WIDGET(spin_edicao_pulseiras), 1, row++, 1, 1);

    gtk_grid_attach(GTK_GRID(grid_custos_principais), gtk_label_new("Valor das Etiquetas (R$):"), 0, row, 1, 1);
    spin_edicao_etiquetas = GTK_SPIN_BUTTON(gtk_spin_button_new_with_range(0.00, 50000.00, 0.01));
    gtk_grid_attach(GTK_GRID(grid_custos_principais), GTK_WIDGET(spin_edicao_etiquetas), 1, row++, 1, 1);

    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), scrolled_win_custos_principais, gtk_label_new("Custos Principais"));

    // --- ABA 3: CUSTOS COM PESSOAS ---
    GtkWidget *scrolled_win_custos_pessoas = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_win_custos_pessoas), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
    GtkWidget *grid_custos_pessoas = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid_custos_pessoas), 5);
    gtk_grid_set_column_spacing(GTK_GRID(grid_custos_pessoas), 10);
    gtk_container_set_border_width(GTK_CONTAINER(grid_custos_pessoas), 10);
    gtk_container_add(GTK_CONTAINER(scrolled_win_custos_pessoas), grid_custos_pessoas);

    row = 0;
    gtk_grid_attach(GTK_GRID(grid_custos_pessoas), gtk_label_new("Valor do Guia da Viagem (Total R$):"), 0, row, 1, 1);
    spin_edicao_guia_viagem_valor_total = GTK_SPIN_BUTTON(gtk_spin_button_new_with_range(0.00, 200000.00, 0.01));
    gtk_grid_attach(GTK_GRID(grid_custos_pessoas), GTK_WIDGET(spin_edicao_guia_viagem_valor_total), 1, row++, 1, 1);

    gtk_grid_attach(GTK_GRID(grid_custos_pessoas), gtk_label_new("Despesas dos Guias (R$):"), 0, row, 1, 1);
    spin_edicao_despesas_guia_viagem = GTK_SPIN_BUTTON(gtk_spin_button_new_with_range(0.00, 50000.00, 0.01));
    gtk_grid_attach(GTK_GRID(grid_custos_pessoas), GTK_WIDGET(spin_edicao_despesas_guia_viagem), 1, row++, 1, 1);

    gtk_grid_attach(GTK_GRID(grid_custos_pessoas), gtk_label_new("Despesas dos Motoristas (R$):"), 0, row, 1, 1);
    spin_edicao_despesas_motorista = GTK_SPIN_BUTTON(gtk_spin_button_new_with_range(0.00, 50000.00, 0.01));
    gtk_grid_attach(GTK_GRID(grid_custos_pessoas), GTK_WIDGET(spin_edicao_despesas_motorista), 1, row++, 1, 1);

    gtk_grid_attach(GTK_GRID(grid_custos_pessoas), gtk_label_new("Despesas da Luciana (R$):"), 0, row, 1, 1);
    spin_edicao_despesas_luciana = GTK_SPIN_BUTTON(gtk_spin_button_new_with_range(0.00, 50000.00, 0.01));
    gtk_grid_attach(GTK_GRID(grid_custos_pessoas), GTK_WIDGET(spin_edicao_despesas_luciana), 1, row++, 1, 1);

    gtk_grid_attach(GTK_GRID(grid_custos_pessoas), gtk_label_new("Taxa de Apoio da Escola (R$):"), 0, row, 1, 1);
    spin_edicao_taxa_apoio_escola = GTK_SPIN_BUTTON(gtk_spin_button_new_with_range(0.00, 100000.00, 0.01));
    gtk_grid_attach(GTK_GRID(grid_custos_pessoas), GTK_WIDGET(spin_edicao_taxa_apoio_escola), 1, row++, 1, 1);

    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), scrolled_win_custos_pessoas, gtk_label_new("Custos Pessoas"));

    // --- ABA 4: CUSTOS DIVERSOS ---
    GtkWidget *scrolled_win_custos_diversos = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_win_custos_diversos), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
    GtkWidget *grid_custos_diversos = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid_custos_diversos), 5);
    gtk_grid_set_column_spacing(GTK_GRID(grid_custos_diversos), 10);
    gtk_container_set_border_width(GTK_CONTAINER(grid_custos_diversos), 10);
    gtk_container_add(GTK_CONTAINER(scrolled_win_custos_diversos), grid_custos_diversos);

    row = 0;
    gtk_grid_attach(GTK_GRID(grid_custos_diversos), gtk_label_new("Gastos com Farmácia (R$):"), 0, row, 1, 1);
    spin_edicao_farmacia = GTK_SPIN_BUTTON(gtk_spin_button_new_with_range(0.00, 20000.00, 0.01));
    gtk_grid_attach(GTK_GRID(grid_custos_diversos), GTK_WIDGET(spin_edicao_farmacia), 1, row++, 1, 1);

    gtk_grid_attach(GTK_GRID(grid_custos_diversos), gtk_label_new("Gastos Extras Gerais (R$):"), 0, row, 1, 1);
    spin_edicao_gasto_extra_geral = GTK_SPIN_BUTTON(gtk_spin_button_new_with_range(0.00, 50000.00, 0.01));
    gtk_grid_attach(GTK_GRID(grid_custos_diversos), GTK_WIDGET(spin_edicao_gasto_extra_geral), 1, row++, 1, 1);

    gtk_grid_attach(GTK_GRID(grid_custos_diversos), gtk_label_new("Valor para Emergência (R$):"), 0, row, 1, 1);
    spin_edicao_emergencia = GTK_SPIN_BUTTON(gtk_spin_button_new_with_range(0.00, 100000.00, 0.01));
    gtk_grid_attach(GTK_GRID(grid_custos_diversos), GTK_WIDGET(spin_edicao_emergencia), 1, row++, 1, 1);

    gtk_grid_attach(GTK_GRID(grid_custos_diversos), gtk_label_new("Poupança Lumaster (R$):"), 0, row, 1, 1);
    spin_edicao_poupanca_lumaster = GTK_SPIN_BUTTON(gtk_spin_button_new_with_range(0.00, 100000.00, 0.01));
    gtk_grid_attach(GTK_GRID(grid_custos_diversos), GTK_WIDGET(spin_edicao_poupanca_lumaster), 1, row++, 1, 1);

    gtk_grid_attach(GTK_GRID(grid_custos_diversos), gtk_label_new("Seguro Viagem (R$):"), 0, row, 1, 1);
    spin_edicao_seguro_viagem = GTK_SPIN_BUTTON(gtk_spin_button_new_with_range(0.00, 200000.00, 0.01));
    gtk_grid_attach(GTK_GRID(grid_custos_diversos), GTK_WIDGET(spin_edicao_seguro_viagem), 1, row++, 1, 1);

    // --- Seção de Outros Custos Adicionais ---
    GtkWidget *frame_outros_custos = gtk_frame_new("Outros Custos Adicionais");
    gtk_widget_set_margin_top(frame_outros_custos, 15);
    gtk_grid_attach(GTK_GRID(grid_custos_diversos), frame_outros_custos, 0, row++, 2, 1);

    GtkWidget *vbox_outros_custos = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_set_border_width(GTK_CONTAINER(vbox_outros_custos), 5);
    gtk_container_add(GTK_CONTAINER(frame_outros_custos), vbox_outros_custos);

    listbox_edicao_outros_custos = GTK_LIST_BOX(gtk_list_box_new());
    gtk_list_box_set_selection_mode(listbox_edicao_outros_custos, GTK_SELECTION_NONE);
    gtk_box_pack_start(GTK_BOX(vbox_outros_custos), GTK_WIDGET(listbox_edicao_outros_custos), TRUE, TRUE, 0);

    // Salva o ponteiro do botão na variável estática
    btn_add_outro_custo_edicao = gtk_button_new_with_label("Adicionar Outro Custo"); 
    gtk_box_pack_start(GTK_BOX(vbox_outros_custos), btn_add_outro_custo_edicao, FALSE, FALSE, 0);
    g_signal_connect(btn_add_outro_custo_edicao, "clicked", G_CALLBACK(on_add_edicao_outro_custo_clicked), NULL);
    
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), scrolled_win_custos_diversos, gtk_label_new("Custos Diversos"));

    // --- ABA 5: CUSTOS COM ALIMENTAÇÃO ---
    grid_edicao_custos_alimentacao = gtk_grid_new();
    GtkWidget *local_scrolled_win_custos_alimentacao = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(local_scrolled_win_custos_alimentacao), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);

    gtk_grid_set_row_spacing(GTK_GRID(grid_edicao_custos_alimentacao), 5);
    gtk_grid_set_column_spacing(GTK_GRID(grid_edicao_custos_alimentacao), 10);
    gtk_container_set_border_width(GTK_CONTAINER(grid_edicao_custos_alimentacao), 10);
    gtk_container_add(GTK_CONTAINER(local_scrolled_win_custos_alimentacao), grid_edicao_custos_alimentacao);

    // O spin_edicao_dias será desativado, então este sinal não terá efeito interativo
    g_signal_connect(spin_edicao_dias, "value-changed", G_CALLBACK(on_edicao_dias_viagem_changed), grid_edicao_custos_alimentacao);

    row = 0;
    gtk_grid_attach(GTK_GRID(grid_edicao_custos_alimentacao), gtk_label_new("Dias com Almoço Alunos:"), 0, row, 1, 1);
    spin_edicao_quant_almocos_alunos_total_dias = GTK_SPIN_BUTTON(gtk_spin_button_new_with_range(0, MAX_DIAS_VIAGEM, 1));
    g_signal_connect(spin_edicao_quant_almocos_alunos_total_dias, "value-changed", G_CALLBACK(on_edicao_quant_almocos_alunos_changed), grid_edicao_custos_alimentacao);
    gtk_grid_attach(GTK_GRID(grid_edicao_custos_alimentacao), GTK_WIDGET(spin_edicao_quant_almocos_alunos_total_dias), 1, row++, 1, 1);
    for (int i = 0; i < MAX_DIAS_VIAGEM; i++) {
        char label_text[50];
        snprintf(label_text, sizeof(label_text), "Valor Almoço Alunos Dia %d (R$):", i + 1);
        gtk_grid_attach(GTK_GRID(grid_edicao_custos_alimentacao), gtk_label_new(label_text), 0, row, 1, 1);
        spin_edicao_val_almo_alunos_dias[i] = GTK_SPIN_BUTTON(gtk_spin_button_new_with_range(0.00, 500.00, 0.01));
        gtk_grid_attach(GTK_GRID(grid_edicao_custos_alimentacao), GTK_WIDGET(spin_edicao_val_almo_alunos_dias[i]), 1, row++, 1, 1);
        gtk_widget_set_visible(gtk_grid_get_child_at(GTK_GRID(grid_edicao_custos_alimentacao), 0, row - 1), FALSE);
        gtk_widget_set_visible(GTK_WIDGET(spin_edicao_val_almo_alunos_dias[i]), FALSE);
    }

    row++;
    gtk_grid_attach(GTK_GRID(grid_edicao_custos_alimentacao), gtk_label_new("Dias com Jantar Alunos:"), 0, row, 1, 1);
    spin_edicao_quant_jantares_alunos_total_dias = GTK_SPIN_BUTTON(gtk_spin_button_new_with_range(0, MAX_DIAS_VIAGEM, 1));
    g_signal_connect(spin_edicao_quant_jantares_alunos_total_dias, "value-changed", G_CALLBACK(on_edicao_quant_jantares_alunos_changed), grid_edicao_custos_alimentacao);
    gtk_grid_attach(GTK_GRID(grid_edicao_custos_alimentacao), GTK_WIDGET(spin_edicao_quant_jantares_alunos_total_dias), 1, row++, 1, 1);
    for (int i = 0; i < MAX_DIAS_VIAGEM; i++) {
        char label_text[50];
        snprintf(label_text, sizeof(label_text), "Valor Jantar Alunos Dia %d (R$):", i + 1);
        gtk_grid_attach(GTK_GRID(grid_edicao_custos_alimentacao), gtk_label_new(label_text), 0, row, 1, 1);
        spin_edicao_val_jantar_alunos_dias[i] = GTK_SPIN_BUTTON(gtk_spin_button_new_with_range(0.00, 500.00, 0.01));
        gtk_grid_attach(GTK_GRID(grid_edicao_custos_alimentacao), GTK_WIDGET(spin_edicao_val_jantar_alunos_dias[i]), 1, row++, 1, 1);
        gtk_widget_set_visible(gtk_grid_get_child_at(GTK_GRID(grid_edicao_custos_alimentacao), 0, row - 1), FALSE);
        gtk_widget_set_visible(GTK_WIDGET(spin_edicao_val_jantar_alunos_dias[i]), FALSE);
    }

    row++;
    gtk_grid_attach(GTK_GRID(grid_edicao_custos_alimentacao), gtk_label_new("Dias com Almoço Professores:"), 0, row, 1, 1);
    spin_edicao_quant_almocos_profs_total_dias = GTK_SPIN_BUTTON(gtk_spin_button_new_with_range(0, MAX_DIAS_VIAGEM, 1));
    g_signal_connect(spin_edicao_quant_almocos_profs_total_dias, "value-changed", G_CALLBACK(on_edicao_quant_almocos_profs_changed), grid_edicao_custos_alimentacao);
    gtk_grid_attach(GTK_GRID(grid_edicao_custos_alimentacao), GTK_WIDGET(spin_edicao_quant_almocos_profs_total_dias), 1, row++, 1, 1);
    for (int i = 0; i < MAX_DIAS_VIAGEM; i++) {
        char label_text[50];
        snprintf(label_text, sizeof(label_text), "Valor Almoço Profs Dia %d (R$):", i + 1);
        gtk_grid_attach(GTK_GRID(grid_edicao_custos_alimentacao), gtk_label_new(label_text), 0, row, 1, 1);
        spin_edicao_val_almo_profs_dias[i] = GTK_SPIN_BUTTON(gtk_spin_button_new_with_range(0.00, 500.00, 0.01));
        gtk_grid_attach(GTK_GRID(grid_edicao_custos_alimentacao), GTK_WIDGET(spin_edicao_val_almo_profs_dias[i]), 1, row++, 1, 1);
        gtk_widget_set_visible(gtk_grid_get_child_at(GTK_GRID(grid_edicao_custos_alimentacao), 0, row - 1), FALSE);
        gtk_widget_set_visible(GTK_WIDGET(spin_edicao_val_almo_profs_dias[i]), FALSE);
    }

    row++;
    gtk_grid_attach(GTK_GRID(grid_edicao_custos_alimentacao), gtk_label_new("Dias com Jantar Professores:"), 0, row, 1, 1);
    spin_edicao_quant_jantares_profs_total_dias = GTK_SPIN_BUTTON(gtk_spin_button_new_with_range(0, MAX_DIAS_VIAGEM, 1));
    g_signal_connect(spin_edicao_quant_jantares_profs_total_dias, "value-changed", G_CALLBACK(on_edicao_quant_jantares_profs_changed), grid_edicao_custos_alimentacao);
    gtk_grid_attach(GTK_GRID(grid_edicao_custos_alimentacao), GTK_WIDGET(spin_edicao_quant_jantares_profs_total_dias), 1, row++, 1, 1);
    for (int i = 0; i < MAX_DIAS_VIAGEM; i++) {
        char label_text[50];
        snprintf(label_text, sizeof(label_text), "Valor Jantar Profs Dia %d (R$):", i + 1);
        gtk_grid_attach(GTK_GRID(grid_edicao_custos_alimentacao), gtk_label_new(label_text), 0, row, 1, 1);
        spin_edicao_val_jantares_profs_dias[i] = GTK_SPIN_BUTTON(gtk_spin_button_new_with_range(0.00, 500.00, 0.01));
        gtk_grid_attach(GTK_GRID(grid_edicao_custos_alimentacao), GTK_WIDGET(spin_edicao_val_jantares_profs_dias[i]), 1, row++, 1, 1);
        gtk_widget_set_visible(gtk_grid_get_child_at(GTK_GRID(grid_edicao_custos_alimentacao), 0, row - 1), FALSE);
        gtk_widget_set_visible(GTK_WIDGET(spin_edicao_val_jantares_profs_dias[i]), FALSE);
    }

    row++;
    gtk_grid_attach(GTK_GRID(grid_edicao_custos_alimentacao), gtk_label_new("Custos Serviço de Bordo (R$):"), 0, row, 1, 1);
    spin_edicao_val_servico_bordo = GTK_SPIN_BUTTON(gtk_spin_button_new_with_range(0.00, 50000.00, 0.01));
    gtk_grid_attach(GTK_GRID(grid_edicao_custos_alimentacao), GTK_WIDGET(spin_edicao_val_servico_bordo), 1, row++, 1, 1);

    gtk_grid_attach(GTK_GRID(grid_edicao_custos_alimentacao), gtk_label_new("Outros Gastos Alimentação (R$):"), 0, row, 1, 1);
    spin_edicao_val_alimentacao_extra = GTK_SPIN_BUTTON(gtk_spin_button_new_with_range(0.00, 50000.00, 0.01));
    gtk_grid_attach(GTK_GRID(grid_edicao_custos_alimentacao), GTK_WIDGET(spin_edicao_val_alimentacao_extra), 1, row++, 1, 1);

    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), local_scrolled_win_custos_alimentacao, gtk_label_new("Alimentação"));
    
    // --- ABA 6: DEFINIÇÃO DE LUCRO ---
    GtkWidget *scrolled_win_lucro = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_win_lucro), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
    GtkWidget *grid_lucro = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid_lucro), 5);
    gtk_grid_set_column_spacing(GTK_GRID(grid_lucro), 10);
    gtk_container_set_border_width(GTK_CONTAINER(grid_lucro), 10);
    gtk_container_add(GTK_CONTAINER(scrolled_win_lucro), grid_lucro);

    row = 0;
    gtk_grid_attach(GTK_GRID(grid_lucro), gtk_label_new("Custo Total Base (sem lucro):"), 0, row, 1, 1);
    label_edicao_custo_total_base = GTK_LABEL(gtk_label_new("R$ 0.00"));
    gtk_label_set_xalign(label_edicao_custo_total_base, 0); gtk_grid_attach(GTK_GRID(grid_lucro), GTK_WIDGET(label_edicao_custo_total_base), 1, row++, 1, 1);

    gtk_grid_attach(GTK_GRID(grid_lucro), gtk_label_new("Custo Base por Aluno:"), 0, row, 1, 1);
    label_edicao_custo_aluno_base = GTK_LABEL(gtk_label_new("R$ 0.00"));
    gtk_label_set_xalign(label_edicao_custo_aluno_base, 0); gtk_grid_attach(GTK_GRID(grid_lucro), GTK_WIDGET(label_edicao_custo_aluno_base), 1, row++, 1, 1);

    gtk_grid_attach(GTK_GRID(grid_lucro), gtk_label_new("Porcentagem de Lucro Desejada (%):"), 0, row, 1, 1);
    spin_edicao_percentual_lucro_desejado = GTK_SPIN_BUTTON(gtk_spin_button_new_with_range(0.00, 100.00, 0.01));
    gtk_spin_button_set_value(spin_edicao_percentual_lucro_desejado, 0.00);
    gtk_grid_attach(GTK_GRID(grid_lucro), GTK_WIDGET(spin_edicao_percentual_lucro_desejado), 1, row++, 1, 1);

    gtk_grid_attach(GTK_GRID(grid_lucro), gtk_label_new("Lucro Calculado:"), 0, row, 1, 1);
    label_edicao_lucro_calculado = GTK_LABEL(gtk_label_new("R$ 0.00 (0.00%)"));
    gtk_label_set_xalign(label_edicao_lucro_calculado, 0); gtk_grid_attach(GTK_GRID(grid_lucro), GTK_WIDGET(label_edicao_lucro_calculado), 1, row++, 1, 1);

    gtk_grid_attach(GTK_GRID(grid_lucro), gtk_label_new("Custo Final Total:"), 0, row, 1, 1);
    label_edicao_custo_final_total = GTK_LABEL(gtk_label_new("R$ 0.00"));
    gtk_label_set_xalign(label_edicao_custo_final_total, 0); gtk_grid_attach(GTK_GRID(grid_lucro), GTK_WIDGET(label_edicao_custo_final_total), 1, row++, 1, 1);

    gtk_grid_attach(GTK_GRID(grid_lucro), gtk_label_new("Custo Final por Aluno:"), 0, row, 1, 1);
    label_edicao_custo_final_por_aluno = GTK_LABEL(gtk_label_new("R$ 0.00"));
    gtk_label_set_xalign(label_edicao_custo_final_por_aluno, 0); gtk_grid_attach(GTK_GRID(grid_lucro), GTK_WIDGET(label_edicao_custo_final_por_aluno), 1, row++, 1, 1);


    GtkWidget *btn_salvar_edicao_viagem = gtk_button_new_with_label("Salvar Edição da Viagem");
    gtk_widget_set_halign(btn_salvar_edicao_viagem, GTK_ALIGN_CENTER);
    g_signal_connect(btn_salvar_edicao_viagem, "clicked", G_CALLBACK(on_salvar_edicao_viagem_clicked), NULL);
    gtk_grid_attach(GTK_GRID(grid_lucro), btn_salvar_edicao_viagem, 0, row++, 2, 1);

    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), scrolled_win_lucro, gtk_label_new("Lucro"));

    gtk_widget_show_all(pagina_edicao_viagem);
}