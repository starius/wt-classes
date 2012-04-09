/*
 * wt-classes, utility classes used by Wt applications
 * Copyright (C) 2011 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#ifndef WC_GRAVATAR_IMAGE_HPP_
#define WC_GRAVATAR_IMAGE_HPP_

#include <Wt/WGlobal>
#include <Wt/WImage>

#include "config.hpp"

namespace Wt {

namespace Wc {

/** Show an avatar from Gravatar.

For more information, see http://gravatar.com/site/implement/images/

\ingroup bindings
*/
class GravatarImage : public WImage {
public:
    /** If an image is appropriate for a certain audience.
    The image is selt-rated by a user uploaded it to Gravatar.

    The definitions were copied from Gravatar documentation.
    */
    enum Rating {
        G,  /**< Suitable for display on all websites with any audience type.
                 The default rating */
        PG, /**< May contain rude gestures, provocatively dressed individuals,
                 the lesser swear words, or mild violencea */
        R,  /**< May contain such things as harsh profanity, intense violence,
                 nudity, or hard drug use */
        X   /**< May contain hardcore sexual imagery or
                 extremely disturbing violence */
    };

    /** Built in options for default avatar.

    Some definitions were copied from Gravatar documentation.
    */
    enum BuiltIn {
        DEFAULT,    /**< Standard default image */
        RETURN_404, /**< Do not load any image if none is associated
                         with the email hash, instead return an HTTP 404
                         (File Not Found) response */
        MM,         /**< (mystery-man) a simple, cartoon-style silhouetted
                          outline of a person (does not vary by email hash) */
        IDENTICON,  /**< A geometric pattern based on an email hash */
        MONSTERID,  /**< A generated 'monster' with different
                         colors, faces, etc */
        WAVATAR,    /**< Generated faces with differing
                         features and backgrounds */
        RETRO,      /**< Awesome generated,
                         8-bit arcade-style pixelated faces */
    };

    /** Whether to use secure requests (https) to Gravatar */
    enum SecureRequests {
        INHERIT, /**< Use the same scheme, as the Wt application does.
                     The default variant.
                     \see WEnvironment::urlScheme() */
        ALWAYS,  /**< Always use https */
        NEVER    /**< Always use http */
    };

    /** Constructor.
    \param email  E-mail address of the user (may be empty or incorrect).
    \param parent Parent widget.
    */
    GravatarImage(const std::string& email, WContainerWidget* parent = 0);

    /** Set e-mail address of the user */
    void set_email(const std::string& email);

    /** Set image height and width.
    The size may be anywhere from 1px up to 512px.
    Defaultss to 80px.
    */
    void set_size(const WLength& size);

    /** Set default image.
    By default, Gravatar returns <a href=http://www.gravatar.com/avatar/>
    this image</a> if an email address has no matching Gravatar image.

    You can use this method to change it to other default image.
    */
    void set_default(const std::string& url);

#ifdef WC_HAVE_WLINK
    /** Set default image.
    \see set_default(std::string)
    */
    void set_default(const WLink& link);
#endif

    /** Set default built-in image.

    \see set_default(const WLink&)
    */
    void set_default(BuiltIn option);

    /** Set rating */
    void set_rating(Rating rating);

    /** Set whether the default image should be returned for any e-mail */
    void set_force_default(bool force_default);

    /** Set http/https selection rules */
    void set_secure_requests(SecureRequests secure_requests);

private:
    std::string email_;
    short size_;
    std::string default_;
    Rating rating_;
    bool force_default_;
    SecureRequests secure_requests_;

    void update_url();
    void resize_image(const WLength& size);
    bool https() const;
    std::string rating_str() const;
};

}

}

#endif

