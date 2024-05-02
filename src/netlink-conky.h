#include "config.h"

#ifndef _CONKY_NETLINK_H_
#define _CONKY_NETLINK_H_

extern "C" {
#include <netlink/attr.h>
#include <netlink/cache.h>
#include <netlink/genl/ctrl.h>
#include <netlink/genl/family.h>
#include <netlink/genl/genl.h>
#include <netlink/msg.h>
#include <netlink/route/link.h>

#include <linux/ieee80211.h>
#include <uapi/linux/nl80211.h>
}

#include "net_stat.h"

#include <atomic>
#include <functional>
#include <map>
#include <string>
#include <variant>

using nl_link_id = std::variant<int, char *, std::string>;

/// @brief State of the callback.
///
/// Values smaller than 0 indicate an error.
/// 0 means the callback is finished.
/// Values greater than 0 indicate the callback isn't finished yet.
enum class callback_state : int {
  INVALID = -1,
  DONE = 0,
  IN_FLIGHT,
};

template <typename Data>
class nl_task {
  struct nl_cb *cb;
  std::atomic<callback_state> state;
  Data data;

 public:
  nl_task(int family, uint8_t request,
          std::function<int(struct nl_msg *, Data *)> processor);
  ~nl_task();
};

class net_device_cache {
  struct nl_sock *sock;
  struct nl_cache *nl_cache;
  int nl_cache_size;

  int id_nl80211;

  void setup_callbacks();

 public:
  net_device_cache();
  ~net_device_cache();

  /// @brief Update link cache and device information.
  void update();

  struct rtnl_link *get_link(const nl_link_id &id);

  /// @brief Populate `net_stat` interface from netlink cache.
  ///
  /// @param ns interface stats struct to populate
  /// @param link index or name of link
  void populate_interface(struct net_stat *ns, const nl_link_id &link);
};

#endif /* _CONKY_NETLINK_H_ */
