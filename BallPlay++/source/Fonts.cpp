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