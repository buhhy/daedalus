#include <Daedalus.h>
#include "ResourceCache.h"
#include <Models/Items/ItemDataFactory.h>

using namespace items;

ResourceCache::ResourceCache(
	const FPostConstructInitializeProperties & PCIP,
	const ItemDataFactoryPtr & idFact
) {
	const auto itemDataTemplates = idFact->getItemDataTemplates();

	// Preload all item meshes and other resources.
	for (const auto & idPair : itemDataTemplates) {
		const auto rName = idPair.second->resourceName;
		meshSetCache.insert({
			rName,
			SkeletalMeshResourceSet(
				staticLoadResource(skeletalMeshCache, itemMeshName(rName)),
				staticLoadResource(materialCache, itemMaterialName(rName)),
				staticLoadResource(animBPCache, itemAnimBlueprintName(rName)))
		});
		staticLoadResource(iconCache, iconName(rName), rName);
	}

	// Preload the resources for the missing items placeholder.
	missingMesh = {
		staticLoadResource(skeletalMeshCache, itemMeshName("MissingMesh")),
		staticLoadResource(materialCache, itemMaterialName("MissingMesh")),
		staticLoadResource(animBPCache, itemAnimBlueprintName("MissingMesh"))
	};
	missingIcon = staticLoadResource(iconCache, iconName("MissingIcon"), "MissingIcon");
}

std::string ResourceCache::itemMeshName(const std::string & resourceName) const {
	return "SkeletalMesh'/Game/" + resourceName + "/Mesh.Mesh'";
}

std::string ResourceCache::itemMaterialName(const std::string & resourceName) const {
	return "Material'/Game/" + resourceName + "/Material.Material'";
}

std::string ResourceCache::itemAnimBlueprintName(const std::string & resourceName) const {
	return "AnimBlueprint'/Game/" + resourceName + "/AnimBlueprint.AnimBlueprint'";
}

std::string ResourceCache::iconName(const std::string & resourceName) const {
	return "Texture2D'/Game/Icons/" + resourceName + "." + resourceName + "'";
}

const ResourceCache::SkeletalMeshResourceSet & ResourceCache::findItemResourceSet(
	const std::string & resourceName
) const {
	auto found = meshSetCache.find(resourceName);
	if (found == meshSetCache.end())
		return missingMesh;
	else
		return found->second;
}

UTexture2D * ResourceCache::findIcon(const std::string & resourceName) const {
	auto found = iconCache.find(resourceName);
	if (found == iconCache.end())
		return missingIcon;
	else
		return found->second;
}
