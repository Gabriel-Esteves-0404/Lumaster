#ifndef UTILS_H
#define UTILS_H

#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <string.h>
#include <glib.h>

void mostrar_dialogo_info(GtkWindow *parent, const char *titulo, const char *mensagem);
void mostrar_dialogo_erro(GtkWindow *parent, const char *titulo, const char *mensagem);
void mostrar_dialogo_aviso(GtkWindow *parent, const char *titulo, const char *mensagem);
gboolean mostrar_dialogo_confirmacao(GtkWindow *parent, const char *titulo, const char *mensagem);


char* string_to_utf8(const char* local_string);


void toLowerStr_util(char *str);
int contemPalavras_util(char *texto_completo_lower, char *busca_lower);
    
char* limpar_string_numerica(const char* original_str);


GtkWidget* criar_label_com_markup_dinamico(const char *text, const char *size, const char *weight, GtkJustification justify, GtkAlign halign, GtkAlign valign);


GtkSpinButton* criar_spin_button_monetario(double min, double max, double step);


GtkSpinButton* criar_spin_button_inteiro(int min, int max, int step);

#endif