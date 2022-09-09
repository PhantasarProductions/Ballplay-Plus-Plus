// C++
#include <iostream>

// Tricky's Units
#include <QuickString.hpp>

// BallPlay++
#include <builddate.h>
#include <Resource.hpp>

using namespace TrickyUnits;
using namespace BallPlay;

int main(int cnt, char** args) {
	std::cout << "BallPlay ++\nBuild: " << BPP_BuildDate << "\nCoded by: Tricky\n(C) Jeroen P. Broks - Source licensed under the terms of the GPL3\n\n";
	ResourceFile = TReplace(ExtractDir(args[0]),'\\','/') + "/BallPlay++.jcr";
	std::cout << ID("Title") << " resource file by " << ID("Author") << "\n";
	std::cout << "Last built: " << ID("LastBuild")<< "\n";
	std::cout << "Build number: " << ID("Build")<< "\n";
	return 0;
}