#pragma once

#include <map>
#include <string>
#include <vector>
#include <memory>
#include <iostream>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/dom/DOMNode.hpp>
#include <xercesc/dom/DOMElement.hpp>
#include <xercesc/dom/DOMAttr.hpp>
#include <xercesc/dom/DOMText.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/framework/MemBufInputSource.hpp>
#include "error_code.hpp"

namespace xml {
namespace dom {

  class node {
  public:

    operator const std::string&() const { return value_; }
    operator std::string&() { return value_; }

    const std::string& name() const;
    const std::string& value() const;

    std::string& name();
    std::string& value();
    xercesc::DOMNode* xerces_node();

    typedef std::shared_ptr<node> ptr;

    virtual bool init(support::error_code& err) = 0;

  protected:

    node(xercesc::DOMNode* nodep);
    virtual ~node() = 0;

    std::string        name_;
    std::string        value_;
    xercesc::DOMNode*  node_;
  };

  class element : public node {
  public:

    element(xercesc::DOMElement* nodep);
    virtual bool init(support::error_code& err);

  };

  class attribute : public node {
  public:

    attribute(xercesc::DOMAttr* nodep);
    virtual bool init(support::error_code& err);

  };

  class text_node : public node {
  public:

    text_node(xercesc::DOMText* dnp);
    virtual bool init(support::error_code& err);

  };

  class node_factory {
  public:
    static node::ptr create(support::error_code& err, xercesc::DOMNode* xnode);
  };

  class parser {
  public:

    typedef std::shared_ptr<parser> ptr;

    bool parse(support::error_code& err, const std::string& content);
    node::ptr root();
    ~parser();

  private:
    node::ptr          root_;
    xercesc::XercesDOMParser* parser_;
  };

  /// implementations follow
  inline
  node::
  node(xercesc::DOMNode* nodep) : node_(nodep)
  {}


  inline
  node::
  ~node() {
  }

  inline const std::string&
  node::
  name() const {
    return name_;
  }

  inline std::string&
  node::
  name() {
    return name_;
  }

  inline const std::string&
  node::
  value() const {
    return value_;
  }

  inline std::string&
  node::
  value() {
    return value_;
  }

  inline xercesc::DOMNode*
  node::
  xerces_node() {
    return node_;
  }

  inline
  element::
  element(xercesc::DOMElement* xnode) :
    node(xnode) {
  }

  inline bool
  element::
  init(support::error_code& err) {

    if (!node_) {
      /// ??
      return true;
    }
    char* tmp = xercesc::XMLString::transcode(node_->getNodeName());
    if (tmp != (char *) NULL) {
      name_ = tmp;
      xercesc::XMLString::release(&tmp);
    }
    else {
      /// ?
      return false;
    }
    xercesc::DOMNode* link = node_->getFirstChild();
    for (; link != 0; link = link->getNextSibling() ) {

      if (link && link->getNodeType() == xercesc::DOMNode::TEXT_NODE) {
        tmp = xercesc::XMLString::transcode(link->getNodeValue());
        if (tmp != (char *) NULL) {
          value_ = tmp;
          xercesc::XMLString::release(&tmp);
        }
        break;
      }
    }
    return true;
  }

  inline
  attribute::
  attribute(xercesc::DOMAttr* xnode) :
    node(xnode) {
  }

  inline bool
  attribute::
  init(support::error_code& err) {

    if (!node_) {
      /// ??
      return true;
    }
    char* tmp = xercesc::XMLString::transcode(node_->getNodeName());
    if (tmp != (char *) NULL) {
      name_ = tmp;
      xercesc::XMLString::release(&tmp);
    }
    else {
      /// ?
      return false;
    }
    /// this is an attribute node, get the value
    char* s = xercesc::XMLString::transcode(node_->getNodeValue());
    value_ = s;
    xercesc::XMLString::release(&s);
    return true;
  }

  inline
  text_node::
  text_node(xercesc::DOMText* xnode) :
    node(xnode) {
  }

  inline bool
  text_node::
  init(support::error_code& err) {
    /// ???
    return true;
  }

  inline node::ptr
  node_factory::
  create(support::error_code& err,
         xercesc::DOMNode* dnp) {

    if (! dnp) {
      return node::ptr();
    }
    dom::node::ptr np;
    switch (dnp->getNodeType()) {

      case xercesc::DOMNode::ELEMENT_NODE: {

        xercesc::DOMElement*  dep = (xercesc::DOMElement*) dnp;
        np = std::make_shared<element>(dep);
        break;

      }
      case xercesc::DOMNode::ATTRIBUTE_NODE: {

        xercesc::DOMAttr*  dap = (xercesc::DOMAttr*) dnp;
        np = std::make_shared<attribute>(dap);
        break;

      }
      case xercesc::DOMNode::TEXT_NODE: {

        xercesc::DOMText*  dtp = (xercesc::DOMText*) dnp;
        np = std::make_shared<text_node>(dtp);
        break;

      }
      default:
        break;
    }
    if (np) {
      np->init(err);
    }
    return np;
  }

  inline bool
  parser::
  parse(support::error_code& err,
        const std::string& content) {

    try {

      parser_ = new xercesc::XercesDOMParser();
      parser_->setValidationScheme(xercesc::XercesDOMParser::Val_Never);
      parser_->setDoNamespaces(false);
      parser_->setHandleMultipleImports(false);
      parser_->setValidationSchemaFullChecking(false);
      parser_->setCreateEntityReferenceNodes(false);

      xercesc::MemBufInputSource memory_buffer(
        (const XMLByte *) content.c_str(),
        content.size(),
        "test",
        false);

      parser_->parse(memory_buffer);
      xercesc::DOMDocument* doc = parser_->getDocument();
      xercesc::DOMElement* elem = doc->getDocumentElement();
      root_ = std::make_shared<element>(elem);
    }
    catch (const xercesc::OutOfMemoryException& e) {
      char* es = xercesc::XMLString::transcode(e.getMessage());
      std::string m = es;
      xercesc::XMLString::release(&es);
      std::string s = "Parsing failed, out of memory: " + m;
      err.attach(support::error_code(-1, s));
      return false;
    }
    catch (const xercesc::XMLException& e) {
      char* es = xercesc::XMLString::transcode(e.getMessage());
      std::string m = es;
      xercesc::XMLString::release(&es);
      std::string s = "Parsing failed, xml exception: " + m;
      err.attach(support::error_code(-1, s));
      return false;
    }
    catch (...) {
      std::string s = "Unknown exception parsing: " + content;
      err.attach(support::error_code(-1, s));
      return false;
    }
    return true;
  }

  inline node::ptr
  parser::
  root() {
    return root_;
  }

  inline
  parser::
  ~parser() {
    delete parser_;
  }

}}
