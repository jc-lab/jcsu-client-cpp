/**
 * @file	client-builder.h
 * @author	Joseph Lee <development@jc-lab.net>
 * @date	2019/11/11
 * @copyright Copyright (C) 2019 jc-lab. All rights reserved.
 */

#include "jcsu/client.h"

namespace jcsu {

    Client::Builder &Client::Builder::createStaticBuilder(std::shared_ptr<Client>* target) {
        Builder *builder = new Builder(target);
        builder->self_.reset(builder);
        return *builder;
    }

    std::unique_ptr<Client::Builder> Client::Builder::createBuilder() {
        std::unique_ptr<Builder> instance(new Builder());
        return std::move(instance);
    }

    Client::Builder::Builder(std::shared_ptr<Client>* target) {
        target_ = target;
    }

    Client::Builder::Builder() {
        target_ = NULL;
    }

    Client::Builder& Client::Builder::withHttpClient(std::shared_ptr<jcu::http::Client> client) {
        http_client_ = client;
        return *this;
    }

    Client::Builder& Client::Builder::withApiEndpoint(const char* url) {
        api_endpoint_ = url;
        return *this;
    }

    Client::Builder& Client::Builder::withClientHandler(ClientHandler* handler) {
        client_handler_ = handler;
        return *this;
    }

    Client::Builder& Client::Builder::withClientHandler(std::unique_ptr<ClientHandler> handler) {
        client_handler_ = std::move(handler);
        return *this;
    }

    Client::Builder& Client::Builder::withClientHandler(std::shared_ptr<ClientHandler> handler) {
        client_handler_ = handler;
        return *this;
    }

    Client* Client::Builder::_build() {
        return new Client(http_client_, api_endpoint_, std::move(client_handler_));
    }

}
