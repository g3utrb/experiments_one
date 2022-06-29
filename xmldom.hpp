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
  };

  class attribute : public node {
  public:
    attribute(xercesc::DOMAttr* nodep);
  };

  class text_node : public node {
  public:
    text_node(xercesc::DOMText* dnp);
  };

  class node_factory {
  public:
    static node::ptr create(xercesc::DOMNode* xnode);
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
  node(xercesc::DOMNode* nodep) : node_(nodep) {

    /// assuming we're passed a valid xercesc dom node pointer.
    /// what would happen if it's null? should we throw an exception
    /// or is that too restrctive; maybe it should be the caller's
    /// responsibility to handle it..
    if (node_) {

      char* tmp = xercesc::XMLString::transcode(node_->getNodeName());
      if (tmp != (char *) NULL) {
        name_ = tmp;
        xercesc::XMLString::release(&tmp);
      }
      xercesc::DOMNode* link = node_->getFirstChild();
      if (! link) return;

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
    }
  }

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

  inline
  attribute::
  attribute(xercesc::DOMAttr* xnode) :
    node(xnode) {
  }

  inline
  text_node::
  text_node(xercesc::DOMText* xnode) :
    node(xnode) {
  }

  inline node::ptr
  node_factory::
  create(xercesc::DOMNode* dnp) {

    if (! dnp) return node::ptr();

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
      /// transfer error details into err
      std::cout << "parsing failed" << std::endl;
      throw;
      return false;
    }
    catch (const xercesc::XMLException& e) {
      std::cout << "parsing failed" << std::endl;
      /// transfer error details into err
      throw;
      return false;
    }
    catch (...) {
      std::cout << "parsing failed" << std::endl;
      /// transfer error details into err
      throw;
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
