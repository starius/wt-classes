/*
 * wt-classes, utility classes used by Wt applications
 * Copyright (C) 2011 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#ifndef WC_DOT_RESOURCE_HPP_
#define WC_DOT_RESOURCE_HPP_

#include <fstream>

#include <Wt/WFileResource>

namespace Wt {

namespace Wc {

/** Abstract resource, streaming data through an external program.
Temporary output file is created using \c system().
The command called must be available.

\ingroup util
*/
class FilterResource : public WFileResource {
public:
    /** Constructor.
    \param cmd The command to be executed.
        Should contain two positional arguments:
        {1} -- input file; {2} -- output file.
    \param parent The parent.
    */
    FilterResource(const WString& cmd, WObject* parent = 0);

    /** Destructor.
    Delete output file
    */
    ~FilterResource();

    /** Method writing input file for the external command */
    virtual void write_input(std::ostream& out) const = 0;

    /** Handles a request.
    Output file is lazily created here.
    */
    void handleRequest(const Http::Request& request, Http::Response& response);

private:
    WString cmd_;
    std::string output_file_;
};

}

}

#endif

