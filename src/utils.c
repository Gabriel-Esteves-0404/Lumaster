#include "utils.h"
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h> // Para isdigit, embora g_ascii_isdigit seja melhor com GLib


void mostrar_dialogo_info(GtkWindow *parent, const char *titulo, const char *mensagem) {
    GtkWidget *dialog = gtk_message_dialog_new(parent,
                                                 GTK_DIALOG_MODAL,
                                                 GTK_MESSAGE_INFO,
                                                 GTK_BUTTONS_OK,
                                                 "%s", mensagem);
    gtk_window_set_title(GTK_WINDOW(dialog), titulo);
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

void mostrar_dialogo_erro(GtkWindow *parent, const char *titulo, const char *mensagem) {
    GtkWidget *dialog = gtk_message_dialog_new(parent,
                                                 GTK_DIALOG_MODAL,
                                                 GTK_MESSAGE_ERROR,
                                                 GTK_BUTTONS_OK,
                                                 "%s", mensagem);
    gtk_window_set_title(GTK_WINDOW(dialog), titulo);
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

void mostrar_dialogo_aviso(GtkWindow *parent, const char *titulo, const char *mensagem) {
    GtkWidget *dialog = gtk_message_dialog_new(parent,
                                                 GTK_DIALOG_MODAL,
                                                 GTK_MESSAGE_WARNING,
                                                 GTK_BUTTONS_OK,
                                                 "%s", mensagem);
    gtk_window_set_title(GTK_WINDOW(dialog), titulo);
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

gboolean mostrar_dialogo_confirmacao(GtkWindow *parent, const char *titulo, const char *mensagem) {
    GtkWidget *dialog = gtk_message_dialog_new(parent,
                                                 GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                                                 GTK_MESSAGE_QUESTION,
                                                 GTK_BUTTONS_YES_NO,
                                                 "%s", mensagem);
    gtk_window_set_title(GTK_WINDOW(dialog), titulo);
    gint result = gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
    return (result == GTK_RESPONSE_YES);
}


char* string_to_utf8(const char* local_string) {
    return g_locale_to_utf8(local_string, -1, NULL, NULL, NULL);
}

void toLowerStr_util(char *str) {
    for (int i = 0; str[i]; i++) {
        str[i] = tolower((unsigned char)str[i]);
    }
}

int contemPalavras_util(char *texto_completo_lower, char *busca_lower) {
    char busca_copy[200]; // Usar um buffer local para strtok
    strncpy(busca_copy, busca_lower, sizeof(busca_copy) - 1);
    busca_copy[sizeof(busca_copy) - 1] = '\0';

    char *palavra = strtok(busca_copy, " ");
    while (palavra) {
        if (strstr(texto_completo_lower, palavra) == NULL)
            return 0;
        palavra = strtok(NULL, " ");
    }
    return 1;
}

// NOVO: Implementação da função utilitária de limpeza numérica
char* limpar_string_numerica(const char* original_str) {
    if (!original_str) return NULL;
    GString *cleaned_gstr = g_string_new(NULL);
    for (int i = 0; original_str[i] != '\0'; i++) {
        if (g_ascii_isdigit(original_str[i])) { // Usando g_ascii_isdigit do GLib
            g_string_append_c(cleaned_gstr, original_str[i]);
        }
    }
    return g_string_free(cleaned_gstr, FALSE); // FALSE para não liberar a string, apenas o GString
}


GtkWidget* criar_label_com_markup_dinamico(const char *text, const char *size, const char *weight, GtkJustification justify, GtkAlign halign, GtkAlign valign) {
    GtkWidget *label = gtk_label_new(NULL);
    char markup_text[512];

    if (size && weight) {
        snprintf(markup_text, sizeof(markup_text), "<span size='%s' weight='%s'>%s</span>", size, weight, text);
    } else if (size) {
        snprintf(markup_text, sizeof(markup_text), "<span size='%s'>%s</span>", size, text);
    } else if (weight) {
        snprintf(markup_text, sizeof(markup_text), "<span weight='%s'>%s</span>", weight, text);
    } else {
        strncpy(markup_text, text, sizeof(markup_text) - 1);
        markup_text[sizeof(markup_text) - 1] = '\0';
    }

    gchar *utf8_markup_text = string_to_utf8(markup_text);
    if (utf8_markup_text) {
        gtk_label_set_markup(GTK_LABEL(label), utf8_markup_text);
        g_free(utf8_markup_text);
    } else {
        gtk_label_set_text(GTK_LABEL(label), text);
    }

    gtk_label_set_use_markup(GTK_LABEL(label), TRUE);
    gtk_label_set_line_wrap(GTK_LABEL(label), TRUE);
    gtk_label_set_justify(GTK_LABEL(label), justify);
    gtk_widget_set_halign(label, halign);
    gtk_widget_set_valign(label, valign);

    return label;
}

GtkSpinButton* criar_spin_button_monetario(double min, double max, double step) {
    GtkAdjustment *adj = gtk_adjustment_new(0.00, min, max, step, step * 10, 0);
    GtkSpinButton *spin = GTK_SPIN_BUTTON(gtk_spin_button_new(adj, 0.0, 2));
    gtk_spin_button_set_numeric(spin, TRUE);
    return spin;
}

GtkSpinButton* criar_spin_button_inteiro(int min, int max, int step) {
    GtkAdjustment *adj = gtk_adjustment_new(0, min, max, step, step * 10, 0);
    GtkSpinButton *spin = GTK_SPIN_BUTTON(gtk_spin_button_new(adj, 0.0, 0));
    gtk_spin_button_set_numeric(spin, TRUE);
    return spin;
}