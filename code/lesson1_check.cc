// Lesson 1 — From layers to hexagon: the driving side.
//
// Claim verified here: the *test* drives the application through the very same
// driving port a REST controller (or CLI) would use, and the app cannot tell
// the difference. That symmetry is Cockburn's stated intent:
//   "Allow an application to equally be driven by users, programs, automated
//    test or batch scripts..."
//
// Build & run:  c++ -std=c++20 -Wall -Wextra -o lesson1_check lesson1_check.cc && ./lesson1_check

#include <cassert>
#include <memory>
#include <string>
#include <vector>

// ---------- Inside the hexagon: domain ----------

struct Order {
  int id;
  std::string sku;
  int quantity;
  bool quantity_valid() const { return quantity > 0; }  // invariant lives here
};

// ---------- Driven port (right side): what the app NEEDS from outside ----------

struct OrderRepository {
  virtual ~OrderRepository() = default;
  virtual int next_id() = 0;
  virtual void save(const Order&) = 0;
};

struct CustomerNotifier {
  virtual ~CustomerNotifier() = default;
  virtual void order_placed(int order_id) = 0;
};

// ---------- Driving port (left side): what the app OFFERS to outside ----------
// Named after the conversation ("place an order"), not after any technology.

struct PlaceOrderCommand {
  std::string sku;
  int quantity;
};

struct PlaceOrder {
  virtual ~PlaceOrder() = default;
  virtual int execute(const PlaceOrderCommand&) = 0;  // returns new order id
};

// ---------- Inside the hexagon: application service implements the driving port ----------

class PlaceOrderService final : public PlaceOrder {
 public:
  PlaceOrderService(OrderRepository& repo, CustomerNotifier& notifier)
      : repo_(repo), notifier_(notifier) {}

  int execute(const PlaceOrderCommand& cmd) override {
    Order order{repo_.next_id(), cmd.sku, cmd.quantity};
    assert(order.quantity_valid());  // domain rule, technology-free
    repo_.save(order);
    notifier_.order_placed(order.id);
    return order.id;
  }

 private:
  OrderRepository& repo_;      // the app holds the PORT, never an adapter type
  CustomerNotifier& notifier_;
};

// ---------- Driven adapters: test doubles ARE adapters, same rank as SQL/SMTP ----------

class InMemoryOrderRepository final : public OrderRepository {
 public:
  int next_id() override { return static_cast<int>(saved_.size()) + 1; }
  void save(const Order& o) override { saved_.push_back(o); }
  std::vector<Order> saved_;
};

class RecordingNotifier final : public CustomerNotifier {
 public:
  void order_placed(int order_id) override { notified_.push_back(order_id); }
  std::vector<int> notified_;
};

// ---------- Driving adapters: two different drivers, one identical port ----------

// Driver #1: a "controller" translating an HTTP-ish request into the port's language.
int fake_http_controller(PlaceOrder& app, const std::string& body_sku, int body_qty) {
  return app.execute(PlaceOrderCommand{body_sku, body_qty});
}

// Driver #2: the test itself — no controller, straight into the same port.
int main() {
  InMemoryOrderRepository repo;   // driven adapter, plugged in from outside
  RecordingNotifier notifier;     // driven adapter, plugged in from outside
  PlaceOrderService app(repo, notifier);

  // The test drives the app exactly like the controller does: same port, same rank.
  const int id_from_test = app.execute(PlaceOrderCommand{"SKU-42", 3});
  const int id_from_http = fake_http_controller(app, "SKU-7", 1);

  assert(id_from_test == 1);
  assert(id_from_http == 2);
  assert(repo.saved_.size() == 2);
  assert(repo.saved_[0].sku == "SKU-42");
  assert(notifier.notified_ == (std::vector<int>{1, 2}));

  // Nothing in PlaceOrderService names HTTP, SQL, SMTP, or "test".
  // Swap any adapter; the hexagon's interior does not change.
  return 0;
}
