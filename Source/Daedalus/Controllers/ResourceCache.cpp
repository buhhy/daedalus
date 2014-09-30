#include <Daedalus.h>
#include "ResourceCache.h"
#include <Models/Items/ItemDataFactory.h>

using namespace items;

const std::string ResourceCache::ICON_DEFAULT_FOLDER = "Icons";
const std::string ResourceCache::ICON_CURSOR_FOLDER = "Cursors";

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
				staticLoadResource(skeletalMeshCache, itemMeshResourceName(rName)),
				staticLoadResource(materialCache, itemMaterialResourceName(rName)),
				staticLoadResource(animBPCache, itemAnimBPResourceName(rName)))
		});
		staticLoadResource(iconCache, iconName(rName), rName);
	}

	// Preload the resources for the missing items placeholder.
	missingMesh = {
		staticLoadResource(skeletalMeshCache, itemMeshResourceName("MissingMesh")),
		staticLoadResource(materialCache, itemMaterialResourceName("MissingMesh")),
		staticLoadResource(animBPCache, itemAnimBPResourceName("MissingMesh"))
	};
	missingIcon = staticLoadResource(
		iconCache, iconName("MissingIcon"), iconKeyString("MissingIcon"));

	// Preload cursors.
	const std::string & cursorFolder = ICON_CURSOR_FOLDER;
	staticLoadResource(
		iconCache, iconName("Pointer", cursorFolder),
		iconKeyString("Pointer", cursorFolder));
	staticLoadResource(
		iconCache, iconName("Pointer-Hover", cursorFolder),
		iconKeyString("Pointer-Hover", cursorFolder));
	staticLoadResource(
		iconCache, iconName("Pointer-Active", cursorFolder),
		iconKeyString("Pointer-Active", cursorFolder));
}

std::string ResourceCache::itemMeshResourceName(const std::string & resourceName) const {
	return "SkeletalMesh'/Game/" + resourceName + "/Mesh.Mesh'";
}

std::string ResourceCache::itemMaterialResourceName(const std::string & resourceName) const {
	return "Material'/Game/" + resourceName + "/Material.Material'";
}

std::string ResourceCache::itemAnimBPResourceName(const std::string & resourceName) const {
	return "AnimBlueprint'/Game/" + resourceName + "/AnimBlueprint.AnimBlueprint'";
}

std::string ResourceCache::iconName(
	const std::string & resourceName,
	const std::string & folderName
) const {
	std::stringstream ss;
	ss
		<< "Texture2D'/Game/" << folderName << "/" <<
		resourceName << "." << resourceName << "'";
	return ss.str();
}

std::string ResourceCache::iconKeyString(
	const std::string & resourceName,
	const std::string & folderName
) const {
	return folderName + ":" + resourceName;
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

UTexture2D * ResourceCache::findIcon(
	const std::string & resourceName,
	const std::string & folderName
) const {
	auto found = iconCache.find(iconKeyString(resourceName, folderName));
	if (found == iconCache.end())
		return missingIcon;
	else
		return found->second;
}
