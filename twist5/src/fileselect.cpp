#include "fileselect.h"
#include <allegro5/allegro_native_dialog.h>

using namespace std;

FileSelect::FileSelect(const char *aQuestion, const char *aPath, const char *filter, int flag)
{
	question = aQuestion;
	path = aPath;

	/* TODO: flags */

}

bool FileSelect::Popup(Dialog *parent)
{
	ALLEGRO_FILECHOOSER *file_dialog;
	file_dialog = al_create_native_file_dialog(
		path.c_str(), question.c_str(), NULL,
		ALLEGRO_FILECHOOSER_MULTIPLE);

	bool ok = al_show_native_file_dialog(al_get_current_display(), file_dialog);

	if (!ok) return false;
	//TODO: use these functions

	// Returns the number of files selected, or 0 if the dialog was cancelled.
	int count = al_get_native_file_dialog_count(file_dialog);

	results.clear();
	for (int i = 0; i < count; ++i) {
		const char *path = al_get_native_file_dialog_path(file_dialog, i);
		results.push_back(path);
	}

	al_destroy_native_file_dialog(file_dialog);

	return (count > 0);
}
