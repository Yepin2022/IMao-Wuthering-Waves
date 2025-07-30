#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Storage.h>
#include <fileapi.h>
#include <filesystem>
#include <string>

namespace robmikh::common::desktop
{
    inline winrt::Windows::Foundation::IAsyncOperation<winrt::Windows::Storage::StorageFile> GetStorageFileFromPathAsync(std::wstring const& path)
    {
        auto fullPath = std::filesystem::canonical(path);
        auto parentPath = fullPath.parent_path();
        auto fileName = fullPath.filename();

        auto folder = co_await winrt::Windows::Storage::StorageFolder::GetFolderFromPathAsync(parentPath.wstring());
        auto file = co_await folder.GetFileAsync(fileName.wstring());
        co_return file;
    }

    inline winrt::Windows::Foundation::IAsyncOperation<winrt::Windows::Storage::StorageFile> CreateStorageFileFromPathAsync(
        std::wstring const& path, 
        winrt::Windows::Storage::CreationCollisionOption collisionOption = winrt::Windows::Storage::CreationCollisionOption::ReplaceExisting)
    {
        // We use GetFullPathNameW here since std::filesystem::canonical assumes the file already exists.
        auto length = GetFullPathNameW(path.c_str(), 0, nullptr, nullptr);
        if (length == 0)
        {
            winrt::throw_last_error();
        }
        std::wstring fullStringPath(length + 1, L'\0');
        winrt::check_bool(GetFullPathNameW(path.c_str(), static_cast<DWORD>(fullStringPath.size()), fullStringPath.data(), nullptr));
        fullStringPath.erase(fullStringPath.find(L'\0'));
        auto fullPath = std::filesystem::path(fullStringPath);
        auto parentPath = fullPath.parent_path();
        auto fileName = fullPath.filename();

        auto folder = co_await winrt::Windows::Storage::StorageFolder::GetFolderFromPathAsync(parentPath.wstring());
        auto file = co_await folder.CreateFileAsync(fileName.wstring(), collisionOption);
        co_return file;
    }
}