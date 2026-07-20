// Lesson 4 — Test strategy: drive left, double right (S2 isolation).
//
// Claim verified here: after stage S2 of Cockburn's development sequence
// (as documented by co-author Juan M. Garrido de Paz), the hexagon is done —
// business logic runs under a driving *test* adapter with *test doubles* on
// every driven port. No UI. No real database. That isolation is the pattern's
// stated main goal.
//
//   DRIVER          HEXAGON           DRIVEN
//   test cases  →   real app     →    test doubles
//
// Build & run:
//   c++ -std=c++20 -Wall -Wextra -o lesson4_isolation lesson4_isolation.cc \
//     && ./lesson4_isolation

#include <cassert>
#include <string>
#include <unordered_map>
#include <vector>

// ---------- Ports ----------

struct OrderRecord {
  int id;
  std::string sku;
};

struct ForStoringOrders {
  virtual ~ForStoringOrders() = default;
  virtual void save(const OrderRecord&) = 0;
  virtual OrderRecord load(int id) = 0;
};

// Notify-only driven port — secondary actor "to merely notify" (Cockburn 2005).
struct ForNotifyingCustomers {
  virtual ~ForNotifyingCustomers() = default;
  virtual void order_placed(int order_id) = 0;
};

struct ForPlacingOrders {
  virtual ~ForPlacingOrders() = default;
  virtual int place(const std::string& sku) = 0;
};

// ---------- Real hexagon (S2: not hardcoded) ----------

class PlaceOrderApp final : public ForPlacingOrders {
 public:
  PlaceOrderApp(ForStoringOrders& store, ForNotifyingCustomers& notifier)
      : store_(store), notifier_(notifier), next_id_(1) {}

  int place(const std::string& sku) override {
    assert(!sku.empty());  // domain rule lives inside
    OrderRecord order{next_id_++, sku};
    store_.save(order);
    notifier_.order_placed(order.id);  // merely notify
    return order.id;
  }

 private:
  ForStoringOrders& store_;
  ForNotifyingCustomers& notifier_;
  int next_id_;
};

// ---------- Driven-side test doubles (adapters, not ports) ----------

class InMemoryOrderStore final : public ForStoringOrders {
 public:
  void save(const OrderRecord& o) override { by_id_[o.id] = o; }
  OrderRecord load(int id) override { return by_id_.at(id); }

 private:
  std::unordered_map<int, OrderRecord> by_id_;
};

// Spy: records what the app notified — classic secondary-side double.
class SpyNotifier final : public ForNotifyingCustomers {
 public:
  void order_placed(int order_id) override { placed_.push_back(order_id); }
  std::vector<int> placed_;
};

// ---------- Driving-side test adapter (= first user of the app) ----------
//
// Composition root for this "fully isolated" run: wires doubles, then drives
// the app through the driving port — never through a fake UI or real DB.

int main() {
  InMemoryOrderStore store;   // driven double
  SpyNotifier notifier;       // driven double (spy)
  PlaceOrderApp app(store, notifier);

  // Act — test harness drives ForPlacingOrders (same port a CLI/REST would use)
  const int id = app.place("SKU-42");

  // Assert on driving-port answer
  assert(id == 1);
  assert(store.load(1).sku == "SKU-42");

  // Assert on driven-side spy — app did notify, without SMTP existing
  assert(notifier.placed_.size() == 1);
  assert(notifier.placed_[0] == 1);

  // Evidence: S2 complete. Hexagon tested in isolation from real-world devices.
  // No Postgres. No SMTP. No REST. That is the matrix cell this lesson nails.
  return 0;
}
