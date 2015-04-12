#include <gtk/gtk.h>
#include <stdio.h>
#include <string.h>

#include "piesolve.h"

char text[1000];
char dynamicRes[1000];

typedef struct{
	GtkWidget *window;
	GtkWidget *about;
	GtkEntry *entry;
	GtkLabel *label;
} Gui;

typedef struct{
	char *btn;
	char *function;
} Translate;

Translate table[] = {
	{"RND", "rand()"},
	{"√", "sqrt"},
	{"x!", "!"},
	{"x²", "^2"},
	{"e^x", "e^"},
	{"∛", "cbrt("}
};


void show_about(GtkButton *w, Gui *gui)
{
	gtk_dialog_run(GTK_DIALOG(gui -> about));
	gtk_widget_hide(gui -> about);
}

char *get_func(const char *text)
{
	for(unsigned int i = 0; i < sizeof(table) / sizeof(Translate); i++){
		if(strcmp(text, table[i].btn) == 0)
			return table[i].function;
	}

	return NULL;
}

/**
 * @brief Function, which handles all numeric buttons - it can get the actual number from its label
 * 
 * @param w Button which has sent the signal
 * @param entry The display of the calculator
 */
void numeric_button_clicked(GtkButton *w, GtkEntry *entry)
{
	strcpy(text, gtk_entry_get_text(entry));
	strcat(text, gtk_button_get_label(w));
	gtk_entry_set_text(entry, text);
}

// Dynamický výpis výsledku do GtkLabel
void dynamic_result(GtkButton *w, GtkLabel *label)
{
	gtk_label_set_text(label, dynamicRes);
}

void arith_button_clicked(GtkButton *w, GtkEntry *entry)
{
	sprintf(dynamicRes, "%.2f", pieSolver((char *)gtk_entry_get_text(entry)));
	strcpy(text, gtk_entry_get_text(entry));
	strcat(text, gtk_button_get_label(w));
	gtk_entry_set_text(entry, text);
}

// Přeložení textu tlačítek na funkce
void function_clicked(GtkButton *w, GtkEntry *entry)
{
	strcpy(text, gtk_entry_get_text(entry));
	strcat(text, get_func(gtk_button_get_label(w)));
	gtk_entry_set_text(entry, text);
}

// Smaže poslední znak na obrazovce
void delete_one(GtkButton *w, GtkEntry *entry)
{
	strcpy(text, gtk_entry_get_text(entry));
	text[strlen(text) - 1] = 0;
	gtk_entry_set_text(entry, text);
}

// Vymazání textového okna
void clear_clicked(GtkButton *w, GtkEntry *entry)
{
	gtk_entry_set_text(entry, "");
}

// Vymazání GtkLabel
void clear_label(GtkButton *w, GtkLabel *label)
{
	gtk_label_set_text(label, "");
}

// Odeslání rovnice a výpis výsledku
void send_equation(GtkButton *w, GtkEntry *entry)
{
	sprintf(text, "%.2f", pieSolver( (char *)gtk_entry_get_text(entry) ));
	gtk_entry_set_text(entry, text);
}

// Výpis výsledku do GtkLabel
void label_equation(GtkButton *w, GtkLabel *label)
{
	gtk_label_set_text(label, text);
}

int main(int argc, char *argv[])
{
	GtkBuilder *gtkBuilder;
	Gui *gui;

	gui = g_slice_new(Gui);

	gtk_init(&argc, &argv);

	gtkBuilder = gtk_builder_new();
	gtk_builder_add_from_file(gtkBuilder, "pie_calc.glade", NULL);
	
	gui -> window = GTK_WIDGET(gtk_builder_get_object(gtkBuilder, "PieCalc"));
	gui -> about = GTK_WIDGET(gtk_builder_get_object(gtkBuilder, "aboutDialog"));
	gui -> entry = GTK_ENTRY(gtk_builder_get_object(gtkBuilder, "entry1"));

	gui -> label = GTK_LABEL(gtk_builder_get_object(gtkBuilder, "label1"));

	gtk_builder_connect_signals (gtkBuilder, gui);

	g_object_unref(G_OBJECT(gtkBuilder));

	g_signal_connect(gui -> window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

	gtk_widget_show(gui -> window);

	gtk_main();

	return 0;
}
