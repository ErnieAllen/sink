#include "msg_tool.h"

msg_tool_queue::msg_tool_queue ( Session & _session,
                                 char const * _queue_name,
                                 char const *  arguments
                               )
  : queue_name ( _queue_name ),
    sent_count ( 0 ),
    received_count ( 0 )
{
  address = queue_name;
  if ( !arguments )
  {
    address += ";{create:always,node:{type:queue}}";
  }
  else
  {
    address += ";{create:always,node:{x-declare:{arguments:{";
    address += arguments;
    address += "}}}}";
  }


  sender   = _session.createSender   ( address );
  receiver = _session.createReceiver ( address );
}

