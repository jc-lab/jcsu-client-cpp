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
        const std::vector<std::string> flags;

        FileItem(const FileItem& obj) :
            file_hash_md5(obj.file_hash_md5),
            file_hash_sha256(obj.file_hash_sha256),
            file_size(obj.file_size),
            dist_path(obj.dist_path),
            url(obj.url),
            flags(obj.flags) {}

        FileItem(const std::string& _file_hash_md_5, const std::string& _file_hash_sha_256, uint64_t _file_size, const std::string& _dist_path, const std::string& _url, const std::vector<std::string> &_flags)
            : file_hash_md5(_file_hash_md_5), file_hash_sha256(_file_hash_sha_256), file_size(_file_size), dist_path(_dist_path), url(_url), flags(_flags) {}

        bool hasFlag(std::string target) const {
            int target_length = target.length();
            for (auto it = target.begin(); it != target.end(); it++) {
                *it = toupper(*it);
            }
            for (auto it = flags.cbegin(); it != flags.end(); it++) {
                if (it->length() == target_length) {
                    std::string cur_upper(*it);
                    for (auto it = cur_upper.begin(); it != cur_upper.end(); it++) {
                        *it = toupper(*it);
                    }
                    if (cur_upper == target) {
                        return true;
                    }
                }
            }
            return false;
        }
    };

}

#endif //__JCSU_FILE_ITEM_H__
