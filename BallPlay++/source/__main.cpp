// Lic:
// BallPlay++
// Start up
// 
// 
// 
// (c) Jeroen P. Broks, 2022
// 
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
// 
// Please note that some references to data like pictures or audio, do not automatically
// fall under this licenses. Mostly this is noted in the respective files.
// 
// Version: 22.09.09
// EndLic
// 

// C++
#include <iostream>

// Tricky's Units
#include <QuickString.hpp>

// TQSG and stuff
#include <TQSA.hpp>
#include <TQSG.hpp>
#include <TQSE.hpp>

// BallPlay++
#include <builddate.h>
#include <Resource.hpp>
#include <Chain.hpp>

using namespace TrickyUnits;
using namespace BallPlay;

int main(int cnt, char** args) {
	std::cout << "BallPlay ++\nBuild: " << BPP_BuildDate << "\nCoded by: Tricky\n(C) Jeroen P. Broks - Source licensed under the terms of the GPL3\n\n";
	ResourceFile = TReplace(ExtractDir(args[0]),'\\','/') + "/BallPlay++.jcr";
	std::cout << ID("Title") << " resource file by " << ID("Author") << "\n";
	std::cout << "Last built: " << ID("LastBuild")<< "\n";
	std::cout << "Build number: " << ID("Build")<< "\n";
	TQSG_Init("BallPlay++", 1200, 1000);
	ChainRun();
	return 0;
}