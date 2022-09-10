// Lic:
// BallPlay++
// Fonts
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
// Version: 22.09.10
// EndLic
#include <Resource.hpp>
#include <TQSG.hpp>
#include <QuickString.hpp>
#include <iostream>
#include <map>

using namespace std;
using namespace TrickyUnits;

namespace BallPlay {
	static map<string, TQSG_AutoImageFont> FontMap;

	TQSG_AutoImageFont GetFont(string f) {
		auto uf{ Upper(f) };
		if (!FontMap.count(f)) {
			FontMap[f] = TQSG_LoadAutoImageFont(Resource(), "Fonts/" + f + ".jfbf");
			cout << "Font " << f << " requested and loaded\n";
		}
		return FontMap[f];
	}
}