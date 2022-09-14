// Lic:
// BallPlay++
// User Management (header)
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
// Version: 22.09.14
// EndLic
#pragma once
#include <memory>
#include <string>
#include <GINIE.hpp>
#include <QuickTypes.hpp>


namespace BallPlay {
	class _User;
	typedef std::shared_ptr<_User> User;
	

	class _User {
	private:
		TrickyUnits::GINIE Data;
		static std::string _CurrentUserName;
		static User _CurrentUser;
		std::string _UserName;
	public:
		static std::string UserDir;
		static std::string NameToFile(std::string name);
		static bool Exists(std::string username);
		static void Set(std::string username);
		static User Get();
		static std::string Sec2Time(TrickyUnits::uint32 secs);
		int Solved(std::string pack, std::string puzzle);
		void Solved(std::string pack, std::string puzzle, int Value);
		int BestMoves(std::string pack, std::string puzzle);
		void BestMoves(std::string pack, std::string puzzle, int Value, bool OnlyTheBest = true);
		int BestTime(std::string pack, std::string puzzle);
		std::string BestTimeStr(std::string pack, std::string puzzle);
		void BestTime(std::string pack, std::string puzzle, int Value, bool OnlyTheBest = true);
		_User(std::string UserName);
		std::string UserName();
	};

	void CheckUserStartUp();

	bool NewUser();
	void RenewUserList();
	bool ChangeUser();

}