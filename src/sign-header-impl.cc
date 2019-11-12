/**
 * @file	sign-header-impl.cc
 * @author	Joseph Lee <development@jc-lab.net>
 * @date	2019/11/12
 * @copyright Copyright (C) 2019 jc-lab. All rights reserved.
 */

#include "sign-header-impl.h"

#include <openssl/x509.h>

namespace jcsu {

    std::unique_ptr<SignHeaderImpl> SignHeaderImpl::fromJws(jose::sp_jws jws) {
        std::unique_ptr<SignHeaderImpl> inst(new SignHeaderImpl());
        auto hdr_get = jws->hdr().get();
        std::list<std::string> x5c_list = hdr_get.x5c();

        inst->jws_ = jws;

        inst->alg_ = hdr_get.alg();

        inst->x5c_.reserve(x5c_list.size());

        for(auto it = x5c_list.cbegin(); it != x5c_list.cend(); it++) {
            inst->x5c_.push_back(*it);
        }

        inst->x5t_ = hdr_get.x5t();

        return std::move(inst);
    }

    bool SignHeaderImpl::verifyWithCertificate(const std::string &pem) {
        bool result = false;

        int i;
        STACK_OF(X509_INFO) *x509_info;
        BIO *bio = BIO_new_mem_buf((char *)pem.data(), pem.length());
        if(bio) {
            x509_info = PEM_X509_INFO_read_bio(bio, NULL, NULL, NULL);
            BIO_free(bio);
        }

        if(!x509_info) {
            return false;
        }

        for(i = 0; i < sk_X509_INFO_num(x509_info); i++) {
            X509_INFO *itmp = sk_X509_INFO_value(x509_info, i);
            if(itmp->x509) {
                result = verifyWithX509(itmp->x509);
                break;
            }
        }

        sk_X509_INFO_pop_free(x509_info, X509_INFO_free);

        return result;
    }

    bool SignHeaderImpl::verifyWithCertificate(const std::vector<char> &der) {
        bool result = false;
        const unsigned char *ptr = (const unsigned char*)der.data();

        X509 *cert = d2i_X509(NULL, &ptr, der.size());
        if(cert) {
            result = verifyWithX509(cert);
            X509_free(cert);
        }

        return result;
    }

    bool SignHeaderImpl::verifyWithX509(X509 *cert) {
        bool result = false;
        EVP_PKEY *pkey = X509_get_pubkey(cert);
        int pkey_type = EVP_PKEY_type(EVP_PKEY_id(pkey));

        switch(pkey_type) {
        case EVP_PKEY_RSA:
            do {
                RSA *okey = EVP_PKEY_get1_RSA(pkey);
                std::shared_ptr<RSA> spkey(okey, ::RSA_free);
                result = verifyWithRSA(spkey);
            } while(false);
        case EVP_PKEY_EC:
            do {
                EC_KEY *okey = EVP_PKEY_get1_EC_KEY(pkey);
                std::shared_ptr<EC_KEY> spkey(okey, ::EC_KEY_free);
                result = verifyWithEC(spkey);
            } while(false);
        }

        return result;
    }

    bool SignHeaderImpl::verifyWithEC(std::shared_ptr<EC_KEY> key) {
        jose::sp_ecdsa jwk = jose::ecdsa::make_shared(jose::alg::ES256, key);
        return jws_->verify(jwk);
    }
    bool SignHeaderImpl::verifyWithRSA(std::shared_ptr<RSA> key) {
        jose::sp_rsa jwk = jose::rsa::make_shared(jose::alg::RS256, key);
        return jws_->verify(jwk);
    }
}
