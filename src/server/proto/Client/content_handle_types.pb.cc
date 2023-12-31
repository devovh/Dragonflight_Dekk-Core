// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: content_handle_types.proto

#define INTERNAL_SUPPRESS_PROTOBUF_FIELD_DEPRECATION
#include "content_handle_types.pb.h"

#include <utility>

#include <google/protobuf/stubs/common.h>
#include <google/protobuf/stubs/once.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/wire_format_lite_inl.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/generated_message_reflection.h>
#include <google/protobuf/reflection_ops.h>
#include <google/protobuf/wire_format.h>
// @@protoc_insertion_point(includes)

namespace bgs {
namespace protocol {

namespace {

const ::google::protobuf::Descriptor* ContentHandle_descriptor_ = NULL;
const ::google::protobuf::internal::GeneratedMessageReflection*
  ContentHandle_reflection_ = NULL;
const ::google::protobuf::Descriptor* TitleIconContentHandle_descriptor_ = NULL;
const ::google::protobuf::internal::GeneratedMessageReflection*
  TitleIconContentHandle_reflection_ = NULL;

}  // namespace

void protobuf_AssignDesc_content_5fhandle_5ftypes_2eproto() {
  protobuf_AddDesc_content_5fhandle_5ftypes_2eproto();
  const ::google::protobuf::FileDescriptor* file =
    ::google::protobuf::DescriptorPool::generated_pool()->FindFileByName(
      "content_handle_types.proto");
  GOOGLE_CHECK(file != NULL);
  ContentHandle_descriptor_ = file->message_type(0);
  static const int ContentHandle_offsets_[4] = {
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(ContentHandle, region_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(ContentHandle, usage_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(ContentHandle, hash_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(ContentHandle, proto_url_),
  };
  ContentHandle_reflection_ =
    new ::google::protobuf::internal::GeneratedMessageReflection(
      ContentHandle_descriptor_,
      ContentHandle::default_instance_,
      ContentHandle_offsets_,
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(ContentHandle, _has_bits_[0]),
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(ContentHandle, _unknown_fields_),
      -1,
      ::google::protobuf::DescriptorPool::generated_pool(),
      ::google::protobuf::MessageFactory::generated_factory(),
      sizeof(ContentHandle));
  TitleIconContentHandle_descriptor_ = file->message_type(1);
  static const int TitleIconContentHandle_offsets_[2] = {
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(TitleIconContentHandle, title_id_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(TitleIconContentHandle, content_handle_),
  };
  TitleIconContentHandle_reflection_ =
    new ::google::protobuf::internal::GeneratedMessageReflection(
      TitleIconContentHandle_descriptor_,
      TitleIconContentHandle::default_instance_,
      TitleIconContentHandle_offsets_,
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(TitleIconContentHandle, _has_bits_[0]),
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(TitleIconContentHandle, _unknown_fields_),
      -1,
      ::google::protobuf::DescriptorPool::generated_pool(),
      ::google::protobuf::MessageFactory::generated_factory(),
      sizeof(TitleIconContentHandle));
}

namespace {

GOOGLE_PROTOBUF_DECLARE_ONCE(protobuf_AssignDescriptors_once_);
inline void protobuf_AssignDescriptorsOnce() {
  ::google::protobuf::GoogleOnceInit(&protobuf_AssignDescriptors_once_,
                 &protobuf_AssignDesc_content_5fhandle_5ftypes_2eproto);
}

void protobuf_RegisterTypes(const ::std::string&) {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedMessage(
    ContentHandle_descriptor_, &ContentHandle::default_instance());
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedMessage(
    TitleIconContentHandle_descriptor_, &TitleIconContentHandle::default_instance());
}

}  // namespace

void protobuf_ShutdownFile_content_5fhandle_5ftypes_2eproto() {
  delete ContentHandle::default_instance_;
  delete ContentHandle_reflection_;
  delete TitleIconContentHandle::default_instance_;
  delete TitleIconContentHandle_reflection_;
}

void protobuf_AddDesc_content_5fhandle_5ftypes_2eproto() {
  static bool already_here = false;
  if (already_here) return;
  already_here = true;
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  ::google::protobuf::DescriptorPool::InternalAddGeneratedFile(
    "\n\032content_handle_types.proto\022\014bgs.protoc"
    "ol\"O\n\rContentHandle\022\016\n\006region\030\001 \002(\007\022\r\n\005u"
    "sage\030\002 \002(\007\022\014\n\004hash\030\003 \002(\014\022\021\n\tproto_url\030\004 "
    "\001(\t\"_\n\026TitleIconContentHandle\022\020\n\010title_i"
    "d\030\001 \001(\r\0223\n\016content_handle\030\002 \001(\0132\033.bgs.pr"
    "otocol.ContentHandleB$\n\014bgs.protocolB\022Co"
    "ntentHandleProtoH\002", 258);
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedFile(
    "content_handle_types.proto", &protobuf_RegisterTypes);
  ContentHandle::default_instance_ = new ContentHandle();
  TitleIconContentHandle::default_instance_ = new TitleIconContentHandle();
  ContentHandle::default_instance_->InitAsDefaultInstance();
  TitleIconContentHandle::default_instance_->InitAsDefaultInstance();
  ::google::protobuf::internal::OnShutdown(&protobuf_ShutdownFile_content_5fhandle_5ftypes_2eproto);
}

// Force AddDescriptors() to be called at static initialization time.
struct StaticDescriptorInitializer_content_5fhandle_5ftypes_2eproto {
  StaticDescriptorInitializer_content_5fhandle_5ftypes_2eproto() {
    protobuf_AddDesc_content_5fhandle_5ftypes_2eproto();
  }
} static_descriptor_initializer_content_5fhandle_5ftypes_2eproto_;
// ===================================================================

#ifndef _MSC_VER
const int ContentHandle::kRegionFieldNumber;
const int ContentHandle::kUsageFieldNumber;
const int ContentHandle::kHashFieldNumber;
const int ContentHandle::kProtoUrlFieldNumber;
#endif  // !_MSC_VER

ContentHandle::ContentHandle()
  : ::google::protobuf::Message() {
  SharedCtor();
  // @@protoc_insertion_point(constructor:bgs.protocol.ContentHandle)
}

void ContentHandle::InitAsDefaultInstance() {
}

ContentHandle::ContentHandle(const ContentHandle& from)
  : ::google::protobuf::Message() {
  SharedCtor();
  MergeFrom(from);
  // @@protoc_insertion_point(copy_constructor:bgs.protocol.ContentHandle)
}

void ContentHandle::SharedCtor() {
  ::google::protobuf::internal::GetEmptyString();
  _cached_size_ = 0;
  region_ = 0u;
  usage_ = 0u;
  hash_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  proto_url_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
}

ContentHandle::~ContentHandle() {
  // @@protoc_insertion_point(destructor:bgs.protocol.ContentHandle)
  SharedDtor();
}

void ContentHandle::SharedDtor() {
  if (hash_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    delete hash_;
  }
  if (proto_url_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    delete proto_url_;
  }
  if (this != default_instance_) {
  }
}

void ContentHandle::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const ::google::protobuf::Descriptor* ContentHandle::descriptor() {
  protobuf_AssignDescriptorsOnce();
  return ContentHandle_descriptor_;
}

const ContentHandle& ContentHandle::default_instance() {
  if (default_instance_ == NULL) protobuf_AddDesc_content_5fhandle_5ftypes_2eproto();
  return *default_instance_;
}

ContentHandle* ContentHandle::default_instance_ = NULL;

ContentHandle* ContentHandle::New() const {
  return new ContentHandle;
}

void ContentHandle::Swap(ContentHandle* other) {
  if (other != this) {
    GetReflection()->Swap(this, other);}
}

::google::protobuf::Metadata ContentHandle::GetMetadata() const {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::Metadata metadata;
  metadata.descriptor = ContentHandle_descriptor_;
  metadata.reflection = ContentHandle_reflection_;
  return metadata;
}

// ===================================================================

#ifndef _MSC_VER
const int TitleIconContentHandle::kTitleIdFieldNumber;
const int TitleIconContentHandle::kContentHandleFieldNumber;
#endif  // !_MSC_VER

TitleIconContentHandle::TitleIconContentHandle()
  : ::google::protobuf::Message() {
  SharedCtor();
  // @@protoc_insertion_point(constructor:bgs.protocol.TitleIconContentHandle)
}

void TitleIconContentHandle::InitAsDefaultInstance() {
  content_handle_ = const_cast< ::bgs::protocol::ContentHandle*>(&::bgs::protocol::ContentHandle::default_instance());
}

TitleIconContentHandle::TitleIconContentHandle(const TitleIconContentHandle& from)
  : ::google::protobuf::Message() {
  SharedCtor();
  MergeFrom(from);
  // @@protoc_insertion_point(copy_constructor:bgs.protocol.TitleIconContentHandle)
}

void TitleIconContentHandle::SharedCtor() {
  _cached_size_ = 0;
  title_id_ = 0u;
  content_handle_ = NULL;
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
}

TitleIconContentHandle::~TitleIconContentHandle() {
  // @@protoc_insertion_point(destructor:bgs.protocol.TitleIconContentHandle)
  SharedDtor();
}

void TitleIconContentHandle::SharedDtor() {
  if (this != default_instance_) {
    delete content_handle_;
  }
}

void TitleIconContentHandle::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const ::google::protobuf::Descriptor* TitleIconContentHandle::descriptor() {
  protobuf_AssignDescriptorsOnce();
  return TitleIconContentHandle_descriptor_;
}

const TitleIconContentHandle& TitleIconContentHandle::default_instance() {
  if (default_instance_ == NULL) protobuf_AddDesc_content_5fhandle_5ftypes_2eproto();
  return *default_instance_;
}

TitleIconContentHandle* TitleIconContentHandle::default_instance_ = NULL;

TitleIconContentHandle* TitleIconContentHandle::New() const {
  return new TitleIconContentHandle;
}

void TitleIconContentHandle::Swap(TitleIconContentHandle* other) {
  if (other != this) {
    GetReflection()->Swap(this, other);}
}

::google::protobuf::Metadata TitleIconContentHandle::GetMetadata() const {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::Metadata metadata;
  metadata.descriptor = TitleIconContentHandle_descriptor_;
  metadata.reflection = TitleIconContentHandle_reflection_;
  return metadata;
}

// @@protoc_insertion_point(namespace_scope)

}  // namespace protocol
}  // namespace bgs

// @@protoc_insertion_point(global_scope)
