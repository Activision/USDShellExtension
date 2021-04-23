#pragma once

PXR_NAMESPACE_OPEN_SCOPE

class ArResolverShellExtension : public ArDefaultResolver
{
public:
	std::shared_ptr<ArAsset> OpenAsset( const std::string &resolvedPath ) override;
};

PXR_NAMESPACE_CLOSE_SCOPE