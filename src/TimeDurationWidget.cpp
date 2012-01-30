/*
 * wt-classes, utility classes used by Wt applications
 * Copyright (C) 2011 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <Wt/WContainerWidget>
#include <Wt/WComboBox>

#include "TimeDurationWidget.hpp"
#include "ConstrainedSpinBox.hpp"

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
        spin_box_ = new ConstrainedDoubleSpinBox(this);
        spin_box_->setRange(min.total_seconds(), max.total_seconds());
        spin_box_->setValue(value.total_seconds());
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
        return spin_box_->value() *
               INTERVALS[combo_box_->currentIndex()];
    }

    TimeDuration corrected_value() const {
        return spin_box_->corrected_value() *
               INTERVALS[combo_box_->currentIndex()];
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
    ConstrainedDoubleSpinBox* spin_box_;
    WComboBox* combo_box_;
    TimeDuration min_;
    TimeDuration max_;
    TimeDuration unit_;

    void select_handler_(int i) {
        TimeDuration s = spin_box_->corrected_value() * unit_;
        unit_ = INTERVALS[i];
        spin_box_->setRange(min_ / unit_, max_ / unit_);
        spin_box_->setValue(s / unit_);
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

