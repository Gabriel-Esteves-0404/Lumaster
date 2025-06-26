#ifndef GUI_GERENCIADOR_USUARIOS_H
#define GUI_GERENCIADOR_USUARIOS_H
#include <gtk/gtk.h> 

extern GtkWidget *janela_principal;
extern GtkWidget *pilha_telas;

void mostrar_dialogo_mensagem(GtkWindow *pai, GtkMessageType tipo, const char *titulo, const char *mensagem);

void atualizar_exibicao_lista_usuarios();

void Lumaster_Shutdown();

GtkWidget *criar_tela_gerenciamento_usuarios();

void on_gerenciamento_usuarios_clicado(GtkButton *botao, gpointer dados_usuario);

#endif