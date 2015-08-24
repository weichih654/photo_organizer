#ifndef FILE_SYSTEM_HEADER
#define FILE_SYSTEM_HEADER
#include <string>
#include <vector>
class FS
{
public:
    class path
    {
    public:
        path (std::string path);
        void append (std::string dir);
        std::string pop ();
        int get_full_path (std::string& path);
        typedef std::vector<std::string>::iterator iterator;
        typedef const std::vector<std::string>::iterator const_iterator;
        iterator begin() { return mPathVct.begin(); }
        iterator end() { return mPathVct.end(); }

    private:
        std::vector<std::string> mPathVct;
    };
public:
    static int mkdir (FS::path path);
    static void get_all_file_paths(const std::string& dir, int depth, std::vector<FS::path>& filePaths); 
    static int cp (const std::string& src, const std::string& dest);
    static bool check_file_consistency (const std::string& src, const std::string& dest);
};
#endif //FILE_SYSTEM_HEADER
