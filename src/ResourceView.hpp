/*
 * wt-classes, utility classes used by Wt applications
 * Copyright (C) 2011 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#ifndef WC_RESOURCE_VIEW_HPP_
#define WC_RESOURCE_VIEW_HPP_

#include <Wt/WGlobal>

#include "StreamView.hpp"

namespace Wt {

namespace Wc {

/** View of a resource.

\ingroup util
*/
class ResourceView : public StreamView {
public:
    /** Constructor */
    ResourceView(WResource* resource, WContainerWidget* parent = 0);

    /** Constructor */
    ResourceView(WContainerWidget* parent = 0);

    /** Set resource */
    void set_resource(WResource* resource);

protected:
    WWidget* renderView();

private:
    WResource* resource_;
};

}

}

#endif

