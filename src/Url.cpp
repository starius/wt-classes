/*
 * wt-classes, utility classes used by Wt applications
 * Copyright (C) 2011 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <vector>
#include <utility>
#include <sstream>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>

#include <Wt/WApplication>

#include "Url.hpp"
#include "util.hpp"

namespace Wt {

namespace Wc {

namespace url {

Node::Node(WObject* parent):
    WObject(parent),
    slash_strategy_(DEFAULT),
    opened_(0)
{ }

Node::~Node() {
    delete opened_;
}

void Node::write_to(std::ostream& path, bool is_last) const {
    path << value_;
    if (slash_strategy_ == ALWAYS ||
            (slash_strategy_ == IF_NOT_LAST && !is_last) ||
            (slash_strategy_ == IF_HAS_CHILD && !children().empty())) {
        path << '/';
    }
}

void Node::write_all_to(std::ostream& path, Node* root) const {
    const Node* node = this;
    std::vector<const Node*> nodes;
    while (node) {
        nodes.push_back(node);
        if (node == root) { // and root != 0
            break;
        }
        node = node->node_parent();
    }
    if (root && !root->value().empty()) {
        // we stop in the middle
        path << '/';
    }
    BOOST_REVERSE_FOREACH (const Node* node, nodes) {
        bool is_last = node == nodes.front();
        node->write_to(path, is_last);
    }
}

std::string Node::full_path() const {
    std::stringstream ss;
    write_all_to(ss);
    return ss.str();
}

#ifdef WC_HAVE_WLINK
WLink Node::link() const {
    return WLink(WLink::InternalPath, full_path());
}
#endif

Node* Node::node_parent() const {
    return isinstance<Node>(parent()) ? DOWNCAST<Node*>(parent()) : 0;
}

Parser* Node::parser() const {
    Node* node = const_cast<Node*>(this);
    while (!isinstance<Parser>(node) && isinstance<Node>(node)) {
        node = node->node_parent();
    }
    return node ? DOWNCAST<Parser*>(node) : 0;
}

void Node::open(bool change_path) {
    if (change_path) {
        wApp->setInternalPath(full_path(), /*emitChange */ false);
    }
    Parser* p = parser();
    if (p) {
        p->open(this);
    }
}

Signal<>& Node::opened() {
    if (!opened_) {
        opened_ = new Signal<>;
    }
    return *opened_;
}

void Node::set_value(const std::string& v, bool check) {
    if (!check || meet(v)) {
        value_ = v;
    }
}

PredefinedNode::PredefinedNode(const std::string& predefined, WObject* parent):
    Node(parent), predefined_(predefined) {
    set_value(predefined_);
}

bool PredefinedNode::meet(const std::string& part) const {
    return part == predefined_;
}

IntegerNode::IntegerNode(WObject* parent):
    Node(parent)
{ }

bool IntegerNode::meet(const std::string& part) const {
    try {
        boost::lexical_cast<long long>(part);
        return true;
    } catch (...) {
        return false;
    }
}

long long IntegerNode::integer() const {
    return boost::lexical_cast<long long>(value());
}

void IntegerNode::set_integer_value(long long v) {
    set_value(boost::lexical_cast<std::string>(v), false);
}

std::string IntegerNode::get_full_path(long long v) {
    set_integer_value(v);
    return full_path();
}

#ifdef WC_HAVE_WLINK
WLink IntegerNode::get_link(long long v) {
    return WLink(WLink::InternalPath, get_full_path(v));
}
#endif

StringNode::StringNode(WObject* parent):
    Node(parent)
{ }

bool StringNode::meet(const std::string&) const {
    return true;
}

void StringNode::set_string(const std::string& string) {
    set_value(string);
}

const std::string& StringNode::string() const {
    return value();
}

std::string StringNode::get_full_path(const std::string& v) {
    set_value(v);
    return full_path();
}

#ifdef WC_HAVE_WLINK
WLink StringNode::get_link(const std::string& v) {
    return WLink(WLink::InternalPath, get_full_path(v));
}
#endif

Parser::Parser(WObject* parent):
    Node(parent)
{ }

bool Parser::meet(const std::string& part) const {
    return part.empty();
}

Node* Parser::parse(const std::string& path) {
    using namespace boost::algorithm;
    std::vector<std::string> parts;
    split(parts, path, is_any_of("/"), token_compress_on);
    Node* node = this;
    BOOST_FOREACH (const std::string& part, parts) {
        if (part.empty()) {
            continue;
        }
        bool next = false;
        BOOST_FOREACH (WObject* o, node->children()) {
            if (isinstance<Node>(o) && DOWNCAST<Node*>(o)->meet(part)) {
                next = true;
                node = DOWNCAST<Node*>(o);
                DOWNCAST<Node*>(o)->set_value(part);
                break;
            }
        }
        if (!next) {
            return 0;
        }
    }
    return node;
}

void Parser::open(Node* node) {
    if (node->opened_) {
        node->opened().emit();
    }
    child_opened_.emit(node);
    typedef Handlers::iterator It;
    typedef std::pair<It, It> ItPair;
    ItPair begin_end = handlers_.equal_range(node);
    for (It it = begin_end.first; it != begin_end.second; ++it) {
        const Handler& handler = it->second;
        handler();
    }
}

void Parser::open(const std::string& path) {
    Node* node = parse(path);
    if (node) {
        open(node);
    } else {
        error404().emit();
    }
}

#ifdef WC_HAVE_WLINK
void Parser::open(const WLink& internal_path) {
    open(internal_path.internalPath().toUTF8());
}
#endif

void Parser::connect(Node* child, boost::function<void()> handler) {
    handlers_.insert(std::make_pair(child, handler));
}

void Parser::disconnect(Node* child) {
    typedef Handlers::iterator It;
    typedef std::pair<It, It> ItPair;
    ItPair begin_end = handlers_.equal_range(child);
    handlers_.erase(begin_end.first, begin_end.second);
}

}

}

}

