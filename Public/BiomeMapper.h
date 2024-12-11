#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

/**
 * Module for managing the Biome Mapper plugin.
 */
class FBiomeMapperModule : public IModuleInterface
{
public:
    /** Called when the module is starting up. */
    virtual void StartupModule() override;

    /** Called when the module is shutting down. */
    virtual void ShutdownModule() override;
};
