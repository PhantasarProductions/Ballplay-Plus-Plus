// Lic:
// BallPlay++
// Audio
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
// Version: 22.09.22
// EndLic

#include <iostream>

#include <QuickString.hpp>

#include <Chain.hpp>
#include <Audio.hpp>
#include <Resource.hpp>

using namespace std;
using namespace TrickyUnits;
namespace BallPlay {

	static map<string, TQSA_AutoAudio> AudioRegister;

	TrickyUnits::TQSA_AutoAudio GetAudio(std::string entry, bool loop) {
		auto uent{ Upper(entry) };
		if (!AudioRegister.count(uent)) {
			cout << "Loading audio: " << entry << endl;
			Assert(Resource()->EntryExists(entry), "Audio entry  '" + entry + "' not found in BallPlay++.JCR");
			AudioRegister[uent] = LoadAudio(*Resource(), entry); 
			if (!AudioRegister[uent]) Crash("Loading audio entry '" + entry + "' failed!\n\n" + SDL_GetError());
			//AudioRegister[uent]->AlwaysLoop = loop;
		}
		return AudioRegister[uent];
	}

	void SFX(std::string effect) {
		GetAudio("Audio/" + effect + ".ogg")->ChPlay(1);
	}

}