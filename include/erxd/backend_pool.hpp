#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace erxd {

using BackendId = std::uint16_t;

// Connection details supplied when registering a backend.
struct BackendSpec {
  std::string host;
  std::uint16_t port;
};

// Connection details returned for a registered backend.
struct BackendEndpoint {
  BackendId id;
  std::string host;
  std::uint16_t port;
};

// Selects and manages backend endpoints without exposing storage details.
class IBackendPool {
public:
  virtual ~IBackendPool() = default;

  // Returns the next backend, or no value when the pool is empty.
  virtual std::optional<BackendEndpoint> next() = 0;

  // Generates an ID and registers the backend. Returns no value on error.
  virtual std::optional<BackendId> add_backend(BackendSpec backend) = 0;

  // Removes a backend. Returns true when removed, false when not found.
  virtual bool remove_backend(BackendId id) = 0;
};

// Default backend-pool implementation.
class BackendPool final : public IBackendPool {
public:
  std::optional<BackendEndpoint> next() override;
  std::optional<BackendId> add_backend(BackendSpec backend) override;
  bool remove_backend(BackendId id) override;

private:
  std::unordered_map<BackendId, BackendEndpoint> backends_;
  // Removed IDs remain as selection-order tombstones.
  std::vector<BackendId> selection_order_;
  // IDs are monotonic and are never reassigned.
  BackendId next_backend_id_{1};
  std::size_t selection_index_{0};
};

} // namespace erxd
