#include "Oven/ovenpch.h"
#include "Oven/Utils/PlatformUtils.h"

#define GLFW_EXPOSE_NATIVE_COCOA
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#import <AppKit/AppKit.h>
#import <UniformTypeIdentifiers/UniformTypeIdentifiers.h>
#include "Oven/Core/Application.h"

namespace Oven
{
    // Fonction utilitaire pour convertir une extension (ex: "png") en UTType macOS
    static NSMutableArray<UTType*>* ParseFilterToUTTypes(const char* filter)
    {
        NSMutableArray<UTType*>* allowedTypes = [NSMutableArray array];
        if (!filter || strlen(filter) == 0)
            return allowedTypes;

        std::string filterStr(filter);
        // Exemples de parsing selon le format passé : si le string contient une extension directe
        // On extrait l'extension (ex: "oven", "png", "txt")
        size_t dotPos = filterStr.find_last_of('.');
        std::string ext = (dotPos != std::string::npos) ? filterStr.substr(dotPos + 1) : filterStr;

        // Nettoyage sommaire si le filtre contient des parenthèses ou étoiles
        size_t cleanPos = ext.find_first_of(" \t\r\n)");
        if (cleanPos != std::string::npos)
            ext = ext.substr(0, cleanPos);

        NSString* nsExt = [NSString stringWithUTF8String:ext.c_str()];
        UTType* type = [UTType typeWithFilenameExtension:nsExt];
        if (type)
        {
            [allowedTypes addObject:type];
        }

        return allowedTypes;
    }

    std::string FileDialogs::OpenFile(const char* filter)
    {
        @autoreleasepool
        {
            NSOpenPanel* panel = [NSOpenPanel openPanel];
            [panel setCanChooseFiles:YES];
            [panel setCanChooseDirectories:NO];
            [panel setAllowsMultipleSelection:NO];

            // Appliquer le filtre de type de fichier
            NSMutableArray<UTType*>* allowedTypes = ParseFilterToUTTypes(filter);
            if ([allowedTypes count] > 0)
            {
                [panel setAllowedContentTypes:allowedTypes];
            }

            // Récupérer la fenêtre GLFW pour le focus
            GLFWwindow* glfwWin = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
            if (glfwWin)
            {
                [NSApp activateIgnoringOtherApps:YES];
            }

            if ([panel runModal] == NSModalResponseOK)
            {
                NSURL* selectedUrl = [[panel URLs] firstObject];
                return std::string([[selectedUrl path] UTF8String]);
            }
        }
        return std::string();
    }

    std::string FileDialogs::SaveFile(const char* filter)
    {
        @autoreleasepool
        {
            NSSavePanel* panel = [NSSavePanel savePanel];

            // Appliquer le filtre de type de fichier
            NSMutableArray<UTType*>* allowedTypes = ParseFilterToUTTypes(filter);
            if ([allowedTypes count] > 0)
            {
                [panel setAllowedContentTypes:allowedTypes];
            }

            GLFWwindow* glfwWin = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
            if (glfwWin)
            {
                [NSApp activateIgnoringOtherApps:YES];
            }

            if ([panel runModal] == NSModalResponseOK)
            {
                NSURL* selectedUrl = [panel URL];
                return std::string([[selectedUrl path] UTF8String]);
            }
        }
        return std::string();
    }
} // namespace Oven