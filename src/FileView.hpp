/*
 * wt-classes, utility classes used by Wt applications
 * Copyright (C) 2011 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#ifndef WC_FILE_VIEW_HPP_
#define WC_FILE_VIEW_HPP_

#include "StreamView.hpp"

namespace Wt {

namespace Wc {

/** View of a file.

\ingroup util
*/
class FileView : public StreamView {
public:
    /** Constructor */
    FileView(const std::string& filename, WContainerWidget* parent = 0);

    /** Constructor */
    FileView(WContainerWidget* parent = 0);

    /** Set input file */
    void set_filename(const std::string& filename);

protected:
    WWidget* renderView();

private:
    std::string filename_;
};

}

}

#endif

