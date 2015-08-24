#include "file_system.hpp"
#include <sys/stat.h>
#include <cstdio>
#include <dirent.h>
#include <string>
#include <errno.h>
#include <cstdlib>
#include <unistd.h>  
#include <string.h>  
#include <iostream>
#include <sstream>

int FS::mkdir (FS::path path)
{
    std::string fullPath;
    for (FS::path::iterator it = path.begin(); it != path.end(); ++it)
    {
        fullPath.append("/");
        fullPath.append(*it);
#if defined(_WIN32)
        ::_mkdir ((fullPath).c_str());
#elif defined(__linux__)
        ::mkdir ((fullPath).c_str(), 0775);
    }
#endif
}

void FS::get_all_file_paths(const std::string& dir, int depth, std::vector<FS::path>& filePaths)  
{  
    DIR *dp;  
    struct dirent *entry;  
    struct stat statbuf;  

    if ((dp = opendir(dir.c_str())) == NULL) {  
        fprintf(stderr, "Can`t open directory %s\n", dir.c_str());  
        return ;  
    }  

    chdir(dir.c_str());  
    static FS::path filePath (dir);
    std::string preFix = dir.substr(0, 2);

    while ((entry = readdir(dp)) != NULL) {  
        lstat(entry->d_name, &statbuf);  
        if (S_ISDIR(statbuf.st_mode)) {  
            if (strcmp(entry->d_name, ".") == 0 ||   
                    strcmp(entry->d_name, "..") == 0 )    
                continue;     
            std::string name (entry->d_name);
            filePath.append (name);
            get_all_file_paths(entry->d_name, depth+4, filePaths);  
            filePath.pop ();
        } 
        else  
        {
            std::string name (entry->d_name);
            filePath.append (name);
            filePaths.push_back (filePath);
            filePath.pop ();
        }
    }  
    chdir("..");  
    closedir(dp);     
}  

int FS::cp (const std::string& src, const std::string& dest)
{
    std::ostringstream os;
    os << "cp -an ";
    os << src;
    os << " ";
    os << dest;
    printf ("%s\n", os.str().c_str());
    system (os.str().c_str());

    if (FS::check_file_consistency (src, dest) == true)
        return 0;
    else 
    {
        std::ostringstream rmFile;
        rmFile << "rm -f ";
        rmFile << dest;
        printf ("%s\n", rmFile.str().c_str());
        system (rmFile.str().c_str());
        return -1;
    }

}

bool FS::check_file_consistency (const std::string& src, const std::string& dest)
{
    int ret = -1;
    struct stat srcStat;
    ret = stat(src.c_str(), &srcStat);
    if (ret < 0)
        printf("strerror(errno) = %s, filename = %s\n", strerror(errno), src.c_str());

    struct stat destStat;
    ret = stat(dest.c_str(), &destStat);
    if (ret < 0)
        printf("strerror(errno) = %s, filename = %s\n", strerror(errno), dest.c_str());

    return (srcStat.st_size == destStat.st_size) ? true : false;
}

#if defined(_WIN32)
static std::vector<std::string> split_path (std::string path)
{
    std::string::iterator head;
    std::string::iterator tail;

    std::vector<std::string> _path;

    size_t found = 0;
    found = path.find(":");
    std::string drive;
    if (found != std::string::npos)
    {
        if (found > 0)
            drive = path[found - 1];
        path = path.substr(found + 1);
    }

    if (path == "")
        return _path;
    
    if (drive != "")
        _path.push_back (drive);
    else if (path.c_str()[0] != 92 && path.c_str()[0] != 46)
        _path.push_back (".");

    for (std::string::iterator it = path.begin(); it != path.end(); ++it)
    {
        if (it != path.end() && (*(it - 1) == 92 && *it != 92) || it == path.begin())
            head = it;

        if ((it - 1) != path.end() && (*it != 92) && ((it + 1) == path.end() || *(it + 1) == 92))
        {
            tail = it;
            std::string dir (head, tail + 1);
            _path.push_back (dir);
        }
    }
}
#else
static std::vector<std::string> split_path (std::string path)
{
    std::string::iterator head;
    std::string::iterator tail;

    std::vector<std::string> _path;

    if (path == "")
        return _path;
    
    if (path.c_str()[0] != '/' && path.c_str()[0] != 46)
        _path.push_back (".");

    for (std::string::iterator it = path.begin(); it != path.end(); ++it)
    {
        if (it != path.end() && (*(it - 1) == '/' && *it != '/') || it == path.begin())
            head = it;

        if ((it - 1) != path.end() && (*it != '/') && ((it + 1) == path.end() || *(it + 1) == '/'))
        {
            tail = it;
            std::string dir (head, tail + 1);
            _path.push_back (dir);
        }
    }
}
#endif

FS::path::path (std::string path)
{
    std::vector<std::string> _path = split_path (path);
    _path.swap (mPathVct);
}

void FS::path::append (std::string dir)
{
    mPathVct.push_back (dir);
}

std::string FS::path::pop ()
{
    std::string back;
    if (mPathVct.size() != 0)
    {
        back = mPathVct.back();
        mPathVct.pop_back ();
    }
    return back;
}

#if defined(_WIN32)
int FS::path::get_full_path (std::string& path)
{
    std::string _path;
    int index = 0;
    for (std::vector<std::string>::iterator it = mPathVct.begin(); it != mPathVct.end(); ++it)
    {
        if (index == 0 && (*it) != ".")
        {
            _path = _path + *it + ":\\";
        }
        else
            _path = _path + *it + "\\";
        index ++;
    }
    path.swap (_path);
    return 0;
}
#else
int FS::path::get_full_path (std::string& path)
{
    std::string _path;
    for (std::vector<std::string>::iterator it = mPathVct.begin(); it != mPathVct.end(); ++it)
    {
        if (*it == ".")
            _path += ".";
        else
            _path += "/" + *it;
    }
    path.swap (_path);
    return 0;
}
#endif
