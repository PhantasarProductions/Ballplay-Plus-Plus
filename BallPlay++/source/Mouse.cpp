#include <Mouse.hpp>
#include <Resource.hpp>
#include <TQSG.hpp>
#include <TQSE.hpp>

using namespace TrickyUnits;

namespace BallPlay {
	static TQSG_AutoImage _mouse{nullptr};

	static void LoadMousePointer() {
		if (!_mouse) _mouse = TQSG_LoadAutoImage(Resource(), "GFX/Mouse.png");
	}


	void ShowMouse() {
		LoadMousePointer();
		_mouse->Draw(TQSE_MouseX(), TQSE_MouseY());
	}

}