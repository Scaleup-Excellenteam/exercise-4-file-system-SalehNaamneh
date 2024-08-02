#include "myfs.h"
#include <string.h>
#include <iostream>
#include <sstream>
#include <vector>

const char *MyFs::MYFS_MAGIC = "MYFS";

MyFs::MyFs(BlockDeviceSimulator *blkdevsim_):blkdevsim(blkdevsim_) {
    myfs_header header;
    blkdevsim->read(0, sizeof(header), (char *)&header);

    if (strncmp(header.magic, MYFS_MAGIC, sizeof(header.magic)) != 0 ||
        (header.version != CURR_VERSION)) {
        std::cout << "Did not find myfs instance on blkdev" << std::endl;
        std::cout << "Creating..." << std::endl;
        format();
        std::cout << "Finished!" << std::endl;
    }
}

void MyFs::format() {
    // put the header in place
    myfs_header header;
    strncpy(header.magic, MYFS_MAGIC, sizeof(header.magic));
    header.version = CURR_VERSION;
    blkdevsim->write(0, sizeof(header), (const char*)&header);

    // Initialize root directory
    myfs_directory_entry root;
    root.inode_number = 0; // Root inode
    root.is_directory = true;
    strncpy(root.name, "/", sizeof(root.name));
    blkdevsim->write(sizeof(header), sizeof(root), (const char*)&root);

    // Initialize the inode for root directory
    myfs_inode root_inode;
    root_inode.size = sizeof(myfs_directory_entry);
    root_inode.is_directory = true;
    blkdevsim->write(sizeof(header) + sizeof(root), sizeof(root_inode), (const char*)&root_inode);
}

void MyFs::create_file(std::string path_str, bool directory) {
    myfs_directory_entry new_entry;
    new_entry.inode_number = get_next_inode();
    new_entry.is_directory = directory;
    strncpy(new_entry.name, path_str.c_str(), sizeof(new_entry.name));
    blkdevsim->write(sizeof(myfs_header) + new_entry.inode_number * sizeof(new_entry), sizeof(new_entry), (const char*)&new_entry);

    // Initialize the inode for new file/directory
    myfs_inode new_inode;
    new_inode.size = 0;
    new_inode.is_directory = directory;
    blkdevsim->write(sizeof(myfs_header) + 1024 + new_entry.inode_number * sizeof(new_inode), sizeof(new_inode), (const char*)&new_inode);
}

std::string MyFs::get_content(std::string path_str) {
    myfs_directory_entry entry = find_entry(path_str);
    if (entry.is_directory) {
        throw std::runtime_error("Path is a directory, not a file");
    }

    myfs_inode inode;
    blkdevsim->read(sizeof(myfs_header) + 1024 + entry.inode_number * sizeof(inode), sizeof(inode), (char*)&inode);

    char *content = new char[inode.size + 1];
    blkdevsim->read(sizeof(myfs_header) + 2048 + entry.inode_number * 1024, inode.size, content);
    content[inode.size] = '\0';
    std::string result(content);
    delete[] content;
    return result;
}

void MyFs::set_content(std::string path_str, std::string content) {
    myfs_directory_entry entry = find_entry(path_str);
    if (entry.is_directory) {
        throw std::runtime_error("Path is a directory, not a file");
    }

    myfs_inode inode;
    blkdevsim->read(sizeof(myfs_header) + 1024 + entry.inode_number * sizeof(inode), sizeof(inode), (char*)&inode);
    inode.size = content.size();
    blkdevsim->write(sizeof(myfs_header) + 1024 + entry.inode_number * sizeof(inode), sizeof(inode), (const char*)&inode);
    blkdevsim->write(sizeof(myfs_header) + 2048 + entry.inode_number * 1024, content.size(), content.c_str());
}

void MyFs::list_dir(std::string path_str) {
    myfs_directory_entry entry;
    blkdevsim->read(sizeof(myfs_header), sizeof(entry), (char*)&entry);

    if (!entry.is_directory) {
        throw std::runtime_error("Path is not a directory");
    }

    for (int i = 0; i < get_next_inode(); ++i) {
        myfs_directory_entry dir_entry;
        blkdevsim->read(sizeof(myfs_header) + i * sizeof(dir_entry), sizeof(dir_entry), (char*)&dir_entry);
        if (dir_entry.name[0] != '\0' && !std::string(dir_entry.name).empty()) {
            std::cout << dir_entry.name << " " << dir_entry.inode_number << std::endl;
        }
    }
}

MyFs::myfs_directory_entry MyFs::find_entry(std::string path_str) {
    for (int i = 0; i < get_next_inode(); ++i) {
        myfs_directory_entry dir_entry;
        blkdevsim->read(sizeof(myfs_header) + i * sizeof(dir_entry), sizeof(dir_entry), (char*)&dir_entry);
        if (std::string(dir_entry.name) == path_str) {
            return dir_entry;
        }
    }
    throw std::runtime_error("File not found");
}

int MyFs::get_next_inode() {
    int inode_count = 0;
    for (size_t i = 0; i < 1024 / sizeof(myfs_directory_entry); ++i) {
        myfs_directory_entry dir_entry;
        blkdevsim->read(sizeof(myfs_header) + i * sizeof(dir_entry), sizeof(dir_entry), (char*)&dir_entry);
        if (dir_entry.name[0] != '\0') {
            ++inode_count;
        }
    }
    return inode_count;
}
