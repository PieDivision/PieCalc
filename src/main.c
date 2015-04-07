#include <gtk/gtk.h>
#include <stdio.h>
#include <string.h>

const gchar *textBuffer;
gchar *prom;

typedef struct {
	GtkWidget *window;
	GtkEntry *entry;
	GtkLabel *label;
} Gui;

/**
 * Function, which handles all numeric button - it can get the actual number from its label
 * 
 * @param w Button which has sent the signal
 * @param entry The display of the calculator
 */
void numeric_button_clicked(GtkButton *w, GtkEntry *entry)
{
	textBuffer = gtk_entry_get_text(entry);
	prom = g_strdup(textBuffer);
	g_strlcat(prom, gtk_button_get_label(w), 5);
	gtk_entry_set_text(entry, prom);
}

void arith_button_clicked(GtkButton *w, GtkEntry *entry)
{
	
}

// Vymazání textového okna
void clear_clicked(GtkButton *w, GtkEntry *entry)
{
	gtk_entry_set_text(entry, "");
}

// Odeslání rovnice a výpis výsledku
void send_equation(GtkButton *w, GtkEntry *entry)
{
	textBuffer = gtk_entry_get_text(entry);
	char *equation = (char *)textBuffer;
	if(strlen(equation) == 0)
		gtk_entry_set_text(entry, "Syntax error");
	else
		printf("%s\n", equation);
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
	gui -> entry = GTK_ENTRY(gtk_builder_get_object(gtkBuilder, "entry1"));
	gui -> label = GTK_LABEL(gtk_builder_get_object(gtkBuilder, "label1"));

	gtk_builder_connect_signals (gtkBuilder, gui);

	g_object_unref(G_OBJECT(gtkBuilder));

	g_signal_connect(gui -> window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

	gtk_widget_show(gui -> window);

	gtk_main();

	return 0;
}
