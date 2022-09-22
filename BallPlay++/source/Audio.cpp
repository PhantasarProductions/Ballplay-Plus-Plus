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
		if (!AudioRegister.count(entry)) {
			Assert(Resource()->EntryExists(entry), "Audio entry  '" + entry + "' not found in BallPlay++.JCR");
			AudioRegister[uent] = LoadAudio(*Resource(), entry);
			AudioRegister[uent]->AlwaysLoop = loop;
		}
		return AudioRegister[uent];
	}

	void SFX(std::string effect) {
		GetAudio("Audio/" + effect + ".ogg", false)->Play();
	}

}