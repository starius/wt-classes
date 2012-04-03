/*
 * wt-classes, utility classes used by Wt applications
 * Copyright (C) 2011 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#ifndef WC_STREAM_VIEW_HPP_
#define WC_STREAM_VIEW_HPP_

#include <iosfwd>

#include <Wt/WGlobal>
#include <Wt/WViewWidget>

namespace Wt {

namespace Wc {

/** View of a stream.

\ingroup util
*/
class StreamView : public WViewWidget {
public:
    /** Constructor */
    StreamView(std::istream* input_stream, WContainerWidget* parent = 0);

    /** Constructor.
    \note Input stream must be \ref set_stream "set" before renderView() runs
    */
    StreamView(WContainerWidget* parent = 0);

    /** Set encoding of string to WString conversion.
    Defaults to UTF8.
    */
    void set_encoding(CharEncoding encoding) {
        encoding_ = encoding;
    }

    /** Set the text format */
    void set_format(TextFormat format) {
        format_ = format;
    }

    /** Configure word wrapping */
    void set_word_wrap(bool word_wrap) {
        word_wrap_ = word_wrap;
    }

    /** Set input stream.
    \note Input stream must be \ref set_stream "set" before renderView() runs.
    */
    void set_stream(std::istream* input_stream);

protected:
    /** Return WText with contents of resource.
    \see create_text
    */
    WWidget* renderView();

    /** Return WText with contents of resource.
    Read input stream and return WText.
    If input_stream is not set, this method returns empty WText.
    */
    WText* create_text(std::istream* input_stream) const;

private:
    std::istream* input_stream_;
    CharEncoding encoding_ : 8;
    TextFormat format_ : 8;
    bool word_wrap_ : 1;
};

}

}

#endif

