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

    std::future<std::unique_ptr<FileDownloadResponse>> FileDownloadRequest::execute(std::shared_ptr<Client> client) {
        std::shared_ptr<jcu::http::Request> req = jcu::http::HttpGet::create(file_item_->url);
        std::shared_ptr<FileDownloadResponse::WorkingContext> inst(new FileDownloadResponse::WorkingContext(std::move(file_item_), std::move(file_handle_)));
        client->prepareHttp(req)
            .withUserContext(inst)
            .onData([inst](const void *data, size_t length, jcu::http::Response *response) -> bool {
              return inst->onData(data, length);
            })
            .onDone([](jcu::http::Response *response) -> void {
                std::shared_ptr<void> user_ctx = response->getUserContext();
                FileDownloadResponse::WorkingContext *work_ctx = (FileDownloadResponse::WorkingContext*)user_ctx.get();
                int status_code = response->getStatusCode();
                work_ctx->file_handle_->close();
                work_ctx->promise_.set_value(std::unique_ptr<FileDownloadResponse>(new FileDownloadResponse(
                    std::move(work_ctx->file_handle_),
                    std::move(work_ctx->file_item_),
                    (response->getErrorCode() != jcu::http::Response::E_OK) || (status_code < 200) || (status_code >= 400) || (work_ctx->file_result_ != 0),
                    response->getStatusCode(),
                    work_ctx->file_result_
                )));
            })
            .execute();
        return std::move(inst->promise_.get_future());
    }

    FileDownloadResponse::WorkingContext::WorkingContext(std::unique_ptr<FileItem> file_item, std::unique_ptr<jcu::file::FileHandler> file_handle)
        : file_item_(std::move(file_item)), file_handle_(std::move(file_handle)), file_result_(0)
    {
    }

    bool FileDownloadResponse::WorkingContext::onData(const void *data, size_t length) {
        int rc;
        if(!file_handle_->isOpen()) {
            rc = file_handle_->open(jcu::file::MODE_WRITE | jcu::file::MODE_CREATE | jcu::file::RENAME_IF_EXISTS | jcu::file::REMOVE_IF_EXISTS | jcu::file::SHARE_READ | jcu::file::USE_TEMPNAME);
            if(rc != 0) {
                file_result_ = rc;
                return true;
            }
        }
        rc = file_handle_->write(data, length);
        if(rc < 0) {
            file_result_ = rc;
            return false;
        }
        return true;
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
