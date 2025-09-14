#include "TwinPluginFramework/SDK/ToolCreationWizard.h"
#include "Core/Logs.h"
#include "HAL/FileManager.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"

void UTwinToolCreationWizard::CreateTool(const FToolCreationSettings& Settings)
{
    if (bIsCreating)
    {
        UE_LOG(LogMode, Warning, TEXT("Tool creation already in progress"));
        return;
    }

    FString ErrorMessage;
    if (!ValidateSettings(Settings, ErrorMessage))
    {
        CompleteCreation(false, ErrorMessage);
        return;
    }

    bIsCreating = true;
    CurrentProgress = 0.0f;

    UpdateProgress(0.1f, TEXT("Validating settings..."));

    // Create C++ files if requested
    if (Settings.bCreateCppClass)
    {
        UpdateProgress(0.3f, TEXT("Creating C++ files..."));
        CreateCppFiles(Settings);
    }

    // Create Blueprint template if requested
    if (Settings.bCreateBlueprintTemplate)
    {
        UpdateProgress(0.6f, TEXT("Creating Blueprint template..."));
        CreateBlueprintAssets(Settings);
    }

    // Register tool in registry if requested
    if (Settings.bAutoRegisterTool)
    {
        UpdateProgress(0.8f, TEXT("Registering tool..."));
        RegisterToolInRegistry(Settings);
    }

    // Generate documentation
    UpdateProgress(0.9f, TEXT("Generating documentation..."));
    GenerateDocumentation(Settings);

    UpdateProgress(1.0f, TEXT("Tool creation complete!"));
    CompleteCreation(true, TEXT("Tool created successfully"));
}

bool UTwinToolCreationWizard::ValidateSettings(const FToolCreationSettings& Settings, FString& ErrorMessage)
{
    if (Settings.ToolName.IsEmpty())
    {
        ErrorMessage = TEXT("Tool name cannot be empty");
        return false;
    }

    if (Settings.ClassName.IsEmpty())
    {
        ErrorMessage = TEXT("Class name cannot be empty");
        return false;
    }

    if (!IsValidClassName(Settings.ClassName))
    {
        ErrorMessage = TEXT("Invalid class name format");
        return false;
    }

    if (Settings.SupportedModes.IsEmpty())
    {
        ErrorMessage = TEXT("At least one supported mode must be specified");
        return false;
    }

    return true;
}

FToolCreationSettings UTwinToolCreationWizard::GetDefaultSettings() const
{
    FToolCreationSettings DefaultSettings;
    DefaultSettings.ToolName = TEXT("MyTool");
    DefaultSettings.ClassName = TEXT("UMyTool");
    DefaultSettings.Description = TEXT("A custom tool for Twin Framework");
    DefaultSettings.Author = TEXT("Developer");
    DefaultSettings.Version = TEXT("1.0.0");
    DefaultSettings.TemplateType = EToolTemplateType::BasicTool;
    DefaultSettings.Category = EToolCategory::Custom;
    DefaultSettings.SupportedModes = { EAppMode::InGame, EAppMode::Editor };
    return DefaultSettings;
}

bool UTwinToolCreationWizard::IsValidClassName(const FString& ClassName) const
{
    // Check if class name starts with 'U' for UObject classes
    if (!ClassName.StartsWith(TEXT("U")))
    {
        return false;
    }

    // Check for valid characters (alphanumeric and underscore)
    for (const TCHAR& Char : ClassName)
    {
        if (!FChar::IsAlnum(Char) && Char != TEXT('_'))
        {
            return false;
        }
    }

    return true;
}

FString UTwinToolCreationWizard::SanitizeClassName(const FString& ClassName) const
{
    FString Sanitized = ClassName;

    // Ensure it starts with 'U'
    if (!Sanitized.StartsWith(TEXT("U")))
    {
        Sanitized = TEXT("U") + Sanitized;
    }

    // Remove invalid characters
    Sanitized = Sanitized.Replace(TEXT(" "), TEXT(""));
    Sanitized = Sanitized.Replace(TEXT("-"), TEXT(""));

    return Sanitized;
}

void UTwinToolCreationWizard::CreateCppFiles(const FToolCreationSettings& Settings)
{
    // Generate header file content
    FString HeaderContent = GenerateHeaderContent(Settings);
    FString HeaderPath = FPaths::Combine(Settings.OutputDirectory, TEXT("Public"), Settings.ClassName + TEXT(".h"));

    // Generate cpp file content
    FString CppContent = GenerateCppContent(Settings);
    FString CppPath = FPaths::Combine(Settings.OutputDirectory, TEXT("Private"), Settings.ClassName + TEXT(".cpp"));

    // Ensure directories exist
    IFileManager::Get().MakeDirectory(*FPaths::GetPath(HeaderPath), true);
    IFileManager::Get().MakeDirectory(*FPaths::GetPath(CppPath), true);

    // Write files
    FFileHelper::SaveStringToFile(HeaderContent, *HeaderPath);
    FFileHelper::SaveStringToFile(CppContent, *CppPath);

    UE_LOG(LogMode, Log, TEXT("Created C++ files: %s, %s"), *HeaderPath, *CppPath);
}

FString UTwinToolCreationWizard::GenerateHeaderContent(const FToolCreationSettings& Settings)
{
    FString BaseClass = GetBaseClassForTemplate(Settings.TemplateType);

    FString HeaderContent = FString::Printf(TEXT(
        "#pragma once\n"
        "#include \"CoreMinimal.h\"\n"
        "#include \"Templates/%s.h\"\n"
        "#include \"%s.generated.h\"\n"
        "\n"
        "/**\n"
        " * %s\n"
        " * %s\n"
        " * Author: %s\n"
        " * Version: %s\n"
        " */\n"
        "UCLASS(BlueprintType, Blueprintable)\n"
        "class TWINPLUSV2_API %s : public %s\n"
        "{\n"
        "    GENERATED_BODY()\n"
        "\n"
        "public:\n"
        "    %s();\n"
        "\n"
        "    // ITwinToolInterface Implementation\n"
        "    virtual FToolMetadata GetToolMetadata_Implementation() const override;\n"
        "    virtual void ActivateTool_Implementation() override;\n"
        "    virtual void DeactivateTool_Implementation() override;\n"
        "\n"
        "protected:\n"
        "    // Add your tool-specific functions here\n"
        "\n"
        "private:\n"
        "    // Add your private members here\n"
        "};\n"
    ),
        *BaseClass,
        *Settings.ClassName,
        *Settings.ToolName,
        *Settings.Description,
        *Settings.Author,
        *Settings.Version,
        *Settings.ClassName,
        *BaseClass,
        *Settings.ClassName
    );

    return HeaderContent;
}

FString UTwinToolCreationWizard::GenerateCppContent(const FToolCreationSettings& Settings)
{
    FString CppContent = FString::Printf(TEXT(
        "#include \"%s.h\"\n"
        "#include \"Core/Logs.h\"\n"
        "\n"
        "%s::%s()\n"
        "{\n"
        "    // Initialize tool metadata\n"
        "    ToolMetadata.ToolName = TEXT(\"%s\");\n"
        "    ToolMetadata.Description = TEXT(\"%s\");\n"
        "    ToolMetadata.Version = TEXT(\"%s\");\n"
        "    ToolMetadata.Author = TEXT(\"%s\");\n"
        "    ToolMetadata.Category = EToolCategory::%s;\n"
        "    ToolMetadata.Scope = EToolScope::%s;\n"
        "    \n"
        "    // Set supported modes\n"
        "    ToolMetadata.SupportedModes = {%s};\n"
        "}\n"
        "\n"
        "FToolMetadata %s::GetToolMetadata_Implementation() const\n"
        "{\n"
        "    return ToolMetadata;\n"
        "}\n"
        "\n"
        "void %s::ActivateTool_Implementation()\n"
        "{\n"
        "    Super::ActivateTool_Implementation();\n"
        "    \n"
        "    UE_LOG(LogMode, Log, TEXT(\"%s activated\"));\n"
        "    \n"
        "    // Add your activation logic here\n"
        "}\n"
        "\n"
        "void %s::DeactivateTool_Implementation()\n"
        "{\n"
        "    // Add your deactivation logic here\n"
        "    \n"
        "    UE_LOG(LogMode, Log, TEXT(\"%s deactivated\"));\n"
        "    \n"
        "    Super::DeactivateTool_Implementation();\n"
        "}\n"
    ),
        *Settings.ClassName,
        *Settings.ClassName, *Settings.ClassName,
        *Settings.ToolName,
        *Settings.Description,
        *Settings.Version,
        *Settings.Author,
        *UEnum::GetValueAsString(Settings.Category).Replace(TEXT("EToolCategory::"), TEXT("")),
        *UEnum::GetValueAsString(Settings.Scope).Replace(TEXT("EToolScope::"), TEXT("")),
        TEXT("EAppMode::InGame"), // Simplified - would generate from Settings.SupportedModes
        *Settings.ClassName,
        *Settings.ClassName,
        *Settings.ToolName,
        *Settings.ClassName,
        *Settings.ToolName
    );

    return CppContent;
}

FString UTwinToolCreationWizard::GetBaseClassForTemplate(EToolTemplateType TemplateType)
{
    switch (TemplateType)
    {
    case EToolTemplateType::BasicTool:
        return TEXT("UBasicToolTemplate");
    case EToolTemplateType::UITool:
        return TEXT("UUIToolTemplate");
    case EToolTemplateType::DataProcessor:
        return TEXT("UDataProcessorTemplate");
    default:
        return TEXT("UBasicToolTemplate");
    }
}

void UTwinToolCreationWizard::UpdateProgress(float Progress, const FString& Status)
{
    CurrentProgress = Progress;
    OnProgress.Broadcast(Progress, Status);
}

void UTwinToolCreationWizard::CompleteCreation(bool bSuccess, const FString& Message)
{
    bIsCreating = false;
    CurrentProgress = 0.0f;
    OnComplete.Broadcast(bSuccess, Message);
}


TArray<EToolTemplateType> UTwinToolCreationWizard::GetAvailableTemplates() const
{
    return {
        EToolTemplateType::BasicTool,
        EToolTemplateType::UITool,
        EToolTemplateType::DataProcessor,
        EToolTemplateType::AnalysisTool,
        EToolTemplateType::MeasurementTool,
        EToolTemplateType::SelectionTool,
        EToolTemplateType::ImportExportTool,
        EToolTemplateType::Custom
    };
}

FString UTwinToolCreationWizard::GetTemplateDescription(EToolTemplateType TemplateType) const
{
    switch (TemplateType)
    {
    case EToolTemplateType::BasicTool:
        return TEXT("Simple tool with basic functionality");
    case EToolTemplateType::UITool:
        return TEXT("Tool with user interface components");
    case EToolTemplateType::DataProcessor:
        return TEXT("Tool for processing and analyzing data");
    case EToolTemplateType::AnalysisTool:
        return TEXT("Tool for performing complex analysis");
    case EToolTemplateType::MeasurementTool:
        return TEXT("Tool for measuring objects and distances");
    case EToolTemplateType::SelectionTool:
        return TEXT("Tool for selecting and manipulating objects");
    case EToolTemplateType::ImportExportTool:
        return TEXT("Tool for importing and exporting data");
    case EToolTemplateType::Custom:
        return TEXT("Custom template with user-defined functionality");
    default:
        return TEXT("Unknown template type");
    }
}

FString UTwinToolCreationWizard::GenerateToolID(const FString& ToolName) const
{
    FString ToolID = ToolName;
    ToolID = ToolID.Replace(TEXT(" "), TEXT("_"));
    ToolID = ToolID.Replace(TEXT("-"), TEXT("_"));
    return ToolID.ToLower();
}

bool UTwinToolCreationWizard::CheckFileExists(const FString& FilePath) const
{
    return FPaths::FileExists(FilePath);
}

void UTwinToolCreationWizard::CreateBlueprintAssets(const FToolCreationSettings& Settings)
{
    UE_LOG(LogMode, Log, TEXT("Creating Blueprint assets for tool: %s"), *Settings.ToolName);
    // TODO: Implement Blueprint asset creation
}

void UTwinToolCreationWizard::RegisterToolInRegistry(const FToolCreationSettings& Settings)
{
    UE_LOG(LogMode, Log, TEXT("Registering tool in registry: %s"), *Settings.ToolName);
    // TODO: Implement tool registration
}

void UTwinToolCreationWizard::GenerateDocumentation(const FToolCreationSettings& Settings)
{
    UE_LOG(LogMode, Log, TEXT("Generating documentation for tool: %s"), *Settings.ToolName);
    // TODO: Implement documentation generation
}
