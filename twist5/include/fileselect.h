#ifndef _FILESELECT_H_
#define _FILESELECT_H_

#include "dialog.h"

//TODO
#define FA_ARCH -1

class FileSelect : public Dialog
{
	std::string question;
	std::string path;

	std::vector<std::string> results;
public:
	FileSelect(const char *question, const char *path, const char *filter, int flag);
	bool Popup(Dialog *parent);

	std::string getSelectedFile() {
		assert (results.size() == 1);
		return results[0];
	}
};


#endif
