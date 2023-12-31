// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: attribute_types.proto

#define INTERNAL_SUPPRESS_PROTOBUF_FIELD_DEPRECATION
#include "attribute_types.pb.h"

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

const ::google::protobuf::Descriptor* Variant_descriptor_ = NULL;
const ::google::protobuf::internal::GeneratedMessageReflection*
  Variant_reflection_ = NULL;
const ::google::protobuf::Descriptor* Attribute_descriptor_ = NULL;
const ::google::protobuf::internal::GeneratedMessageReflection*
  Attribute_reflection_ = NULL;
const ::google::protobuf::Descriptor* AttributeFilter_descriptor_ = NULL;
const ::google::protobuf::internal::GeneratedMessageReflection*
  AttributeFilter_reflection_ = NULL;
const ::google::protobuf::EnumDescriptor* AttributeFilter_Operation_descriptor_ = NULL;

}  // namespace

void protobuf_AssignDesc_attribute_5ftypes_2eproto() {
  protobuf_AddDesc_attribute_5ftypes_2eproto();
  const ::google::protobuf::FileDescriptor* file =
    ::google::protobuf::DescriptorPool::generated_pool()->FindFileByName(
      "attribute_types.proto");
  GOOGLE_CHECK(file != NULL);
  Variant_descriptor_ = file->message_type(0);
  static const int Variant_offsets_[9] = {
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Variant, bool_value_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Variant, int_value_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Variant, float_value_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Variant, string_value_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Variant, blob_value_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Variant, message_value_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Variant, fourcc_value_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Variant, uint_value_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Variant, entity_id_value_),
  };
  Variant_reflection_ =
    new ::google::protobuf::internal::GeneratedMessageReflection(
      Variant_descriptor_,
      Variant::default_instance_,
      Variant_offsets_,
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Variant, _has_bits_[0]),
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Variant, _unknown_fields_),
      -1,
      ::google::protobuf::DescriptorPool::generated_pool(),
      ::google::protobuf::MessageFactory::generated_factory(),
      sizeof(Variant));
  Attribute_descriptor_ = file->message_type(1);
  static const int Attribute_offsets_[2] = {
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Attribute, name_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Attribute, value_),
  };
  Attribute_reflection_ =
    new ::google::protobuf::internal::GeneratedMessageReflection(
      Attribute_descriptor_,
      Attribute::default_instance_,
      Attribute_offsets_,
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Attribute, _has_bits_[0]),
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Attribute, _unknown_fields_),
      -1,
      ::google::protobuf::DescriptorPool::generated_pool(),
      ::google::protobuf::MessageFactory::generated_factory(),
      sizeof(Attribute));
  AttributeFilter_descriptor_ = file->message_type(2);
  static const int AttributeFilter_offsets_[2] = {
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(AttributeFilter, op_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(AttributeFilter, attribute_),
  };
  AttributeFilter_reflection_ =
    new ::google::protobuf::internal::GeneratedMessageReflection(
      AttributeFilter_descriptor_,
      AttributeFilter::default_instance_,
      AttributeFilter_offsets_,
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(AttributeFilter, _has_bits_[0]),
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(AttributeFilter, _unknown_fields_),
      -1,
      ::google::protobuf::DescriptorPool::generated_pool(),
      ::google::protobuf::MessageFactory::generated_factory(),
      sizeof(AttributeFilter));
  AttributeFilter_Operation_descriptor_ = AttributeFilter_descriptor_->enum_type(0);
}

namespace {

GOOGLE_PROTOBUF_DECLARE_ONCE(protobuf_AssignDescriptors_once_);
inline void protobuf_AssignDescriptorsOnce() {
  ::google::protobuf::GoogleOnceInit(&protobuf_AssignDescriptors_once_,
                 &protobuf_AssignDesc_attribute_5ftypes_2eproto);
}

void protobuf_RegisterTypes(const ::std::string&) {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedMessage(
    Variant_descriptor_, &Variant::default_instance());
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedMessage(
    Attribute_descriptor_, &Attribute::default_instance());
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedMessage(
    AttributeFilter_descriptor_, &AttributeFilter::default_instance());
}

}  // namespace

void protobuf_ShutdownFile_attribute_5ftypes_2eproto() {
  delete Variant::default_instance_;
  delete Variant_reflection_;
  delete Attribute::default_instance_;
  delete Attribute_reflection_;
  delete AttributeFilter::default_instance_;
  delete AttributeFilter_reflection_;
}

void protobuf_AddDesc_attribute_5ftypes_2eproto() {
  static bool already_here = false;
  if (already_here) return;
  already_here = true;
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  ::bgs::protocol::protobuf_AddDesc_entity_5ftypes_2eproto();
  ::google::protobuf::DescriptorPool::InternalAddGeneratedFile(
    "\n\025attribute_types.proto\022\014bgs.protocol\032\022e"
    "ntity_types.proto\"\341\001\n\007Variant\022\022\n\nbool_va"
    "lue\030\002 \001(\010\022\021\n\tint_value\030\003 \001(\003\022\023\n\013float_va"
    "lue\030\004 \001(\001\022\024\n\014string_value\030\005 \001(\t\022\022\n\nblob_"
    "value\030\006 \001(\014\022\025\n\rmessage_value\030\007 \001(\014\022\024\n\014fo"
    "urcc_value\030\010 \001(\t\022\022\n\nuint_value\030\t \001(\004\022/\n\017"
    "entity_id_value\030\n \001(\0132\026.bgs.protocol.Ent"
    "ityId\"\?\n\tAttribute\022\014\n\004name\030\001 \002(\t\022$\n\005valu"
    "e\030\002 \002(\0132\025.bgs.protocol.Variant\"\312\001\n\017Attri"
    "buteFilter\0223\n\002op\030\001 \002(\0162\'.bgs.protocol.At"
    "tributeFilter.Operation\022*\n\tattribute\030\002 \003"
    "(\0132\027.bgs.protocol.Attribute\"V\n\tOperation"
    "\022\016\n\nMATCH_NONE\020\000\022\r\n\tMATCH_ANY\020\001\022\r\n\tMATCH"
    "_ALL\020\002\022\033\n\027MATCH_ALL_MOST_SPECIFIC\020\003B \n\014b"
    "gs.protocolB\016AttributeProtoH\002", 589);
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedFile(
    "attribute_types.proto", &protobuf_RegisterTypes);
  Variant::default_instance_ = new Variant();
  Attribute::default_instance_ = new Attribute();
  AttributeFilter::default_instance_ = new AttributeFilter();
  Variant::default_instance_->InitAsDefaultInstance();
  Attribute::default_instance_->InitAsDefaultInstance();
  AttributeFilter::default_instance_->InitAsDefaultInstance();
  ::google::protobuf::internal::OnShutdown(&protobuf_ShutdownFile_attribute_5ftypes_2eproto);
}

// Force AddDescriptors() to be called at static initialization time.
struct StaticDescriptorInitializer_attribute_5ftypes_2eproto {
  StaticDescriptorInitializer_attribute_5ftypes_2eproto() {
    protobuf_AddDesc_attribute_5ftypes_2eproto();
  }
} static_descriptor_initializer_attribute_5ftypes_2eproto_;
// ===================================================================

#ifndef _MSC_VER
const int Variant::kBoolValueFieldNumber;
const int Variant::kIntValueFieldNumber;
const int Variant::kFloatValueFieldNumber;
const int Variant::kStringValueFieldNumber;
const int Variant::kBlobValueFieldNumber;
const int Variant::kMessageValueFieldNumber;
const int Variant::kFourccValueFieldNumber;
const int Variant::kUintValueFieldNumber;
const int Variant::kEntityIdValueFieldNumber;
#endif  // !_MSC_VER

Variant::Variant()
  : ::google::protobuf::Message() {
  SharedCtor();
  // @@protoc_insertion_point(constructor:bgs.protocol.Variant)
}

void Variant::InitAsDefaultInstance() {
  entity_id_value_ = const_cast< ::bgs::protocol::EntityId*>(&::bgs::protocol::EntityId::default_instance());
}

Variant::Variant(const Variant& from)
  : ::google::protobuf::Message() {
  SharedCtor();
  MergeFrom(from);
  // @@protoc_insertion_point(copy_constructor:bgs.protocol.Variant)
}

void Variant::SharedCtor() {
  ::google::protobuf::internal::GetEmptyString();
  _cached_size_ = 0;
  bool_value_ = false;
  int_value_ = GOOGLE_LONGLONG(0);
  float_value_ = 0;
  string_value_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  blob_value_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  message_value_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  fourcc_value_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  uint_value_ = GOOGLE_ULONGLONG(0);
  entity_id_value_ = NULL;
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
}

Variant::~Variant() {
  // @@protoc_insertion_point(destructor:bgs.protocol.Variant)
  SharedDtor();
}

void Variant::SharedDtor() {
  if (string_value_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    delete string_value_;
  }
  if (blob_value_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    delete blob_value_;
  }
  if (message_value_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    delete message_value_;
  }
  if (fourcc_value_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    delete fourcc_value_;
  }
  if (this != default_instance_) {
    delete entity_id_value_;
  }
}

void Variant::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const ::google::protobuf::Descriptor* Variant::descriptor() {
  protobuf_AssignDescriptorsOnce();
  return Variant_descriptor_;
}

const Variant& Variant::default_instance() {
  if (default_instance_ == NULL) protobuf_AddDesc_attribute_5ftypes_2eproto();
  return *default_instance_;
}

Variant* Variant::default_instance_ = NULL;

Variant* Variant::New() const {
  return new Variant;
}

void Variant::Swap(Variant* other) {
  if (other != this) {
    GetReflection()->Swap(this, other);}
}

::google::protobuf::Metadata Variant::GetMetadata() const {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::Metadata metadata;
  metadata.descriptor = Variant_descriptor_;
  metadata.reflection = Variant_reflection_;
  return metadata;
}

// ===================================================================

#ifndef _MSC_VER
const int Attribute::kNameFieldNumber;
const int Attribute::kValueFieldNumber;
#endif  // !_MSC_VER

Attribute::Attribute()
  : ::google::protobuf::Message() {
  SharedCtor();
  // @@protoc_insertion_point(constructor:bgs.protocol.Attribute)
}

void Attribute::InitAsDefaultInstance() {
  value_ = const_cast< ::bgs::protocol::Variant*>(&::bgs::protocol::Variant::default_instance());
}

Attribute::Attribute(const Attribute& from)
  : ::google::protobuf::Message() {
  SharedCtor();
  MergeFrom(from);
  // @@protoc_insertion_point(copy_constructor:bgs.protocol.Attribute)
}

void Attribute::SharedCtor() {
  ::google::protobuf::internal::GetEmptyString();
  _cached_size_ = 0;
  name_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  value_ = NULL;
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
}

Attribute::~Attribute() {
  // @@protoc_insertion_point(destructor:bgs.protocol.Attribute)
  SharedDtor();
}

void Attribute::SharedDtor() {
  if (name_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    delete name_;
  }
  if (this != default_instance_) {
    delete value_;
  }
}

void Attribute::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const ::google::protobuf::Descriptor* Attribute::descriptor() {
  protobuf_AssignDescriptorsOnce();
  return Attribute_descriptor_;
}

const Attribute& Attribute::default_instance() {
  if (default_instance_ == NULL) protobuf_AddDesc_attribute_5ftypes_2eproto();
  return *default_instance_;
}

Attribute* Attribute::default_instance_ = NULL;

Attribute* Attribute::New() const {
  return new Attribute;
}

void Attribute::Swap(Attribute* other) {
  if (other != this) {
    GetReflection()->Swap(this, other);}
}

::google::protobuf::Metadata Attribute::GetMetadata() const {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::Metadata metadata;
  metadata.descriptor = Attribute_descriptor_;
  metadata.reflection = Attribute_reflection_;
  return metadata;
}

// ===================================================================

const ::google::protobuf::EnumDescriptor* AttributeFilter_Operation_descriptor() {
  protobuf_AssignDescriptorsOnce();
  return AttributeFilter_Operation_descriptor_;
}
bool AttributeFilter_Operation_IsValid(int value) {
  switch(value) {
    case 0:
    case 1:
    case 2:
    case 3:
      return true;
    default:
      return false;
  }
}

#ifndef _MSC_VER
const AttributeFilter_Operation AttributeFilter::MATCH_NONE;
const AttributeFilter_Operation AttributeFilter::MATCH_ANY;
const AttributeFilter_Operation AttributeFilter::MATCH_ALL;
const AttributeFilter_Operation AttributeFilter::MATCH_ALL_MOST_SPECIFIC;
const AttributeFilter_Operation AttributeFilter::Operation_MIN;
const AttributeFilter_Operation AttributeFilter::Operation_MAX;
const int AttributeFilter::Operation_ARRAYSIZE;
#endif  // _MSC_VER
#ifndef _MSC_VER
const int AttributeFilter::kOpFieldNumber;
const int AttributeFilter::kAttributeFieldNumber;
#endif  // !_MSC_VER

AttributeFilter::AttributeFilter()
  : ::google::protobuf::Message() {
  SharedCtor();
  // @@protoc_insertion_point(constructor:bgs.protocol.AttributeFilter)
}

void AttributeFilter::InitAsDefaultInstance() {
}

AttributeFilter::AttributeFilter(const AttributeFilter& from)
  : ::google::protobuf::Message() {
  SharedCtor();
  MergeFrom(from);
  // @@protoc_insertion_point(copy_constructor:bgs.protocol.AttributeFilter)
}

void AttributeFilter::SharedCtor() {
  _cached_size_ = 0;
  op_ = 0;
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
}

AttributeFilter::~AttributeFilter() {
  // @@protoc_insertion_point(destructor:bgs.protocol.AttributeFilter)
  SharedDtor();
}

void AttributeFilter::SharedDtor() {
  if (this != default_instance_) {
  }
}

void AttributeFilter::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const ::google::protobuf::Descriptor* AttributeFilter::descriptor() {
  protobuf_AssignDescriptorsOnce();
  return AttributeFilter_descriptor_;
}

const AttributeFilter& AttributeFilter::default_instance() {
  if (default_instance_ == NULL) protobuf_AddDesc_attribute_5ftypes_2eproto();
  return *default_instance_;
}

AttributeFilter* AttributeFilter::default_instance_ = NULL;

AttributeFilter* AttributeFilter::New() const {
  return new AttributeFilter;
}

void AttributeFilter::Swap(AttributeFilter* other) {
  if (other != this) {
    GetReflection()->Swap(this, other);}
}

::google::protobuf::Metadata AttributeFilter::GetMetadata() const {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::Metadata metadata;
  metadata.descriptor = AttributeFilter_descriptor_;
  metadata.reflection = AttributeFilter_reflection_;
  return metadata;
}

// @@protoc_insertion_point(namespace_scope)

}  // namespace protocol
}  // namespace bgs

// @@protoc_insertion_point(global_scope)
