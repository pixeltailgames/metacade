#pragma once

#include <map>
#include <vector>

struct CAssetFileDef
{
	CString _name;
	CString _path;
	CString _params;
	EAssetType _assetType;
};

class CProject
{
public:
	//JSON HERE
	CProject(const CString& filepath, const CString& rootpath);

	CString getProjectName() const;
	CString getProjectPath() const;
	CString getRootDirectory() const;

	bool getMetaValue(const CString& key, CVariant& value) const;
	void setMetaValue(const CString& key, const CVariant& value);

	bool addAssetDef(EAssetType type, const CString& name, const CString& path);

	int32 numAssetDefs(EAssetType type) const;
	CAssetFileDef getAssetDef(EAssetType type, int32 num) const;

	void removeAssetDef(EAssetType type, int32 num);

	void makeDirty();
	bool isDirty() const;

	IPackage* buildPackage(IRuntime* runtime);

private:
	friend class CProjectManager;

	bool load(IFileObject* file);
	bool save(IFileObject* file) const;

	bool _dirty;
	CString _filepath;
	CString _rootpath;
	std::map<CString, CVariant> _meta;
	std::map<EAssetType, std::vector<CAssetFileDef>> _assets;
};