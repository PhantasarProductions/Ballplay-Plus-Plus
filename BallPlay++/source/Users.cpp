// Lic:
// BallPlay++
// User Management
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
#include <Users.hpp>
#include <Config.hpp>
#include <Chain.hpp>
#include <Sinus.hpp>

#include <Dirry.hpp>
#include <FileList.hpp>
#include <QuickStream.hpp>
#include <TQSE.hpp>
#include <TQSG.hpp>

using namespace TrickyUnits;

namespace BallPlay {

	std::string _User::UserDir{ Dirry("$AppSupport$/BallPlay++/Users") };
	std::string _User::CurrentUser{ "" };

	_User::_User(std::string UserName) {
		std::string fname{ UserDir + "/" + UserName + ".ini" };
		MakeDir(UserDir);
		Data.FromFile(fname,true);
		Data.AutoSave = fname;
	}

	void CheckUserStartUp(){
		auto chku{ Config("Users","Last") };
		if (chku.size() && FileExists(_User::UserDir + "/" + chku + ".ini")) {
			Crash("No chain to main menu possible yet!"); 
			return;
		}
		if (FileList(_User::UserDir).size()) {
			Crash("No chain yet possible to user list");
			return;
		}
		SetChain(NewUser);
		return;
	}

	bool NewUser() {
		TQSG_Cls();
		TQSE_Poll();
		DoCheckQuit();
		Sinus();
		Flip();
		return true;
	}

}