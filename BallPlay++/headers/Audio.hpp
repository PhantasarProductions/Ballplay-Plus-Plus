#pragma once
#include <TQSA.hpp>


namespace BallPlay {
	TrickyUnits::TQSA_AutoAudio GetAudio(std::string entry,bool loop=false);
	void SFX(std::string effect);
}
