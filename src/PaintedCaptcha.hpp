/*
 * wt-classes, utility classes used by Wt applications
 * Copyright (C) 2011 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#ifndef WC_PAINTED_CAPTCHA_HPP_
#define WC_PAINTED_CAPTCHA_HPP_

#include "AbstractCaptcha.hpp"

namespace Wt {

namespace Wc {

/** Captcha widget using WPaintedWidget.

\ingroup protection
*/
class PaintedCaptcha : public AbstractCaptcha {
public:
    /** Constructor */
    PaintedCaptcha(WContainerWidget* parent = 0);

    /** Return correctness of the key, entered by user.
    If user-entered key is invalid (and non-empty),
    this also calls update_impl() to prevent brute-force attacks.

    If the true key is entered, this is memorized and the widget
    is disabled for editing (see WWidget::disable()).
    */
    WValidator::State validate();

    /** Randomly created secret key.
    The purpose of user is to guess this key.
    */
    const std::string& true_key() const {
        return true_key_;
    }

    /** The key, entered by user */
    const std::string& user_key() const;

    /** Get if keys are trimmed before compare */
    bool is_compare_trimmed() const {
        return is_compare_trimmed_;
    }

    /** Set if keys are trimmed before compare.
    Defaults to true.
    */
    void set_compare_trimmed(bool compare_trimmed) {
        is_compare_trimmed_ = compare_trimmed;
    }

    /** Get if keys are lowcased before compare */
    bool is_compare_nocase() const {
        return is_compare_nocase_;
    }

    /** Set if keys are lowcased before compare.
    Defaults to true.
    */
    void set_compare_nocase(bool compare_nocase) {
        is_compare_nocase_ = compare_nocase;
    }

    /** Get secret key length */
    int key_length() const {
        return key_length_;
    }

    /** Set secret key length and update secret key.
    Defaults to 6.
    */
    void set_key_length(int key_length);

protected:
    void update_impl();

    /** Generate new random secret key */
    std::string random_key() const;

private:
    std::string true_key_;
    bool passed_: 1;
    bool is_compare_trimmed_: 1;
    bool is_compare_nocase_: 1;
    int key_length_;

    std::string prepare_key(const std::string& key) const;
};

}

}

#endif

