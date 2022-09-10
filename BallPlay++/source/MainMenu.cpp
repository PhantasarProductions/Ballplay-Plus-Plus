#include <MainMenu.hpp>
#include <Resource.hpp>
#include <iostream>

using namespace std;
using namespace TrickyUnits;

namespace BallPlay {
	TQSG_AutoImage Logo() {
		static TQSG_AutoImage _logo{ nullptr };
		if (!_logo) {
			_logo = TQSG_LoadAutoImage(Resource(), "GFX/Logo/Ballplay.png");
			_logo->HotCenter();
			cout << "Loaded the BallPlay logo\n";
		}
		return _logo;
	}

}