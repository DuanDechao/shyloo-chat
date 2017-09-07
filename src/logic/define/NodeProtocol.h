#ifndef SL_NODE_PROTOCOL_H
#define SL_NODE_PROTOCOL_H
#include "GameDefine.h"

enum NodeProtocol{
	ASK_FOR_ALLOC_ID_AREA = 1,
	GIVE_ID_AREA = 2,
	DB_COMMAND_EXECUTE = 3,
	MASTER_MSG_START_NODE = 4,
	MASTER_MSG_NEW_NODE = 5,
	CLUSTER_MSG_NEW_NODE_COMING = 6,
	NODE_CAPACITY_LOAD_REPORT = 7,
	MASTER_MSG_ASK_SHUTDOWN = 8,
	CLUSTER_MSG_SHUTDOWN_ACK = 9,
	CLUSTER_MSG_ASK_DATA_LAND = 10,
	MASTER_MSG_STOP_NODES = 11,
	MASTER_MSG_SERVER_STARTED = 12,

	GATE_MSG_TRANSMIT_MSG_TO_LOGIC = 20,
	GATE_MSG_BIND_ACCOUNT_REQ,
	GATE_MSG_DISTRIBUTE_LOGIC_REQ,
	GATE_MSG_BIND_PLAYER_REQ,
	GATE_MSG_UNBIND_PLAYER_REQ,
	GATE_MSG_UNBIND_ACCOUNT_REQ,
	GATE_MSG_BROCAST,
	GATE_MSG_GATE_REGISTER,
	GATE_MSG_LOAD_REPORT,

	CHANNEL_MSG_CHANNEL_REGISTER = 100,
	CHANNEL_MSG_LOAD_REPORT,

	ACCOUNT_MSG_KICK_FROM_ACCOUNT = 500,
	ACCOUNT_MSG_BIND_ACCOUNT_ACK,

	BALANCE_MSG_SYNC_LOGIN_TICKET = 700,

	SCENEMGR_MSG_DISTRIBUTE_LOGIC_ACK = 1000,
	SCENEMGR_MSG_CREATE_SCENE,
	SCENEMGR_MSG_ENTER_SCENE,
	SCENEMGR_MSG_APPEAR_SCENE,
	SCENEMGR_MSG_LEAVE_SCENE,
	SCENEMGR_MSG_SYNC_SCENE,

	SCENE_MSG_SCENE_CONFIRMED = 2000,
	SCENE_MSG_ADD_INTERESTER,
	SCENE_MSG_ADD_WATCHER,
	SCENE_MSG_REMOVE_INTERESTER,
	SCENE_MSG_REMOVE_WATCHER,

	DB_MSG_UPDATE_DATABASE_FINISHED = 3000,
	

	LOGIC_MSG_BIND_PLAYER_ACK = 4000,
	LOGIC_MSG_NOTIFY_ADD_PLAYER,
	LOGIC_MSG_NOTIFY_REMOVE_PLAYER,
	LOGIC_MSG_TRANSFOR,
	LOGIC_MSG_BROCAST,
	LOGIC_MSG_ALLSVR_BROCAST,
	LOGIC_MSG_NOTIFY_SCENEMGR_APPEAR_SCENE,
	LOGIC_MSG_NOTIFY_SCENEMGR_ENTER_SCENE,
	LOGIC_MSG_NOTIFY_SCENEMGR_LEAVE_SCENE,
	LOGIC_MSG_SYNC_SCENE,
	LOGIC_MSG_SYNC_OBJECT_LOCATION,
	LOGIC_MSG_REMOVE_OBJECT_LOCATION,
	LOGIC_MSG_CREATE_SHADOW,
	LOGIC_MSG_SYNC_SHADOW,
	LOGIC_MSG_DESTROY_SHADOW,
	LOGIC_MSG_COMMAND,
	LOGIC_MSG_FORWARD_COMMAND,

	RELATION_MSG_FORWARD_COMMAND = 5000,
};

namespace client{
	struct Header{
		int32 messageId;
		int32 size;
	};

	struct Transfor{
		int8 delay;
		int64 actorId;
	};

	struct Brocast{
		int8 delay;
		int32 gate;
		int32 count;
	};
}

namespace shadow{
	struct Attribute{
		int32 name;
		int8 type;
		union{
			int8 valueInt8;
			int16 valueInt16;
			int32 valueInt32;
			int64 valueInt64;
			float valueFloat;
			char valueString[game::MAX_PROP_VALUE_LEN];
		} data;
	};

	struct CreateShadow{
		int64 id;
		char objectTypeName[game::MAX_OBJECT_TYPE_LEN];
		int32 count;
	};

	struct SyncShadow{
		int64 id;
		int32 count;
	};
}

#endif