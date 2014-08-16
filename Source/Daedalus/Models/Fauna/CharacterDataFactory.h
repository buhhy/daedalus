#pragma once

#include <Models/Fauna/CharacterData.h>
#include <Utilities/Algebra/Algebra3D.h>

#include <memory>
#include <unordered_map>

namespace fauna {
	class CharacterDataFactory {
	public:
		using CharDataTemplateUPtr = std::unique_ptr<CharacterDataTemplate>;
		using CharDataTemplateMap = std::unordered_map<CharacterType, CharDataTemplateUPtr>;

	private:
		CharDataTemplateMap ItemTemplates;

		void LoadCharDataTemplates();

	public:
		CharacterDataFactory();

		CharacterDataPtr BuildCharData(const CharacterType type) const;

	};

	using CharacterDataFactoryPtr = std::shared_ptr<CharacterDataFactory>;
}
