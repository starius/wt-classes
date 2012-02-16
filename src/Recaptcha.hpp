/*
 * wt-classes, utility classes used by Wt applications
 * Copyright (C) 2011 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#ifndef WC_RECAPTCHA_HPP_
#define WC_RECAPTCHA_HPP_

#include <boost/system/error_code.hpp>

#include <Wt/WGlobal>

#include "AbstractCaptcha.hpp"

namespace Wt {

namespace Wc {

/** reCAPTCHA widget.

\ingroup protection
*/
class Recaptcha : public AbstractCaptcha {
public:
    /** Constructor */
    Recaptcha(const std::string& public_key, const std::string& private_key,
              WContainerWidget* parent = 0);

    /** Destructor */
    ~Recaptcha();

protected:
    void update_impl();
    void check_impl();

private:
    Http::Client* http_;
    std::string public_key_;
    std::string private_key_;
    WLineEdit* response_field_;
    WLineEdit* challenge_field_;

    bool js() const;
    WContainerWidget* get_impl();
    void http_done(const boost::system::error_code& e,
                   const Http::Message& message);
};

}

}

#endif

