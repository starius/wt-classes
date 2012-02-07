/*
 * wt-classes, utility classes used by Wt applications
 * Copyright (C) 2011 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#ifndef WT_URL_HPP_
#define WT_URL_HPP_

#include <string>
#include <ostream>

#include <Wt/WGlobal>
#include <Wt/WObject>
#include <Wt/WSignal>
#include <Wt/WLink>

#include "global.hpp"

namespace Wt {

namespace Wc {

/** \defgroup url URL-related
Classes for URL rules.
This classes allow you to keep all url-related code in one place.

Example:
\code
Parser* parser = new Parser(wApp);
wApp->internalPathChanged().connect(parser, &Parser::open);
PredefinedNode* users = new PredefinedNode("user", parser);
users->opened().connect(...);        // show user list for "/user/"
IntegerNode* user_profile = new IntegerNode(users);
user_profile->opened().connect(...); // show user profile for "/user/xxx/"
\endcode
*/

/** Namespace for dealing with internal paths */
namespace url {

/** Part of URL.
This class represents a node in url parsing tree.

For example, for internal path \c "/user/1/", there are two nodes:
 - \c "user", is represented with PredefinedNode;
 - \c "1", is represented with IntegerNode.

\ingroup url
*/
class Node : public WObject {
public:
    /** Constructor.
    \param parent Node or Parser.
    */
    Node(WObject* parent = 0);

    /** Return if this part of an url meets the node */
    virtual bool meet(const std::string& part) const = 0;

    /** Set value.
    If check is true, the value is checked using meet() before setting.
    */
    void set_value(const std::string& v, bool check = false);

    /** Get value */
    const std::string& value() const {
        return value_;
    }

    /** Add this node to output stream.
    Add representation of this node and '/' to the stream.
    */
    void write_to(std::ostream& path) const;

    /** Write all parents and this node to the stream */
    void write_all_to(std::ostream& path) const;

    /** Return full internal path to this node */
    std::string full_path() const;

    /** Return a link to this node */
    WLink link() const;

    /** Return parent of type Node or 0 */
    Node* node_parent() const;

    /** Return parser or 0 */
    Parser* parser() const;

    /** Emit signal.
    \param change_path Whether internal path of wApp should be changed.
    */
    void open(bool change_path = false);

    /** Signal emitted when url is opened */
    Signal<>& opened() {
        return opened_;
    }

private:
    Signal<> opened_;
    std::string value_;
};

/** Predefined part of an URL.

\ingroup url
*/
class PredefinedNode : public Node {
public:
    /** Constructor */
    PredefinedNode(const std::string& predefined, WObject* parent = 0);

    bool meet(const std::string& part) const;

    /** Get predefined string */
    const std::string& predefined() const {
        return predefined_;
    }

private:
    const std::string predefined_;
};

/** Part of an URL, represented with arbitrary integer (long long).
\note The value is temporary and used for
      generation and parsing arbitrary paths.

\ingroup url
*/
class IntegerNode : public Node {
public:
    /** Constructor */
    IntegerNode(WObject* parent = 0);

    bool meet(const std::string& part) const;

    /** Get integer value.
    \attention Throws boost::bad_lexical_cast exception if value is not integer.
    */
    long long integer() const;

    /** Set value */
    void set_integer_value(long long v);

    /** Set value and return full internal path to this node */
    std::string get_full_path(long long v);

    /** Set value and return a link to this node */
    WLink get_link(long long v);
};

/** Part of an URL, represented with arbitrary string.
\note The value is temporary and used for
      generation and parsing arbitrary paths.

\ingroup url
*/
class StringNode : public Node {
public:
    /** Constructor */
    StringNode(WObject* parent = 0);

    bool meet(const std::string& part) const;

    /** Set value and return full internal path to this node */
    std::string get_full_path(const std::string& v);

    /** Set value and return a link to this node */
    WLink get_link(const std::string& v);
};

/** Path parser.
In addition, it is the root url node ("index" page, internal path "/").
All other nodes should be children (direct or indirect) of this node.

This object should be bound to session.

\ingroup url
*/
class Parser : public Node {
public:
    /** Constructor */
    Parser(WObject* parent = 0);

    bool meet(const std::string& part) const;

    /** Parse the internal path.
    Set values to all non-predefined nodes and return last node.
    If node is undefined, returns 0.
    */
    Node* parse(const std::string& path);

    /** Parse the internal path and open correcponding path node */
    void open(const std::string& path);
};

}

}

}

#endif

