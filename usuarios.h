#ifndef USUARIOS_H
#define USUARIOS_H

int obterPermissaoUsuario(const char *nome_usuario);


typedef struct {
    char nome_usuario[50];
    char senha_usuario[50];
    char dica_senha[100];
    int permissao_usuario; // 1 = admin, 0 = convidado
} Usuario;


extern Usuario usuarios[];
extern int qntd_usuarios;
extern int usuario_logado_idx;



/**
 * @brief
 */
void carregarUsuarios();

/**
 * @brief 
 */
void salvarUsuarios();

/**
 * @brief
 * @param nome
 * @param senha
 * @param dica
 * @param permissao 
 * @param senha_admin 
 * @param buffer_mensagem 
 * @return 
 */
int registrarUsuario(const char *nome, const char *senha, const char *dica, int permissao, const char *senha_admin, char *buffer_mensagem);

/**
 * @brief
 * @param nome
 * @param senha
 * @param buffer_mensagem
 * @return
 */
int logar_gui(const char *nome, const char *senha, char *buffer_mensagem);

/**
 * @brief 
 * @param nome_remover
 * @param buffer_mensagem 
 * @return 
 */
int removerUsuario_gui(const char *nome_remover, char *buffer_mensagem);

/**
 * @brief
 * @param nome_original
 * @param novo_nome
 * @param nova_senha
 * @param nova_dica
 * @param buffer_mensagem
 * @return
 */
int editarUsuario_gui(const char *nome_original, const char *novo_nome, const char *nova_senha, const char *nova_dica, char *buffer_mensagem);

/**
 * @brief 
 * @param buffer_saida 
 * @param tamanho_buffer
 */
void listarUsuarios_gui(char *buffer_saida, int tamanho_buffer);

/**
 * @brief
 * @param buffer_mensagem
 * @return
 */
int exportarCSV_gui(char *buffer_mensagem);

#endif