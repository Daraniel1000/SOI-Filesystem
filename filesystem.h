#ifndef SOI_FILESYSTEM
#define SOI_FILESYSTEM
#include<iostream>
#include<fstream>
#include<stdexcept>
#include<string>
#include<iomanip>
#include<vector>
#include"boost/filesystem.hpp"
#include "boost/program_options.hpp"

class filesystem
{
private:
    static const unsigned BLOCK_SIZE = 1024; // 1kB
    static const unsigned SYSTEM_BLOCKS = 4;

    struct inode
    {
        bool used = true;
        // 3 bajty zarezerwowane dla wyrownania
        unsigned begin;
        unsigned end() { return begin + blocks; }
        unsigned size;
        unsigned blocks;
        char name[48];
    }; // sizeof(inode) = 64

    std::vector<inode> inodes;
    typedef char buffer [BLOCK_SIZE];
    std::string _name;
    unsigned _size;
    unsigned total_blocks_used;

    unsigned alloc(unsigned);
    void close();
    inode* find_inode_by_name(std::string);
    static int cmp_inodes(inode, inode);
    void clearmem(unsigned, unsigned);
public:
    filesystem(std::string);
    filesystem(std::string, size_t);
    ~filesystem();
    void create_fs_file(unsigned);
    void open_fs_file();
    void upload_file(std::string);
    void download_file(std::string);
    void remove_file(std::string);
    void defragment();
    void list_files();
    void map();
    void open();
    std::string get_fs_name()
    {
        return _name;
    }
};

#endif // SOI_FILESYSTEM
