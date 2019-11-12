/**
 * @file	file-download.cc
 * @author	Joseph Lee <development@jc-lab.net>
 * @date	2019/11/11
 * @copyright Copyright (C) 2019 jc-lab. All rights reserved.
 */

#include "jcsu/file-download.h"
#include "jcsu/client.h"

#include <jcu-http/http-get.h>

#include <jcu-file/file-factory.h>

namespace jcsu {

    FileDownloadRequest & FileDownloadRequest::withFileHandle(std::unique_ptr<jcu::file::FileHandler> file_handle) {
        file_handle_ = std::move(file_handle);
        return *this;
    }

    FileDownloadRequest & FileDownloadRequest::withFilePath(const jcu::file::Path &path) {
        file_handle_ = jcu::file::fs()->createFileHandle(path);
        return *this;
    }

    FileDownloadRequest & FileDownloadRequest::withFileItem(const jcsu::FileItem &file_item) {
        file_item_.reset(new FileItem(file_item));
        return *this;
    }

    std::unique_ptr<FileDownloadResponse::Future> FileDownloadRequest::execute(std::shared_ptr<Client> client) {
        std::unique_ptr<jcu::http::Request> req = jcu::http::HttpGet::create(file_item_->url);
        std::unique_ptr<FileDownloadResponse::Future> inst(new FileDownloadResponse::Future(std::move(file_item_), std::move(file_handle_)));
        inst->res_future_ = client->executeHttp(std::move(req), inst.get());
        return std::move(inst);
    }

    FileDownloadResponse::Future::Future(std::unique_ptr<FileItem> file_item, std::unique_ptr<jcu::file::FileHandler> file_handle)
        : file_item_(std::move(file_item)), file_handle_(std::move(file_handle)), file_result_(0)
    {
    }

    bool FileDownloadResponse::Future::onData(const void *data, size_t length, size_t *readed_bytes) {
        int rc;
        if(!file_handle_->isOpen()) {
            rc = file_handle_->open(jcu::file::MODE_WRITE | jcu::file::MODE_CREATE | jcu::file::RENAME_IF_EXISTS | jcu::file::REMOVE_IF_EXISTS | jcu::file::SHARE_READ | jcu::file::USE_TEMPNAME);
            if(rc != 0) {
                file_result_ = rc;
                return true;
            }
        }
        rc = file_handle_->write(data, length);
        if(rc > 0) {
            *readed_bytes = rc;
        }else{
            file_result_ = rc;
        }
        return true;
    }

    std::unique_ptr<FileDownloadResponse> FileDownloadResponse::Future::get() {
        std::unique_ptr<jcu::http::Response> response(res_future_->get());
        int status_code = response->getStatusCode();
        file_handle_->close();
        return std::unique_ptr<FileDownloadResponse>(new FileDownloadResponse(
            std::move(file_handle_),
            std::move(file_item_),
            (response->getErrorCode() != jcu::http::Response::E_OK) || (status_code < 200) || (status_code >= 400) || (file_result_ != 0),
            response->getStatusCode(),
            file_result_
        ));
    }

    FileDownloadResponse::FileDownloadResponse(std::unique_ptr<jcu::file::FileHandler> file_handle,
                                               std::unique_ptr<FileItem> file_item,
                                               bool error,
                                               int status_code,
                                               int file_result)
        : file_handle_(std::move(file_handle)),
          file_item_(std::move(file_item)),
          error_(error),
          status_code_(status_code),
          file_result_(file_result) {}
}
