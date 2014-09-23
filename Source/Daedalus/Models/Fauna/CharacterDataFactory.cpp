#include <Daedalus.h>
#include "CharacterDataFactory.h"

namespace fauna {
	using namespace utils;

	CharacterDataFactory::CharacterDataFactory() {
		LoadCharDataTemplates();
	}

	void CharacterDataFactory::LoadCharDataTemplates() {
		// TODO: load character template data from files
		ItemTemplates.insert(std::make_pair(
			C_Hero, CharDataTemplateUPtr(new CharacterDataTemplate(C_Hero, 32, 8, 200, 200))));
	}

	CharacterDataPtr CharacterDataFactory::BuildCharData(const CharacterType type) const {
		const auto found = ItemTemplates.find(type);
		if (found == ItemTemplates.end())
			return NULL;
		return CharacterDataPtr(new CharacterData(*found->second, 1));
	}
}
