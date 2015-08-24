#ifndef PHOTO_HEADER
#define PHOTO_HEADER
#include <string>
#include <sys/stat.h>
#include <unistd.h>
class Photo
{
public:
    enum FileType 
    {
        JPEG,
        PNG,
        VIDEO,
    };
    Photo (const std::string& filePath);
    bool get_absolute_path (std::string& filePath) const;
    bool get_name (std::string& fileName) const;
    bool get_type (Photo::FileType& type) const;
    bool get_size (int& fileSize) const;
    bool get_original_size (size_t& fileSize) const;
    bool get_datetime (int& year, int& month, int& day, int& hour, int& min, int& sec) const;
private:
    struct stat mFileStat;
    std::string mFilePath;
};
#endif //PHOTO_HEADER
