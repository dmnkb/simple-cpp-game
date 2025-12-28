#pragma once

#include <nfd.h>
#include <optional>
#include <string>
#include <vector>

namespace Engine
{

class FileDialog
{
  public:
    struct Filter
    {
        std::string name; // e.g., "Image Files"
        std::string spec; // e.g., "png,jpg,jpeg"
    };

    // Opens a native file open dialog
    // Returns the selected file path, or std::nullopt if cancelled
    static std::optional<std::string> openFile(const std::string& title = "Open File",
                                               const std::vector<Filter>& filters = {},
                                               const std::string& defaultPath = "")
    {
        NFD_Init();

        nfdchar_t* outPath = nullptr;
        nfdresult_t result;

        if (filters.empty())
        {
            result = NFD_OpenDialog(&outPath, nullptr, 0, defaultPath.empty() ? nullptr : defaultPath.c_str());
        }
        else
        {
            std::vector<nfdfilteritem_t> nfdFilters;
            for (const auto& f : filters)
            {
                nfdFilters.push_back({f.name.c_str(), f.spec.c_str()});
            }
            result = NFD_OpenDialog(&outPath, nfdFilters.data(), static_cast<nfdfiltersize_t>(nfdFilters.size()),
                                    defaultPath.empty() ? nullptr : defaultPath.c_str());
        }

        std::optional<std::string> resultPath;
        if (result == NFD_OKAY && outPath)
        {
            resultPath = std::string(outPath);
            NFD_FreePath(outPath);
        }

        NFD_Quit();
        return resultPath;
    }

    // Opens a native file save dialog
    // Returns the selected file path, or std::nullopt if cancelled
    static std::optional<std::string> saveFile(const std::string& title = "Save File",
                                               const std::vector<Filter>& filters = {},
                                               const std::string& defaultPath = "", const std::string& defaultName = "")
    {
        NFD_Init();

        std::println("Invoking save file dialog with title '{}'", title);

        nfdchar_t* outPath = nullptr;
        nfdresult_t result;

        if (filters.empty())
        {
            result = NFD_SaveDialog(&outPath, nullptr, 0, defaultPath.empty() ? nullptr : defaultPath.c_str(),
                                    defaultName.empty() ? nullptr : defaultName.c_str());
        }
        else
        {
            std::vector<nfdfilteritem_t> nfdFilters;
            for (const auto& f : filters)
            {
                nfdFilters.push_back({f.name.c_str(), f.spec.c_str()});
            }
            result = NFD_SaveDialog(&outPath, nfdFilters.data(), static_cast<nfdfiltersize_t>(nfdFilters.size()),
                                    defaultPath.empty() ? nullptr : defaultPath.c_str(),
                                    defaultName.empty() ? nullptr : defaultName.c_str());
        }

        std::optional<std::string> resultPath;
        if (result == NFD_OKAY && outPath)
        {
            resultPath = std::string(outPath);
            NFD_FreePath(outPath);
        }

        NFD_Quit();
        return resultPath;
    }

    // Opens a native folder selection dialog
    // Returns the selected folder path, or std::nullopt if cancelled
    static std::optional<std::string> selectFolder(const std::string& title = "Select Folder",
                                                   const std::string& defaultPath = "")
    {
        NFD_Init();

        nfdchar_t* outPath = nullptr;
        nfdresult_t result = NFD_PickFolder(&outPath, defaultPath.empty() ? nullptr : defaultPath.c_str());

        std::optional<std::string> resultPath;
        if (result == NFD_OKAY && outPath)
        {
            resultPath = std::string(outPath);
            NFD_FreePath(outPath);
        }

        NFD_Quit();
        return resultPath;
    }
};

} // namespace Engine
