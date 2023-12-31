// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: content_handle_types.proto

#ifndef PROTOBUF_content_5fhandle_5ftypes_2eproto__INCLUDED
#define PROTOBUF_content_5fhandle_5ftypes_2eproto__INCLUDED

#include <string>

#include <google/protobuf/stubs/common.h>

#if GOOGLE_PROTOBUF_VERSION < 2006000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please update
#error your headers.
#endif
#if 2006001 < GOOGLE_PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/message.h>
#include <google/protobuf/repeated_field.h>
#include <google/protobuf/extension_set.h>
#include <google/protobuf/unknown_field_set.h>
#include "Define.h" // for TC_PROTO_API
// @@protoc_insertion_point(includes)

namespace bgs {
namespace protocol {

// Internal implementation detail -- do not call these.
void TC_PROTO_API protobuf_AddDesc_content_5fhandle_5ftypes_2eproto();
void protobuf_AssignDesc_content_5fhandle_5ftypes_2eproto();
void protobuf_ShutdownFile_content_5fhandle_5ftypes_2eproto();

class ContentHandle;
class TitleIconContentHandle;

// ===================================================================

class TC_PROTO_API ContentHandle : public ::google::protobuf::Message {
 public:
  ContentHandle();
  virtual ~ContentHandle();

  ContentHandle(const ContentHandle& from);

  inline ContentHandle& operator=(const ContentHandle& from) {
    CopyFrom(from);
    return *this;
  }

  inline const ::google::protobuf::UnknownFieldSet& unknown_fields() const {
    return _unknown_fields_;
  }

  inline ::google::protobuf::UnknownFieldSet* mutable_unknown_fields() {
    return &_unknown_fields_;
  }

  static const ::google::protobuf::Descriptor* descriptor();
  static const ContentHandle& default_instance();

  void Swap(ContentHandle* other);

  // implements Message ----------------------------------------------

  ContentHandle* New() const;
  int GetCachedSize() const { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const;
  public:
  ::google::protobuf::Metadata GetMetadata() const;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // required fixed32 region = 1;
  inline bool has_region() const;
  inline void clear_region();
  static const int kRegionFieldNumber = 1;
  inline ::google::protobuf::uint32 region() const;
  inline void set_region(::google::protobuf::uint32 value);

  // required fixed32 usage = 2;
  inline bool has_usage() const;
  inline void clear_usage();
  static const int kUsageFieldNumber = 2;
  inline ::google::protobuf::uint32 usage() const;
  inline void set_usage(::google::protobuf::uint32 value);

  // required bytes hash = 3;
  inline bool has_hash() const;
  inline void clear_hash();
  static const int kHashFieldNumber = 3;
  inline const ::std::string& hash() const;
  inline void set_hash(const ::std::string& value);
  inline void set_hash(const char* value);
  inline void set_hash(const void* value, size_t size);
  inline ::std::string* mutable_hash();
  inline ::std::string* release_hash();
  inline void set_allocated_hash(::std::string* hash);

  // optional string proto_url = 4;
  inline bool has_proto_url() const;
  inline void clear_proto_url();
  static const int kProtoUrlFieldNumber = 4;
  inline const ::std::string& proto_url() const;
  inline void set_proto_url(const ::std::string& value);
  inline void set_proto_url(const char* value);
  inline void set_proto_url(const char* value, size_t size);
  inline ::std::string* mutable_proto_url();
  inline ::std::string* release_proto_url();
  inline void set_allocated_proto_url(::std::string* proto_url);

  // @@protoc_insertion_point(class_scope:bgs.protocol.ContentHandle)
 private:
  inline void set_has_region();
  inline void clear_has_region();
  inline void set_has_usage();
  inline void clear_has_usage();
  inline void set_has_hash();
  inline void clear_has_hash();
  inline void set_has_proto_url();
  inline void clear_has_proto_url();

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::google::protobuf::uint32 _has_bits_[1];
  mutable int _cached_size_;
  ::google::protobuf::uint32 region_;
  ::google::protobuf::uint32 usage_;
  ::std::string* hash_;
  ::std::string* proto_url_;
  friend void TC_PROTO_API protobuf_AddDesc_content_5fhandle_5ftypes_2eproto();
  friend void protobuf_AssignDesc_content_5fhandle_5ftypes_2eproto();
  friend void protobuf_ShutdownFile_content_5fhandle_5ftypes_2eproto();

  void InitAsDefaultInstance();
  static ContentHandle* default_instance_;
};
// -------------------------------------------------------------------

class TC_PROTO_API TitleIconContentHandle : public ::google::protobuf::Message {
 public:
  TitleIconContentHandle();
  virtual ~TitleIconContentHandle();

  TitleIconContentHandle(const TitleIconContentHandle& from);

  inline TitleIconContentHandle& operator=(const TitleIconContentHandle& from) {
    CopyFrom(from);
    return *this;
  }

  inline const ::google::protobuf::UnknownFieldSet& unknown_fields() const {
    return _unknown_fields_;
  }

  inline ::google::protobuf::UnknownFieldSet* mutable_unknown_fields() {
    return &_unknown_fields_;
  }

  static const ::google::protobuf::Descriptor* descriptor();
  static const TitleIconContentHandle& default_instance();

  void Swap(TitleIconContentHandle* other);

  // implements Message ----------------------------------------------

  TitleIconContentHandle* New() const;
  int GetCachedSize() const { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const;
  public:
  ::google::protobuf::Metadata GetMetadata() const;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // optional uint32 title_id = 1;
  inline bool has_title_id() const;
  inline void clear_title_id();
  static const int kTitleIdFieldNumber = 1;
  inline ::google::protobuf::uint32 title_id() const;
  inline void set_title_id(::google::protobuf::uint32 value);

  // optional .bgs.protocol.ContentHandle content_handle = 2;
  inline bool has_content_handle() const;
  inline void clear_content_handle();
  static const int kContentHandleFieldNumber = 2;
  inline const ::bgs::protocol::ContentHandle& content_handle() const;
  inline ::bgs::protocol::ContentHandle* mutable_content_handle();
  inline ::bgs::protocol::ContentHandle* release_content_handle();
  inline void set_allocated_content_handle(::bgs::protocol::ContentHandle* content_handle);

  // @@protoc_insertion_point(class_scope:bgs.protocol.TitleIconContentHandle)
 private:
  inline void set_has_title_id();
  inline void clear_has_title_id();
  inline void set_has_content_handle();
  inline void clear_has_content_handle();

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::google::protobuf::uint32 _has_bits_[1];
  mutable int _cached_size_;
  ::bgs::protocol::ContentHandle* content_handle_;
  ::google::protobuf::uint32 title_id_;
  friend void TC_PROTO_API protobuf_AddDesc_content_5fhandle_5ftypes_2eproto();
  friend void protobuf_AssignDesc_content_5fhandle_5ftypes_2eproto();
  friend void protobuf_ShutdownFile_content_5fhandle_5ftypes_2eproto();

  void InitAsDefaultInstance();
  static TitleIconContentHandle* default_instance_;
};
// ===================================================================

// ===================================================================

// ===================================================================

// ContentHandle

// required fixed32 region = 1;
inline bool ContentHandle::has_region() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void ContentHandle::set_has_region() {
  _has_bits_[0] |= 0x00000001u;
}
inline void ContentHandle::clear_has_region() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void ContentHandle::clear_region() {
  region_ = 0u;
  clear_has_region();
}
inline ::google::protobuf::uint32 ContentHandle::region() const {
  // @@protoc_insertion_point(field_get:bgs.protocol.ContentHandle.region)
  return region_;
}
inline void ContentHandle::set_region(::google::protobuf::uint32 value) {
  set_has_region();
  region_ = value;
  // @@protoc_insertion_point(field_set:bgs.protocol.ContentHandle.region)
}

// required fixed32 usage = 2;
inline bool ContentHandle::has_usage() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void ContentHandle::set_has_usage() {
  _has_bits_[0] |= 0x00000002u;
}
inline void ContentHandle::clear_has_usage() {
  _has_bits_[0] &= ~0x00000002u;
}
inline void ContentHandle::clear_usage() {
  usage_ = 0u;
  clear_has_usage();
}
inline ::google::protobuf::uint32 ContentHandle::usage() const {
  // @@protoc_insertion_point(field_get:bgs.protocol.ContentHandle.usage)
  return usage_;
}
inline void ContentHandle::set_usage(::google::protobuf::uint32 value) {
  set_has_usage();
  usage_ = value;
  // @@protoc_insertion_point(field_set:bgs.protocol.ContentHandle.usage)
}

// required bytes hash = 3;
inline bool ContentHandle::has_hash() const {
  return (_has_bits_[0] & 0x00000004u) != 0;
}
inline void ContentHandle::set_has_hash() {
  _has_bits_[0] |= 0x00000004u;
}
inline void ContentHandle::clear_has_hash() {
  _has_bits_[0] &= ~0x00000004u;
}
inline void ContentHandle::clear_hash() {
  if (hash_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    hash_->clear();
  }
  clear_has_hash();
}
inline const ::std::string& ContentHandle::hash() const {
  // @@protoc_insertion_point(field_get:bgs.protocol.ContentHandle.hash)
  return *hash_;
}
inline void ContentHandle::set_hash(const ::std::string& value) {
  set_has_hash();
  if (hash_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    hash_ = new ::std::string;
  }
  hash_->assign(value);
  // @@protoc_insertion_point(field_set:bgs.protocol.ContentHandle.hash)
}
inline void ContentHandle::set_hash(const char* value) {
  set_has_hash();
  if (hash_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    hash_ = new ::std::string;
  }
  hash_->assign(value);
  // @@protoc_insertion_point(field_set_char:bgs.protocol.ContentHandle.hash)
}
inline void ContentHandle::set_hash(const void* value, size_t size) {
  set_has_hash();
  if (hash_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    hash_ = new ::std::string;
  }
  hash_->assign(reinterpret_cast<const char*>(value), size);
  // @@protoc_insertion_point(field_set_pointer:bgs.protocol.ContentHandle.hash)
}
inline ::std::string* ContentHandle::mutable_hash() {
  set_has_hash();
  if (hash_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    hash_ = new ::std::string;
  }
  // @@protoc_insertion_point(field_mutable:bgs.protocol.ContentHandle.hash)
  return hash_;
}
inline ::std::string* ContentHandle::release_hash() {
  clear_has_hash();
  if (hash_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    return NULL;
  } else {
    ::std::string* temp = hash_;
    hash_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
    return temp;
  }
}
inline void ContentHandle::set_allocated_hash(::std::string* hash) {
  if (hash_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    delete hash_;
  }
  if (hash) {
    set_has_hash();
    hash_ = hash;
  } else {
    clear_has_hash();
    hash_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  }
  // @@protoc_insertion_point(field_set_allocated:bgs.protocol.ContentHandle.hash)
}

// optional string proto_url = 4;
inline bool ContentHandle::has_proto_url() const {
  return (_has_bits_[0] & 0x00000008u) != 0;
}
inline void ContentHandle::set_has_proto_url() {
  _has_bits_[0] |= 0x00000008u;
}
inline void ContentHandle::clear_has_proto_url() {
  _has_bits_[0] &= ~0x00000008u;
}
inline void ContentHandle::clear_proto_url() {
  if (proto_url_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    proto_url_->clear();
  }
  clear_has_proto_url();
}
inline const ::std::string& ContentHandle::proto_url() const {
  // @@protoc_insertion_point(field_get:bgs.protocol.ContentHandle.proto_url)
  return *proto_url_;
}
inline void ContentHandle::set_proto_url(const ::std::string& value) {
  set_has_proto_url();
  if (proto_url_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    proto_url_ = new ::std::string;
  }
  proto_url_->assign(value);
  // @@protoc_insertion_point(field_set:bgs.protocol.ContentHandle.proto_url)
}
inline void ContentHandle::set_proto_url(const char* value) {
  set_has_proto_url();
  if (proto_url_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    proto_url_ = new ::std::string;
  }
  proto_url_->assign(value);
  // @@protoc_insertion_point(field_set_char:bgs.protocol.ContentHandle.proto_url)
}
inline void ContentHandle::set_proto_url(const char* value, size_t size) {
  set_has_proto_url();
  if (proto_url_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    proto_url_ = new ::std::string;
  }
  proto_url_->assign(reinterpret_cast<const char*>(value), size);
  // @@protoc_insertion_point(field_set_pointer:bgs.protocol.ContentHandle.proto_url)
}
inline ::std::string* ContentHandle::mutable_proto_url() {
  set_has_proto_url();
  if (proto_url_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    proto_url_ = new ::std::string;
  }
  // @@protoc_insertion_point(field_mutable:bgs.protocol.ContentHandle.proto_url)
  return proto_url_;
}
inline ::std::string* ContentHandle::release_proto_url() {
  clear_has_proto_url();
  if (proto_url_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    return NULL;
  } else {
    ::std::string* temp = proto_url_;
    proto_url_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
    return temp;
  }
}
inline void ContentHandle::set_allocated_proto_url(::std::string* proto_url) {
  if (proto_url_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    delete proto_url_;
  }
  if (proto_url) {
    set_has_proto_url();
    proto_url_ = proto_url;
  } else {
    clear_has_proto_url();
    proto_url_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  }
  // @@protoc_insertion_point(field_set_allocated:bgs.protocol.ContentHandle.proto_url)
}

// -------------------------------------------------------------------

// TitleIconContentHandle

// optional uint32 title_id = 1;
inline bool TitleIconContentHandle::has_title_id() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void TitleIconContentHandle::set_has_title_id() {
  _has_bits_[0] |= 0x00000001u;
}
inline void TitleIconContentHandle::clear_has_title_id() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void TitleIconContentHandle::clear_title_id() {
  title_id_ = 0u;
  clear_has_title_id();
}
inline ::google::protobuf::uint32 TitleIconContentHandle::title_id() const {
  // @@protoc_insertion_point(field_get:bgs.protocol.TitleIconContentHandle.title_id)
  return title_id_;
}
inline void TitleIconContentHandle::set_title_id(::google::protobuf::uint32 value) {
  set_has_title_id();
  title_id_ = value;
  // @@protoc_insertion_point(field_set:bgs.protocol.TitleIconContentHandle.title_id)
}

// optional .bgs.protocol.ContentHandle content_handle = 2;
inline bool TitleIconContentHandle::has_content_handle() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void TitleIconContentHandle::set_has_content_handle() {
  _has_bits_[0] |= 0x00000002u;
}
inline void TitleIconContentHandle::clear_has_content_handle() {
  _has_bits_[0] &= ~0x00000002u;
}
inline void TitleIconContentHandle::clear_content_handle() {
  if (content_handle_ != NULL) content_handle_->::bgs::protocol::ContentHandle::Clear();
  clear_has_content_handle();
}
inline const ::bgs::protocol::ContentHandle& TitleIconContentHandle::content_handle() const {
  // @@protoc_insertion_point(field_get:bgs.protocol.TitleIconContentHandle.content_handle)
  return content_handle_ != NULL ? *content_handle_ : *default_instance_->content_handle_;
}
inline ::bgs::protocol::ContentHandle* TitleIconContentHandle::mutable_content_handle() {
  set_has_content_handle();
  if (content_handle_ == NULL) content_handle_ = new ::bgs::protocol::ContentHandle;
  // @@protoc_insertion_point(field_mutable:bgs.protocol.TitleIconContentHandle.content_handle)
  return content_handle_;
}
inline ::bgs::protocol::ContentHandle* TitleIconContentHandle::release_content_handle() {
  clear_has_content_handle();
  ::bgs::protocol::ContentHandle* temp = content_handle_;
  content_handle_ = NULL;
  return temp;
}
inline void TitleIconContentHandle::set_allocated_content_handle(::bgs::protocol::ContentHandle* content_handle) {
  delete content_handle_;
  content_handle_ = content_handle;
  if (content_handle) {
    set_has_content_handle();
  } else {
    clear_has_content_handle();
  }
  // @@protoc_insertion_point(field_set_allocated:bgs.protocol.TitleIconContentHandle.content_handle)
}

// @@protoc_insertion_point(namespace_scope)

}  // namespace protocol
}  // namespace bgs

#ifndef SWIG
namespace google {
namespace protobuf {
}  // namespace google
}  // namespace protobuf
#endif  // SWIG

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_content_5fhandle_5ftypes_2eproto__INCLUDED
