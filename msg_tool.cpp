#include "msg_tool.h"

msg_tool_queue::msg_tool_queue ( Session & _session,
                                 char const * _queue_name,
                                 QueueType  queueType
                               )
  : queue_name ( _queue_name ),
    sent_count ( 0 ),
    received_count ( 0 )
{
  address = queue_name;
  if ( queueType == priorityQueue )
  {
    address += ";{create:always,node:{type:queue}}";
  }
  else
  if ( queueType == lastValueQueue )
  {
    address += ";{create:always,node:{x-declare:{arguments:{'qpid.last_value_queue':1}}}}";
  }


  sender   = _session.createSender   ( address );
  receiver = _session.createReceiver ( address );
}

