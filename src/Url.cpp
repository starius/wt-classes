/*
 * wt-classes, utility classes used by Wt applications
 * Copyright (C) 2011 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <vector>
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
    slash_strategy_(DEFAULT)
{ }

void Node::write_to(std::ostream& path, bool is_last) const {
    path << urlencode(value_);
    if (slash_strategy_ == ALWAYS ||
            (slash_strategy_ == IF_NOT_LAST && !is_last) ||
            (slash_strategy_ == IF_HAS_CHILD && !children().empty())) {
        path << '/';
    }
}

void Node::write_all_to(std::ostream& path) const {
    const Node* node = this;
    std::vector<const Node*> nodes;
    while (node) {
        nodes.push_back(node);
        node = node->node_parent();
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
    opened_.emit();
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

static std::string urldecode(const std::string& text) {
    // source: src/Wt/Utils.C
    std::stringstream result;
    for (unsigned i = 0; i < text.length(); ++i) {
        char c = text[i];
        if (c == '+') {
            result << ' ';
        } else if (c == '%' && i + 2 < text.length()) {
            std::string h = text.substr(i + 1, 2);
            char* e = 0;
            int hval = std::strtol(h.c_str(), &e, 16);
            if (*e == 0) {
                result << (char)hval;
                i += 2;
            } else {
                // not a proper %XX with XX hexadecimal format
                result << c;
            }
        } else {
            result << c;
        }
    }
    return result.str();
}

Node* Parser::parse(const std::string& path) {
    using namespace boost::algorithm;
    std::vector<std::string> parts;
    split(parts, path, is_any_of("/"), token_compress_on);
    Node* node = this;
    BOOST_FOREACH (std::string part, parts) {
        part = urldecode(part);
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

void Parser::open(const std::string& path) {
    Node* node = parse(path);
    if (node) {
        node->open(/* change_path */ false);
    } else {
        error404().emit();
    }
}

#ifdef WC_HAVE_WLINK
void Parser::open(const WLink& internal_path) {
    open(internal_path.internalPath().toUTF8());
}
#endif

}

}

}

