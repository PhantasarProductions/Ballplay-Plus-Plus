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

	typedef void (*MenuResponse)();
	static bool GoOn{ true };

	static void PlayGame() { cout << "TODO: Play Game\n"; }
	static void ChangeUserItem() { cout << "Change User\n"; RenewUserList();  SetChain(ChangeUser); }
	static void JukeBox() { cout << "TODO: Jukebox\n"; }
	static void Repository() { cout << "TODO: Repository\n"; }
	static void Site() { cout << "TODO: Site\n"; }
	static void Quit() { cout << "User requested to quit!\nGoodbye!\n\n"; GoOn = false; }


	class MenuItem {
	private:
		static int cnt;
		string _name;
		MenuResponse _Response;
		int _index;
		TQSG_AutoImage _image{ nullptr };
		int Y() { return (_index * 70)+150; }
		int X;
	public:
		static vector<MenuItem> Items;
		MenuItem(string n, MenuResponse r) {
			_name = n;
			_Response = r;
			_index = cnt++;
			X = -1;  
			cout << "Created item #" << _index << " '" << _name << "';  (" << X << "," << Y() << ")\n";
		}

		void Show() {
			int
				mx{ TQSE_MouseX() },
				my{ TQSE_MouseY() },
				ml{ TQSE_MouseHit(1) };
			static int
				cx{ TQSG_ScreenWidth() / 2 };
			if (X<0) X= TQSG_ScreenWidth() + (200 * _index);
			if (!_image) {
				_image = TQSG_LoadAutoImage(Resource(), "GFX/MainMenu/" + _name + ".png");
				_image->Hot(_image->W() / 2, 0);
				cout << "Loaded menu item: " << _name << "\n";
			}
			if (my > Y() && my < Y() + _image->H()) {
				TQSG_Color(80, 80, 80);
				TQSG_Rect(5, Y(), TQSG_ScreenWidth() - 10, _image->H());
				if (ml) _Response();
			}
			TQSG_Color(255, 255, 255);
			_image->Draw(X, Y());
			if (X > cx) {
				X = std::max(cx, X - 4);
				//printf("Iten %d: (%04d,%04d)\n", _index, X, Y());
			}
		}
	};
	int MenuItem::cnt{ 0 };
	vector<MenuItem> MenuItem::Items {
		MenuItem("Play Game",PlayGame),
		MenuItem("Change User",ChangeUserItem),
		MenuItem("Jukebox",JukeBox),
		MenuItem("Repository",Repository),
		MenuItem("Site",Site),
		MenuItem("Quit",Quit)
	};

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
		for (int i = 0; i < MenuItem::Items.size(); ++i) MenuItem::Items[i].Show();
		Flip();
		return GoOn;
	}

}