#ifndef __REPLICATION_ACTION_TYPES_H__
#define __REPLICATION_ACTION_TYPES_H__

namespace sand {

enum class ReplicationActionType : U16 {
    CREATE,
    UPDATE,
    REMOVE,

    COUNT,
    INVALID
};
}

#endif
