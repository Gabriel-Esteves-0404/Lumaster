#include "gerenciador_viagens.h"
#include "alunos_gerenciador.h" 
#include "utils.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

// --- Funções Auxiliares Internas ---

// Calcula o total de alunos ativos em todas as turmas de uma viagem.
int get_total_alunos_em_turmas(const Viagem* v) {
    int total = 0;
    for (int i = 0; i < v->num_turmas_cadastradas; ++i) {
        if (v->turmas[i].ativa) {
            total += v->turmas[i].alunos;
        }
    }
    return total;
}

// Calcula o total de professores ativos em todas as turmas de uma viagem.
int get_total_professores(const Viagem* v) {
    int total = 0;
    for (int i = 0; i < v->num_turmas_cadastradas; ++i) {
        if (v->turmas[i].ativa) {
            total += v->turmas[i].numprofessores;
        }
    }
    return total;
}

// Calcula e define o custo final por aluno para a viagem.
void calcular_e_definir_custo_final_aluno(Viagem* v) {
    int total_professores = get_total_professores(v);
    int minQteAlunos_para_calculo = v->minQteAlunos;

    double total_custos_principais = v->onibus + v->hotel + (v->actes_por_professor * total_professores) + v->pulseiras + v->etiquetas;
    if (v->opcao_guia_local == 'S') total_custos_principais += v->guialocalvalor;
    if (v->opcao_viagem_extra == 'S') total_custos_principais += v->viagemextra_valor;

    double total_custos_pessoas = v->guia_viagem_valor_total + v->despesas_guia_viagem + v->despesas_motorista + v->despesas_luciana + v->taxa_apoio_escola;

    double total_outros_custos_adicionais = 0;
    for (int i = 0; i < v->num_outros_custos_diversos; i++) {
        if (v->outros_custos_diversos[i].ativa) total_outros_custos_adicionais += v->outros_custos_diversos[i].valor;
    }
    double total_custos_diversos = v->farmacia + v->gasto_extra_geral + v->emergencia + v->poupanca_lumaster + v->seguro_viagem + total_outros_custos_adicionais;

    double total_custos_alimentacao = 0;
    for (int i = 0; i < v->dias; ++i) {
        total_custos_alimentacao += v->val_almo_alunos_dias[i] * (i < v->quant_almocos_alunos_total_dias ? minQteAlunos_para_calculo : 0);
        total_custos_alimentacao += v->val_jantar_alunos_dias[i] * (i < v->quant_jantares_alunos_total_dias ? minQteAlunos_para_calculo : 0);
        total_custos_alimentacao += v->val_almo_profs_dias[i] * (i < v->quant_almocos_profs_total_dias ? total_professores : 0);
        total_custos_alimentacao += v->val_jantares_profs_dias[i] * (i < v->quant_jantares_profs_total_dias ? total_professores : 0);
    }
    total_custos_alimentacao += v->val_servico_bordo + v->val_alimentacao_extra;

    double total_base_da_viagem = total_custos_principais + total_custos_pessoas + total_custos_diversos + total_custos_alimentacao;
    
    double lucro_total = total_base_da_viagem * (v->percentual_lucro_desejado / 100.0);
    double total_final_da_viagem = total_base_da_viagem + lucro_total;

    v->totalPorAluno = (minQteAlunos_para_calculo > 0) ? total_final_da_viagem / minQteAlunos_para_calculo : total_final_da_viagem;
}

// Calcula os custos base de uma viagem, usando 'minQteAlunos' para o cálculo por aluno.
void calcular_custos_base(const Viagem* v, double* p_total_base, double* p_custo_aluno_base, double* p_total_custos_principais, double* p_total_custos_pessoas, double* p_total_custos_diversos, double* p_total_custos_alimentacao) {
    int total_professores = get_total_professores(v);
    int minQteAlunos_para_calculo = v->minQteAlunos;

    *p_total_custos_principais = v->onibus + v->hotel + (v->actes_por_professor * total_professores) + v->pulseiras + v->etiquetas;
    if (v->opcao_guia_local == 'S') *p_total_custos_principais += v->guialocalvalor;
    if (v->opcao_viagem_extra == 'S') *p_total_custos_principais += v->viagemextra_valor;

    *p_total_custos_pessoas = v->guia_viagem_valor_total + v->despesas_guia_viagem + v->despesas_motorista + v->despesas_luciana + v->taxa_apoio_escola;

    double total_outros_custos_adicionais = 0;
    for (int i = 0; i < v->num_outros_custos_diversos; i++) {
        if (v->outros_custos_diversos[i].ativa) total_outros_custos_adicionais += v->outros_custos_diversos[i].valor;
    }
    *p_total_custos_diversos = v->farmacia + v->gasto_extra_geral + v->emergencia + v->poupanca_lumaster + v->seguro_viagem + total_outros_custos_adicionais;

    *p_total_custos_alimentacao = 0;
    for (int i = 0; i < v->dias; ++i) {
        *p_total_custos_alimentacao += v->val_almo_alunos_dias[i] * (i < v->quant_almocos_alunos_total_dias ? minQteAlunos_para_calculo : 0);
        *p_total_custos_alimentacao += v->val_jantar_alunos_dias[i] * (i < v->quant_jantares_alunos_total_dias ? minQteAlunos_para_calculo : 0);
        *p_total_custos_alimentacao += v->val_almo_profs_dias[i] * (i < v->quant_almocos_profs_total_dias ? total_professores : 0);
        *p_total_custos_alimentacao += v->val_jantares_profs_dias[i] * (i < v->quant_jantares_profs_total_dias ? total_professores : 0);
    }
    *p_total_custos_alimentacao += v->val_servico_bordo + v->val_alimentacao_extra;

    *p_total_base = *p_total_custos_principais + *p_total_custos_pessoas + *p_total_custos_diversos + *p_total_custos_alimentacao;
    
    if (minQteAlunos_para_calculo > 0) *p_custo_aluno_base = *p_total_base / minQteAlunos_para_calculo;
    else *p_custo_aluno_base = *p_total_base;
}

// --- Funções de Gerenciamento de Viagens ---

// Adiciona uma nova viagem ao sistema.
int adicionar_viagem_ao_sistema(Viagem* nova_viagem) {
    if (num_total_viagens_sistema >= MAX_VIAGENS) {
        return -1; 
    }

    int idx_viagem = -1;
    for (int i = 0; i < MAX_VIAGENS; ++i) {
        if (!viagens[i].ativa) { 
            idx_viagem = i;
            break;
        }
    }
    if (idx_viagem == -1) {
        return -1; 
    }

    viagens[idx_viagem] = *nova_viagem; 
    viagens[idx_viagem].ativa = 1; 
    calcular_e_definir_custo_final_aluno(&viagens[idx_viagem]); 

    num_total_viagens_sistema++;
    salvar_dados(); 
    return idx_viagem;
}

// Atualiza os dados de uma viagem existente no sistema.
int atualizar_viagem_no_sistema(int index_viagem, const Viagem* dados_atualizados) {
    if (index_viagem < 0 || index_viagem >= MAX_VIAGENS || !viagens[index_viagem].ativa) {
        return 0; 
    }
    viagens[index_viagem] = *dados_atualizados;
    calcular_e_definir_custo_final_aluno(&viagens[index_viagem]); 
    salvar_dados();
    return 1;
}

// Remove logicamente uma viagem do sistema (desativa).
int remover_viagem_do_sistema(int index_viagem) {
    if (index_viagem < 0 || index_viagem >= MAX_VIAGENS || !viagens[index_viagem].ativa) {
        return 0; 
    }
    viagens[index_viagem].ativa = 0; 
    for (int i = 0; i < viagens[index_viagem].num_turmas_cadastradas; i++) {
        viagens[index_viagem].turmas[i].ativa = 0;
        viagens[index_viagem].turmas[i].alunos = 0; 
        viagens[index_viagem].turmas[i].numprofessores = 0;
    }

    inativar_alunos_por_viagem(index_viagem); 

    salvar_dados(); 
    return 1;
}

// --- Funções de Manipulação de Arquivos ---

// Salva os dados das viagens para um arquivo binário.
void salvar_dados() {
    FILE *arquivo = fopen(NOME_ARQUIVO_DADOS, "wb");
    if (arquivo == NULL) {
        perror("Erro ao abrir arquivo para escrita");
        return;
    }
    fwrite(&num_total_viagens_sistema, sizeof(int), 1, arquivo);
    fwrite(viagens, sizeof(Viagem), MAX_VIAGENS, arquivo);
    fclose(arquivo);
}

// Carrega os dados das viagens de um arquivo binário.
void carregar_dados() {
    FILE *arquivo = fopen(NOME_ARQUIVO_DADOS, "rb");
    if (arquivo == NULL) {
        num_total_viagens_sistema = 0;
        for (int i = 0; i < MAX_VIAGENS; ++i) {
            memset(&viagens[i], 0, sizeof(Viagem)); 
            viagens[i].ativa = 0; 
        }
        return;
    }
    
    fread(&num_total_viagens_sistema, sizeof(int), 1, arquivo);
    fread(viagens, sizeof(Viagem), MAX_VIAGENS, arquivo);
    
    for (int i = 0; i < MAX_VIAGENS; ++i) {
        if (viagens[i].ativa) {
            calcular_e_definir_custo_final_aluno(&viagens[i]); 
        }
    }

    fclose(arquivo);
}

// Exporta os detalhes financeiros de uma viagem para um arquivo CSV.
void exportar_planilha_para_csv(int index_viagem) {
    Viagem* v = &viagens[index_viagem];
    char nome_arquivo[MAX_NOME + 20];
    char nome_seguro[MAX_NOME];
    
    strncpy(nome_seguro, v->nome_viagem, sizeof(nome_seguro) - 1);
    nome_seguro[sizeof(nome_seguro) - 1] = '\0';
    for(int i = 0; nome_seguro[i]; i++){
        if(nome_seguro[i] == ' ' || nome_seguro[i] == '/' || nome_seguro[i] == '\\') {
            nome_seguro[i] = '_';
        }
    }
    sprintf(nome_arquivo, "Custos_%s.csv", nome_seguro);
    FILE* arquivo_csv = fopen(nome_arquivo, "w");
    if (arquivo_csv == NULL) {
        perror("Erro ao criar o arquivo CSV para exportação");
        return;
    }

    int total_alunos_nas_turmas = get_total_alunos_em_turmas(v);
    int total_profs = get_total_professores(v);
    int minQteAlunos_para_calculo_export = v->minQteAlunos;

    fprintf(arquivo_csv, "PLANILHA DE CUSTOS - VIAGEM\n");
    fprintf(arquivo_csv, "Viagem;%s\n", v->nome_viagem);
    fprintf(arquivo_csv, "Escola;%s\n", v->nome_escola);
    fprintf(arquivo_csv, "Destino;%s\n", v->destino);
    fprintf(arquivo_csv, "Duracao (dias);%d\n", v->dias);
    fprintf(arquivo_csv, "Base Calculo Alunos para Preco;%d\n\n", v->minQteAlunos);
    
    fprintf(arquivo_csv, "PARTICIPANTES\n");
    fprintf(arquivo_csv, "Turma;Alunos;Professores\n");
    for(int i = 0; i < v->num_turmas_cadastradas; i++){
        if(v->turmas[i].ativa){
            fprintf(arquivo_csv, "%s;%d;%d\n", v->turmas[i].nome_turma, v->turmas[i].alunos, v->turmas[i].numprofessores);
        }
    }
    fprintf(arquivo_csv, "TOTAL (Alunos em Turmas);%d;%d\n\n", total_alunos_nas_turmas, total_profs);

    fprintf(arquivo_csv, "DETALHAMENTO DE CUSTOS\n");
    fprintf(arquivo_csv, "Categoria;Item;Valor Unitario;Quantidade;Subtotal\n");

    fprintf(arquivo_csv, "Custos Principais;Onibus;;;%.2f\n", v->onibus);
    fprintf(arquivo_csv, "Custos Principais;Hotel;;;%.2f\n", v->hotel);
    fprintf(arquivo_csv, "Custos Principais;Taxa ACTES p/ Professor;%.2f;%d;%.2f\n", v->actes_por_professor, total_profs, v->actes_por_professor * total_profs);
    fprintf(arquivo_csv, "Custos Principais;Pulseiras;;;%.2f\n", v->pulseiras);
    fprintf(arquivo_csv, "Custos Principais;Etiquetas;;;%.2f\n", v->etiquetas);
    if(v->opcao_guia_local == 'S') fprintf(arquivo_csv, "Custos Principais;Guia Local;;;%.2f\n", v->guialocalvalor);
    if(v->opcao_viagem_extra == 'S') fprintf(arquivo_csv, "Custos Principais;%s;;;%.2f\n", v->viagemextra_nome, v->viagemextra_valor);

    fprintf(arquivo_csv, "Custos com Pessoas;Guias (valor total);;;%.2f\n", v->guia_viagem_valor_total);
    fprintf(arquivo_csv, "Custos com Pessoas;Despesas Guias;;;%.2f\n", v->despesas_guia_viagem);
    fprintf(arquivo_csv, "Custos com Pessoas;Despesas Motoristas;;;%.2f\n", v->despesas_motorista);
    fprintf(arquivo_csv, "Custos com Pessoas;Despesas Luciana;;;%.2f\n", v->despesas_luciana);
    fprintf(arquivo_csv, "Custos com Pessoas;Taxa de Apoio Escola;;;%.2f\n", v->taxa_apoio_escola);

    fprintf(arquivo_csv, "Custos Diversos;Farmacia;;;%.2f\n", v->farmacia);
    fprintf(arquivo_csv, "Custos Diversos;Gasto Extra Geral;;;%.2f\n", v->gasto_extra_geral);
    fprintf(arquivo_csv, "Custos Diversos;Emergencia;;;%.2f\n", v->emergencia);
    fprintf(arquivo_csv, "Custos Diversos;Poupanca Lumaster;;;%.2f\n", v->poupanca_lumaster);
    fprintf(arquivo_csv, "Custos Diversos;Seguro Viagem;;;%.2f\n", v->seguro_viagem);
    for(int i=0; i < v->num_outros_custos_diversos; i++) {
        if(v->outros_custos_diversos[i].ativa) {
            fprintf(arquivo_csv, "Custos Diversos;%s;;;%.2f\n", v->outros_custos_diversos[i].descricao, v->outros_custos_diversos[i].valor);
        }
    }

    for(int i=0; i < v->quant_almocos_alunos_total_dias; i++) {
        char item_desc[100];
        snprintf(item_desc, sizeof(item_desc), "Almoco Alunos Dia %d", i+1);
        fprintf(arquivo_csv, "Alimentacao;%s;%.2f;%d;%.2f\n", item_desc, v->val_almo_alunos_dias[i], minQteAlunos_para_calculo_export, v->val_almo_alunos_dias[i] * minQteAlunos_para_calculo_export);
    }
    for(int i=0; i < v->quant_jantares_alunos_total_dias; i++) {
        char item_desc[100];
        snprintf(item_desc, sizeof(item_desc), "Jantar Alunos Dia %d", i+1);
        fprintf(arquivo_csv, "Alimentacao;%s;%.2f;%d;%.2f\n", item_desc, v->val_jantar_alunos_dias[i], minQteAlunos_para_calculo_export, v->val_jantar_alunos_dias[i] * minQteAlunos_para_calculo_export);
    }
    for(int i=0; i < v->quant_almocos_profs_total_dias; i++) {
        char item_desc[100];
        snprintf(item_desc, sizeof(item_desc), "Almoco Profs Dia %d", i+1);
        fprintf(arquivo_csv, "Alimentacao;%s;%.2f;%d;%.2f\n", item_desc, v->val_almo_profs_dias[i], total_profs, v->val_almo_profs_dias[i] * total_profs);
    }
    for(int i=0; i < v->quant_jantares_profs_total_dias; i++) {
        char item_desc[100];
        snprintf(item_desc, sizeof(item_desc), "Jantar Profs Dia %d", i+1);
        fprintf(arquivo_csv, "Alimentacao;%s;%.2f;%d;%.2f\n", item_desc, v->val_jantares_profs_dias[i], total_profs, v->val_jantares_profs_dias[i] * total_profs);
    }
    fprintf(arquivo_csv, "Alimentacao;Servico de Bordo;;;%.2f\n", v->val_servico_bordo);
    fprintf(arquivo_csv, "Alimentacao;Alimentacao Extra;;;%.2f\n", v->val_alimentacao_extra);


    double total_base_viagem_final, custo_aluno_base_dummy_for_display, t_principais_dummy, t_pessoas_dummy, t_diversos_dummy, t_alimentacao_dummy;
    calcular_custos_base(v, &total_base_viagem_final, &custo_aluno_base_dummy_for_display, &t_principais_dummy, &t_pessoas_dummy, &t_diversos_dummy, &t_alimentacao_dummy);
    double lucro_total_calc = total_base_viagem_final * (v->percentual_lucro_desejado / 100.0);
    double total_final_viagem_display = total_base_viagem_final + lucro_total_calc;

    fprintf(arquivo_csv, "\nRESUMO FINANCEIRO\n");
    fprintf(arquivo_csv, "Item;Valor\n");
    fprintf(arquivo_csv, "Custo Base Total;%.2f\n", total_base_viagem_final);
    if(v->minQteAlunos > 0) fprintf(arquivo_csv, "Custo Base por Aluno (Base %d Alunos);%.2f\n", v->minQteAlunos, custo_aluno_base_dummy_for_display);
    fprintf(arquivo_csv, "Lucro (%.2f%%);%.2f\n", v->percentual_lucro_desejado, lucro_total_calc);
    fprintf(arquivo_csv, "VALOR FINAL DA VIAGEM;%.2f\n", total_final_viagem_display);
    if(v->minQteAlunos > 0) fprintf(arquivo_csv, "Custo Final por Aluno (Base %d Alunos);%.2f\n", v->minQteAlunos, v->totalPorAluno);

    fclose(arquivo_csv);
}