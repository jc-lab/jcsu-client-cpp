/**
 * @file	sign-header-impl.h
 * @author	Joseph Lee <development@jc-lab.net>
 * @date	2019/11/12
 * @copyright Copyright (C) 2019 jc-lab. All rights reserved.
 */

#ifndef __SIGN_HEADER_IMPL_H__
#define __SIGN_HEADER_IMPL_H__

#include "jcsu/sign-header.h"

#include <josepp/jws.hpp>

#include <openssl/x509.h>

namespace jcsu {

    class SignHeaderImpl : public SignHeader {
    private:
        jose::sp_jws jws_;

    public:
        static std::unique_ptr<SignHeaderImpl> fromJws(jose::sp_jws jws);

        bool verifyWithCertificate(const std::string& pem) override;
        bool verifyWithCertificate(const std::vector<char>& der) override;
        bool verifyWithX509(X509 *cert);
        bool verifyWithEC(std::shared_ptr<EC_KEY> key);
        bool verifyWithRSA(std::shared_ptr<RSA> key);
    };

}

#endif //__JCSU_JCSU_INTL_SIGN_HEADER_IMPL_H__
