#include "TwinPluginFramework/SDK/DocumentationGenerator.h"
#include "Core/Logs.h"
#include "HAL/FileManager.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"

void UTwinDocumentationGenerator::GenerateDocumentation(const FTwinDocumentationSettings& Settings)
{
    if (bIsGenerating)
    {
        UE_LOG(LogMode, Warning, TEXT("Documentation generation already in progress"));
        return;
    }

    bIsGenerating = true;
    CurrentProgress = 0.0f;
    CurrentOutputPath = Settings.OutputDirectory;

    UpdateProgress(0.1f, TEXT("Initializing documentation generation"));

    // Ensure output directory exists
    IFileManager::Get().MakeDirectory(*Settings.OutputDirectory, true);

    // Generate each requested documentation type
    float ProgressStep = 0.8f / Settings.DocumentationTypes.Num();

    for (int32 i = 0; i < Settings.DocumentationTypes.Num(); i++)
    {
        float Progress = 0.1f + (i * ProgressStep);
        EDocumentationType DocType = Settings.DocumentationTypes[i];

        FString TypeName = UEnum::GetValueAsString(DocType);
        UpdateProgress(Progress, FString::Printf(TEXT("Generating %s"), *TypeName));

        GenerateDocumentationType(DocType, TEXT(""), Settings);
    }

    UpdateProgress(1.0f, TEXT("Documentation generation complete"));
    CompleteGeneration(true, Settings.OutputDirectory);
}

void UTwinDocumentationGenerator::GenerateToolDocumentation(const FString& ToolID, const FTwinDocumentationSettings& Settings)
{
    if (bIsGenerating)
    {
        UE_LOG(LogMode, Warning, TEXT("Documentation generation already in progress"));
        return;
    }

    bIsGenerating = true;
    CurrentProgress = 0.0f;
    CurrentOutputPath = Settings.OutputDirectory;

    UpdateProgress(0.1f, FString::Printf(TEXT("Generating documentation for tool: %s"), *ToolID));

    // Ensure output directory exists
    IFileManager::Get().MakeDirectory(*Settings.OutputDirectory, true);

    // Generate documentation for specific tool
    float ProgressStep = 0.8f / Settings.DocumentationTypes.Num();

    for (int32 i = 0; i < Settings.DocumentationTypes.Num(); i++)
    {
        float Progress = 0.1f + (i * ProgressStep);
        EDocumentationType DocType = Settings.DocumentationTypes[i];

        FString TypeName = UEnum::GetValueAsString(DocType);
        UpdateProgress(Progress, FString::Printf(TEXT("Generating %s for %s"), *TypeName, *ToolID));

        GenerateDocumentationType(DocType, ToolID, Settings);
    }

    UpdateProgress(1.0f, TEXT("Tool documentation generation complete"));
    CompleteGeneration(true, Settings.OutputDirectory);
}

void UTwinDocumentationGenerator::GenerateAllToolsDocumentation(const FTwinDocumentationSettings& Settings)
{
    if (bIsGenerating)
    {
        UE_LOG(LogMode, Warning, TEXT("Documentation generation already in progress"));
        return;
    }

    bIsGenerating = true;
    CurrentProgress = 0.0f;
    CurrentOutputPath = Settings.OutputDirectory;

    UpdateProgress(0.1f, TEXT("Generating documentation for all tools"));

    // Ensure output directory exists
    IFileManager::Get().MakeDirectory(*Settings.OutputDirectory, true);

    // TODO: Get list of all registered tools from plugin manager
    // For now, generate general documentation
    TArray<FString> AllTools = { TEXT("DefaultTool"), TEXT("SampleTool") };

    float ProgressStep = 0.8f / FMath::Max(1, AllTools.Num());

    for (int32 i = 0; i < AllTools.Num(); i++)
    {
        float Progress = 0.1f + (i * ProgressStep);
        const FString& ToolID = AllTools[i];

        UpdateProgress(Progress, FString::Printf(TEXT("Generating documentation for: %s"), *ToolID));

        // Generate each documentation type for this tool
        for (EDocumentationType DocType : Settings.DocumentationTypes)
        {
            GenerateDocumentationType(DocType, ToolID, Settings);
        }
    }

    UpdateProgress(1.0f, TEXT("All tools documentation generation complete"));
    CompleteGeneration(true, Settings.OutputDirectory);
}

void UTwinDocumentationGenerator::CancelGeneration()
{
    if (bIsGenerating)
    {
        bIsGenerating = false;
        CurrentProgress = 0.0f;
        UE_LOG(LogMode, Warning, TEXT("Documentation generation cancelled"));
        OnComplete.Broadcast(false, CurrentOutputPath);
    }
    else
    {
        UE_LOG(LogMode, Warning, TEXT("No documentation generation in progress to cancel"));
    }
}

FString UTwinDocumentationGenerator::GenerateAPIReference(const FString& ToolID)
{
    FString APIDoc = FString::Printf(TEXT(
        "# API Reference for %s\n\n"
        "## Overview\n"
        "This document provides detailed API reference for the %s tool.\n\n"
        "## Classes\n"
        "### Main Tool Class\n"
        "- **Class Name**: %s\n"
        "- **Base Class**: ITwinToolInterface\n"
        "- **Category**: Tool\n\n"
        "## Methods\n"
        "### InitializeTool()\n"
        "Initializes the tool and prepares it for use.\n\n"
        "**Returns**: bool - True if initialization was successful\n\n"
        "### ActivateTool()\n"
        "Activates the tool and begins its functionality.\n\n"
        "**Returns**: void\n\n"
        "### DeactivateTool()\n"
        "Deactivates the tool and cleans up resources.\n\n"
        "**Returns**: void\n\n"
        "### GetToolMetadata()\n"
        "Returns metadata information about the tool.\n\n"
        "**Returns**: FToolMetadata - Metadata structure containing tool information\n\n"
        "## Events\n"
        "### OnToolActivated\n"
        "Broadcast when the tool is activated.\n\n"
        "### OnToolDeactivated\n"
        "Broadcast when the tool is deactivated.\n\n"
    ), *ToolID, *ToolID, *ToolID);

    return APIDoc;
}

FString UTwinDocumentationGenerator::GenerateUserGuide(const FString& ToolID)
{
    FString UserGuide = FString::Printf(TEXT(
        "# User Guide for %s\n\n"
        "## Getting Started\n"
        "This guide will help you understand how to use the %s tool effectively.\n\n"
        "## Installation\n"
        "1. Ensure the Twin Framework plugin is installed\n"
        "2. Enable the %s tool in your project settings\n"
        "3. Restart the editor if required\n\n"
        "## Basic Usage\n"
        "1. **Activate the tool**: Select %s from the tool palette\n"
        "2. **Configure settings**: Adjust tool parameters in the details panel\n"
        "3. **Use the tool**: Follow the on-screen instructions\n"
        "4. **Complete your task**: Apply changes when satisfied\n"
        "5. **Deactivate**: Click the deactivate button or select another tool\n\n"
        "## Interface Overview\n"
        "### Tool Palette\n"
        "- Located in the main toolbar\n"
        "- Shows all available tools\n"
        "- Click to activate/deactivate tools\n\n"
        "### Settings Panel\n"
        "- Appears when tool is active\n"
        "- Contains tool-specific options\n"
        "- Changes apply in real-time\n\n"
        "## Advanced Features\n"
        "### Keyboard Shortcuts\n"
        "- `Ctrl+Alt+T`: Quick tool activation\n"
        "- `Esc`: Deactivate current tool\n"
        "- `F1`: Show tool help\n\n"
        "### Custom Presets\n"
        "Save frequently used settings as presets for quick access.\n\n"
        "## Troubleshooting\n"
        "### Common Issues\n"
        "- **Tool not appearing**: Check if plugin is enabled\n"
        "- **Performance issues**: Reduce quality settings\n"
        "- **Crashes**: Check log files for error messages\n\n"
        "### Getting Help\n"
        "- Check the documentation folder\n"
        "- Contact support team\n"
        "- Visit the community forums\n\n"
    ), *ToolID, *ToolID, *ToolID, *ToolID);

    return UserGuide;
}

FString UTwinDocumentationGenerator::GenerateTutorial(const FString& ToolID)
{
    FString Tutorial = FString::Printf(TEXT(
        "# %s Tutorial\n\n"
        "## Introduction\n"
        "Welcome to the %s tutorial! This step-by-step guide will teach you how to use this tool effectively.\n\n"
        "## Prerequisites\n"
        "- Twin Framework plugin installed\n"
        "- Basic familiarity with Unreal Engine\n"
        "- Sample project (recommended)\n\n"
        "## Step 1: Setting Up\n"
        "1. Open your Unreal Engine project\n"
        "2. Navigate to the Tools menu\n"
        "3. Locate and select %s\n"
        "4. The tool interface should appear\n\n"
        "## Step 2: Basic Configuration\n"
        "1. In the tool settings panel:\n"
        "   - Set your preferred quality level\n"
        "   - Choose appropriate output directory\n"
        "   - Enable/disable optional features\n"
        "2. Click 'Apply Settings' to confirm\n\n"
        "## Step 3: First Use\n"
        "1. Click the 'Start' button\n"
        "2. Follow the on-screen prompts\n"
        "3. Monitor progress in the status bar\n"
        "4. Wait for completion notification\n\n"
        "## Step 4: Reviewing Results\n"
        "1. Check the output directory for generated files\n"
        "2. Review logs for any warnings or errors\n"
        "3. Test the results in your project\n\n"
        "## Advanced Tutorial\n"
        "### Custom Workflows\n"
        "Learn how to create custom workflows with %s:\n"
        "1. Create a new workflow template\n"
        "2. Define custom parameters\n"
        "3. Save and share with team members\n\n"
        "### Automation\n"
        "Set up automated processes:\n"
        "1. Use Blueprint scripting\n"
        "2. Create command-line shortcuts\n"
        "3. Integrate with build systems\n\n"
        "## Tips and Best Practices\n"
        "- Always backup your project before using new tools\n"
        "- Start with default settings before customizing\n"
        "- Monitor system resources during operation\n"
        "- Keep tool documentation handy\n\n"
        "## Next Steps\n"
        "- Explore other Twin Framework tools\n"
        "- Join the community discussions\n"
        "- Share your experiences and feedback\n\n"
    ), *ToolID, *ToolID, *ToolID, *ToolID);

    return Tutorial;
}

void UTwinDocumentationGenerator::GenerateDocumentationType(EDocumentationType Type, const FString& ToolID, const FTwinDocumentationSettings& Settings)
{
    FString Content;
    FString FileName;

    switch (Type)
    {
    case EDocumentationType::API:
        Content = GenerateAPIReference(ToolID.IsEmpty() ? TEXT("AllTools") : ToolID);
        FileName = ToolID.IsEmpty() ? TEXT("API_Reference_All") : FString::Printf(TEXT("API_Reference_%s"), *ToolID);
        break;
    case EDocumentationType::UserGuide:
        Content = GenerateUserGuide(ToolID.IsEmpty() ? TEXT("AllTools") : ToolID);
        FileName = ToolID.IsEmpty() ? TEXT("User_Guide_All") : FString::Printf(TEXT("User_Guide_%s"), *ToolID);
        break;
    case EDocumentationType::Tutorial:
        Content = GenerateTutorial(ToolID.IsEmpty() ? TEXT("AllTools") : ToolID);
        FileName = ToolID.IsEmpty() ? TEXT("Tutorial_All") : FString::Printf(TEXT("Tutorial_%s"), *ToolID);
        break;
    case EDocumentationType::TechnicalRef:
        Content = FString::Printf(TEXT("# Technical Reference for %s\n\nDetailed technical specifications and implementation details."), *ToolID);
        FileName = ToolID.IsEmpty() ? TEXT("Technical_Reference_All") : FString::Printf(TEXT("Technical_Reference_%s"), *ToolID);
        break;
    case EDocumentationType::Examples:
        Content = FString::Printf(TEXT("# Code Examples for %s\n\nPractical code examples and usage scenarios."), *ToolID);
        FileName = ToolID.IsEmpty() ? TEXT("Code_Examples_All") : FString::Printf(TEXT("Code_Examples_%s"), *ToolID);
        break;
    default:
        Content = TEXT("# Documentation\n\nGenerated documentation content.");
        FileName = TEXT("Documentation");
        break;
    }

    // Save in requested formats
    if (Settings.bGenerateMarkdown)
    {
        FString MarkdownPath = FPaths::Combine(Settings.OutputDirectory, FileName + TEXT(".md"));
        SaveDocument(Content, MarkdownPath, TEXT("markdown"));
    }

    if (Settings.bGenerateHTML)
    {
        FString HTMLContent = CreateHTMLTemplate(FileName, Content);
        FString HTMLPath = FPaths::Combine(Settings.OutputDirectory, FileName + TEXT(".html"));
        SaveDocument(HTMLContent, HTMLPath, TEXT("html"));
    }
}

FString UTwinDocumentationGenerator::CreateHTMLTemplate(const FString& Title, const FString& Content)
{
    // Convert markdown-like content to HTML
    FString HTMLContent = Content;
    HTMLContent = HTMLContent.Replace(TEXT("\n# "), TEXT("\n<h1>"));
    HTMLContent = HTMLContent.Replace(TEXT("\n## "), TEXT("\n<h2>"));
    HTMLContent = HTMLContent.Replace(TEXT("\n### "), TEXT("\n<h3>"));
    HTMLContent = HTMLContent.Replace(TEXT("\n#### "), TEXT("\n<h4>"));

    // Handle the first header (no newline prefix)
    if (HTMLContent.StartsWith(TEXT("# ")))
    {
        HTMLContent = HTMLContent.Replace(TEXT("# "), TEXT("<h1>"), ESearchCase::CaseSensitive);
    }

    // Close headers
    HTMLContent = HTMLContent.Replace(TEXT("\n"), TEXT("</h1>\n"), ESearchCase::CaseSensitive);
    HTMLContent = HTMLContent.Replace(TEXT("</h1>\n<h2>"), TEXT("</h1>\n\n<h2>"));
    HTMLContent = HTMLContent.Replace(TEXT("</h2>\n<h3>"), TEXT("</h2>\n\n<h3>"));
    HTMLContent = HTMLContent.Replace(TEXT("</h3>\n<h4>"), TEXT("</h3>\n\n<h4>"));

    // Fix the replacement mess and properly close headers
    HTMLContent = HTMLContent.Replace(TEXT("<h1>"), TEXT("<h1>"));
    HTMLContent = HTMLContent.Replace(TEXT("<h2>"), TEXT("</h1>\n<h2>"));
    HTMLContent = HTMLContent.Replace(TEXT("<h3>"), TEXT("</h2>\n<h3>"));
    HTMLContent = HTMLContent.Replace(TEXT("<h4>"), TEXT("</h3>\n<h4>"));

    // Simple paragraph handling
    HTMLContent = HTMLContent.Replace(TEXT("\n\n"), TEXT("</p>\n<p>"));
    HTMLContent = TEXT("<p>") + HTMLContent + TEXT("</p>");

    // Handle code blocks
    HTMLContent = HTMLContent.Replace(TEXT("`"), TEXT("<code>"), ESearchCase::CaseSensitive);
    HTMLContent = HTMLContent.Replace(TEXT("<code>"), TEXT("</code>"), ESearchCase::CaseSensitive);

    FString HTMLTemplate = FString::Printf(TEXT(
        "<!DOCTYPE html>\n"
        "<html>\n"
        "<head>\n"
        "    <title>%s - %s Documentation</title>\n"
        "    <meta charset=\"UTF-8\">\n"
        "    <style>\n"
        "        body {\n"
        "            font-family: 'Segoe UI', Arial, sans-serif;\n"
        "            margin: 40px;\n"
        "            line-height: 1.6;\n"
        "            color: #333;\n"
        "        }\n"
        "        h1 {\n"
        "            color: #2c3e50;\n"
        "            border-bottom: 3px solid #3498db;\n"
        "            padding-bottom: 10px;\n"
        "        }\n"
        "        h2 {\n"
        "            color: #34495e;\n"
        "            margin-top: 30px;\n"
        "        }\n"
        "        h3 {\n"
        "            color: #7f8c8d;\n"
        "            margin-top: 25px;\n"
        "        }\n"
        "        code {\n"
        "            background-color: #f8f9fa;\n"
        "            border: 1px solid #e9ecef;\n"
        "            border-radius: 3px;\n"
        "            padding: 2px 4px;\n"
        "            font-family: 'Courier New', monospace;\n"
        "        }\n"
        "        p {\n"
        "            margin-bottom: 15px;\n"
        "        }\n"
        "        ul, ol {\n"
        "            margin-left: 20px;\n"
        "        }\n"
        "        .header {\n"
        "            background-color: #f8f9fa;\n"
        "            padding: 20px;\n"
        "            border-radius: 5px;\n"
        "            margin-bottom: 30px;\n"
        "        }\n"
        "    </style>\n"
        "</head>\n"
        "<body>\n"
        "    <div class=\"header\">\n"
        "        <h1>%s Documentation</h1>\n"
        "        <p>Generated by Twin Framework Documentation Generator</p>\n"
        "    </div>\n"
        "    <div class=\"content\">\n"
        "%s\n"
        "    </div>\n"
        "</body>\n"
        "</html>\n"
    ), *Title, TEXT("TP-KA"), *Title, *HTMLContent);

    return HTMLTemplate;
}

FString UTwinDocumentationGenerator::CreateMarkdownTemplate(const FString& Title, const FString& Content)
{
    FString ProjectName = TEXT("Twin Framework");
    FString Version = TEXT("1.0.0");
    FString MarkdownTemplate = FString::Printf(TEXT(
        "---\n"
        "title: %s\n"
        "project: %s\n"
        "version: %s\n"
        "generated: %s\n"
        "---\n\n"
        "%s\n"
    ), *Title, *ProjectName, *Version, *FDateTime::Now().ToString(), *Content);

    return MarkdownTemplate;
}

void UTwinDocumentationGenerator::SaveDocument(const FString& Content, const FString& FilePath, const FString& Format)
{
    // Ensure directory exists
    IFileManager::Get().MakeDirectory(*FPaths::GetPath(FilePath), true);

    // Save file with UTF-8 encoding
    bool bSuccess = FFileHelper::SaveStringToFile(Content, *FilePath, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM);

    if (bSuccess)
    {
        UE_LOG(LogMode, Log, TEXT("Saved %s documentation to: %s"), *Format, *FilePath);
    }
    else
    {
        UE_LOG(LogMode, Error, TEXT("Failed to save documentation to: %s"), *FilePath);
    }
}

void UTwinDocumentationGenerator::UpdateProgress(float Progress, const FString& CurrentDocument)
{
    CurrentProgress = FMath::Clamp(Progress, 0.0f, 1.0f);
    UE_LOG(LogMode, Log, TEXT("Documentation Progress: %.1f%% - %s"), CurrentProgress * 100.0f, *CurrentDocument);
    OnProgress.Broadcast(CurrentProgress, CurrentDocument);
}

void UTwinDocumentationGenerator::CompleteGeneration(bool bSuccess, const FString& OutputPath)
{
    bIsGenerating = false;
    CurrentProgress = bSuccess ? 1.0f : 0.0f;

    if (bSuccess)
    {
        UE_LOG(LogMode, Log, TEXT("Documentation generation completed successfully. Output: %s"), *OutputPath);
    }
    else
    {
        UE_LOG(LogMode, Error, TEXT("Documentation generation failed"));
    }

    OnComplete.Broadcast(bSuccess, OutputPath);
}

FTwinDocumentationSettings UTwinDocumentationGenerator::GetDefaultSettings() const
{
    FTwinDocumentationSettings DefaultSettings;
    DefaultSettings.DocumentationTypes = {
        EDocumentationType::API,
        EDocumentationType::UserGuide,
        EDocumentationType::Tutorial
    };
    DefaultSettings.OutputDirectory = FPaths::Combine(FPaths::ProjectDir(), TEXT("Documentation"));
    DefaultSettings.bGenerateHTML = true;
    DefaultSettings.bGenerateMarkdown = true;
    DefaultSettings.bGeneratePDF = false;
    DefaultSettings.bIncludeScreenshots = true;
    DefaultSettings.bIncludeCodeExamples = true;
    DefaultSettings.ProjectName = TEXT("Twin Framework");
    DefaultSettings.Version = TEXT("1.0.0");
    return DefaultSettings;
}