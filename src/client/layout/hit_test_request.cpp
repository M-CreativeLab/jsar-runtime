#include "./hit_test_request.hpp"
#include "./layout_object.hpp"

namespace client_layout
{
  using namespace std;

  HitTestRequest::HitTestRequest(HitTestRequestType requestType, shared_ptr<LayoutObject> stopNode)
      : request_type_(requestType),
        stop_node_(stopNode)
  {
  }
}
