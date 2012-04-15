/*
 * wt-classes, utility classes used by Wt applications
 * Copyright (C) 2011 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <boost/array.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/case_conv.hpp>
#include <boost/algorithm/string/replace.hpp>

#include <Wt/WContainerWidget>
#include <Wt/WRasterImage>
#include <Wt/WImage>
#include <Wt/WPainter>
#include <Wt/WLineEdit>
#include <Wt/WPushButton>

#include "config.hpp"
#include "global.hpp"

#ifndef WC_HAVE_WCOMPOSITEWIDGET_IMPLEMENTATION
// FIXME nasty public morozov
#define private friend class Wt::Wc::PaintedCaptcha; private
#include <Wt/WCompositeWidget>
#undef private
#define implementation() Wt::WCompositeWidget::impl_
#endif // WC_HAVE_WCOMPOSITEWIDGET_IMPLEMENTATION

#include "PaintedCaptcha.hpp"
#include "rand.hpp"
#include "util.hpp"

namespace Wt {

namespace Wc {

const int PAINTED_CAPTCHA_WIDTH = 100;
const int PAINTED_CAPTCHA_HEIGHT = 50;
const int PAINTED_CAPTCHA_MIN_SIZE = 10;
const int PAINTED_CAPTCHA_MAX_SIZE = 25;

const boost::array<WFont::Style, 3> STYLES =
{{ WFont::NormalStyle, WFont::Italic, WFont::Oblique }};

const boost::array<WFont::Variant, 2> VARIANTS =
{{ WFont::NormalVariant, WFont::SmallCaps }};

const boost::array<WFont::GenericFamily, 2> FAMILIES =
{{ WFont::SansSerif, WFont::Cursive }};

WFont random_font() {
    WFont font;
    font.setStyle(STYLES[rr(STYLES.size())]); // FIXME
    font.setVariant(VARIANTS[rr(VARIANTS.size())]); // FIXME
    font.setFamily(FAMILIES[rr(FAMILIES.size())]); // FIXME
    font.setSize(drr(PAINTED_CAPTCHA_MIN_SIZE, PAINTED_CAPTCHA_MAX_SIZE));
    font.setWeight(WFont::Value, rr(100, 900));
    return font;
}

class PaintedCaptcha::Impl : public WContainerWidget {
public:
    Impl(PaintedCaptcha* captcha):
        captcha_(captcha),
        raster_image_("png", PAINTED_CAPTCHA_WIDTH, PAINTED_CAPTCHA_HEIGHT),
        image_(&raster_image_, /* altText */ "", this),
        update_(0) {
        edit_ = new WLineEdit(this);
        image_.setInline(false);
        image_.resize(PAINTED_CAPTCHA_WIDTH, PAINTED_CAPTCHA_HEIGHT);
        set_buttons(true);
    }

    void set_key(const std::string& key) {
        raster_image_.clear();
        WPainter painter(&raster_image_);
        painter.setFont(random_font());
        const double ANGLE = 15;
        const double SCALE = 0.1;
        const int BORDERS = 2;
        double letter_width = PAINTED_CAPTCHA_WIDTH / (key.size() + BORDERS);
        double letter_height = painter.font().fixedSize().toPixels();
        double x = 0;
        for (int i = 0; i < key.size(); i++) {
            const double X_RANDOM = 0.2;
            x += letter_width * drr(1 - X_RANDOM, 1 + X_RANDOM);
            double y = drr(0, letter_height);
            painter.translate(x, y);
            painter.rotate(drr(-ANGLE, ANGLE));
            painter.scale(drr(1 - SCALE, 1 + SCALE), drr(1 - SCALE, 1 + SCALE));
            painter.drawText(painter.window(), 0, key.substr(i, 1));
            painter.resetTransform();
        }
        raster_image_.WResource::setChanged();
    }

    std::string user_key() const {
        return value_text(edit_).toUTF8();
    }

    void set_buttons(bool enabled) {
        if (!enabled && update_) {
            removeWidget(update_);
            delete update_;
            update_ = 0;
        } else if (enabled && !update_) {
            update_ = new WPushButton(tr("wc.common.Update"), this);
            update_->clicked().connect(captcha_, &AbstractCaptcha::update);
        }
    }

    void set_input(WFormWidget* input) {
        if (edit_->parent() == this) {
            removeWidget(edit_);
            delete edit_;
        }
        edit_ = input;
    }

private:
    PaintedCaptcha* captcha_;
    WRasterImage raster_image_;
    WImage image_;
    WFormWidget* edit_;
    WPushButton* update_;
};

PaintedCaptcha::PaintedCaptcha(WContainerWidget* parent):
    AbstractCaptcha(parent),
    is_compare_trimmed_(true),
    is_compare_nocase_(true),
    key_length_(6) {
    update();
}

std::string PaintedCaptcha::user_key() const {
    PaintedCaptcha* t = const_cast<PaintedCaptcha*>(this);
    Impl* impl = 0;
    if (t->implementation()) {
        impl = t->get_impl();
    }
    return impl ? impl->user_key() : "";
}

void PaintedCaptcha::set_key_length(int key_length) {
    key_length_ = key_length;
    update();
}

void PaintedCaptcha::set_buttons(bool enabled) {
    get_impl()->set_buttons(enabled);
}

void PaintedCaptcha::set_input(WFormWidget* input) {
    get_impl()->set_input(input);
}

void PaintedCaptcha::update_impl() {
    true_key_ = random_key();
    if (!implementation()) {
        setImplementation(new Impl(this));
    }
    get_impl()->set_key(true_key());
}

void PaintedCaptcha::check_impl() {
    if (prepare_key(user_key()) == prepare_key(true_key())) {
        solve();
    } else {
        mistake(tr("wc.captcha.Wrong_response"));
    }
}

std::string PaintedCaptcha::random_key() const {
    std::string result =  rand_string(key_length());
    boost::replace_all(result, "l", "L");
    boost::replace_all(result, "1", "L");
    boost::replace_all(result, "o", "p");
    boost::replace_all(result, "O", "E");
    boost::replace_all(result, "0", "5");
    return result;
}

std::string PaintedCaptcha::prepare_key(const std::string& key) const {
    using namespace boost::algorithm;
    std::string result = key;
    if (is_compare_trimmed_) {
        trim(result);
    }
    if (is_compare_nocase_) {
        to_lower(result);
    }
    return result;
}

PaintedCaptcha::Impl* PaintedCaptcha::get_impl() {
    return DOWNCAST<Impl*>(implementation());
}

}

}

