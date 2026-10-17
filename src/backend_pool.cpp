#include <erxd/backend_pool.hpp>

#include <limits>
#include <utility>

namespace erxd {
std::optional<BackendId> BackendPool::add_backend(BackendSpec backend) {
  if (backend.host.empty() || backend.port == 0 ||
      next_backend_id_ == std::numeric_limits<BackendId>::max()) {
    return std::nullopt;
  }

  const BackendId id = next_backend_id_++;
  backends_.emplace(id,
                    BackendEndpoint{id, std::move(backend.host), backend.port});
  selection_order_.push_back(id);
  return id;
}

bool BackendPool::remove_backend(BackendId id) {
  auto it = backends_.find(id);
  if (it == backends_.end()) {
    return false;
  }

  backends_.erase(it);
  return true;
}

std::optional<BackendEndpoint> BackendPool::next() {
  if (selection_order_.empty()) {
    return std::nullopt;
  }

  for (std::size_t checked = 0; checked < selection_order_.size(); ++checked) {
    const BackendId id = selection_order_[selection_index_];
    selection_index_ = (selection_index_ + 1) % selection_order_.size();

    const auto it = backends_.find(id);
    if (it != backends_.end()) {
      return it->second;
    }
  }

  return std::nullopt;
}
} // namespace erxd
