#pragma once
#include <jcr6_core.hpp>
namespace BallPlay {
	extern std::string ResourceFile;
	jcr6::JT_Dir* Resource();
	std::string ID(std::string Tag);
	std::string ID(std::string Cat, std::string Tag);
}