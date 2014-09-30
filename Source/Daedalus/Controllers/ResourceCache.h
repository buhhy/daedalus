#pragma once

#include <Engine.h>
#include <unordered_map>
#include <memory>

namespace items {
	class ItemDataFactory;
	using ItemDataFactoryPtr = std::shared_ptr<ItemDataFactory>;
}

/**
 * All Unreal Engine resources accessed through C++ should be retrieved through this class.
 */
class ResourceCache {
public:
	struct SkeletalMeshResourceSet {
		USkeletalMesh * mesh;
		UMaterial * material;
		UAnimBlueprint * animBP;

		SkeletalMeshResourceSet() : mesh(NULL), material(NULL), animBP(NULL) {}

		SkeletalMeshResourceSet(
			USkeletalMesh * mesh,
			UMaterial * material,
			UAnimBlueprint * animBP
		) : mesh(mesh), material(material), animBP(animBP)
		{}
	};

	static const std::string ICON_DEFAULT_FOLDER;
	static const std::string ICON_CURSOR_FOLDER;

private:
	template <typename T>
	using RefCache = std::unordered_map<std::string, T>;

	template <typename T>
	using PtrCache = std::unordered_map<std::string, T *>;

	// { full resource path : resource }
	PtrCache<USkeletalMesh> skeletalMeshCache;
	PtrCache<UMaterial> materialCache;
	PtrCache<UAnimBlueprint> animBPCache;

	// { simple resource name : resource }
	PtrCache<UTexture2D> iconCache;
	PtrCache<UFont> fontCache;
	RefCache<SkeletalMeshResourceSet> meshSetCache;

	SkeletalMeshResourceSet missingMesh;
	UTexture2D * missingIcon;



	std::string itemMeshResourceName(const std::string & resourceName) const;
	std::string itemMaterialResourceName(const std::string & resourceName) const;
	std::string itemAnimBPResourceName(const std::string & resourceName) const;

	std::string iconName(
		const std::string & resourceName,
		const std::string & folderName = ICON_DEFAULT_FOLDER) const;
	std::string iconKeyString(
		const std::string & resourceName,
		const std::string & folderName = ICON_DEFAULT_FOLDER) const;

	template <typename ObjClass>
	ObjClass * loadObjFromPath(const TCHAR * path) {
		if (path == NULL)
			return NULL;
		return Cast<ObjClass>(StaticLoadObject(ObjClass::StaticClass(), NULL, path));
	}

	template <typename ObjClass>
	ObjClass * dynamicLoadResource(PtrCache<ObjClass> & cache, const std::string & nameStr) {
		const TCHAR * name = UTF8_TO_TCHAR(nameStr.c_str());
		auto found = cache.find(nameStr);

		if (found != cache.end())
			return found->second;
		else {
			auto loaded = loadObjFromPath<ObjClass>(name);
			cache.insert({ nameStr, loaded });
			return loaded;
		}
	}

	template <typename ObjClass>
	ObjClass * staticLoadResource(PtrCache<ObjClass> & cache, const std::string & nameStr) {
		return staticLoadResource(cache, nameStr, nameStr);
	}

	template <typename ObjClass>
	ObjClass * staticLoadResource(
		PtrCache<ObjClass> & cache,
		const std::string & nameStr,
		const std::string & key
	) {
		const TCHAR * name = UTF8_TO_TCHAR(nameStr.c_str());
		auto found = cache.find(key);
		
		if (found != cache.end()) {
			return found->second;
		} else {
			ConstructorHelpers::FObjectFinder<ObjClass> loaded(name);
			if (loaded.Succeeded()) {
				cache.insert({ key, loaded.Object });
				return loaded.Object;
			}
			return NULL;
		}
	}

public:
	ResourceCache(
		const FPostConstructInitializeProperties & PCIP,
		const items::ItemDataFactoryPtr & idFact);

	const SkeletalMeshResourceSet & findItemResourceSet(const std::string & resourceName) const;
	UTexture2D * findIcon(
		const std::string & resourceName,
		const std::string & folderName = ICON_DEFAULT_FOLDER) const;
};

using ResourceCachePtr = std::shared_ptr<ResourceCache>;
using ResourceCacheCPtr = std::shared_ptr<const ResourceCache>;
