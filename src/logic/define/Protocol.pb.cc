// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: Protocol.proto

#define INTERNAL_SUPPRESS_PROTOBUF_FIELD_DEPRECATION
#include "Protocol.pb.h"

#include <algorithm>

#include <google/protobuf/stubs/common.h>
#include <google/protobuf/stubs/port.h>
#include <google/protobuf/stubs/once.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/wire_format_lite_inl.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/generated_message_reflection.h>
#include <google/protobuf/reflection_ops.h>
#include <google/protobuf/wire_format.h>
// @@protoc_insertion_point(includes)

namespace protocol {

namespace protobuf_Protocol_2eproto {


namespace {

const ::google::protobuf::EnumDescriptor* file_level_enum_descriptors[3];

}  // namespace

PROTOBUF_CONSTEXPR_VAR ::google::protobuf::internal::ParseTableField
    const TableStruct::entries[] = {
  {0, 0, 0, ::google::protobuf::internal::kInvalidMask, 0, 0},
};

PROTOBUF_CONSTEXPR_VAR ::google::protobuf::internal::AuxillaryParseTableField
    const TableStruct::aux[] = {
  ::google::protobuf::internal::AuxillaryParseTableField(),
};
PROTOBUF_CONSTEXPR_VAR ::google::protobuf::internal::ParseTable const
    TableStruct::schema[] = {
  { NULL, NULL, 0, -1, -1, false },
};

const ::google::protobuf::uint32 TableStruct::offsets[] = { ~0u };
static const ::google::protobuf::internal::MigrationSchema* schemas = NULL;
static const ::google::protobuf::Message* const* file_default_instances = NULL;
namespace {

void protobuf_AssignDescriptors() {
  AddDescriptors();
  ::google::protobuf::MessageFactory* factory = NULL;
  AssignDescriptors(
      "Protocol.proto", schemas, file_default_instances, TableStruct::offsets, factory,
      NULL, file_level_enum_descriptors, NULL);
}

void protobuf_AssignDescriptorsOnce() {
  static GOOGLE_PROTOBUF_DECLARE_ONCE(once);
  ::google::protobuf::GoogleOnceInit(&once, &protobuf_AssignDescriptors);
}

void protobuf_RegisterTypes(const ::std::string&) GOOGLE_ATTRIBUTE_COLD;
void protobuf_RegisterTypes(const ::std::string&) {
  protobuf_AssignDescriptorsOnce();
}

}  // namespace

void TableStruct::Shutdown() {
}

void TableStruct::InitDefaultsImpl() {
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  ::google::protobuf::internal::InitProtobufDefaults();
}

void InitDefaults() {
  static GOOGLE_PROTOBUF_DECLARE_ONCE(once);
  ::google::protobuf::GoogleOnceInit(&once, &TableStruct::InitDefaultsImpl);
}
void AddDescriptorsImpl() {
  InitDefaults();
  static const char descriptor[] = {
      "\n\016Protocol.proto\022\010protocol*\250\001\n\nAttribTyp"
      "e\022\022\n\016DTYPE_INVAILED\020\000\022\016\n\nDTYPE_INT8\020\001\022\017\n"
      "\013DTYPE_INT16\020\002\022\017\n\013DTYPE_INT32\020\003\022\017\n\013DTYPE"
      "_INT64\020\004\022\020\n\014DTYPE_STRING\020\005\022\017\n\013DTYPE_FLOA"
      "T\020\006\022\020\n\014DTYPE_STRUCT\020\007\022\016\n\nDTYPE_BLOB\020\010*=\n"
      "\nObjectType\022\027\n\023OBJECT_TYPE_UNKNOWN\020\000\022\026\n\022"
      "OBJECT_TYPE_PLAYER\020\001*\206\003\n\tErrorCode\022\022\n\016ER"
      "ROR_NO_ERROR\020\000\022\037\n\033ERROR_ACCOUNT_AUTHEN_F"
      "AILED\020\001\022\036\n\032ERROR_GET_ROLE_LIST_FAILED\020\002\022"
      "!\n\035ERROR_DISTRIBUTE_LOGIC_FAILED\020\003\022\034\n\030ER"
      "ROR_LOAD_PLAYER_FAILED\020\004\022\027\n\023ERROR_TOO_MU"
      "CH_ROLE\020\005\022\034\n\030ERROR_CREATE_ROLE_FAILED\020\006\022"
      "\026\n\022ERROR_SYSTEM_ERROR\020\007\022#\n\037ERROR_LOGIN_C"
      "HECK_TICKET_FAILED\020\010\022\'\n#ERROR_CHAT_ANOTH"
      "ER_CLIENT_VERIFYING\020\n\022!\n\035ERROR_CHAT_VERI"
      "FY_TOKEN_ERROR\020\013\022#\n\037ERROR_CHAT_ANOTHER_C"
      "LIENT_LOGIN\020\014"
  };
  ::google::protobuf::DescriptorPool::InternalAddGeneratedFile(
      descriptor, 653);
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedFile(
    "Protocol.proto", &protobuf_RegisterTypes);
  ::google::protobuf::internal::OnShutdown(&TableStruct::Shutdown);
}

void AddDescriptors() {
  static GOOGLE_PROTOBUF_DECLARE_ONCE(once);
  ::google::protobuf::GoogleOnceInit(&once, &AddDescriptorsImpl);
}
// Force AddDescriptors() to be called at static initialization time.
struct StaticDescriptorInitializer {
  StaticDescriptorInitializer() {
    AddDescriptors();
  }
} static_descriptor_initializer;

}  // namespace protobuf_Protocol_2eproto

const ::google::protobuf::EnumDescriptor* AttribType_descriptor() {
  protobuf_Protocol_2eproto::protobuf_AssignDescriptorsOnce();
  return protobuf_Protocol_2eproto::file_level_enum_descriptors[0];
}
bool AttribType_IsValid(int value) {
  switch (value) {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
      return true;
    default:
      return false;
  }
}

const ::google::protobuf::EnumDescriptor* ObjectType_descriptor() {
  protobuf_Protocol_2eproto::protobuf_AssignDescriptorsOnce();
  return protobuf_Protocol_2eproto::file_level_enum_descriptors[1];
}
bool ObjectType_IsValid(int value) {
  switch (value) {
    case 0:
    case 1:
      return true;
    default:
      return false;
  }
}

const ::google::protobuf::EnumDescriptor* ErrorCode_descriptor() {
  protobuf_Protocol_2eproto::protobuf_AssignDescriptorsOnce();
  return protobuf_Protocol_2eproto::file_level_enum_descriptors[2];
}
bool ErrorCode_IsValid(int value) {
  switch (value) {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
    case 10:
    case 11:
    case 12:
      return true;
    default:
      return false;
  }
}


// @@protoc_insertion_point(namespace_scope)

}  // namespace protocol

// @@protoc_insertion_point(global_scope)