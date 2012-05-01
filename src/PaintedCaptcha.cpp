/*
 * wt-classes, utility classes used by Wt applications
 * Copyright (C) 2011 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <vector>
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
#include "util.hpp"

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

const int WIDTH = 100;
const int HEIGHT = 50;
const int MIN_SIZE = 17;
const int MAX_SIZE = 25;

const boost::array<WFont::Style, 3> STYLES =
{{ WFont::NormalStyle, WFont::Italic, WFont::Oblique }};

const boost::array<WFont::Variant, 2> VARIANTS =
{{ WFont::NormalVariant, WFont::SmallCaps }};

const boost::array<WFont::GenericFamily, 2> FAMILIES =
{{ WFont::SansSerif, WFont::Cursive }};

static WFont random_font() {
    WFont font;
    font.setStyle(STYLES[rr(STYLES.size())]); // FIXME
    font.setVariant(VARIANTS[rr(VARIANTS.size())]); // FIXME
    font.setFamily(FAMILIES[rr(FAMILIES.size())]); // FIXME
#ifdef WC_HAVE_WFONT_SETSIZE_DOUBLE
    font.setSize(drr(MIN_SIZE, MAX_SIZE));
#endif
    font.setWeight(WFont::Value, rr(100, 900));
    return font;
}

class PaintedCaptcha::Impl : public WContainerWidget {
public:
    Impl(PaintedCaptcha* captcha):
        captcha_(captcha),
        raster_image_("png", WIDTH, HEIGHT),
        image_(&raster_image_, /* altText */ "", this),
        update_(0),
        background_(white),
        foreground_(black) {
        edit_ = new WLineEdit(this);
        image_.setInline(false);
        image_.resize(WIDTH, HEIGHT);
        set_buttons(true);
    }

    void set_key(const std::string& key) {
        WPainter painter(&raster_image_);
        painter.fillRect(painter.window(), background_);
        painter.end();
        painter.begin(&raster_image_);
        painter.setPen(foreground_);
        painter.setFont(random_font());
        const double ANGLE = 5;
        const double SCALE = 0.1;
        const int BORDERS = 3;
        double letter_width = WIDTH / (key.size() + BORDERS);
        double letter_height = 2 * letter_width;
        double x = rr(letter_width);
        double y = drr(0, HEIGHT - letter_height);
        std::vector<double> Xs(key.size()), Ys(key.size());
        for (int i = 0; i < key.size(); i++) {
            x += drr(0.8, 1.5) * letter_width;
            y += drr(-letter_height / 2, letter_height / 2);
            y = constrained_value(0, y, HEIGHT - letter_height);
            Xs[i] = x + drr(0, 1) * letter_width;
            Ys[i] = y + drr(0, 1) * letter_height;
            painter.translate(x, y);
            painter.rotate(drr(-ANGLE, ANGLE));
            painter.scale(drr(1 - SCALE, 1 + SCALE), drr(1 - SCALE, 1 + SCALE));
            painter.drawText(painter.window(), 0, key.substr(i, 1));
            painter.resetTransform();
        }
        int middle = Xs.size() / 2;
        curve(painter, Xs.front(), Ys.front(), Xs[middle], Ys[middle],
              Xs.back(), Ys.back());
        raster_image_.WResource::setChanged();
    }

    std::string user_key() const {
        return value_text(edit_).toUTF8();
    }

    void curve(WPainter& painter, double x1, double y1, double x2, double y2,
               double x3, double y3) {
        WPen pen(foreground_);
        pen.setWidth(2);
        WPainterPath path(WPointF(x1, y1));
        path.quadTo(x2, y2, x3, y3);
        painter.strokePath(path, pen);
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
    WColor background_;
    WColor foreground_;
};

PaintedCaptcha::PaintedCaptcha(WContainerWidget* parent):
    AbstractCaptcha(parent),
    is_compare_trimmed_(true),
    is_compare_nocase_(true),
    key_length_(5) {
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

