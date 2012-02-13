/*
 * wt-classes, utility classes used by Wt applications
 * Copyright (C) 2011 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#ifndef WC_ABSTRACT_CAPTCHA_HPP_
#define WC_ABSTRACT_CAPTCHA_HPP_

#include <Wt/WGlobal>
#include <Wt/WCompositeWidget>
#include <Wt/WValidator>

namespace Wt {

namespace Wc {

/** Abstract captcha widget.
Automatic test used to ensure that the response is generated by a person.

\ingroup protection
*/
class AbstractCaptcha : public WCompositeWidget {
public:
    /** Constructor */
    AbstractCaptcha(WContainerWidget* parent = 0);

    /** Return correctness of the key, entered by user */
    virtual WValidator::State validate() = 0;

    /** Update the widget with new secret key */
    void update();

protected:
    /** Update the widget with new secret key (implementation).
    This method should setImplementation(), or update existing one.
    */
    virtual void update_impl() = 0;
};

}

}

#endif
