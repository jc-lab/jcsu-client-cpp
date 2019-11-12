/**
 * @file	file-downloader.h
 * @author	Joseph Lee <development@jc-lab.net>
 * @date	2019/11/11
 * @copyright Copyright (C) 2019 jc-lab. All rights reserved.
 */

#ifndef __JCSU_FILE_DOWNLOAD_H__
#define __JCSU_FILE_DOWNLOAD_H__

#include <stdint.h>

#include <jcu-http/response.h>
#include <jcu-http/request.h>
#include <jcu-http/custom-handler.h>
#include <jcu-file/file-handler.h>

#include "jcsu-intl/request.h"

#include "file-item.h"

namespace jcsu {

    class FileDownloadRequest;

    class FileDownloadResponse {
    private:
        std::unique_ptr<jcu::file::FileHandler> file_handle_;
        std::unique_ptr<FileItem> file_item_;
        bool error_;
        int status_code_;
        int file_result_;

    public:
        class Future : jcu::http::CustomHandler {
        private:
            friend class FileDownloadRequest;

            std::unique_ptr<FileItem> file_item_;
            std::unique_ptr<jcu::file::FileHandler> file_handle_;

            std::shared_ptr<jcu::http::ResponseFuture> res_future_;

            int file_result_;

        public:
            Future(std::unique_ptr<FileItem> file_item, std::unique_ptr<jcu::file::FileHandler> file_handle);

            void wait() {
                res_future_->wait();
            }

            template<typename _Rep, typename _Period>
            std::future_status wait_for(const std::chrono::duration<_Rep, _Period>& __rel) {
                return res_future_->wait_for(__rel);
            }

            std::unique_ptr<FileDownloadResponse> get();

        private:
            bool onData(const void *data, size_t length, size_t *readed_bytes) override;
        };

        FileDownloadResponse(std::unique_ptr<jcu::file::FileHandler> file_handle,
                             std::unique_ptr<FileItem> file_item,
                             bool error,
                             int status_code,
                             int file_result);

        bool hasError() const {
            return error_;
        }

        jcu::file::FileHandler* getFileHandle() const {
            return file_handle_.get();
        }
        FileItem* getFileItem() const {
            return file_item_.get();
        }
        int getStatusCode() const {
            return status_code_;
        }
        int getFileResult() const {
            return file_result_;
        }

        std::unique_ptr<jcu::file::FileHandler> moveFileHandle() {
            return std::move(file_handle_);
        }
        std::unique_ptr<FileItem> moveFileItem() {
            return std::move(file_item_);
        }
    };

    class FileDownloadRequest : public Request {
    private:
        std::unique_ptr<FileItem> file_item_;
        std::unique_ptr<jcu::file::FileHandler> file_handle_;

    public:
        FileDownloadRequest &withFileHandle(std::unique_ptr<jcu::file::FileHandler> file_handle);
        FileDownloadRequest &withFilePath(const jcu::file::Path &path);
        FileDownloadRequest &withFileItem(const FileItem &file_item);
        std::unique_ptr<FileDownloadResponse::Future> execute(std::shared_ptr<Client> client);
    };

}

#endif //__JCSU_FILE_DOWNLOAD_H__
