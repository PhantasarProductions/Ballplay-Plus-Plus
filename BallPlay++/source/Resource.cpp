#include <iostream>
#include <Resource.hpp>
#include <jcr6_zlib.hpp>
#include <QuickString.hpp>
#include <GINIE.hpp>

using namespace std;
using namespace jcr6;
using namespace TrickyUnits;

namespace BallPlay {

	JT_Dir _Resource;
	string ResourceFile;
	GINIE _ID;

	jcr6::JT_Dir* Resource() {
		static bool init{ false };
		if (!init) {
			cout << "Initiating JCR6\n";
			init_JCR6();
			cout << "Initiating zlib driver for JCR6\n";
			init_zlib();
			cout << "Reading JCR: " << ResourceFile<<"\n";
			_Resource = Dir(ResourceFile);
			init = true;
		}
		return &_Resource;
		
		return nullptr;
	}
	std::string ID(string Tag) { return ID("ID", Tag); }
	std::string ID(string Cat, string Tag) {
		static bool init{ false };
		if (!init) {
			cout << "Identifying\n";
			_ID.Parse(Resource()->String("ID/ID.ini"));
			init = true;
		}
		return _ID.Value(Cat,Tag);
	}
}
