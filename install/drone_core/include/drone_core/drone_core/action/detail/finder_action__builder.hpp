// generated from rosidl_generator_cpp/resource/idl__builder.hpp.em
// with input from drone_core:action/FinderAction.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "drone_core/action/finder_action.hpp"


#ifndef DRONE_CORE__ACTION__DETAIL__FINDER_ACTION__BUILDER_HPP_
#define DRONE_CORE__ACTION__DETAIL__FINDER_ACTION__BUILDER_HPP_

#include <algorithm>
#include <utility>

#include "drone_core/action/detail/finder_action__struct.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


namespace drone_core
{

namespace action
{

namespace builder
{

class Init_FinderAction_Goal_target_pose
{
public:
  explicit Init_FinderAction_Goal_target_pose(::drone_core::action::FinderAction_Goal & msg)
  : msg_(msg)
  {}
  ::drone_core::action::FinderAction_Goal target_pose(::drone_core::action::FinderAction_Goal::_target_pose_type arg)
  {
    msg_.target_pose = std::move(arg);
    return std::move(msg_);
  }

private:
  ::drone_core::action::FinderAction_Goal msg_;
};

class Init_FinderAction_Goal_command_type
{
public:
  Init_FinderAction_Goal_command_type()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_FinderAction_Goal_target_pose command_type(::drone_core::action::FinderAction_Goal::_command_type_type arg)
  {
    msg_.command_type = std::move(arg);
    return Init_FinderAction_Goal_target_pose(msg_);
  }

private:
  ::drone_core::action::FinderAction_Goal msg_;
};

}  // namespace builder

}  // namespace action

template<typename MessageType>
auto build();

template<>
inline
auto build<::drone_core::action::FinderAction_Goal>()
{
  return drone_core::action::builder::Init_FinderAction_Goal_command_type();
}

}  // namespace drone_core


namespace drone_core
{

namespace action
{

namespace builder
{

class Init_FinderAction_Result_success
{
public:
  Init_FinderAction_Result_success()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  ::drone_core::action::FinderAction_Result success(::drone_core::action::FinderAction_Result::_success_type arg)
  {
    msg_.success = std::move(arg);
    return std::move(msg_);
  }

private:
  ::drone_core::action::FinderAction_Result msg_;
};

}  // namespace builder

}  // namespace action

template<typename MessageType>
auto build();

template<>
inline
auto build<::drone_core::action::FinderAction_Result>()
{
  return drone_core::action::builder::Init_FinderAction_Result_success();
}

}  // namespace drone_core


namespace drone_core
{

namespace action
{

namespace builder
{

class Init_FinderAction_Feedback_numb_labels
{
public:
  Init_FinderAction_Feedback_numb_labels()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  ::drone_core::action::FinderAction_Feedback numb_labels(::drone_core::action::FinderAction_Feedback::_numb_labels_type arg)
  {
    msg_.numb_labels = std::move(arg);
    return std::move(msg_);
  }

private:
  ::drone_core::action::FinderAction_Feedback msg_;
};

}  // namespace builder

}  // namespace action

template<typename MessageType>
auto build();

template<>
inline
auto build<::drone_core::action::FinderAction_Feedback>()
{
  return drone_core::action::builder::Init_FinderAction_Feedback_numb_labels();
}

}  // namespace drone_core


namespace drone_core
{

namespace action
{

namespace builder
{

class Init_FinderAction_SendGoal_Request_goal
{
public:
  explicit Init_FinderAction_SendGoal_Request_goal(::drone_core::action::FinderAction_SendGoal_Request & msg)
  : msg_(msg)
  {}
  ::drone_core::action::FinderAction_SendGoal_Request goal(::drone_core::action::FinderAction_SendGoal_Request::_goal_type arg)
  {
    msg_.goal = std::move(arg);
    return std::move(msg_);
  }

private:
  ::drone_core::action::FinderAction_SendGoal_Request msg_;
};

class Init_FinderAction_SendGoal_Request_goal_id
{
public:
  Init_FinderAction_SendGoal_Request_goal_id()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_FinderAction_SendGoal_Request_goal goal_id(::drone_core::action::FinderAction_SendGoal_Request::_goal_id_type arg)
  {
    msg_.goal_id = std::move(arg);
    return Init_FinderAction_SendGoal_Request_goal(msg_);
  }

private:
  ::drone_core::action::FinderAction_SendGoal_Request msg_;
};

}  // namespace builder

}  // namespace action

template<typename MessageType>
auto build();

template<>
inline
auto build<::drone_core::action::FinderAction_SendGoal_Request>()
{
  return drone_core::action::builder::Init_FinderAction_SendGoal_Request_goal_id();
}

}  // namespace drone_core


namespace drone_core
{

namespace action
{

namespace builder
{

class Init_FinderAction_SendGoal_Response_stamp
{
public:
  explicit Init_FinderAction_SendGoal_Response_stamp(::drone_core::action::FinderAction_SendGoal_Response & msg)
  : msg_(msg)
  {}
  ::drone_core::action::FinderAction_SendGoal_Response stamp(::drone_core::action::FinderAction_SendGoal_Response::_stamp_type arg)
  {
    msg_.stamp = std::move(arg);
    return std::move(msg_);
  }

private:
  ::drone_core::action::FinderAction_SendGoal_Response msg_;
};

class Init_FinderAction_SendGoal_Response_accepted
{
public:
  Init_FinderAction_SendGoal_Response_accepted()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_FinderAction_SendGoal_Response_stamp accepted(::drone_core::action::FinderAction_SendGoal_Response::_accepted_type arg)
  {
    msg_.accepted = std::move(arg);
    return Init_FinderAction_SendGoal_Response_stamp(msg_);
  }

private:
  ::drone_core::action::FinderAction_SendGoal_Response msg_;
};

}  // namespace builder

}  // namespace action

template<typename MessageType>
auto build();

template<>
inline
auto build<::drone_core::action::FinderAction_SendGoal_Response>()
{
  return drone_core::action::builder::Init_FinderAction_SendGoal_Response_accepted();
}

}  // namespace drone_core


namespace drone_core
{

namespace action
{

namespace builder
{

class Init_FinderAction_SendGoal_Event_response
{
public:
  explicit Init_FinderAction_SendGoal_Event_response(::drone_core::action::FinderAction_SendGoal_Event & msg)
  : msg_(msg)
  {}
  ::drone_core::action::FinderAction_SendGoal_Event response(::drone_core::action::FinderAction_SendGoal_Event::_response_type arg)
  {
    msg_.response = std::move(arg);
    return std::move(msg_);
  }

private:
  ::drone_core::action::FinderAction_SendGoal_Event msg_;
};

class Init_FinderAction_SendGoal_Event_request
{
public:
  explicit Init_FinderAction_SendGoal_Event_request(::drone_core::action::FinderAction_SendGoal_Event & msg)
  : msg_(msg)
  {}
  Init_FinderAction_SendGoal_Event_response request(::drone_core::action::FinderAction_SendGoal_Event::_request_type arg)
  {
    msg_.request = std::move(arg);
    return Init_FinderAction_SendGoal_Event_response(msg_);
  }

private:
  ::drone_core::action::FinderAction_SendGoal_Event msg_;
};

class Init_FinderAction_SendGoal_Event_info
{
public:
  Init_FinderAction_SendGoal_Event_info()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_FinderAction_SendGoal_Event_request info(::drone_core::action::FinderAction_SendGoal_Event::_info_type arg)
  {
    msg_.info = std::move(arg);
    return Init_FinderAction_SendGoal_Event_request(msg_);
  }

private:
  ::drone_core::action::FinderAction_SendGoal_Event msg_;
};

}  // namespace builder

}  // namespace action

template<typename MessageType>
auto build();

template<>
inline
auto build<::drone_core::action::FinderAction_SendGoal_Event>()
{
  return drone_core::action::builder::Init_FinderAction_SendGoal_Event_info();
}

}  // namespace drone_core


namespace drone_core
{

namespace action
{

namespace builder
{

class Init_FinderAction_GetResult_Request_goal_id
{
public:
  Init_FinderAction_GetResult_Request_goal_id()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  ::drone_core::action::FinderAction_GetResult_Request goal_id(::drone_core::action::FinderAction_GetResult_Request::_goal_id_type arg)
  {
    msg_.goal_id = std::move(arg);
    return std::move(msg_);
  }

private:
  ::drone_core::action::FinderAction_GetResult_Request msg_;
};

}  // namespace builder

}  // namespace action

template<typename MessageType>
auto build();

template<>
inline
auto build<::drone_core::action::FinderAction_GetResult_Request>()
{
  return drone_core::action::builder::Init_FinderAction_GetResult_Request_goal_id();
}

}  // namespace drone_core


namespace drone_core
{

namespace action
{

namespace builder
{

class Init_FinderAction_GetResult_Response_result
{
public:
  explicit Init_FinderAction_GetResult_Response_result(::drone_core::action::FinderAction_GetResult_Response & msg)
  : msg_(msg)
  {}
  ::drone_core::action::FinderAction_GetResult_Response result(::drone_core::action::FinderAction_GetResult_Response::_result_type arg)
  {
    msg_.result = std::move(arg);
    return std::move(msg_);
  }

private:
  ::drone_core::action::FinderAction_GetResult_Response msg_;
};

class Init_FinderAction_GetResult_Response_status
{
public:
  Init_FinderAction_GetResult_Response_status()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_FinderAction_GetResult_Response_result status(::drone_core::action::FinderAction_GetResult_Response::_status_type arg)
  {
    msg_.status = std::move(arg);
    return Init_FinderAction_GetResult_Response_result(msg_);
  }

private:
  ::drone_core::action::FinderAction_GetResult_Response msg_;
};

}  // namespace builder

}  // namespace action

template<typename MessageType>
auto build();

template<>
inline
auto build<::drone_core::action::FinderAction_GetResult_Response>()
{
  return drone_core::action::builder::Init_FinderAction_GetResult_Response_status();
}

}  // namespace drone_core


namespace drone_core
{

namespace action
{

namespace builder
{

class Init_FinderAction_GetResult_Event_response
{
public:
  explicit Init_FinderAction_GetResult_Event_response(::drone_core::action::FinderAction_GetResult_Event & msg)
  : msg_(msg)
  {}
  ::drone_core::action::FinderAction_GetResult_Event response(::drone_core::action::FinderAction_GetResult_Event::_response_type arg)
  {
    msg_.response = std::move(arg);
    return std::move(msg_);
  }

private:
  ::drone_core::action::FinderAction_GetResult_Event msg_;
};

class Init_FinderAction_GetResult_Event_request
{
public:
  explicit Init_FinderAction_GetResult_Event_request(::drone_core::action::FinderAction_GetResult_Event & msg)
  : msg_(msg)
  {}
  Init_FinderAction_GetResult_Event_response request(::drone_core::action::FinderAction_GetResult_Event::_request_type arg)
  {
    msg_.request = std::move(arg);
    return Init_FinderAction_GetResult_Event_response(msg_);
  }

private:
  ::drone_core::action::FinderAction_GetResult_Event msg_;
};

class Init_FinderAction_GetResult_Event_info
{
public:
  Init_FinderAction_GetResult_Event_info()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_FinderAction_GetResult_Event_request info(::drone_core::action::FinderAction_GetResult_Event::_info_type arg)
  {
    msg_.info = std::move(arg);
    return Init_FinderAction_GetResult_Event_request(msg_);
  }

private:
  ::drone_core::action::FinderAction_GetResult_Event msg_;
};

}  // namespace builder

}  // namespace action

template<typename MessageType>
auto build();

template<>
inline
auto build<::drone_core::action::FinderAction_GetResult_Event>()
{
  return drone_core::action::builder::Init_FinderAction_GetResult_Event_info();
}

}  // namespace drone_core


namespace drone_core
{

namespace action
{

namespace builder
{

class Init_FinderAction_FeedbackMessage_feedback
{
public:
  explicit Init_FinderAction_FeedbackMessage_feedback(::drone_core::action::FinderAction_FeedbackMessage & msg)
  : msg_(msg)
  {}
  ::drone_core::action::FinderAction_FeedbackMessage feedback(::drone_core::action::FinderAction_FeedbackMessage::_feedback_type arg)
  {
    msg_.feedback = std::move(arg);
    return std::move(msg_);
  }

private:
  ::drone_core::action::FinderAction_FeedbackMessage msg_;
};

class Init_FinderAction_FeedbackMessage_goal_id
{
public:
  Init_FinderAction_FeedbackMessage_goal_id()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_FinderAction_FeedbackMessage_feedback goal_id(::drone_core::action::FinderAction_FeedbackMessage::_goal_id_type arg)
  {
    msg_.goal_id = std::move(arg);
    return Init_FinderAction_FeedbackMessage_feedback(msg_);
  }

private:
  ::drone_core::action::FinderAction_FeedbackMessage msg_;
};

}  // namespace builder

}  // namespace action

template<typename MessageType>
auto build();

template<>
inline
auto build<::drone_core::action::FinderAction_FeedbackMessage>()
{
  return drone_core::action::builder::Init_FinderAction_FeedbackMessage_goal_id();
}

}  // namespace drone_core

#endif  // DRONE_CORE__ACTION__DETAIL__FINDER_ACTION__BUILDER_HPP_
