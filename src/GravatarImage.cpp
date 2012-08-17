/*
 * wt-classes, utility classes used by Wt applications
 * Copyright (C) 2011 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include "config.hpp"

#include <sstream>
#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/case_conv.hpp>

#ifdef WC_HAVE_WLINK
#include <Wt/WLink>
#endif
#include <Wt/WApplication>
#include <Wt/WEnvironment>

#include "GravatarImage.hpp"
#include "util.hpp"

namespace Wt {

namespace Wc {

const GravatarImage::Rating GRAVATAR_DEFAULT_RATING = GravatarImage::G;
const short GRAVATAR_DEFAULT_SIZE = 80;

GravatarImage::GravatarImage(const std::string& email,
                             WContainerWidget* parent):
    WImage(parent),
    size_(GRAVATAR_DEFAULT_SIZE),
    rating_(GRAVATAR_DEFAULT_RATING),
    force_default_(false),
    secure_requests_(INHERIT) {
    set_email(email);
    resize_image(size_);
}

void GravatarImage::set_email(const std::string& email) {
    using namespace boost::algorithm;
    email_ = email;
    trim(email_);
    to_lower(email_);
    update_url();
}

void GravatarImage::set_size(const WLength& size) {
    size_ = size.toPixels();
    update_url();
    resize_image(size);
}

void GravatarImage::set_default(const std::string& url) {
    default_ = url;
    update_url();
}

#ifdef WC_HAVE_WLINK
void GravatarImage::set_default(const WLink& link) {
    set_default(link.url());
}
#endif

void GravatarImage::set_default(BuiltIn option) {
    if (option == DEFAULT) {
        default_ = "";
    } else if (option == RETURN_404) {
        default_ = "404";
    } else if (option == MM) {
        default_ = "mm";
    } else if (option == IDENTICON) {
        default_ = "identicon";
    } else if (option == MONSTERID) {
        default_ = "monsterid";
    } else if (option == WAVATAR) {
        default_ = "wavatar";
    } else if (option == RETRO) {
        default_ = "retro";
    }
    update_url();
}

void GravatarImage::set_rating(Rating rating) {
    rating_ = rating;
    update_url();
}

void GravatarImage::set_force_default(bool force_default) {
    force_default_ = force_default;
    update_url();
}

void GravatarImage::set_secure_requests(SecureRequests secure_requests) {
    secure_requests_ = secure_requests;
    update_url();
}

std::string GravatarImage::url(const std::string& email, short size,
                               bool force_default,
                               const std::string& default_url,
                               Rating rating, SecureRequests secure_requests) {
    std::stringstream url;
    if (https(secure_requests)) {
        url << "https://secure.";
    } else {
        url << "http://www.";
    }
    url << "gravatar.com/avatar/";
    url << md5(email);
    url << ".jpg";
    url << "?";
    if (size != GRAVATAR_DEFAULT_SIZE) {
        url << "s=" << TO_S(size) << "&";
    }
    if (!default_url.empty()) {
        url << "d=" << urlencode(default_url) << "&";
    }
    if (rating != GRAVATAR_DEFAULT_RATING) {
        url << "r=" << rating_str(rating) << "&";
    }
    if (force_default) {
        url << "f=y";
    }
    return url.str();
}

void GravatarImage::update_url() {
    setImageRef(url(email_, size_, force_default_, default_,
                    rating_, secure_requests_));
}

void GravatarImage::resize_image(const WLength& size) {
    resize(size, size);
}

bool GravatarImage::https(SecureRequests secure_requests) {
    return secure_requests == ALWAYS ||
           (secure_requests == INHERIT &&
            wApp && wApp->environment().urlScheme() == "https");
}

bool GravatarImage::https() const {
    return https(secure_requests_);
}

std::string GravatarImage::rating_str(Rating rating) {
    std::string result = "g";
    if (rating == PG) {
        result = "pg";
    } else if (rating == R) {
        result = "r";
    } else if (rating == X) {
        result = "x";
    }
    return result;
}

std::string GravatarImage::rating_str() const {
    return rating_str(rating_);
}

}

}

