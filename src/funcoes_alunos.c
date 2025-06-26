#include "alunos_gerenciador.h"
#include "gerenciador_viagens.h"
#include "utils.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>

void avancar_mes_tm_ajustado(struct tm *data) {
    int original_mday = data->tm_mday;

    if (data->tm_mon == 11) {
        data->tm_mon = 0;
        data->tm_year++;
    } else {
        data->tm_mon++;
    }

    mktime(data);

    if (data->tm_mday != original_mday) {
        int target_mon = data->tm_mon;
        int target_year = data->tm_year;
        data->tm_mday = original_mday;
        mktime(data);

        if (data->tm_mon != target_mon || data->tm_year != target_year) {
            data->tm_mday = 0;
            mktime(data);
            data->tm_mon++;
            mktime(data);
        }
    }
}

void gerar_parcelas_aluno(Aluno *aluno, double valor_total_viagem,
                          const struct tm *data_viagem, int is_credito, double valor_entrada) {

    for (int i = 0; i < MAX_PARCELAS_ALUNO; i++) {
        memset(&aluno->parcelas[i], 0, sizeof(Parcela));
        aluno->parcelas[i].ativa = 0;
    }
    aluno->num_parcelas_cadastradas = 0;

    struct tm temp_inicio_pagamento = aluno->data_inicio_pagamento;
    struct tm temp_data_viagem = *data_viagem;

    mktime(&temp_inicio_pagamento);
    mktime(&temp_data_viagem);

    int total_meses_entre_datas = 0;
    struct tm current_month_iterator = temp_inicio_pagamento;

    int dia_limite_ultima_parcela = temp_data_viagem.tm_mday - 5;
    if (dia_limite_ultima_parcela <= 0) {
        dia_limite_ultima_parcela = 1;
    }

    while (1) {
        if (current_month_iterator.tm_year > temp_data_viagem.tm_year ||
            (current_month_iterator.tm_year == temp_data_viagem.tm_year &&
             current_month_iterator.tm_mon > temp_data_viagem.tm_mon)) {
            break;
        }

        if (current_month_iterator.tm_year == temp_data_viagem.tm_year &&
            current_month_iterator.tm_mon == temp_data_viagem.tm_mon) {
            if (aluno->data_inicio_pagamento.tm_mday > dia_limite_ultima_parcela) {
                break;
            }
        }
        total_meses_entre_datas++;

        avancar_mes_tm_ajustado(&current_month_iterator);
    }

    g_print("DEBUG: Meses calculados para parcelamento: %d\n", total_meses_entre_datas);

    if (total_meses_entre_datas <= 0 || valor_total_viagem <= 0) {
        g_print("AVISO: Nao ha parcelas a serem geradas (total de meses <= 0 ou valor total <= 0).\n");
        return;
    }

    if (valor_entrada > 0 && aluno->num_parcelas_cadastradas < MAX_PARCELAS_ALUNO) {
        Parcela *p = &aluno->parcelas[aluno->num_parcelas_cadastradas];
        p->data_vencimento = aluno->data_inicio_pagamento;
        p->valor = valor_entrada;
        p->quitada = 1;
        p->ativa = 1;
        aluno->num_parcelas_cadastradas++;
        valor_total_viagem -= valor_entrada;
        total_meses_entre_datas--;
        g_print("DEBUG: Parcela de entrada gerada: R$%.2f\n", valor_entrada);
    }

    double valor_por_parcela = 0.0;
    if (total_meses_entre_datas > 0) {
        valor_por_parcela = valor_total_viagem / total_meses_entre_datas;
    } else if (valor_total_viagem > 0 && aluno->num_parcelas_cadastradas < MAX_PARCELAS_ALUNO) {
        valor_por_parcela = valor_total_viagem;
        total_meses_entre_datas = 1;
        g_print("AVISO: Valor restante sera em uma unica parcela residual.\n");
    } else {
        g_print("DEBUG: Nenhuma parcela restante a ser gerada.\n");
        return;
    }

    struct tm current_parcel_date = aluno->data_inicio_pagamento;
    if (valor_entrada > 0) {
        avancar_mes_tm_ajustado(&current_parcel_date);
    }
    
    for (int i = 0; i < total_meses_entre_datas; i++) {
        if (aluno->num_parcelas_cadastradas >= MAX_PARCELAS_ALUNO) {
            g_print("AVISO: Limite de parcelas por aluno atingido. Nem todas as parcelas foram geradas.\n");
            break;
        }

        Parcela *p = &aluno->parcelas[aluno->num_parcelas_cadastradas];
        p->data_vencimento = current_parcel_date;

        if (p->data_vencimento.tm_year == temp_data_viagem.tm_year &&
            p->data_vencimento.tm_mon == temp_data_viagem.tm_mon) {
            
            p->data_vencimento.tm_mday = dia_limite_ultima_parcela;
            mktime(&p->data_vencimento);
            
            if (mktime(&p->data_vencimento) > mktime(&temp_data_viagem)) {
                g_print("AVISO: Ultima parcela ajustada excede a data da viagem. Ignorada.\n");
                break;
            }
        }

        p->valor = valor_por_parcela;
        p->quitada = is_credito ? 1 : 0;
        p->ativa = 1;
        aluno->num_parcelas_cadastradas++;

        avancar_mes_tm_ajustado(&current_parcel_date);
    }

    g_print("DEBUG: Parcelas geradas para o aluno. Total de parcelas: %d\n", aluno->num_parcelas_cadastradas);
}

int adicionar_aluno(const char* nome, const char* rg, const char* cpf, const char* nascimento,
                    int id_viagem, const char* nome_turma, const struct tm data_inicio_pagamento,
                    int is_credito, double valor_entrada) {
    if (num_total_alunos_sistema >= MAX_ALUNOS) {
        g_print("ERRO (adicionar_aluno): Limite maximo de alunos no sistema atingido.\n");
        return -1;
    }

    if (id_viagem < 0 || id_viagem >= MAX_VIAGENS || !viagens[id_viagem].ativa) {
        g_print("ERRO (adicionar_aluno): Tentativa de adicionar aluno a uma viagem invalida ou inativa (ID: %d).\n", id_viagem);
        return -1;
    }

    int idx_turma = -1;
    for(int i = 0; i < viagens[id_viagem].num_turmas_cadastradas; i++){
        if(viagens[id_viagem].turmas[i].ativa && strcmp(viagens[id_viagem].turmas[i].nome_turma, nome_turma) == 0){
            idx_turma = i;
            break;
        }
    }

    if(idx_turma == -1){
        g_print("ERRO (adicionar_aluno): Turma '%s' nao encontrada ou inativa na viagem ID %d.\n", nome_turma, id_viagem);
        return -1;
    }

    int idx_aluno = -1;
    for(int i = 0; i < MAX_ALUNOS; i++) {
        if (!alunos[i].ativo) {
            idx_aluno = i;
            break;
        }
    }
    if(idx_aluno == -1) {
        g_print("ERRO (adicionar_aluno): Nao ha slots de aluno disponiveis.\n");
        return -1;
    }

    alunos[idx_aluno].ativo = 1;
    alunos[idx_aluno].id_aluno = idx_aluno;
    alunos[idx_aluno].id_viagem = id_viagem;
    strncpy(alunos[idx_aluno].nome_turma, nome_turma, sizeof(alunos[idx_aluno].nome_turma) - 1);
    alunos[idx_aluno].nome_turma[sizeof(alunos[idx_aluno].nome_turma) - 1] = '\0';
    strncpy(alunos[idx_aluno].nome, nome, sizeof(alunos[idx_aluno].nome) - 1);
    alunos[idx_aluno].nome[sizeof(alunos[idx_aluno].nome) - 1] = '\0';
    strncpy(alunos[idx_aluno].rg, rg, sizeof(alunos[idx_aluno].rg) - 1);
    alunos[idx_aluno].rg[sizeof(alunos[idx_aluno].rg) - 1] = '\0';
    strncpy(alunos[idx_aluno].cpf, cpf, sizeof(alunos[idx_aluno].cpf) - 1);
    alunos[idx_aluno].cpf[sizeof(alunos[idx_aluno].cpf) - 1] = '\0';
    strncpy(alunos[idx_aluno].nascimento, nascimento, sizeof(alunos[idx_aluno].nascimento) - 1);
    alunos[idx_aluno].nascimento[sizeof(alunos[idx_aluno].nascimento) - 1] = '\0';

    alunos[idx_aluno].data_inicio_pagamento = data_inicio_pagamento;
    alunos[idx_aluno].num_parcelas_cadastradas = 0;

    num_total_alunos_sistema++;
    
    viagens[id_viagem].turmas[idx_turma].alunos++;

    g_print("DEBUG (adicionar_aluno): totalPorAluno da viagem %d ('%s') antes de gerar parcelas: R$%.2f\n",
        id_viagem, viagens[id_viagem].nome_viagem, viagens[id_viagem].totalPorAluno);

    double valor_total_viagem_aluno = viagens[id_viagem].totalPorAluno;
    gerar_parcelas_aluno(&alunos[idx_aluno], valor_total_viagem_aluno,
                         &viagens[id_viagem].data_da_viagem, is_credito, valor_entrada);

    salvar_dados_alunos();
    salvar_dados();

    g_print("DEBUG: Aluno '%s' adicionado. Viagem ID: %d, Turma: '%s'. Alunos na turma: %d\n",
            nome, id_viagem, nome_turma, viagens[id_viagem].turmas[idx_turma].alunos);

    return idx_aluno;
}

Aluno* buscar_aluno_por_cpf_ptr(const char* cpf) {
    for (int i = 0; i < MAX_ALUNOS; i++) {
        if (alunos[i].ativo && strcmp(alunos[i].cpf, cpf) == 0) {
            return &alunos[i];
        }
    }
    return NULL;
}

Aluno* buscar_aluno_por_nome_ptr(const char* nome_busca) {
    char busca_lower[MAX_NOME];
    strncpy(busca_lower, nome_busca, sizeof(busca_lower) - 1);
    busca_lower[sizeof(busca_lower) - 1] = '\0';
    toLowerStr_util(busca_lower);

    for (int i = 0; i < MAX_ALUNOS; i++) {
        if (alunos[i].ativo) {
            char aluno_nome_lower[MAX_NOME];
            strncpy(aluno_nome_lower, alunos[i].nome, sizeof(aluno_nome_lower) - 1);
            aluno_nome_lower[sizeof(aluno_nome_lower) - 1] = '\0';
            toLowerStr_util(aluno_nome_lower);

            if (contemPalavras_util(aluno_nome_lower, busca_lower)) {
                return &alunos[i];
            }
        }
    }
    return NULL;
}

int alterar_dados_aluno_por_cpf(const char* cpf, const char* novo_nome, const char* novo_rg, const char* novo_nascimento) {
    Aluno* aluno_para_alterar = buscar_aluno_por_cpf_ptr(cpf);
    if (aluno_para_alterar) {
        if (novo_nome && strlen(novo_nome) > 0) {
            strncpy(aluno_para_alterar->nome, novo_nome, sizeof(aluno_para_alterar->nome) - 1);
            aluno_para_alterar->nome[sizeof(aluno_para_alterar->nome) - 1] = '\0';
        }
        if (novo_rg && strlen(novo_rg) > 0) {
            strncpy(aluno_para_alterar->rg, novo_rg, sizeof(aluno_para_alterar->rg) - 1);
            aluno_para_alterar->rg[sizeof(aluno_para_alterar->rg) - 1] = '\0';
        }
        if (novo_nascimento && strlen(novo_nascimento) > 0) {
            strncpy(aluno_para_alterar->nascimento, novo_nascimento, sizeof(aluno_para_alterar->nascimento) - 1);
            aluno_para_alterar->nascimento[sizeof(aluno_para_alterar->nascimento) - 1] = '\0';
        }
        salvar_dados_alunos();
        return 1;
    }
    return 0;
}

int remover_aluno_por_cpf(const char* cpf) {
    for (int i = 0; i < MAX_ALUNOS; i++) {
        if (alunos[i].ativo && strcmp(alunos[i].cpf, cpf) == 0) {
            alunos[i].ativo = 0;
            num_total_alunos_sistema--;
            salvar_dados_alunos();
            return 1;
        }
    }
    return 0;
}

void listar_todos_alunos_backend(void (*callback_mostrar_aluno)(const Aluno*, const char*)) {
    if (num_total_alunos_sistema == 0) {
        return;
    }
    for (int i = 0; i < MAX_ALUNOS; i++) {
        if (alunos[i].ativo) {
            char viagem_nome[MAX_NOME] = "Viagem Indefinida";
            if (alunos[i].id_viagem >= 0 && alunos[i].id_viagem < MAX_VIAGENS && viagens[alunos[i].id_viagem].ativa) {
                strncpy(viagem_nome, viagens[alunos[i].id_viagem].nome_viagem, MAX_NOME - 1);
                viagem_nome[MAX_NOME - 1] = '\0';
            }
            callback_mostrar_aluno(&alunos[i], viagem_nome);
        }
    }
}

void listar_alunos_por_viagem_backend(int index_viagem, void (*callback_mostrar_aluno)(const Aluno*, const char*)) {
    if (index_viagem < 0 || index_viagem >= MAX_VIAGENS || !viagens[index_viagem].ativa) {
        return;
    }

    const Viagem* v = &viagens[index_viagem];
    for (int i = 0; i < v->num_turmas_cadastradas; i++) {
        if (v->turmas[i].ativa) {
            for (int j = 0; j < MAX_ALUNOS; j++) {
                if (alunos[j].ativo && alunos[j].id_viagem == index_viagem && strcmp(alunos[j].nome_turma, v->turmas[i].nome_turma) == 0) {
                    callback_mostrar_aluno(&alunos[j], v->turmas[i].nome_turma);
                }
            }
        }
    }
}

void salvar_dados_alunos() {
    FILE *f = fopen(NOME_ARQUIVO_ALUNOS, "wb");
    if (!f) {
        perror("Erro ao salvar dados de alunos");
        return;
    }
    fwrite(&num_total_alunos_sistema, sizeof(int), 1, f);
    fwrite(alunos, sizeof(Aluno), MAX_ALUNOS, f);
    fclose(f);
}

void carregar_dados_alunos() {
    FILE *f = fopen(NOME_ARQUIVO_ALUNOS, "rb");
    if (!f) {
        num_total_alunos_sistema = 0;
        for(int i = 0; i < MAX_ALUNOS; i++) {
            memset(&alunos[i], 0, sizeof(Aluno));
            alunos[i].ativo = 0;
        }
        return;
    }
    fread(&num_total_alunos_sistema, sizeof(int), 1, f);
    fread(alunos, sizeof(Aluno), MAX_ALUNOS, f);
    fclose(f);
}

void exportar_todos_alunos_viagem_csv(const Viagem* v, int index_viagem) {
    char nome_arquivo[MAX_NOME + 30];
    char nome_seguro[MAX_NOME];

    strncpy(nome_seguro, v->nome_viagem, sizeof(nome_seguro) - 1);
    nome_seguro[sizeof(nome_seguro) - 1] = '\0';
    for(int i = 0; nome_seguro[i]; i++) if(nome_seguro[i] == ' ' || nome_seguro[i] == '/') nome_seguro[i] = '_';
    sprintf(nome_arquivo, "Alunos_Viagem_%s_Completo.csv", nome_seguro);

    FILE* f = fopen(nome_arquivo, "w");
    if(!f) {
        perror("Erro ao criar arquivo CSV para alunos");
        return;
    }

    fprintf(f, "Lista de Alunos para a Viagem: %s (%s)\n\n", v->nome_viagem, v->nome_escola);
    fprintf(f, "Nome Completo;RG;CPF;Data de Nascimento;Turma\n");

    for(int i = 0; i < MAX_ALUNOS; i++){
        if(alunos[i].ativo && alunos[i].id_viagem == index_viagem) {
            fprintf(f, "\"%s\";\"%s\";\"%s\";\"%s\";\"%s\"\n",
                alunos[i].nome,
                alunos[i].rg,
                alunos[i].cpf,
                alunos[i].nascimento,
                alunos[i].nome_turma
            );
        }
    }
    fclose(f);
}

void exportar_alunos_por_turma_csv(const Viagem* v, int index_viagem) {
    for (int i = 0; i < v->num_turmas_cadastradas; i++) {
        if(v->turmas[i].ativa) {
            char nome_viagem_seguro[MAX_NOME];
            char nome_turma_seguro[MAX_NOME];
            char nome_arquivo[MAX_NOME * 2 + 50];

            strncpy(nome_viagem_seguro, v->nome_viagem, sizeof(nome_viagem_seguro) - 1);
            nome_viagem_seguro[sizeof(nome_viagem_seguro) - 1] = '\0';
            for(int k=0; nome_viagem_seguro[k]; k++) if(nome_viagem_seguro[k] == ' ' || nome_viagem_seguro[k] == '/') nome_viagem_seguro[k] = '_';

            strncpy(nome_turma_seguro, v->turmas[i].nome_turma, sizeof(nome_turma_seguro) - 1);
            nome_turma_seguro[sizeof(nome_turma_seguro) - 1] = '\0';
            for(int k=0; nome_turma_seguro[k]; k++) if(nome_turma_seguro[k] == ' ' || nome_turma_seguro[k] == '/') nome_turma_seguro[k] = '_';

            sprintf(nome_arquivo, "Alunos_Viagem_%s_Turma_%s.csv", nome_viagem_seguro, nome_turma_seguro);

            FILE* f = fopen(nome_arquivo, "w");
            if(!f) {
                continue;
            }

            fprintf(f, "Lista de Alunos para a Viagem: %s (%s)\n", v->nome_viagem, v->nome_escola);
            fprintf(f, "Turma: %s\n\n", v->turmas[i].nome_turma);
            fprintf(f, "Nome Completo;RG;CPF;Data de Nascimento\n");

            int alunos_exportados_nesta_turma = 0;
            for(int j = 0; j < MAX_ALUNOS; j++){
                if(alunos[j].ativo && alunos[j].id_viagem == index_viagem && strcmp(alunos[j].nome_turma, v->turmas[i].nome_turma) == 0) {
                    fprintf(f, "\"%s\";\"%s\";\"%s\";\"%s\"\n",
                        alunos[j].nome,
                        alunos[j].rg,
                        alunos[j].cpf,
                        alunos[j].nascimento
                    );
                    alunos_exportados_nesta_turma++;
                }
            }
            fclose(f);

            if(alunos_exportados_nesta_turma == 0) {
                remove(nome_arquivo);
            }
        }
    }
}

void exportar_seguro_viagem_por_turma_csv(int index_viagem) {
    if (index_viagem < 0 || index_viagem >= MAX_VIAGENS || !viagens[index_viagem].ativa) {
        g_print("ERRO (exportar_seguro_viagem_por_turma_csv): Viagem invalida ou inativa.\n");
        return;
    }

    const Viagem* v = &viagens[index_viagem];
    g_print("DEBUG: Gerando planilhas de Seguro Viagem por turma para a viagem '%s'...\n", v->nome_viagem);

    char data_viagem_str[11];
    strftime(data_viagem_str, sizeof(data_viagem_str), "%d/%m/%Y", &v->data_da_viagem);

    for (int i = 0; i < v->num_turmas_cadastradas; i++) {
        if (v->turmas[i].ativa) {
            char nome_viagem_seguro[MAX_NOME];
            char nome_turma_seguro[MAX_NOME];
            char nome_arquivo[MAX_NOME * 2 + 50];

            strncpy(nome_viagem_seguro, v->nome_viagem, sizeof(nome_viagem_seguro) - 1);
            nome_viagem_seguro[sizeof(nome_viagem_seguro) - 1] = '\0';
            for (int k = 0; nome_viagem_seguro[k]; k++) {
                if (nome_viagem_seguro[k] == ' ' || nome_viagem_seguro[k] == '/' || nome_viagem_seguro[k] == '\\') {
                    nome_viagem_seguro[k] = '_';
                }
            }

            strncpy(nome_turma_seguro, v->turmas[i].nome_turma, sizeof(nome_turma_seguro) - 1);
            nome_turma_seguro[sizeof(nome_turma_seguro) - 1] = '\0';
            for (int k = 0; nome_turma_seguro[k]; k++) {
                if (nome_turma_seguro[k] == ' ' || nome_turma_seguro[k] == '/' || nome_turma_seguro[k] == '\\') {
                    nome_turma_seguro[k] = '_';
                }
            }

            sprintf(nome_arquivo, "Seguro_Viagem-%s-%s.csv", nome_viagem_seguro, nome_turma_seguro);

            FILE* f = fopen(nome_arquivo, "w");
            if (!f) {
                g_print("ERRO: Nao foi possivel criar o arquivo '%s' para seguro viagem.\n", nome_arquivo);
                continue;
            }

            fprintf(f, "Viagem - %s - %s\n", v->nome_viagem, data_viagem_str);
            fprintf(f, "Escola - %s - Turma - %s\n", v->nome_escola, v->turmas[i].nome_turma);
            fprintf(f, "\n");
            fprintf(f, "Nome do Aluno,RG,CPF,Data de Nascimento\n");

            int alunos_qualificados = 0;
            for (int j = 0; j < MAX_ALUNOS; j++) {
                if (alunos[j].ativo &&
                    alunos[j].id_viagem == index_viagem &&
                    strcmp(alunos[j].nome_turma, v->turmas[i].nome_turma) == 0) {

                    if (verificar_status_pagamento_aluno(&alunos[j]) == PAGAMENTO_QUITADO) {
                        fprintf(f, "\"%s\",\"%s\",\"%s\",\"%s\"\n",
                            alunos[j].nome,
                            alunos[j].rg,
                            alunos[j].cpf,
                            alunos[j].nascimento
                        );
                        alunos_qualificados++;
                    } else {
                        g_print("DEBUG: Aluno '%s' na turma '%s' nao esta com todas as parcelas quitadas. Nao incluido na planilha de seguro viagem.\n", alunos[j].nome, alunos[j].nome_turma);
                    }
                }
            }
            fclose(f);

            if (alunos_qualificados > 0) {
                g_print("DEBUG: Arquivo de seguro viagem '%s' gerado com %d aluno(s) qualificado(s).\n", nome_arquivo, alunos_qualificados);
            } else {
                g_print("DEBUG: Nenhum aluno qualificado para seguro viagem na turma '%s'. Removendo arquivo vazio '%s'.\n", v->turmas[i].nome_turma, nome_arquivo);
                remove(nome_arquivo);
            }
        }
    }
    g_print("DEBUG: Exportacao de planilhas de Seguro Viagem concluida.\n");
}
int contar_alunos_na_turma(int id_viagem, const char* nome_turma) {
    int count = 0;
    for (int i = 0; i < MAX_ALUNOS; i++) {
        if (alunos[i].ativo && alunos[i].id_viagem == id_viagem && strcmp(alunos[i].nome_turma, nome_turma) == 0) {
            count++;
        }
    }
    return count;
}

void inativar_alunos_por_viagem(int id_viagem) {
    g_print("DEBUG: Inativando alunos da viagem ID: %d...\n", id_viagem);
    int alunos_inativados_count = 0;
    for (int i = 0; i < MAX_ALUNOS; i++) {
        if (alunos[i].ativo && alunos[i].id_viagem == id_viagem) {
            alunos[i].ativo = 0;
            num_total_alunos_sistema--;
            alunos_inativados_count++;
            g_print("DEBUG: Aluno '%s' (CPF: %s) inativado.\n", alunos[i].nome, alunos[i].cpf);
        }
    }
    if (alunos_inativados_count > 0) {
        salvar_dados_alunos();
        g_print("DEBUG: Total de %d aluno(s) inativado(s) para a viagem ID %d.\n", alunos_inativados_count, id_viagem);
    } else {
        g_print("DEBUG: Nenhum aluno ativo encontrado para inativar na viagem ID %d.\n", id_viagem);
    }
}

Aluno* buscar_aluno_por_cpf_e_viagem_turma_ptr(const char* cpf, int id_viagem, const char* nome_turma) {
    for (int i = 0; i < MAX_ALUNOS; i++) {
        if (alunos[i].ativo &&
            strcmp(alunos[i].cpf, cpf) == 0 &&
            alunos[i].id_viagem == id_viagem &&
            strcmp(alunos[i].nome_turma, nome_turma) == 0) {
            return &alunos[i];
        }
    }
    return NULL;
}

StatusPagamento verificar_status_pagamento_aluno(const Aluno *aluno) {
    if (!aluno) return PAGAMENTO_EM_DIA;

    time_t raw_time;
    struct tm *current_time_tm;
    time(&raw_time);
    current_time_tm = localtime(&raw_time);

    current_time_tm->tm_mday = 26;
    current_time_tm->tm_mon = 5;
    current_time_tm->tm_year = 2025 - 1900;
    mktime(current_time_tm);

    time_t current_long_time = mktime(current_time_tm);

    int todas_quitadas = 1;
    int tem_atraso = 0;

    if (aluno->num_parcelas_cadastradas == 0) {
        return PAGAMENTO_EM_DIA;
    }

    for (int i = 0; i < aluno->num_parcelas_cadastradas; i++) {
        const Parcela *p = &aluno->parcelas[i];
        if (!p->ativa) continue;

        if (!p->quitada) {
            todas_quitadas = 0;
            time_t vencimento_time = mktime((struct tm*)&p->data_vencimento);

            if (vencimento_time < current_long_time) {
                tem_atraso = 1;
                break;
            }
        }
    }

    if (tem_atraso) {
        return PAGAMENTO_ATRASADO;
    } else if (todas_quitadas) {
        return PAGAMENTO_QUITADO;
    } else {
        return PAGAMENTO_EM_DIA;
    }
}

void exportar_planilha_embarque_por_turma_csv(int index_viagem) {
    if (index_viagem < 0 || index_viagem >= MAX_VIAGENS || !viagens[index_viagem].ativa) {
        g_print("ERRO (exportar_planilha_embarque_por_turma_csv): Viagem invalida ou inativa.\n");
        return;
    }

    const Viagem* v = &viagens[index_viagem];
    g_print("DEBUG: Gerando planilhas de Embarque por turma para a viagem '%s'...\n", v->nome_viagem);

    char data_viagem_str[11];
    strftime(data_viagem_str, sizeof(data_viagem_str), "%d/%m/%Y", &v->data_da_viagem);

    for (int i = 0; i < v->num_turmas_cadastradas; i++) {
        if (v->turmas[i].ativa) {
            char nome_viagem_seguro[MAX_NOME];
            char nome_escola_seguro[MAX_NOME];
            char nome_turma_seguro[MAX_NOME];
            char nome_arquivo[MAX_NOME * 3 + 50];

            strncpy(nome_viagem_seguro, v->nome_viagem, sizeof(nome_viagem_seguro) - 1);
            nome_viagem_seguro[sizeof(nome_viagem_seguro) - 1] = '\0';
            for (int k = 0; nome_viagem_seguro[k]; k++) {
                if (nome_viagem_seguro[k] == ' ' || nome_viagem_seguro[k] == '/' || nome_viagem_seguro[k] == '\\') {
                    nome_viagem_seguro[k] = '_';
                }
            }

            strncpy(nome_escola_seguro, v->nome_escola, sizeof(nome_escola_seguro) - 1);
            nome_escola_seguro[sizeof(nome_escola_seguro) - 1] = '\0';
            for (int k = 0; nome_escola_seguro[k]; k++) {
                if (nome_escola_seguro[k] == ' ' || nome_escola_seguro[k] == '/' || nome_escola_seguro[k] == '\\') {
                    nome_escola_seguro[k] = '_';
                }
            }

            strncpy(nome_turma_seguro, v->turmas[i].nome_turma, sizeof(nome_turma_seguro) - 1);
            nome_turma_seguro[sizeof(nome_turma_seguro) - 1] = '\0';
            for (int k = 0; nome_turma_seguro[k]; k++) {
                if (nome_turma_seguro[k] == ' ' || nome_turma_seguro[k] == '/' || nome_turma_seguro[k] == '\\') {
                    nome_turma_seguro[k] = '_';
                }
            }

            sprintf(nome_arquivo, "Planilha_de_Embarque-%s-%s-%s.csv", nome_turma_seguro, nome_escola_seguro, nome_viagem_seguro);

            FILE* f = fopen(nome_arquivo, "w");
            if (!f) {
                g_print("ERRO: Nao foi possivel criar o arquivo '%s' para a planilha de embarque.\n", nome_arquivo);
                continue;
            }

            fprintf(f, "Viagem - %s - %s\n", v->nome_viagem, data_viagem_str);
            fprintf(f, "Escola - %s - Turma - %s\n", v->nome_escola, v->turmas[i].nome_turma);
            fprintf(f, "\n");
            fprintf(f, "Nome do Aluno,RG,Assinatura\n");

            int alunos_exportados = 0;
            for (int j = 0; j < MAX_ALUNOS; j++) {
                if (alunos[j].ativo &&
                    alunos[j].id_viagem == index_viagem &&
                    strcmp(alunos[j].nome_turma, v->turmas[i].nome_turma) == 0) {

                    fprintf(f, "\"%s\",\"%s\",\"\"\n",
                        alunos[j].nome,
                        alunos[j].rg
                    );
                    alunos_exportados++;
                }
            }
            fclose(f);

            if (alunos_exportados > 0) {
                g_print("DEBUG: Arquivo de planilha de embarque '%s' gerado com %d aluno(s).\n", nome_arquivo, alunos_exportados);
            } else {
                g_print("DEBUG: Nenhum aluno encontrado para a planilha de embarque na turma '%s'. Removendo arquivo vazio '%s'.\n", v->turmas[i].nome_turma, nome_arquivo);
                remove(nome_arquivo);
            }
        }
    }
    g_print("DEBUG: Exportacao de planilhas de Embarque concluida.\n");
}
