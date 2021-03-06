#include "MeshNameSanitizer.h"
#include <regex>
#include <algorithm>
#include <cctype>

std::string rlms::MeshNameSanitizer::Sanitize (std::string name) {
	std::transform (name.begin (), name.end (), name.begin (), [](unsigned char c) { return std::tolower (c); }); // sets everycaracter to lowercase
	name = std::regex_replace (name, std::regex ("\\s+"), "_"); //replace white caracters with _
	name = std::regex_replace (name, std::regex ("\\W+"), "");	//remove everything that is not alpha or num or _
	return name.substr (0, 31);
}

std::string rlms::MeshNameSanitizer::GetAlias (std::string filename) {
	std::regex format{ "(.*\\/|\\\\)*(.+)\\.(vox)$" };
	std::smatch m;
	std::regex_match (filename, m, format);

	return m[2].str ();
}

std::string rlms::MeshNameSanitizer::IncrementAlias (std::string name) {
	std::string newname;
	std::regex format{ "_(\\d+)$" };
	std::smatch m;
	std::regex_match (name, m, format);

	int num = 0;
	int num_size = 1;

	std::string::iterator it_first;
	std::string::iterator it_last;

	newname = name;
	if (m.size () >= 1) {
		//there is an increment already
		num = std::stoi (m[1].str ()) + 1;
		num_size = m[1].str ().length ();

	} else {
		if (name.size () < 30) {
			newname = name + "_";
		}
		if (name.size () < 29) {
			newname = name + "__";
		}
	}

	it_first = newname.end () - (num_size + 1L);
	it_last = newname.end ();
	newname = newname.replace (newname.end () - (num_size + 1L), newname.end (), "_" + std::to_string (num));//replace last increment with new one

	return newname;
}
