// Lic:
// BallPlay++
// Main Menu
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
#include <Chain.hpp>
#include <MainMenu.hpp>
#include <Resource.hpp>
#include <Sinus.hpp>
#include <Fonts.hpp>
#include <Users.hpp>

#include <TQSG.hpp>
#include <TQSE.hpp>



#include <iostream>

using namespace std;
using namespace TrickyUnits;

namespace BallPlay {

	bool VisitedMainMenuBefore{ false };

	TQSG_AutoImage Logo() {
		static TQSG_AutoImage _logo{ nullptr };
		if (!_logo) {
			_logo = TQSG_LoadAutoImage(Resource(), "GFX/Logo/Ballplay.png");
			_logo->HotCenter();
			cout << "Loaded the BallPlay logo\n";
		}
		return _logo;
	}

	bool MainMenu() {
		int
			mdx = TQSG_ScreenWidth() / 2;
		VisitedMainMenuBefore = true;
		TQSG_Cls();
		TQSE_Poll();
		DoCheckQuit();
		SinusColor(0, 0, 255);
		Sinus();
		Logo()->Draw(mdx, 50);
		TQSG_Color(255, 255, 0); GetFont("Mini")->Draw("User:", 2, 2);
		TQSG_Color(0, 180, 255); GetFont("Mini")->Draw(_User::Get()->UserName(), 2, 22);
		TQSG_Color(255, 255, 255); GetFont("Mini")->Draw("Copyright Jeroen P. Broks, 2022, licensed under the GPL3", TQSG_ScreenWidth() / 2, TQSG_ScreenHeight(), 2, 1);
		Flip();
		return true;
	}

}