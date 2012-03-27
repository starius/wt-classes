/*
 * wt-classes, utility classes used by Wt applications
 * Copyright (C) 2011 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#ifndef WC_URL_HPP_
#define WC_URL_HPP_

#include <string>
#include <ostream>

#include <Wt/WGlobal>
#include <Wt/WObject>
#include <Wt/WSignal>
#include <Wt/WLink>

#include "global.hpp"

namespace Wt {

namespace Wc {

/** Namespace for dealing with internal paths */
namespace url {

/** \defgroup url URL-related
Classes for URL rules.
This classes allow you to keep all url-related code in one place.

\note Before reading the rest of this documentation,
    read about internal paths in the Wt documentation.

<h3>node</h3>

Every internal path is represented as a Node in a tree.
For example, internal path "/about/license" is a child of "/about/".

To create a child node, just pass parent node to the constructor of a child.
Constructors of nodes of some types take additional arguments.

Each node has a \ref Node::value() "value", the string,
representing the last part of that internal path.
For example, the value of "/about/license" is "license".

There are several types of node:
 - PredefinedNode (value is constant),
 - IntegerNode (value represents an integer),
 - StringNode (value represents a string),
 - Parser (value is an empty string; is the root of that tree of nodes).

IntegerNode and StringNode are mutable.
Their value (integer or string) can be changed.
This occurs while \ref Parser::open() "url opening".
Parser sets values of all nodes, involved into internal path parsed.
You can get the value using StringNode::string() or IntegerNode::integer(),
or general method Node::value().
To set the value (to get WLink's involving it), use StringNode::set_string()
of IntegerNode::set_integer_value().

After the creation of nodes, connect their \ref Node::opened() "opened()"
signal to appropriate slots.
For example, let internal path "/user/" be the list of users.
In this case, connect its opened() signal to the function,
displaying the list of users.
It is beyond this documentation, how to write this function.

When you need the internal path, use Node::link() method.
There are methods get_link() in IntegerNode and StringNode, setting
the value and returning the link at once.
You can simulate the opening of an internal path,
using Node::open() method.

<h3>parser</h3>

Parser is the root in internal path tree.
It corresponds to the internal path "/" (main page of an application).
Nodes of other type are direct or indirect children of Parser node.

All created nodes should be bound to session.
You can create your own class, inherited from Parser,
which would create all child nodes, and bind an instance
of that class to an instance of WApplication.
Nodes will be needed to get internal paths, e.g. for WAnchor.
So it is better to provide public access to them.

You should connect wApp->internalPathChanged()
to Parser::open() to make it aware of internal path changes.

In the end of the constructor of WApplication's descendant
(or in other function, creating application for WServer)
pass internalPath() to Parser::open() to process initial internal path
(internal path, being set while application creation).

<h3>Short example</h3>

\code
Parser* parser = new Parser(wApp);
wApp->internalPathChanged().connect(parser, &Parser::open);
PredefinedNode* users = new PredefinedNode("user", parser);
users->opened().connect(...);        // show user list for "/user/"
IntegerNode* user_profile = new IntegerNode(users);
user_profile->opened().connect(...); // show user profile for "/user/xxx"
\endcode

<h3>Detailed example</h3>

In the example bellow, main features of this module are demonstrated:
 - tree of internal path nodes,
 - connection of Node::opened() to function, displaying something,
 - Parser is also a Node, it corresponds to the main page,
 - use of Parser::open(),
 - referring internal paths.

It is a model of content and user oriented site.

Each user has integer identifier.
User profile is shown in internal path like "/user/123".
The list of users is shown in "/user/".

"/about/" represents a list of available articles.
Each article is identified with a string.
Article page is "/about/article-name".

The main page contain references to the list of articles and the list of users.
Every page has a reference to the main page on top.

User identifiers and article names are generated randomly.

\include examples/url.cpp

*/

/** Part of URL.
This class represents a node in url parsing tree.

For example, for internal path \c "/user/1", there are two nodes:
 - \c "user", is represented with PredefinedNode;
 - \c "1", is represented with IntegerNode.

\ingroup url
*/
class Node : public WObject {
public:
    /** When to add a slash after the node */
    enum SlashStrategy {
        IF_NOT_LAST, /**< Add only if the node is not last node in the URL */
        IF_HAS_CHILD, /**< Add only if the node has at least one child object */
        ALWAYS, /**< Always add */
        DEFAULT = IF_HAS_CHILD /**< Default strategy */
    };

    /** Constructor.
    \param parent Node (Parser or other Node)
    */
    Node(WObject* parent = 0);

    /** Return if this part of an url meets the node */
    virtual bool meet(const std::string& part) const = 0;

    /** Get value */
    const std::string& value() const {
        return value_;
    }

    /** Add this node to output stream.
    \param path Output stream.
    \param is_last If the node is the last node in the URL.
    Add to the stream representation of this node and '/',
    depending on is_last and slash().
    */
    void write_to(std::ostream& path, bool is_last = false) const;

    /** Write all parents and this node to the stream */
    void write_all_to(std::ostream& path) const;

    /** Return full internal path to this node */
    std::string full_path() const;

    /** Return an internal path link to this node */
    WLink link() const;

    /** Return parent of type Node or 0 */
    Node* node_parent() const;

    /** Return parser or 0 */
    Parser* parser() const;

    /** Emit signal.
    \param change_path Whether internal path of wApp should be changed.
    */
    void open(bool change_path = true);

    /** Signal emitted when url is opened */
    Signal<>& opened() {
        return opened_;
    }

    /** Get slash strategy */
    SlashStrategy slash_strategy() const {
        return slash_strategy_;
    }

    /** Get slash strategy.
    By default, DEFAULT slash strategy is applied.
    */
    void set_slash_strategy(SlashStrategy slash_strategy) {
        slash_strategy_ = slash_strategy;
    }

protected:
    /** Set value.
    If check is true, the value is checked using meet() before setting.
    */
    void set_value(const std::string& v, bool check = false);

private:
    Signal<> opened_;
    std::string value_;
    SlashStrategy slash_strategy_ : 8;

    friend class Parser;
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

    /** Set value and return an internal path link to this node */
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

    /** Set value */
    void set_string(const std::string& string);

    /** Get value */
    const std::string& string() const;

    /** Set value and return full internal path to this node */
    std::string get_full_path(const std::string& v);

    /** Set value and return an internal path link to this node */
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

    /** Parse the internal path and open corresponding path node.
    This method does not change URL in address line
    (Node::open() is called with change_path = false).
    */
    void open(const std::string& path);

    /** Parse the internal path and open corresponding path node.
    This is an overloaded method for convenience.
    */
    void open(const WLink& internal_path);

    /** Signal emitted when wrong internal path is opened */
    Signal<>& error404() {
        return error404_;
    }

private:
    Signal<> error404_;
};

}

}

}

#endif

