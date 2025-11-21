// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from drone:action/FinderAction.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "drone/action/finder_action.h"


#ifndef DRONE__ACTION__DETAIL__FINDER_ACTION__STRUCT_H_
#define DRONE__ACTION__DETAIL__FINDER_ACTION__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


// Constants defined in the message

// Include directives for member types
// Member 'command_type'
#include "rosidl_runtime_c/string.h"
// Member 'target_pose'
#include "rosidl_runtime_c/primitives_sequence.h"

/// Struct defined in action/FinderAction in the package drone.
typedef struct drone__action__FinderAction_Goal
{
  /// "field"
  rosidl_runtime_c__String command_type;
  rosidl_runtime_c__int32__Sequence target_pose;
} drone__action__FinderAction_Goal;

// Struct for a sequence of drone__action__FinderAction_Goal.
typedef struct drone__action__FinderAction_Goal__Sequence
{
  drone__action__FinderAction_Goal * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} drone__action__FinderAction_Goal__Sequence;

// Constants defined in the message

/// Struct defined in action/FinderAction in the package drone.
typedef struct drone__action__FinderAction_Result
{
  bool success;
} drone__action__FinderAction_Result;

// Struct for a sequence of drone__action__FinderAction_Result.
typedef struct drone__action__FinderAction_Result__Sequence
{
  drone__action__FinderAction_Result * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} drone__action__FinderAction_Result__Sequence;

// Constants defined in the message

/// Struct defined in action/FinderAction in the package drone.
typedef struct drone__action__FinderAction_Feedback
{
  int32_t numb_labels;
} drone__action__FinderAction_Feedback;

// Struct for a sequence of drone__action__FinderAction_Feedback.
typedef struct drone__action__FinderAction_Feedback__Sequence
{
  drone__action__FinderAction_Feedback * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} drone__action__FinderAction_Feedback__Sequence;

// Constants defined in the message

// Include directives for member types
// Member 'goal_id'
#include "unique_identifier_msgs/msg/detail/uuid__struct.h"
// Member 'goal'
#include "drone/action/detail/finder_action__struct.h"

/// Struct defined in action/FinderAction in the package drone.
typedef struct drone__action__FinderAction_SendGoal_Request
{
  unique_identifier_msgs__msg__UUID goal_id;
  drone__action__FinderAction_Goal goal;
} drone__action__FinderAction_SendGoal_Request;

// Struct for a sequence of drone__action__FinderAction_SendGoal_Request.
typedef struct drone__action__FinderAction_SendGoal_Request__Sequence
{
  drone__action__FinderAction_SendGoal_Request * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} drone__action__FinderAction_SendGoal_Request__Sequence;

// Constants defined in the message

// Include directives for member types
// Member 'stamp'
#include "builtin_interfaces/msg/detail/time__struct.h"

/// Struct defined in action/FinderAction in the package drone.
typedef struct drone__action__FinderAction_SendGoal_Response
{
  bool accepted;
  builtin_interfaces__msg__Time stamp;
} drone__action__FinderAction_SendGoal_Response;

// Struct for a sequence of drone__action__FinderAction_SendGoal_Response.
typedef struct drone__action__FinderAction_SendGoal_Response__Sequence
{
  drone__action__FinderAction_SendGoal_Response * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} drone__action__FinderAction_SendGoal_Response__Sequence;

// Constants defined in the message

// Include directives for member types
// Member 'info'
#include "service_msgs/msg/detail/service_event_info__struct.h"

// constants for array fields with an upper bound
// request
enum
{
  drone__action__FinderAction_SendGoal_Event__request__MAX_SIZE = 1
};
// response
enum
{
  drone__action__FinderAction_SendGoal_Event__response__MAX_SIZE = 1
};

/// Struct defined in action/FinderAction in the package drone.
typedef struct drone__action__FinderAction_SendGoal_Event
{
  service_msgs__msg__ServiceEventInfo info;
  drone__action__FinderAction_SendGoal_Request__Sequence request;
  drone__action__FinderAction_SendGoal_Response__Sequence response;
} drone__action__FinderAction_SendGoal_Event;

// Struct for a sequence of drone__action__FinderAction_SendGoal_Event.
typedef struct drone__action__FinderAction_SendGoal_Event__Sequence
{
  drone__action__FinderAction_SendGoal_Event * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} drone__action__FinderAction_SendGoal_Event__Sequence;

// Constants defined in the message

// Include directives for member types
// Member 'goal_id'
// already included above
// #include "unique_identifier_msgs/msg/detail/uuid__struct.h"

/// Struct defined in action/FinderAction in the package drone.
typedef struct drone__action__FinderAction_GetResult_Request
{
  unique_identifier_msgs__msg__UUID goal_id;
} drone__action__FinderAction_GetResult_Request;

// Struct for a sequence of drone__action__FinderAction_GetResult_Request.
typedef struct drone__action__FinderAction_GetResult_Request__Sequence
{
  drone__action__FinderAction_GetResult_Request * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} drone__action__FinderAction_GetResult_Request__Sequence;

// Constants defined in the message

// Include directives for member types
// Member 'result'
// already included above
// #include "drone/action/detail/finder_action__struct.h"

/// Struct defined in action/FinderAction in the package drone.
typedef struct drone__action__FinderAction_GetResult_Response
{
  int8_t status;
  drone__action__FinderAction_Result result;
} drone__action__FinderAction_GetResult_Response;

// Struct for a sequence of drone__action__FinderAction_GetResult_Response.
typedef struct drone__action__FinderAction_GetResult_Response__Sequence
{
  drone__action__FinderAction_GetResult_Response * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} drone__action__FinderAction_GetResult_Response__Sequence;

// Constants defined in the message

// Include directives for member types
// Member 'info'
// already included above
// #include "service_msgs/msg/detail/service_event_info__struct.h"

// constants for array fields with an upper bound
// request
enum
{
  drone__action__FinderAction_GetResult_Event__request__MAX_SIZE = 1
};
// response
enum
{
  drone__action__FinderAction_GetResult_Event__response__MAX_SIZE = 1
};

/// Struct defined in action/FinderAction in the package drone.
typedef struct drone__action__FinderAction_GetResult_Event
{
  service_msgs__msg__ServiceEventInfo info;
  drone__action__FinderAction_GetResult_Request__Sequence request;
  drone__action__FinderAction_GetResult_Response__Sequence response;
} drone__action__FinderAction_GetResult_Event;

// Struct for a sequence of drone__action__FinderAction_GetResult_Event.
typedef struct drone__action__FinderAction_GetResult_Event__Sequence
{
  drone__action__FinderAction_GetResult_Event * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} drone__action__FinderAction_GetResult_Event__Sequence;

// Constants defined in the message

// Include directives for member types
// Member 'goal_id'
// already included above
// #include "unique_identifier_msgs/msg/detail/uuid__struct.h"
// Member 'feedback'
// already included above
// #include "drone/action/detail/finder_action__struct.h"

/// Struct defined in action/FinderAction in the package drone.
typedef struct drone__action__FinderAction_FeedbackMessage
{
  unique_identifier_msgs__msg__UUID goal_id;
  drone__action__FinderAction_Feedback feedback;
} drone__action__FinderAction_FeedbackMessage;

// Struct for a sequence of drone__action__FinderAction_FeedbackMessage.
typedef struct drone__action__FinderAction_FeedbackMessage__Sequence
{
  drone__action__FinderAction_FeedbackMessage * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} drone__action__FinderAction_FeedbackMessage__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // DRONE__ACTION__DETAIL__FINDER_ACTION__STRUCT_H_
