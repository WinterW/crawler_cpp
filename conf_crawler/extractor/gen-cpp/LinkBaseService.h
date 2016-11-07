/**
 * Autogenerated by Thrift Compiler (0.9.1)
 *
 * DO NOT EDIT UNLESS YOU ARE SURE THAT YOU KNOW WHAT YOU ARE DOING
 *  @generated
 */
#ifndef LinkBaseService_H
#define LinkBaseService_H

#include <thrift/TDispatchProcessor.h>
#include "conf_crawler_types.h"



class LinkBaseServiceIf {
 public:
  virtual ~LinkBaseServiceIf() {}
  virtual void load_seed_by_id(const int32_t id, const bool is_add_link) = 0;
  virtual void load_seed_by_url(const std::string& seed_url, const bool is_add_link) = 0;
  virtual void get_download_task(std::vector<DownloadTask> & _return) = 0;
  virtual void get_extract_task(std::vector<ExtractItem> & _return) = 0;
  virtual void upload_download_task(const DownloadedBodyItem& downloaded_body_item) = 0;
  virtual void upload_extract_task(const ExtractItem& extract_item, const MatchedResultItem& matched_result_item) = 0;
};

class LinkBaseServiceIfFactory {
 public:
  typedef LinkBaseServiceIf Handler;

  virtual ~LinkBaseServiceIfFactory() {}

  virtual LinkBaseServiceIf* getHandler(const ::apache::thrift::TConnectionInfo& connInfo) = 0;
  virtual void releaseHandler(LinkBaseServiceIf* /* handler */) = 0;
};

class LinkBaseServiceIfSingletonFactory : virtual public LinkBaseServiceIfFactory {
 public:
  LinkBaseServiceIfSingletonFactory(const boost::shared_ptr<LinkBaseServiceIf>& iface) : iface_(iface) {}
  virtual ~LinkBaseServiceIfSingletonFactory() {}

  virtual LinkBaseServiceIf* getHandler(const ::apache::thrift::TConnectionInfo&) {
    return iface_.get();
  }
  virtual void releaseHandler(LinkBaseServiceIf* /* handler */) {}

 protected:
  boost::shared_ptr<LinkBaseServiceIf> iface_;
};

class LinkBaseServiceNull : virtual public LinkBaseServiceIf {
 public:
  virtual ~LinkBaseServiceNull() {}
  void load_seed_by_id(const int32_t /* id */, const bool /* is_add_link */) {
    return;
  }
  void load_seed_by_url(const std::string& /* seed_url */, const bool /* is_add_link */) {
    return;
  }
  void get_download_task(std::vector<DownloadTask> & /* _return */) {
    return;
  }
  void get_extract_task(std::vector<ExtractItem> & /* _return */) {
    return;
  }
  void upload_download_task(const DownloadedBodyItem& /* downloaded_body_item */) {
    return;
  }
  void upload_extract_task(const ExtractItem& /* extract_item */, const MatchedResultItem& /* matched_result_item */) {
    return;
  }
};

typedef struct _LinkBaseService_load_seed_by_id_args__isset {
  _LinkBaseService_load_seed_by_id_args__isset() : id(false), is_add_link(false) {}
  bool id;
  bool is_add_link;
} _LinkBaseService_load_seed_by_id_args__isset;

class LinkBaseService_load_seed_by_id_args {
 public:

  LinkBaseService_load_seed_by_id_args() : id(0), is_add_link(0) {
  }

  virtual ~LinkBaseService_load_seed_by_id_args() throw() {}

  int32_t id;
  bool is_add_link;

  _LinkBaseService_load_seed_by_id_args__isset __isset;

  void __set_id(const int32_t val) {
    id = val;
  }

  void __set_is_add_link(const bool val) {
    is_add_link = val;
  }

  bool operator == (const LinkBaseService_load_seed_by_id_args & rhs) const
  {
    if (!(id == rhs.id))
      return false;
    if (!(is_add_link == rhs.is_add_link))
      return false;
    return true;
  }
  bool operator != (const LinkBaseService_load_seed_by_id_args &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const LinkBaseService_load_seed_by_id_args & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};


class LinkBaseService_load_seed_by_id_pargs {
 public:


  virtual ~LinkBaseService_load_seed_by_id_pargs() throw() {}

  const int32_t* id;
  const bool* is_add_link;

  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};


class LinkBaseService_load_seed_by_id_result {
 public:

  LinkBaseService_load_seed_by_id_result() {
  }

  virtual ~LinkBaseService_load_seed_by_id_result() throw() {}


  bool operator == (const LinkBaseService_load_seed_by_id_result & /* rhs */) const
  {
    return true;
  }
  bool operator != (const LinkBaseService_load_seed_by_id_result &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const LinkBaseService_load_seed_by_id_result & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};


class LinkBaseService_load_seed_by_id_presult {
 public:


  virtual ~LinkBaseService_load_seed_by_id_presult() throw() {}


  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);

};

typedef struct _LinkBaseService_load_seed_by_url_args__isset {
  _LinkBaseService_load_seed_by_url_args__isset() : seed_url(false), is_add_link(false) {}
  bool seed_url;
  bool is_add_link;
} _LinkBaseService_load_seed_by_url_args__isset;

class LinkBaseService_load_seed_by_url_args {
 public:

  LinkBaseService_load_seed_by_url_args() : seed_url(), is_add_link(0) {
  }

  virtual ~LinkBaseService_load_seed_by_url_args() throw() {}

  std::string seed_url;
  bool is_add_link;

  _LinkBaseService_load_seed_by_url_args__isset __isset;

  void __set_seed_url(const std::string& val) {
    seed_url = val;
  }

  void __set_is_add_link(const bool val) {
    is_add_link = val;
  }

  bool operator == (const LinkBaseService_load_seed_by_url_args & rhs) const
  {
    if (!(seed_url == rhs.seed_url))
      return false;
    if (!(is_add_link == rhs.is_add_link))
      return false;
    return true;
  }
  bool operator != (const LinkBaseService_load_seed_by_url_args &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const LinkBaseService_load_seed_by_url_args & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};


class LinkBaseService_load_seed_by_url_pargs {
 public:


  virtual ~LinkBaseService_load_seed_by_url_pargs() throw() {}

  const std::string* seed_url;
  const bool* is_add_link;

  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};


class LinkBaseService_load_seed_by_url_result {
 public:

  LinkBaseService_load_seed_by_url_result() {
  }

  virtual ~LinkBaseService_load_seed_by_url_result() throw() {}


  bool operator == (const LinkBaseService_load_seed_by_url_result & /* rhs */) const
  {
    return true;
  }
  bool operator != (const LinkBaseService_load_seed_by_url_result &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const LinkBaseService_load_seed_by_url_result & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};


class LinkBaseService_load_seed_by_url_presult {
 public:


  virtual ~LinkBaseService_load_seed_by_url_presult() throw() {}


  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);

};


class LinkBaseService_get_download_task_args {
 public:

  LinkBaseService_get_download_task_args() {
  }

  virtual ~LinkBaseService_get_download_task_args() throw() {}


  bool operator == (const LinkBaseService_get_download_task_args & /* rhs */) const
  {
    return true;
  }
  bool operator != (const LinkBaseService_get_download_task_args &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const LinkBaseService_get_download_task_args & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};


class LinkBaseService_get_download_task_pargs {
 public:


  virtual ~LinkBaseService_get_download_task_pargs() throw() {}


  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};

typedef struct _LinkBaseService_get_download_task_result__isset {
  _LinkBaseService_get_download_task_result__isset() : success(false) {}
  bool success;
} _LinkBaseService_get_download_task_result__isset;

class LinkBaseService_get_download_task_result {
 public:

  LinkBaseService_get_download_task_result() {
  }

  virtual ~LinkBaseService_get_download_task_result() throw() {}

  std::vector<DownloadTask>  success;

  _LinkBaseService_get_download_task_result__isset __isset;

  void __set_success(const std::vector<DownloadTask> & val) {
    success = val;
  }

  bool operator == (const LinkBaseService_get_download_task_result & rhs) const
  {
    if (!(success == rhs.success))
      return false;
    return true;
  }
  bool operator != (const LinkBaseService_get_download_task_result &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const LinkBaseService_get_download_task_result & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};

typedef struct _LinkBaseService_get_download_task_presult__isset {
  _LinkBaseService_get_download_task_presult__isset() : success(false) {}
  bool success;
} _LinkBaseService_get_download_task_presult__isset;

class LinkBaseService_get_download_task_presult {
 public:


  virtual ~LinkBaseService_get_download_task_presult() throw() {}

  std::vector<DownloadTask> * success;

  _LinkBaseService_get_download_task_presult__isset __isset;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);

};


class LinkBaseService_get_extract_task_args {
 public:

  LinkBaseService_get_extract_task_args() {
  }

  virtual ~LinkBaseService_get_extract_task_args() throw() {}


  bool operator == (const LinkBaseService_get_extract_task_args & /* rhs */) const
  {
    return true;
  }
  bool operator != (const LinkBaseService_get_extract_task_args &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const LinkBaseService_get_extract_task_args & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};


class LinkBaseService_get_extract_task_pargs {
 public:


  virtual ~LinkBaseService_get_extract_task_pargs() throw() {}


  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};

typedef struct _LinkBaseService_get_extract_task_result__isset {
  _LinkBaseService_get_extract_task_result__isset() : success(false) {}
  bool success;
} _LinkBaseService_get_extract_task_result__isset;

class LinkBaseService_get_extract_task_result {
 public:

  LinkBaseService_get_extract_task_result() {
  }

  virtual ~LinkBaseService_get_extract_task_result() throw() {}

  std::vector<ExtractItem>  success;

  _LinkBaseService_get_extract_task_result__isset __isset;

  void __set_success(const std::vector<ExtractItem> & val) {
    success = val;
  }

  bool operator == (const LinkBaseService_get_extract_task_result & rhs) const
  {
    if (!(success == rhs.success))
      return false;
    return true;
  }
  bool operator != (const LinkBaseService_get_extract_task_result &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const LinkBaseService_get_extract_task_result & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};

typedef struct _LinkBaseService_get_extract_task_presult__isset {
  _LinkBaseService_get_extract_task_presult__isset() : success(false) {}
  bool success;
} _LinkBaseService_get_extract_task_presult__isset;

class LinkBaseService_get_extract_task_presult {
 public:


  virtual ~LinkBaseService_get_extract_task_presult() throw() {}

  std::vector<ExtractItem> * success;

  _LinkBaseService_get_extract_task_presult__isset __isset;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);

};

typedef struct _LinkBaseService_upload_download_task_args__isset {
  _LinkBaseService_upload_download_task_args__isset() : downloaded_body_item(false) {}
  bool downloaded_body_item;
} _LinkBaseService_upload_download_task_args__isset;

class LinkBaseService_upload_download_task_args {
 public:

  LinkBaseService_upload_download_task_args() {
  }

  virtual ~LinkBaseService_upload_download_task_args() throw() {}

  DownloadedBodyItem downloaded_body_item;

  _LinkBaseService_upload_download_task_args__isset __isset;

  void __set_downloaded_body_item(const DownloadedBodyItem& val) {
    downloaded_body_item = val;
  }

  bool operator == (const LinkBaseService_upload_download_task_args & rhs) const
  {
    if (!(downloaded_body_item == rhs.downloaded_body_item))
      return false;
    return true;
  }
  bool operator != (const LinkBaseService_upload_download_task_args &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const LinkBaseService_upload_download_task_args & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};


class LinkBaseService_upload_download_task_pargs {
 public:


  virtual ~LinkBaseService_upload_download_task_pargs() throw() {}

  const DownloadedBodyItem* downloaded_body_item;

  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};


class LinkBaseService_upload_download_task_result {
 public:

  LinkBaseService_upload_download_task_result() {
  }

  virtual ~LinkBaseService_upload_download_task_result() throw() {}


  bool operator == (const LinkBaseService_upload_download_task_result & /* rhs */) const
  {
    return true;
  }
  bool operator != (const LinkBaseService_upload_download_task_result &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const LinkBaseService_upload_download_task_result & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};


class LinkBaseService_upload_download_task_presult {
 public:


  virtual ~LinkBaseService_upload_download_task_presult() throw() {}


  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);

};

typedef struct _LinkBaseService_upload_extract_task_args__isset {
  _LinkBaseService_upload_extract_task_args__isset() : extract_item(false), matched_result_item(false) {}
  bool extract_item;
  bool matched_result_item;
} _LinkBaseService_upload_extract_task_args__isset;

class LinkBaseService_upload_extract_task_args {
 public:

  LinkBaseService_upload_extract_task_args() {
  }

  virtual ~LinkBaseService_upload_extract_task_args() throw() {}

  ExtractItem extract_item;
  MatchedResultItem matched_result_item;

  _LinkBaseService_upload_extract_task_args__isset __isset;

  void __set_extract_item(const ExtractItem& val) {
    extract_item = val;
  }

  void __set_matched_result_item(const MatchedResultItem& val) {
    matched_result_item = val;
  }

  bool operator == (const LinkBaseService_upload_extract_task_args & rhs) const
  {
    if (!(extract_item == rhs.extract_item))
      return false;
    if (!(matched_result_item == rhs.matched_result_item))
      return false;
    return true;
  }
  bool operator != (const LinkBaseService_upload_extract_task_args &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const LinkBaseService_upload_extract_task_args & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};


class LinkBaseService_upload_extract_task_pargs {
 public:


  virtual ~LinkBaseService_upload_extract_task_pargs() throw() {}

  const ExtractItem* extract_item;
  const MatchedResultItem* matched_result_item;

  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};


class LinkBaseService_upload_extract_task_result {
 public:

  LinkBaseService_upload_extract_task_result() {
  }

  virtual ~LinkBaseService_upload_extract_task_result() throw() {}


  bool operator == (const LinkBaseService_upload_extract_task_result & /* rhs */) const
  {
    return true;
  }
  bool operator != (const LinkBaseService_upload_extract_task_result &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const LinkBaseService_upload_extract_task_result & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};


class LinkBaseService_upload_extract_task_presult {
 public:


  virtual ~LinkBaseService_upload_extract_task_presult() throw() {}


  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);

};

class LinkBaseServiceClient : virtual public LinkBaseServiceIf {
 public:
  LinkBaseServiceClient(boost::shared_ptr< ::apache::thrift::protocol::TProtocol> prot) :
    piprot_(prot),
    poprot_(prot) {
    iprot_ = prot.get();
    oprot_ = prot.get();
  }
  LinkBaseServiceClient(boost::shared_ptr< ::apache::thrift::protocol::TProtocol> iprot, boost::shared_ptr< ::apache::thrift::protocol::TProtocol> oprot) :
    piprot_(iprot),
    poprot_(oprot) {
    iprot_ = iprot.get();
    oprot_ = oprot.get();
  }
  boost::shared_ptr< ::apache::thrift::protocol::TProtocol> getInputProtocol() {
    return piprot_;
  }
  boost::shared_ptr< ::apache::thrift::protocol::TProtocol> getOutputProtocol() {
    return poprot_;
  }
  void load_seed_by_id(const int32_t id, const bool is_add_link);
  void send_load_seed_by_id(const int32_t id, const bool is_add_link);
  void recv_load_seed_by_id();
  void load_seed_by_url(const std::string& seed_url, const bool is_add_link);
  void send_load_seed_by_url(const std::string& seed_url, const bool is_add_link);
  void recv_load_seed_by_url();
  void get_download_task(std::vector<DownloadTask> & _return);
  void send_get_download_task();
  void recv_get_download_task(std::vector<DownloadTask> & _return);
  void get_extract_task(std::vector<ExtractItem> & _return);
  void send_get_extract_task();
  void recv_get_extract_task(std::vector<ExtractItem> & _return);
  void upload_download_task(const DownloadedBodyItem& downloaded_body_item);
  void send_upload_download_task(const DownloadedBodyItem& downloaded_body_item);
  void recv_upload_download_task();
  void upload_extract_task(const ExtractItem& extract_item, const MatchedResultItem& matched_result_item);
  void send_upload_extract_task(const ExtractItem& extract_item, const MatchedResultItem& matched_result_item);
  void recv_upload_extract_task();
 protected:
  boost::shared_ptr< ::apache::thrift::protocol::TProtocol> piprot_;
  boost::shared_ptr< ::apache::thrift::protocol::TProtocol> poprot_;
  ::apache::thrift::protocol::TProtocol* iprot_;
  ::apache::thrift::protocol::TProtocol* oprot_;
};

class LinkBaseServiceProcessor : public ::apache::thrift::TDispatchProcessor {
 protected:
  boost::shared_ptr<LinkBaseServiceIf> iface_;
  virtual bool dispatchCall(::apache::thrift::protocol::TProtocol* iprot, ::apache::thrift::protocol::TProtocol* oprot, const std::string& fname, int32_t seqid, void* callContext);
 private:
  typedef  void (LinkBaseServiceProcessor::*ProcessFunction)(int32_t, ::apache::thrift::protocol::TProtocol*, ::apache::thrift::protocol::TProtocol*, void*);
  typedef std::map<std::string, ProcessFunction> ProcessMap;
  ProcessMap processMap_;
  void process_load_seed_by_id(int32_t seqid, ::apache::thrift::protocol::TProtocol* iprot, ::apache::thrift::protocol::TProtocol* oprot, void* callContext);
  void process_load_seed_by_url(int32_t seqid, ::apache::thrift::protocol::TProtocol* iprot, ::apache::thrift::protocol::TProtocol* oprot, void* callContext);
  void process_get_download_task(int32_t seqid, ::apache::thrift::protocol::TProtocol* iprot, ::apache::thrift::protocol::TProtocol* oprot, void* callContext);
  void process_get_extract_task(int32_t seqid, ::apache::thrift::protocol::TProtocol* iprot, ::apache::thrift::protocol::TProtocol* oprot, void* callContext);
  void process_upload_download_task(int32_t seqid, ::apache::thrift::protocol::TProtocol* iprot, ::apache::thrift::protocol::TProtocol* oprot, void* callContext);
  void process_upload_extract_task(int32_t seqid, ::apache::thrift::protocol::TProtocol* iprot, ::apache::thrift::protocol::TProtocol* oprot, void* callContext);
 public:
  LinkBaseServiceProcessor(boost::shared_ptr<LinkBaseServiceIf> iface) :
    iface_(iface) {
    processMap_["load_seed_by_id"] = &LinkBaseServiceProcessor::process_load_seed_by_id;
    processMap_["load_seed_by_url"] = &LinkBaseServiceProcessor::process_load_seed_by_url;
    processMap_["get_download_task"] = &LinkBaseServiceProcessor::process_get_download_task;
    processMap_["get_extract_task"] = &LinkBaseServiceProcessor::process_get_extract_task;
    processMap_["upload_download_task"] = &LinkBaseServiceProcessor::process_upload_download_task;
    processMap_["upload_extract_task"] = &LinkBaseServiceProcessor::process_upload_extract_task;
  }

  virtual ~LinkBaseServiceProcessor() {}
};

class LinkBaseServiceProcessorFactory : public ::apache::thrift::TProcessorFactory {
 public:
  LinkBaseServiceProcessorFactory(const ::boost::shared_ptr< LinkBaseServiceIfFactory >& handlerFactory) :
      handlerFactory_(handlerFactory) {}

  ::boost::shared_ptr< ::apache::thrift::TProcessor > getProcessor(const ::apache::thrift::TConnectionInfo& connInfo);

 protected:
  ::boost::shared_ptr< LinkBaseServiceIfFactory > handlerFactory_;
};

class LinkBaseServiceMultiface : virtual public LinkBaseServiceIf {
 public:
  LinkBaseServiceMultiface(std::vector<boost::shared_ptr<LinkBaseServiceIf> >& ifaces) : ifaces_(ifaces) {
  }
  virtual ~LinkBaseServiceMultiface() {}
 protected:
  std::vector<boost::shared_ptr<LinkBaseServiceIf> > ifaces_;
  LinkBaseServiceMultiface() {}
  void add(boost::shared_ptr<LinkBaseServiceIf> iface) {
    ifaces_.push_back(iface);
  }
 public:
  void load_seed_by_id(const int32_t id, const bool is_add_link) {
    size_t sz = ifaces_.size();
    size_t i = 0;
    for (; i < (sz - 1); ++i) {
      ifaces_[i]->load_seed_by_id(id, is_add_link);
    }
    ifaces_[i]->load_seed_by_id(id, is_add_link);
  }

  void load_seed_by_url(const std::string& seed_url, const bool is_add_link) {
    size_t sz = ifaces_.size();
    size_t i = 0;
    for (; i < (sz - 1); ++i) {
      ifaces_[i]->load_seed_by_url(seed_url, is_add_link);
    }
    ifaces_[i]->load_seed_by_url(seed_url, is_add_link);
  }

  void get_download_task(std::vector<DownloadTask> & _return) {
    size_t sz = ifaces_.size();
    size_t i = 0;
    for (; i < (sz - 1); ++i) {
      ifaces_[i]->get_download_task(_return);
    }
    ifaces_[i]->get_download_task(_return);
    return;
  }

  void get_extract_task(std::vector<ExtractItem> & _return) {
    size_t sz = ifaces_.size();
    size_t i = 0;
    for (; i < (sz - 1); ++i) {
      ifaces_[i]->get_extract_task(_return);
    }
    ifaces_[i]->get_extract_task(_return);
    return;
  }

  void upload_download_task(const DownloadedBodyItem& downloaded_body_item) {
    size_t sz = ifaces_.size();
    size_t i = 0;
    for (; i < (sz - 1); ++i) {
      ifaces_[i]->upload_download_task(downloaded_body_item);
    }
    ifaces_[i]->upload_download_task(downloaded_body_item);
  }

  void upload_extract_task(const ExtractItem& extract_item, const MatchedResultItem& matched_result_item) {
    size_t sz = ifaces_.size();
    size_t i = 0;
    for (; i < (sz - 1); ++i) {
      ifaces_[i]->upload_extract_task(extract_item, matched_result_item);
    }
    ifaces_[i]->upload_extract_task(extract_item, matched_result_item);
  }

};



#endif