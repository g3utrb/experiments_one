#include <string.h>
#include <stdlib.h>
#include <memory>
#include <time.h>
#include <unistd.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/framework/StdOutFormatTarget.hpp>
#include <xercesc/framework/LocalFileFormatTarget.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/util/XMLUni.hpp>
#include <xercesc/util/OutOfMemoryException.hpp>
#include "xmldom.hpp"
#include "xmlconverter.hpp"
#include "xmlbinding.hpp"


struct vmaster_diary_entry : xml::binding::composite {

  vmaster_diary_entry() {
    insert("diaryText", diary_text);
  }
  xml::binding::element_string  diary_text;
};

struct vmaster_diary : xml::binding::composite {

  vmaster_diary() {
    insert("vMasterDiaryEntry", diary);
  }
  xml::binding::nodelist<vmaster_diary_entry> diary;
};

struct vmaster_header : xml::binding::composite {

  vmaster_header() {
    insert("vMasterInstrument",     instrument);
    insert("vMasterTradeStatus",    trade_status);
    insert("vMasterTradeDate",      trade_date);
    insert("vMasterStartDate",      start_date);
    insert("RTLCReferenceCode",     rtlc_reference_code);
    insert("vMasterEndDate",        end_date);
    insert("vMasterTradeOrigin",    trade_origin);
    insert("vMasterTradeOriginID",  trade_origin_id);
    insert("vMasterTrader",         trader);
    insert("vMasterCoverage",       coverage);
    insert("vMasterLocation",       location);
    insert("vMasterBook",           book);
    insert("vMasterUserLogin",      user_login);
    insert("vMasterBookLocation",   book_location);
    insert("vMasterBookDomicile",   book_domicile);
    insert("vMasterEntity",         entity);
    insert("vMasterEntityCoperID",  entity_coper_id);
    insert("vMasterMLDPGuarantee",  mldp_guarantee);
    insert("vMasterSwapclearFlag",  swap_clear_flag);
    insert("vMasterCreditCode",     credit_code);
    insert("vMasterDesk",           desk);
    insert("vMasterRevisionDate",   revision_date);
    insert("vMasterCreationDate",   creation_date);
    insert("vMasterDiary",          diary);
  }

  xml::binding::element_string  instrument;
  xml::binding::element_string  trade_status;
  xml::binding::element_string  trade_date;
  xml::binding::element_string  start_date;
  xml::binding::element_string  rtlc_reference_code;
  xml::binding::element_string  end_date;
  xml::binding::element_string  trade_origin;
  xml::binding::element_string  trade_origin_id;
  xml::binding::element_string  trader;
  xml::binding::element_string  coverage;
  xml::binding::element_string  location;
  xml::binding::element_string  book;
  xml::binding::element_string  user_login;
  xml::binding::element_string  book_location;
  xml::binding::element_string  book_domicile;
  xml::binding::element_string  entity;
  xml::binding::element_int     entity_coper_id;
  xml::binding::element_string  mldp_guarantee;
  xml::binding::element_string  swap_clear_flag;
  xml::binding::element_string  credit_code;
  xml::binding::element_string  desk;
  xml::binding::element_string  revision_date;
  xml::binding::element_string  creation_date;
  vmaster_diary                 diary;
};

struct vmaster_message : xml::binding::composite {

  vmaster_message() {
    insert("vMasterHeader", vm_header);
  }

  vmaster_header vm_header;
};

void execute() {

  /// load up a sample xml
  std::stringstream ss;
  std::ifstream ifs("./p.xml");
  ss << ifs.rdbuf();
  std::string s = ss.str();
  std::cout << s << std::endl;

  /// parse the xml string
  xml::dom::parser::ptr  par = std::make_shared<xml::dom::parser>();
  support::error_code err;
  bool result = par->parse(err, s);
  xml::dom::node::ptr np = par->root();

  /// timeclock for bind measurement
  struct timespec start, stop;
  clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);

  vmaster_message vm;
  result = vm.bind(err, np);
  std::cout << "bind result: " << result << std::endl;

  /// timeclock stop for bind measurement
  clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &stop);
  double elapsed = (stop.tv_sec - start.tv_sec) * 1e6 + (stop.tv_nsec - start.tv_nsec) / 1e3;
  std::cout << "time in microseconds: " << elapsed << std::endl;

  const vmaster_header& vmh = vm.vm_header;

  std::cout << vmh.instrument() << std::endl;
  std::cout << vmh.entity_coper_id() << std::endl;
  std::cout << vmh.trade_origin_id() << std::endl;

  bool has_trader = vmh.trader;
  std::cout << "Has trader " << std::boolalpha << has_trader << std::endl;
  std::cout << "desk " << vmh.desk() << std::endl;

}

int main(int argC, char* argV[]) {

  /// must be first - need a singleton initializer
  try {
    xercesc::XMLPlatformUtils::Initialize();
  }
  catch(const xercesc::XMLException &toCatch) {
    std::cout << xercesc::XMLString::transcode(toCatch.getMessage()) << std::endl;
    return 1;
  }
  execute();
  /// must be last - but parser is deleted after?
  xercesc::XMLPlatformUtils::Terminate();
}
