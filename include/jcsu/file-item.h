/**
 * @file	file-item.h
 * @author	Joseph Lee <development@jc-lab.net>
 * @date	2019/11/12
 * @copyright Copyright (C) 2019 jc-lab. All rights reserved.
 */

#ifndef __JCSU_FILE_ITEM_H__
#define __JCSU_FILE_ITEM_H__

#include <stdint.h>

#include <string>

namespace jcsu {

    struct FileItem {
        const std::string file_hash_md5;
        const std::string file_hash_sha256;
        const uint64_t file_size;
        const std::string dist_path;
        const std::string url;

        FileItem(const FileItem& obj) :
            file_hash_md5(obj.file_hash_md5),
            file_hash_sha256(obj.file_hash_sha256),
            file_size(obj.file_size),
            dist_path(obj.dist_path),
            url(obj.url) {}

        FileItem(const std::string& _file_hash_md_5, const std::string& _file_hash_sha_256, uint64_t _file_size, const std::string& _dist_path, const std::string& _url)
            : file_hash_md5(_file_hash_md_5), file_hash_sha256(_file_hash_sha_256), file_size(_file_size), dist_path(_dist_path), url(_url) {}
    };

}

#endif //__JCSU_FILE_ITEM_H__
