//------------------------------------------------------------------------------
// <auto-generated>
//     This code was generated by a tool.
//
//     Changes to this file may cause incorrect behavior and will be lost if
//     the code is regenerated.
// </auto-generated>
//------------------------------------------------------------------------------

// Option: missing-value detection (*Specified/ShouldSerialize*/Reset*) enabled
    
// Generated from: ProtocolID.proto
namespace ProtocolID
{
    [global::ProtoBuf.ProtoContract(Name=@"ClientMsgID")]
    public enum ClientMsgID
    {
            
      [global::ProtoBuf.ProtoEnum(Name=@"CLIENT_MSG_LOGIN_REQ", Value=1)]
      CLIENT_MSG_LOGIN_REQ = 1,
            
      [global::ProtoBuf.ProtoEnum(Name=@"CLIENT_MSG_SELECT_ROLE_REQ", Value=2)]
      CLIENT_MSG_SELECT_ROLE_REQ = 2,
            
      [global::ProtoBuf.ProtoEnum(Name=@"CLIENT_MSG_CREATE_ROLE_REQ", Value=3)]
      CLIENT_MSG_CREATE_ROLE_REQ = 3,
            
      [global::ProtoBuf.ProtoEnum(Name=@"CLIENT_MSG_ENTER_SCENE_REQ", Value=4)]
      CLIENT_MSG_ENTER_SCENE_REQ = 4,
            
      [global::ProtoBuf.ProtoEnum(Name=@"CLIENT_MSG_TEST", Value=5)]
      CLIENT_MSG_TEST = 5,
            
      [global::ProtoBuf.ProtoEnum(Name=@"CLIENT_MSG_HEARTBEAT", Value=6)]
      CLIENT_MSG_HEARTBEAT = 6,
            
      [global::ProtoBuf.ProtoEnum(Name=@"CLIENT_MSG_LOGIN_CHAT_SERVER", Value=10)]
      CLIENT_MSG_LOGIN_CHAT_SERVER = 10
    }
  
    [global::ProtoBuf.ProtoContract(Name=@"ServerMsgID")]
    public enum ServerMsgID
    {
            
      [global::ProtoBuf.ProtoEnum(Name=@"SERVER_MSG_LOGIN_RSP", Value=1)]
      SERVER_MSG_LOGIN_RSP = 1,
            
      [global::ProtoBuf.ProtoEnum(Name=@"SERVER_MSG_SELECT_ROLE_RSP", Value=2)]
      SERVER_MSG_SELECT_ROLE_RSP = 2,
            
      [global::ProtoBuf.ProtoEnum(Name=@"SERVER_MSG_CREATE_ROLE_RSP", Value=3)]
      SERVER_MSG_CREATE_ROLE_RSP = 3,
            
      [global::ProtoBuf.ProtoEnum(Name=@"SERVER_MSG_ATTRIB_SYNC", Value=4)]
      SERVER_MSG_ATTRIB_SYNC = 4,
            
      [global::ProtoBuf.ProtoEnum(Name=@"SERVER_MSG_NEW_ROLE_NOTIFY", Value=5)]
      SERVER_MSG_NEW_ROLE_NOTIFY = 5,
            
      [global::ProtoBuf.ProtoEnum(Name=@"SERVER_MSG_REMOVE_ROLE_NOTIFY", Value=6)]
      SERVER_MSG_REMOVE_ROLE_NOTIFY = 6,
            
      [global::ProtoBuf.ProtoEnum(Name=@"SERVER_MSG_GIVE_GATE_ADDRESS_RSP", Value=7)]
      SERVER_MSG_GIVE_GATE_ADDRESS_RSP = 7,
            
      [global::ProtoBuf.ProtoEnum(Name=@"SERVER_MSG_NOTIFY_TICKET_INFO_RSP", Value=8)]
      SERVER_MSG_NOTIFY_TICKET_INFO_RSP = 8,
            
      [global::ProtoBuf.ProtoEnum(Name=@"SERVER_MSG_CHAT_AGENT_INFO_SYNC", Value=9)]
      SERVER_MSG_CHAT_AGENT_INFO_SYNC = 9,
            
      [global::ProtoBuf.ProtoEnum(Name=@"SERVER_MSG_CHAT_SERVER_LOGIN_RSP", Value=10)]
      SERVER_MSG_CHAT_SERVER_LOGIN_RSP = 10,
            
      [global::ProtoBuf.ProtoEnum(Name=@"SERVER_MSG_CHAT_HEARTBEAT_ACK", Value=11)]
      SERVER_MSG_CHAT_HEARTBEAT_ACK = 11
    }
  
}