#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "usuarios.h"
#include "gui_gerenciador_usuarios.h"

// Variáveis globais de gerenciamento de usuários
Usuario usuarios[100]; // Armazena os usuários
int qntd_usuarios = 0; // Contagem de usuários registrados
int usuario_logado_idx = -1; // Índice do usuário logado; -1 se ninguém

/**
 * @brief Carrega os dados dos usuários do arquivo "usuarios.dat".
 */
void carregarUsuarios() {
    FILE *file = fopen("usuarios.dat", "rb");
    if (file != NULL) {
        fread(&qntd_usuarios, sizeof(int), 1, file);
        fread(usuarios, sizeof(Usuario), qntd_usuarios, file);
        fclose(file);
    } else {
        fprintf(stderr, "Erro ao carregar usuários ou arquivo não encontrado. Um novo arquivo será criado ao salvar.\n");
    }
}

/**
 * @brief Salva os dados dos usuários no arquivo "usuarios.dat".
 */
void salvarUsuarios() {
    FILE *file = fopen("usuarios.dat", "wb");
    if (file != NULL) {
        fwrite(&qntd_usuarios, sizeof(int), 1, file);
        fwrite(usuarios, sizeof(Usuario), qntd_usuarios, file);
        fclose(file);
    } else {
        fprintf(stderr, "Erro ao salvar usuários.\n");
    }
}

/**
 * @brief Registra um novo usuário no sistema.
 *
 * @param nome Nome de usuário.
 * @param senha Senha.
 * @param dica Dica de senha.
 * @param permissao Nível de permissão (1 para administrador, 0 para convidado).
 * @param senha_admin Senha de admin existente (necessária se estiver registrando um novo administrador e um administrador já existir).
 * @param buffer_mensagem Buffer para mensagens de retorno.
 * @return 1 em caso de sucesso, 0 em caso de falha.
 */
int registrarUsuario(const char *nome, const char *senha, const char *dica, int permissao, const char *senha_admin, char *buffer_mensagem) {
    // Valida nome de usuário e senha vazios
    if (strlen(nome) == 0) {
        sprintf(buffer_mensagem, "O nome de usuário não pode estar vazio.");
        return 0;
    }
    if (strlen(senha) == 0) {
        sprintf(buffer_mensagem, "A senha não pode estar vazia.");
        return 0;
    }

    if (qntd_usuarios >= 100) {
        sprintf(buffer_mensagem, "Limite de usuários atingido.");
        return 0;
    }

    // Verifica se o nome de usuário já existe
    for (int i = 0; i < qntd_usuarios; i++) {
        if (strcmp(nome, usuarios[i].nome_usuario) == 0) {
            sprintf(buffer_mensagem, "Esse nome de usuário já existe. Tente outro.");
            return 0;
        }
    }

    // Determina se já existe um administrador
    int existe_admin = 0;
    for (int i = 0; i < qntd_usuarios; i++) {
        if (usuarios[i].permissao_usuario == 1) {
            existe_admin = 1;
            break;
        }
    }

    // O primeiro usuário deve ser administrador
    if (qntd_usuarios == 0 && permissao == 0) {
        sprintf(buffer_mensagem, "O primeiro usuário deve ser um administrador.");
        return 0;
    }

    // Autorização para registro de novo administrador
    if (permissao == 1 && existe_admin) {
        int autorizado = 0;
        for (int i = 0; i < qntd_usuarios; i++) {
            if (usuarios[i].permissao_usuario == 1 && strcmp(usuarios[i].senha_usuario, senha_admin) == 0) {
                autorizado = 1;
                break;
            }
        }
        if (!autorizado) {
            sprintf(buffer_mensagem, "Senha de administrador incorreta. Cadastro de admin negado.");
            return 0;
        }
    }

    // Cria e adiciona o novo usuário
    Usuario novo;
    strncpy(novo.nome_usuario, nome, sizeof(novo.nome_usuario) - 1);
    novo.nome_usuario[sizeof(novo.nome_usuario) - 1] = '\0';
    strncpy(novo.senha_usuario, senha, sizeof(novo.senha_usuario) - 1);
    novo.senha_usuario[sizeof(novo.senha_usuario) - 1] = '\0';
    strncpy(novo.dica_senha, dica, sizeof(novo.dica_senha) - 1);
    novo.dica_senha[sizeof(novo.dica_senha) - 1] = '\0';
    novo.permissao_usuario = permissao;

    usuarios[qntd_usuarios++] = novo;
    salvarUsuarios();
    sprintf(buffer_mensagem, "Usuário cadastrado com sucesso!");
    return 1;
}

/**
 * @brief Tenta realizar o login de um usuário.
 *
 * @param nome Nome de usuário.
 * @param senha Senha.
 * @param buffer_mensagem Buffer para mensagens de retorno.
 * @return 1 em caso de login bem-sucedido, 0 em caso de falha.
 */
int logar_gui(const char *nome, const char *senha, char *buffer_mensagem) {
    for (int i = 0; i < qntd_usuarios; i++) {
        if (strcmp(nome, usuarios[i].nome_usuario) == 0) {
            if (strcmp(senha, usuarios[i].senha_usuario) == 0) {
                usuario_logado_idx = i;
                sprintf(buffer_mensagem, "Login bem-sucedido! Bem-vindo(a), %s.", usuarios[i].nome_usuario);
                return 1;
            } else {
                sprintf(buffer_mensagem, "Senha incorreta para o usuário '%s'. Dica: %s", nome, usuarios[i].dica_senha);
                return 0;
            }
        }
    }
    sprintf(buffer_mensagem, "Usuário '%s' não encontrado.", nome);
    return 0;
}

/**
 * @brief Remove um usuário do sistema.
 *
 * @param nome_remover Nome de usuário a ser removido.
 * @param buffer_mensagem Buffer para mensagens de retorno.
 * @return 1 em caso de sucesso, 0 em caso de falha.
 */
int removerUsuario_gui(const char *nome_remover, char *buffer_mensagem) {
    // Não permite remover o próprio usuário logado
    if (usuario_logado_idx != -1 && strcmp(nome_remover, usuarios[usuario_logado_idx].nome_usuario) == 0) {
        sprintf(buffer_mensagem, "Você não pode remover a si mesmo!");
        return 0;
    }

    int encontrado = 0;
    for (int i = 0; i < qntd_usuarios; i++) {
        if (strcmp(usuarios[i].nome_usuario, nome_remover) == 0) {
            encontrado = 1;
            for (int j = i; j < qntd_usuarios - 1; j++) {
                usuarios[j] = usuarios[j + 1];
            }
            qntd_usuarios--;
            salvarUsuarios();
            sprintf(buffer_mensagem, "Usuário '%s' removido com sucesso.", nome_remover);
            return 1;
        }
    }

    if (!encontrado) {
        sprintf(buffer_mensagem, "Usuário '%s' não encontrado.", nome_remover);
    }
    return 0;
}

/**
 * @brief Edita as informações de um usuário existente.
 *
 * @param nome_original Nome atual do usuário a ser editado.
 * @param novo_nome Novo nome de usuário.
 * @param nova_senha Nova senha.
 * @param nova_dica Nova dica de senha.
 * @param buffer_mensagem Buffer para mensagens de retorno.
 * @return 1 em caso de sucesso, 0 em caso de falha.
 */
int editarUsuario_gui(const char *nome_original, const char *novo_nome, const char *nova_senha, const char *nova_dica, char *buffer_mensagem) {
    int encontrado = 0;
    for (int i = 0; i < qntd_usuarios; i++) {
        if (strcmp(usuarios[i].nome_usuario, nome_original) == 0) {
            encontrado = 1;

            // Valida novo nome de usuário e senha vazios
            if (strlen(novo_nome) == 0) {
                sprintf(buffer_mensagem, "O novo nome de usuário não pode estar vazio.");
                return 0;
            }
            if (strlen(nova_senha) == 0) {
                sprintf(buffer_mensagem, "A nova senha não pode estar vazia.");
                return 0;
            }

            // Verifica se o novo nome de usuário já existe
            if (strcmp(nome_original, novo_nome) != 0) {
                for (int k = 0; k < qntd_usuarios; k++) {
                    if (k != i && strcmp(usuarios[k].nome_usuario, novo_nome) == 0) {
                        sprintf(buffer_mensagem, "O novo nome de usuário '%s' já existe. Por favor, escolha outro.", novo_nome);
                        return 0;
                    }
                }
            }

            // Atualiza os dados do usuário
            strncpy(usuarios[i].nome_usuario, novo_nome, sizeof(usuarios[i].nome_usuario) - 1);
            usuarios[i].nome_usuario[sizeof(usuarios[i].nome_usuario) - 1] = '\0';
            strncpy(usuarios[i].senha_usuario, nova_senha, sizeof(usuarios[i].senha_usuario) - 1);
            usuarios[i].senha_usuario[sizeof(usuarios[i].senha_usuario) - 1] = '\0';
            strncpy(usuarios[i].dica_senha, nova_dica, sizeof(usuarios[i].dica_senha) - 1);
            usuarios[i].dica_senha[sizeof(usuarios[i].dica_senha) - 1] = '\0';

            salvarUsuarios();
            sprintf(buffer_mensagem, "Usuário '%s' atualizado com sucesso.", nome_original);
            return 1;
        }
    }

    if (!encontrado) {
        sprintf(buffer_mensagem, "Usuário '%s' não encontrado.", nome_original);
    }
    return 0;
}

/**
 * @brief Preenche um buffer com uma lista formatada de todos os usuários.
 *
 * @param buffer_saida Buffer de saída.
 * @param tamanho_buffer Tamanho máximo do buffer.
 */
void listarUsuarios_gui(char *buffer_saida, int tamanho_buffer) {
    int offset = 0;
    offset += snprintf(buffer_saida + offset, tamanho_buffer - offset, "--- Lista de Usuários Cadastrados ---\n");
    for (int i = 0; i < qntd_usuarios; i++) {
        offset += snprintf(buffer_saida + offset, tamanho_buffer - offset, "%d. %s (%s)\n", i + 1, usuarios[i].nome_usuario,
                           usuarios[i].permissao_usuario == 1 ? "admin" : "convidado");
    }
    if (qntd_usuarios == 0) {
        offset += snprintf(buffer_saida + offset, tamanho_buffer - offset, "Nenhum usuário cadastrado.\n");
    }
}

/**
 * @brief Exporta todos os dados dos usuários para um arquivo CSV.
 *
 * @param buffer_mensagem Buffer para mensagens de retorno.
 * @return 1 em caso de sucesso, 0 em caso de falha.
 */
int exportarCSV_gui(char *buffer_mensagem) {
    FILE *file = fopen("usuarios.csv", "w");
    if (file == NULL) {
        sprintf(buffer_mensagem, "Erro ao criar arquivo CSV.");
        return 0;
    }

    fprintf(file, "Nome;Permissao\n");
    for (int i = 0; i < qntd_usuarios; i++) {
        fprintf(file, "%s;%s\n", usuarios[i].nome_usuario,
                usuarios[i].permissao_usuario == 1 ? "admin" : "convidado");
    }

    fclose(file);
    sprintf(buffer_mensagem, "Exportação concluída: usuarios.csv");
    return 1;
}