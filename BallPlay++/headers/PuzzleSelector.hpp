#pragma once
// C++
#include <string>
#include <memory>
// Tricky's Units
#include <GINIE.hpp>
// BallPlay
#include <Chain.hpp>
#include <Resource.hpp>
namespace BallPlay {

	class _PuzPack;
	typedef std::shared_ptr<_PuzPack> PuzPack;

	class _PuzPack {
	private:
		static std::map<std::string, PuzPack> Pack;
		static std::string _Selected;
		TrickyUnits::GINIE Meta;
	public:
		_PuzPack(std::string pack);
		static void SetPack(std::string pack);
		static PuzPack GetPack(std::string pack, bool autoset = false);
		static PuzPack GetPack();
		int MaxPuzzles();
		std::string Tag(int num);
		std::string Name(int num);
		int Solved(int num); // Solved by the current user that is
	};

	bool PuzzleSelector();

}