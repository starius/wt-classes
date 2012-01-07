/*
 * wt-classes, utility classes used by Wt applications
 * Copyright (C) 2011 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#ifndef WC_PLAIN_TEXT_WRITTER_HPP_
#define WC_PLAIN_TEXT_WRITTER_HPP_

#include <string>
#include <ostream>

namespace Wt {

namespace Wc {

/** Utility class to add line breaks to stream automatically.

\ingroup util
*/
class PlainTextWritter {
public:
    /** Constructor.
    \param out Output stream.
    \param cpl Maximum number of chars per line.
    */
    PlainTextWritter(std::ostream& out, const short cpl = 79);

    /** Destructor.
    Add the last line break.
    */
    virtual ~PlainTextWritter();

    /** Write a word.
    If current line would be too long, line break is primarily inserted.
    */
    void write_word(const std::string& word);

private:
    std::ostream& out_;
    const unsigned short cpl_;
    unsigned short current_cpl_;
};

}

}

#endif

