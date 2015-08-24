#include "photo.hpp"
#include <errno.h>
#include <time.h>
#include <cstdio>
#include <string.h>
#include <cstdlib>

Photo::Photo (const std::string& filePath) : mFilePath (filePath)
{
    int ret = 0;
    ret = stat(filePath.c_str(), &mFileStat);
    if (ret < 0)
        printf("strerror(errno) = %s, filename = %s\n", strerror(errno), filePath.c_str());
}

bool Photo::get_absolute_path (std::string& filePath) const
{
    return false;
}

bool Photo::get_name (std::string& fileName) const
{
    return false;
}

bool Photo::get_type (Photo::FileType& type) const
{

    unsigned found = mFilePath.find_last_of(".");
    std::string extName = mFilePath.substr(found + 1);

    if (extName == "jpg" ||
            extName == "JPG" ||
            extName == "jpeg")
        type = Photo::JPEG;
    else if (extName == "png" ||
            extName == "PNG")
        type = Photo::PNG;
    else if (extName == "mp4" ||
        extName == "MP4" ||
        extName == "AVI" ||
        extName == "avi" ||
        extName == "MOV" ||
        extName == "mov" ||
        extName == "MTS" ||
        extName == "mts")
        type = Photo::VIDEO;
    else
        return false;

    return true;
}

bool Photo::get_size (int& fileSize) const
{
    fileSize = mFileStat.st_size;
    return true;
}

bool Photo::get_original_size (size_t& fileSize) const
{
    return false;
}

bool Photo::get_datetime (int& year, int& month, int& day, int& hour, int& min, int& sec) const
{
    time_t fileTime;
    fileTime = mFileStat.st_mtime;

    struct tm *fileTm;
    fileTm = localtime(&fileTime); 

    year = fileTm->tm_year + 1900;
    month = fileTm->tm_mon + 1;
    day = fileTm->tm_mday;
    hour = fileTm->tm_hour;
    min = fileTm->tm_min;
    sec = fileTm->tm_sec;

    return true;
}
