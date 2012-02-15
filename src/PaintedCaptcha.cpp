/*
 * wt-classes, utility classes used by Wt applications
 * Copyright (C) 2011 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <boost/array.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/case_conv.hpp>

#include <Wt/WContainerWidget>
#include <Wt/WRasterImage>
#include <Wt/WImage>
#include <Wt/WPainter>
#include <Wt/WLineEdit>
#include <Wt/WRandom>

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
    Impl():
        raster_image_("png", PAINTED_CAPTCHA_WIDTH, PAINTED_CAPTCHA_HEIGHT),
        image_(&raster_image_, this),
        edit_(this) {
        edit_.setInline(false);
    }

    void set_key(const std::string& key) {
        raster_image_.clear();
        WPainter painter(&raster_image_);
        painter.setFont(random_font());
        painter.drawText(0, 0, PAINTED_CAPTCHA_WIDTH, PAINTED_CAPTCHA_HEIGHT,
                         AlignCenter | AlignMiddle, key);
        raster_image_.WResource::setChanged();
    }

    std::string user_key() const {
        return edit_.text().toUTF8();
    }

private:
    WRasterImage raster_image_;
    WImage image_;
    WLineEdit edit_;
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

void PaintedCaptcha::update_impl() {
    true_key_ = random_key();
    if (!implementation()) {
        setImplementation(new Impl());
    }
    get_impl()->set_key(true_key());
}

void PaintedCaptcha::check_impl() {
    if (prepare_key(user_key()) == prepare_key(true_key())) {
        solve();
    } else {
        update();
    }
}

std::string PaintedCaptcha::random_key() const {
    return WRandom::generateId(key_length());
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
    return downcast<Impl*>(implementation());
}

}

}

