// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: api/client/v1/channel_id.proto

#define INTERNAL_SUPPRESS_PROTOBUF_FIELD_DEPRECATION
#include "api/client/v1/channel_id.pb.h"

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
namespace channel {
namespace v1 {

namespace {

const ::google::protobuf::Descriptor* ChannelId_descriptor_ = NULL;
const ::google::protobuf::internal::GeneratedMessageReflection*
  ChannelId_reflection_ = NULL;

}  // namespace

void protobuf_AssignDesc_api_2fclient_2fv1_2fchannel_5fid_2eproto() {
  protobuf_AddDesc_api_2fclient_2fv1_2fchannel_5fid_2eproto();
  const ::google::protobuf::FileDescriptor* file =
    ::google::protobuf::DescriptorPool::generated_pool()->FindFileByName(
      "api/client/v1/channel_id.proto");
  GOOGLE_CHECK(file != NULL);
  ChannelId_descriptor_ = file->message_type(0);
  static const int ChannelId_offsets_[4] = {
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(ChannelId, type_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(ChannelId, host_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(ChannelId, id_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(ChannelId, region_),
  };
  ChannelId_reflection_ =
    new ::google::protobuf::internal::GeneratedMessageReflection(
      ChannelId_descriptor_,
      ChannelId::default_instance_,
      ChannelId_offsets_,
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(ChannelId, _has_bits_[0]),
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(ChannelId, _unknown_fields_),
      -1,
      ::google::protobuf::DescriptorPool::generated_pool(),
      ::google::protobuf::MessageFactory::generated_factory(),
      sizeof(ChannelId));
}

namespace {

GOOGLE_PROTOBUF_DECLARE_ONCE(protobuf_AssignDescriptors_once_);
inline void protobuf_AssignDescriptorsOnce() {
  ::google::protobuf::GoogleOnceInit(&protobuf_AssignDescriptors_once_,
                 &protobuf_AssignDesc_api_2fclient_2fv1_2fchannel_5fid_2eproto);
}

void protobuf_RegisterTypes(const ::std::string&) {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedMessage(
    ChannelId_descriptor_, &ChannelId::default_instance());
}

}  // namespace

void protobuf_ShutdownFile_api_2fclient_2fv1_2fchannel_5fid_2eproto() {
  delete ChannelId::default_instance_;
  delete ChannelId_reflection_;
}

void protobuf_AddDesc_api_2fclient_2fv1_2fchannel_5fid_2eproto() {
  static bool already_here = false;
  if (already_here) return;
  already_here = true;
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  ::bgs::protocol::protobuf_AddDesc_rpc_5ftypes_2eproto();
  ::google::protobuf::DescriptorPool::InternalAddGeneratedFile(
    "\n\036api/client/v1/channel_id.proto\022\027bgs.pr"
    "otocol.channel.v1\032\017rpc_types.proto\"d\n\tCh"
    "annelId\022\014\n\004type\030\001 \001(\r\022%\n\004host\030\002 \001(\0132\027.bg"
    "s.protocol.ProcessId\022\n\n\002id\030\003 \001(\007\022\016\n\006regi"
    "on\030\004 \001(\r:\006\202\371+\002\010\001B\002H\002", 180);
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedFile(
    "api/client/v1/channel_id.proto", &protobuf_RegisterTypes);
  ChannelId::default_instance_ = new ChannelId();
  ChannelId::default_instance_->InitAsDefaultInstance();
  ::google::protobuf::internal::OnShutdown(&protobuf_ShutdownFile_api_2fclient_2fv1_2fchannel_5fid_2eproto);
}

// Force AddDescriptors() to be called at static initialization time.
struct StaticDescriptorInitializer_api_2fclient_2fv1_2fchannel_5fid_2eproto {
  StaticDescriptorInitializer_api_2fclient_2fv1_2fchannel_5fid_2eproto() {
    protobuf_AddDesc_api_2fclient_2fv1_2fchannel_5fid_2eproto();
  }
} static_descriptor_initializer_api_2fclient_2fv1_2fchannel_5fid_2eproto_;
// ===================================================================

#ifndef _MSC_VER
const int ChannelId::kTypeFieldNumber;
const int ChannelId::kHostFieldNumber;
const int ChannelId::kIdFieldNumber;
const int ChannelId::kRegionFieldNumber;
#endif  // !_MSC_VER

ChannelId::ChannelId()
  : ::google::protobuf::Message() {
  SharedCtor();
  // @@protoc_insertion_point(constructor:bgs.protocol.channel.v1.ChannelId)
}

void ChannelId::InitAsDefaultInstance() {
  host_ = const_cast< ::bgs::protocol::ProcessId*>(&::bgs::protocol::ProcessId::default_instance());
}

ChannelId::ChannelId(const ChannelId& from)
  : ::google::protobuf::Message() {
  SharedCtor();
  MergeFrom(from);
  // @@protoc_insertion_point(copy_constructor:bgs.protocol.channel.v1.ChannelId)
}

void ChannelId::SharedCtor() {
  _cached_size_ = 0;
  type_ = 0u;
  host_ = NULL;
  id_ = 0u;
  region_ = 0u;
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
}

ChannelId::~ChannelId() {
  // @@protoc_insertion_point(destructor:bgs.protocol.channel.v1.ChannelId)
  SharedDtor();
}

void ChannelId::SharedDtor() {
  if (this != default_instance_) {
    delete host_;
  }
}

void ChannelId::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const ::google::protobuf::Descriptor* ChannelId::descriptor() {
  protobuf_AssignDescriptorsOnce();
  return ChannelId_descriptor_;
}

const ChannelId& ChannelId::default_instance() {
  if (default_instance_ == NULL) protobuf_AddDesc_api_2fclient_2fv1_2fchannel_5fid_2eproto();
  return *default_instance_;
}

ChannelId* ChannelId::default_instance_ = NULL;

ChannelId* ChannelId::New() const {
  return new ChannelId;
}

void ChannelId::Swap(ChannelId* other) {
  if (other != this) {
    GetReflection()->Swap(this, other);}
}

::google::protobuf::Metadata ChannelId::GetMetadata() const {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::Metadata metadata;
  metadata.descriptor = ChannelId_descriptor_;
  metadata.reflection = ChannelId_reflection_;
  return metadata;
}

// @@protoc_insertion_point(namespace_scope)

}  // namespace v1
}  // namespace channel
}  // namespace protocol
}  // namespace bgs

// @@protoc_insertion_point(global_scope)