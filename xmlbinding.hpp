#pragma once

#include <map>
#include <string>
#include <vector>
#include <iostream>
// #include "xmldom.hpp"
// #include "xmlconverter.hpp"

namespace xml {
namespace binding {

  class node_base {
   public:
    virtual bool bind(support::error_code& err, dom::node::ptr np) = 0;
    virtual ~node_base();
  };

  template <class T>  /// where T is the converter type
  class node : public node_base {
  public:

    typedef typename T::converter_type converter_type;
    typedef typename T::value_type     value_type;

    operator const value_type&() const
    { return converter_.access(); }

    operator value_type&()
    { return converter_.access(); }

    const value_type& operator()() const
    { return converter_.access(); }

    value_type& operator()()
    { return converter_.access(); }

    typedef node<T> this_type;
    typedef void (this_type::*bool_hack)() const;
    void bool_workaround() const
    {}

    operator bool_hack() const {
      bool result = converter_;
      return result ? &this_type::bool_workaround : 0;
    }

    const std::string& name() const;
    const std::string& value() const;

    std::string& value();

    typedef std::shared_ptr<node> ptr;
    virtual bool bind(support::error_code& err, dom::node::ptr np);

  protected:
    converter_type converter_;
  };

  template <class T>
  class leaf : public node<T> {
  };

  template <class T>
  class element : public leaf<T> {
  };

  template <class T>
  class attribute : public leaf<T> {
  };

  class composite : public node<string_converter> {
  public:

    composite();

    /// get children of node
    /// iterate over each child
    /// look for an entry in mappings
    /// if found invoke bind on the node pointer *with* the child node
    /// currently being handled
    ///
    virtual bool bind(support::error_code& err, dom::node::ptr np);

  protected:

    /// inserts an entry into mappings based on name and node ptr
    /// this must be done by the child composite in its constructor
    /// the mappings will be used in 'bind' to propagate through
    void insert(const std::string& name, node_base* np);
    void insert(const std::string& name, node_base& n);
    bool process_attributes(support::error_code& err, xercesc::DOMNode* xnode);

    typedef std::map<std::string, node_base*> mappings;
    mappings mappings_;

  };

  template <class T>
  class nodelist : public node<string_converter> {
  public:

    virtual bool bind(support::error_code& err, dom::node::ptr dnp);

    typedef std::vector<T> chain_t;
    const chain_t& chain() const;
    chain_t& chain();

    size_t size() const;
    bool empty() const;

    const T& operator[](size_t i) const;
    T& operator[](size_t i);

  protected:
    chain_t chain_;
  };

}}

#include "xmlbinding.ipp"
