/**
 * @file	file-list.h
 * @author	Joseph Lee <development@jc-lab.net>
 * @date	2019/11/11
 * @copyright Copyright (C) 2019 jc-lab. All rights reserved.
 */

#ifndef __JCSU_FILE_LIST_H__
#define __JCSU_FILE_LIST_H__

#include <stdint.h>

#include <jcu-http/response.h>
#include <jcu-http/request.h>

#include "jcsu-intl/request.h"

#include "file-item.h"

namespace jcsu {

    class FileListRequest;

    class FileListResponse {
    private:
        bool error_;
        bool verify_failed_;
        int status_code_;
        uint64_t vid_;
        uint64_t version_number_;
        std::string version_display_;
        std::string signed_metadata_;
        std::vector<FileItem> file_list_;

        std::string response_;

    public:
        class WorkingContext {
        private:
            friend class FileListRequest;

            std::shared_ptr<Client> client_;

            std::promise<std::unique_ptr<FileListResponse>> promise_;

        public:
            WorkingContext(std::shared_ptr<Client> client);

            void onDone(jcu::http::Response *response);
        };

        FileListResponse(bool err, bool verify_failed, int status_code, const std::string& response);
        FileListResponse(uint64_t vid, uint64_t version_number, const std::string &version_display);

        bool hasError() const;
        bool isVerifyFailed() const;
        uint64_t getVid() const;
        uint64_t getVersionNumber() const;
        const std::string &getVersionDisplay() const;
        const std::string &getSignedMetadata() const;
        const std::string &getResponse() const;

        int getFileCount() const;
        const FileItem &getFileItem(int index) const;
    };

    class FileListRequest : public Request {
    private:
        uint64_t vid_;

    public:
        FileListRequest &withVid(uint64_t vid);
        std::future<std::unique_ptr<FileListResponse>> execute(std::shared_ptr<Client> client);
    };

}

#endif //__JCSU_FILE_LIST_H__

