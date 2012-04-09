/*
 * wt-classes, utility classes used by Wt applications
 * Copyright (C) 2011 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <boost/lexical_cast.hpp>

#include <Wt/WContainerWidget>
#include <Wt/WLineEdit>
#include <Wt/WDoubleValidator>
#include <Wt/WComboBox>

#include "TimeDurationWidget.hpp"
#include "ConstrainedSpinBox.hpp"
#include "util.hpp"
#include "config.hpp"

namespace Wt {

namespace Wc {

using namespace td;

const int INTERVALS_SIZE = 5;
const TimeDuration INTERVALS[5] = {SECOND, MINUTE, HOUR, DAY, WEEK};

class TimeDurationWidgetImpl : public WContainerWidget {
public:
    TimeDurationWidgetImpl(const TimeDuration& min, const TimeDuration& value,
                           const TimeDuration& max) :
        WContainerWidget(), min_(min), max_(max), unit_(SECOND) {
        setInline(true);
#if defined(WC_HAVE_WDOUBLESPINBOX)
        spin_box_ = new ConstrainedDoubleSpinBox(this);
#elif defined(WC_HAVE_WSPINBOX)
        spin_box_ = new ConstrainedSpinBox(this);
#else
        spin_box_ = new WLineEdit(this);
        spin_box_->setValidator(new WDoubleValidator(spin_box_));
        // set parent of WDoubleValidator to prevent memory leak
        // see http://redmine.emweb.be/boards/2/topics/544
#endif
        set_raw_value(min.total_seconds(), value.total_seconds(),
                      max.total_seconds());
        combo_box_ = new WComboBox(this);
        combo_box_->addItem(tr("wc.time.seconds"));
        combo_box_->addItem(tr("wc.time.minutes"));
        combo_box_->addItem(tr("wc.time.hours"));
        combo_box_->addItem(tr("wc.time.days"));
        combo_box_->addItem(tr("wc.time.weeks"));
        combo_box_->activated()
        .connect(this, &TimeDurationWidgetImpl::select_handler_);
        for (int i = INTERVALS_SIZE - 1; i >= 0; i--) {
            if (value >= INTERVALS[i]) {
                combo_box_->setCurrentIndex(i);
                select_handler_(i);
                break;
            }
        }
    }

    TimeDuration value() const {
        return raw_value() * INTERVALS[combo_box_->currentIndex()];
    }

    TimeDuration corrected_value() const {
        return constrained_value(min_, value(), max_);
    }

    TimeDuration minimum() const {
        return min_;
    }

    TimeDuration maximum() const {
        return max_;
    }

    TimeDuration unit() const {
        return unit_;
    }

    WFormWidget* form_widget() {
        return spin_box_;
    }

private:
#if defined(WC_HAVE_WDOUBLESPINBOX)
    ConstrainedDoubleSpinBox* spin_box_;
#elif defined(WC_HAVE_WSPINBOX)
    ConstrainedSpinBox* spin_box_;
#else
    WLineEdit* spin_box_;
#endif
    WComboBox* combo_box_;
    TimeDuration min_;
    TimeDuration max_;
    TimeDuration unit_;

    void select_handler_(int i) {
        TimeDuration s = raw_value() * unit_;
        unit_ = INTERVALS[i];
        set_raw_value(min_ / unit_, s / unit_, max_ / unit_);
    }

    void set_raw_value(double min, double value, double max) {
#if defined(WC_HAVE_WDOUBLESPINBOX) || defined(WC_HAVE_WSPINBOX)
        spin_box_->setRange(min, max);
        spin_box_->setValue(value);
#else
        spin_box_->setText(TO_S(value));
        DOWNCAST<WDoubleValidator*>(spin_box_->validator())->setRange(min, max);
#endif
    }

    double raw_value() const {
#if defined(WC_HAVE_WDOUBLESPINBOX) || defined(WC_HAVE_WSPINBOX)
        return spin_box_->value();
#else
        try {
            return boost::lexical_cast<double>(spin_box_->text().toUTF8());
        } catch (...) {
            return 0;
        }
#endif
    }
};

TimeDurationWidget::TimeDurationWidget(const TimeDuration& min,
                                       const TimeDuration& value,
                                       const TimeDuration& max,
                                       WContainerWidget* parent) :
    WCompositeWidget(parent) {
    impl_ = new TimeDurationWidgetImpl(min, value, max);
    setImplementation(impl_);
}

TimeDuration TimeDurationWidget::value() const {
    return impl_->value();
}

TimeDuration TimeDurationWidget::corrected_value() const {
    return impl_->corrected_value();
}

TimeDuration TimeDurationWidget::minimum() const {
    return impl_->minimum();
}

TimeDuration TimeDurationWidget::maximum() const {
    return impl_->maximum();
}

TimeDuration TimeDurationWidget::unit() const {
    return impl_->unit();
}

WFormWidget* TimeDurationWidget::form_widget() {
    return impl_->form_widget();
}

}

}

