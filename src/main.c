#include <gtk/gtk.h>
#include <stdio.h>
#include <string.h>
#include <locale.h>

#include "piesolve.h"

char text[1000];
char dynamicRes[1000];

typedef struct{
	GtkWidget *window;
	GtkWidget *about;
	GtkWidget *help;
	GtkEntry *entry;
	GtkLabel *label;
} Gui;

typedef struct{
	char *btn;
	char *function;
} Translate;

Translate table[] = {
	{"RND", "rand()"},
	{"√", "sqrt("},
	{"∛", "cbrt("},
	{"log", "log("},
	{"ln", "ln("},
	{"e^x", "e^"},
	{"x²", "^2"},
	{"x!", "!"}
};

char *get_func(const char *text)
{
	for(unsigned int i = 0; i < sizeof(table) / sizeof(Translate); i++){
		if(strcmp(text, table[i].btn) == 0)
			return table[i].function;
	}

	return NULL;
}

// Přeložení textu tlačítek na funkce
G_MODULE_EXPORT void function_clicked(GtkButton *w, GtkEntry *entry)
{
	(void)w;
	strcpy(text, gtk_entry_get_text(entry));
	strcat(text, get_func(gtk_button_get_label(w)));
	gtk_entry_set_text(entry, text);
}

/**
 * @brief Function that shows the aboutDialog
 * 
 * @param w Button which has sent the signal
 * @param gui Structure with the declaration of GtkWidget *about
 */
G_MODULE_EXPORT void show_about(GtkButton *w, Gui *gui)
{
	(void)w;
	gtk_dialog_run(GTK_DIALOG(gui -> about));
	gtk_widget_hide(gui -> about);
}

/**
 * @brief Function that shows the helpDialog
 * 
 * @param w Button which has sent the signal
 * @param gui Structure with the declaration of GtkWidget *help
 */
G_MODULE_EXPORT void show_help(GtkButton *w, Gui *gui)
{
	(void)w;
	gtk_dialog_run(GTK_DIALOG(gui -> help));
	gtk_widget_hide(gui -> help);
}


/**
 * @brief Function, which handles all numeric buttons - it can get the actual number from its label
 * 
 * @param w Button which has sent the signal
 * @param entry The display of the calculator
 */
G_MODULE_EXPORT void numeric_button_clicked(GtkButton *w, GtkEntry *entry)
{
	(void)w;
	strcpy(text, gtk_entry_get_text(entry));
	strcat(text, gtk_button_get_label(w));
	gtk_entry_set_text(entry, text);
}

// Dynamický výpis výsledku do GtkLabel
G_MODULE_EXPORT void dynamic_result(GtkButton *w, GtkLabel *label)
{
	(void)w;
	gtk_label_set_text(label, dynamicRes);
}

G_MODULE_EXPORT void arith_button_clicked(GtkButton *w, GtkEntry *entry)
{
	(void)w;
	sprintf(dynamicRes, "%.2f", pieSolver((char *)gtk_entry_get_text(entry)));
	strcpy(text, gtk_entry_get_text(entry));
	strcat(text, gtk_button_get_label(w));
	gtk_entry_set_text(entry, text);
}

// Smaže poslední znak na obrazovce
G_MODULE_EXPORT void delete_one(GtkButton *w, GtkEntry *entry)
{
	(void)w;
	strcpy(text, gtk_entry_get_text(entry));
	text[strlen(text) - 1] = 0;
	gtk_entry_set_text(entry, text);
}

// Vymazání textového okna
G_MODULE_EXPORT void clear_clicked(GtkButton *w, GtkEntry *entry)
{
	(void)w;
	gtk_entry_set_text(entry, "");
}

// Vymazání GtkLabel
G_MODULE_EXPORT void clear_label(GtkButton *w, GtkLabel *label)
{
	(void)w;
	gtk_label_set_text(label, "");
}

// Odeslání rovnice a výpis výsledku
G_MODULE_EXPORT void send_equation(GtkButton *w, GtkEntry *entry)
{
	(void)w;
	sprintf(text, "%.2f", pieSolver( (char *)gtk_entry_get_text(entry) ));
	gtk_entry_set_text(entry, text);
}

// Výpis výsledku do GtkLabel
G_MODULE_EXPORT void label_equation(GtkButton *w, GtkLabel *label)
{
	(void)w;
	gtk_label_set_text(label, text);
}

/**
 * Method which takes signal from quit button in help dialog and inform the parent to hide the dialog
 * 
 * @param w The button which sended the signal
 * @param d The dialog which should be close
 */
G_MODULE_EXPORT void quitHelp(GtkButton *w, GtkDialog *d)
{
	(void)w;
	gtk_dialog_response(d, 1);
}

int main(int argc, char *argv[])
{

	GtkBuilder *gtkBuilder;
	Gui *gui;

	gui = g_slice_new(Gui);

	gtk_init(&argc, &argv);

	gtkBuilder = gtk_builder_new();
	GError *err = NULL;
	gtk_builder_add_from_file(gtkBuilder, "pie_calc.glade", &err);

	
	gui -> window = GTK_WIDGET(gtk_builder_get_object(gtkBuilder, "PieCalc"));
	gui -> about = GTK_WIDGET(gtk_builder_get_object(gtkBuilder, "aboutDialog"));
	gui -> help = GTK_WIDGET(gtk_builder_get_object(gtkBuilder, "helpDialog"));
	gui -> entry = GTK_ENTRY(gtk_builder_get_object(gtkBuilder, "entry1"));

	gui -> label = GTK_LABEL(gtk_builder_get_object(gtkBuilder, "label1"));

	gtk_builder_connect_signals (gtkBuilder, gui);

	g_object_unref(G_OBJECT(gtkBuilder));

	g_signal_connect(gui -> window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

	gtk_widget_show(gui -> window);

	// Set locale to English - we like decimal point more than decimal comma
	setlocale(LC_NUMERIC, "en_US.UTF-8");

	gtk_main();

	return 0;
}
