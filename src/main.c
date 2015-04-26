/**
 * @file main.c
 * @brief Main source. GTK control.
 * @author David Benes, Stepanka Barotova
 */

#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <math.h>

/// Our awesome lib
#include "piesolve.h"

// Set glade path to nothing when unset
#ifndef GLADE_PATH
	#define GLADE_PATH
#endif

/// Global variables for reading strings from text entry and storing of the result
char text[1000] = "";
char dynamicRes[1000] = "";

/// Struct for GTK elements
typedef struct{
	GtkWidget *window;
	GtkWidget *about;
	GtkWidget *help;
	GtkEntry *entry;
	GtkLabel *label;
} Gui;

/// Struct for translating gtk button label
typedef struct{
	char *btn;
	char *function;
} Translate;

/// Table of translations, first is pattern and second is replacement
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

/**
 * @brief Translate function button into shape which will be compatible with piemath.h
 *
 * @param[text] text of gtk button label which could be translated
 * @return Rutine returns translated text or unchanged text if couldn't be translated
 */
char *get_func(char *text)
{
	for(unsigned int i = 0; i < sizeof(table) / sizeof(Translate); i++){
		if(strcmp(text, table[i].btn) == 0)
			return table[i].function;
	}

	return text;
}

/**
 * @brief Function, which handles all numeric buttons - it can get the actual number from its label
 * 
 * @param w Button which has sent the signal
 * @param entry The display of the calculator
 */
G_MODULE_EXPORT void numeric_button_clicked(GtkButton *w, GtkEntry *entry)
{
	strcpy(text, gtk_entry_get_text(entry));
	strcat(text, get_func((char *)gtk_button_get_label(w)));
	gtk_entry_set_text(entry, text);
}

// Dynamický výpis do GtkLabel from editable entry
/**
 * @brief This rutine control everything what happens in text entry and hadle it
 *
 * @todo Edit dynamic result for end bracket
 *
 * @param[entry] Main editable display of piecalc
 * @param[label] Secondary display of piecal which shows dynamic result or error in equation
 */
G_MODULE_EXPORT void editable_result(GtkEntry *entry, GtkLabel *label)
{
	memset(&dynamicRes[0], 0, sizeof(dynamicRes)); 
	memset(&text[0], 0, sizeof(text)); // Clear arrays
	
	strcpy(dynamicRes, gtk_entry_get_text(entry)); // Load text entry into dynamicRes
	
	if(strlen(dynamicRes) != 0){	// Proceed only if text entry isn't empty because of SIGSEGV
		strncpy(text, dynamicRes, strlen(dynamicRes) - 1); // Copy string without last character

		// Read last character of equation
		char c = dynamicRes[strlen(dynamicRes) - 1];
		
		sprintf(dynamicRes, "%.2f", pieSolver((char *)text));
	
		// if last character of equation is arithmetic operatin make dynamic result
		if(c == '+' || c == '-' || c == '*' || c == '/' || c == ')'){
			if(isnan(pieSolver((char *)text))){
				gtk_label_set_text(label, getError());
			}
			else{
				gtk_label_set_text(label, dynamicRes);
			}
		}
		// If last character is equal to equal print final solution
		else if(c == '='){
			if(isnan(pieSolver((char *) text))){
				gtk_label_set_text(label, getError());
			}
			else{
				gtk_label_set_text(label, dynamicRes);
				gtk_entry_set_text(entry, dynamicRes);	
			}
		}
	}
	else
		gtk_label_set_text(label, ""); // If text entry is empty, clear label too
} /// End of rutine editable_result

/**
 * @brief Delete last character of text entry (eqation)
 *
 * @param[w] Button which has sent a signal
 * @param[entry] Display of the calculator
 */
G_MODULE_EXPORT void delete_one(GtkButton *w, GtkEntry *entry)
{
	(void)w; // This is necessary because of -Wall argument of gcc
	strcpy(text, gtk_entry_get_text(entry));
	text[strlen(text) - 1] = 0;
	gtk_entry_set_text(entry, text);
}

/**
 * @brief This rutine clears arrays and text entry
 *
 * @param[w] Button whis has sent a signal
 * @param[entry] Display of the calculator which has to be cleared
 */
G_MODULE_EXPORT void clear_clicked(GtkButton *w, GtkEntry *entry)
{
	(void)w;
	memset(&dynamicRes[0], 0, sizeof(dynamicRes));
	memset(&text[0], 0, sizeof(text));
	gtk_entry_set_text(entry, "");
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
	Gui *gui;	// Struct for GUI declaration

	gui = g_slice_new(Gui); // Allocate memory for Gui struct

	gtk_init(&argc, &argv); 

	gtkBuilder = gtk_builder_new();
	GError *err = NULL; // Variable for error handling
	gtk_builder_add_from_file(gtkBuilder, GLADE_PATH "piecalc.glade", &err);

	// get object from .glade file into GUI struct	
	gui -> window = GTK_WIDGET(gtk_builder_get_object(gtkBuilder, "PieCalc"));
	gui -> about = GTK_WIDGET(gtk_builder_get_object(gtkBuilder, "aboutDialog"));
	gui -> help = GTK_WIDGET(gtk_builder_get_object(gtkBuilder, "helpDialog"));

	gtk_builder_connect_signals (gtkBuilder, gui);

	g_object_unref(G_OBJECT(gtkBuilder));

	// Signal for exiting GTK window
	g_signal_connect(gui -> window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

	// Display GTK window
	gtk_widget_show(gui -> window);

	// Set locale to English - we like decimal point more than decimal comma
	setlocale(LC_NUMERIC, "en_US.UTF-8");

	// Main loop of GTK
	gtk_main();

	return EXIT_SUCCESS;
} // End of rutine main
/*** End of file ***/
