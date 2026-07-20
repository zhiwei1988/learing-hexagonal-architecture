// Lesson 2 — Port granularity & naming.
//
// Claim verified here: a port is a *purposeful conversation*, named by intent.
// Two storage technologies (in-memory and "file-shaped") plug into ONE driven
// port `ForStoringOrders`. The application never names MySQL, files, or SMTP.
// Splitting that conversation into `ForMysql` / `ForPostgres` would be wrong —
// those are adapters, not ports.
//
// Also shows a notify-only driven port (`ForRecordingAudit`) — secondary actor
// "to merely notify" (Cockburn 2005). Fire-and-forget; no answer expected.
//
// Build & run:
//   c++ -std=c++20 -Wall -Wextra -o lesson2_ports lesson2_ports.cc && ./lesson2_ports

#include <cassert>
#include <string>
#include <unordered_map>
#include <vector>

// ---------- Driven ports: named for the conversation, not the technology ----------

struct OrderRecord {
  int id;
  std::string sku;
};

// Purpose: "store orders so I can load them later." Not "talk to MySQL."
struct ForStoringOrders {
  virtual ~ForStoringOrders() = default;
  virtual void save(const OrderRecord&) = 0;
  virtual OrderRecord load(int id) = 0;
};

// Purpose: "record that something happened." Notify-only — no answer needed.
// Same secondary-actor slot as CustomerNotifier / MetricsSink / AuditLog.
struct ForRecordingAudit {
  virtual ~ForRecordingAudit() = default;
  virtual void record(const std::string& event) = 0;
};

// ---------- Driving port ----------

struct ForPlacingOrders {
  virtual ~ForPlacingOrders() = default;
  virtual int place(const std::string& sku) = 0;
};

// ---------- Inside: app holds ports; zero technology names ----------

class PlaceOrderApp final : public ForPlacingOrders {
 public:
  PlaceOrderApp(ForStoringOrders& store, ForRecordingAudit& audit)
      : store_(store), audit_(audit), next_id_(1) {}

  int place(const std::string& sku) override {
    OrderRecord order{next_id_++, sku};
    store_.save(order);
    audit_.record("order_placed:" + std::to_string(order.id));  // merely notify
    return order.id;
  }

 private:
  ForStoringOrders& store_;
  ForRecordingAudit& audit_;
  int next_id_;
};

// ---------- Two adapters for the SAME storage port — different technologies ----------

class InMemoryOrderStore final : public ForStoringOrders {
 public:
  void save(const OrderRecord& o) override { by_id_[o.id] = o; }
  OrderRecord load(int id) override { return by_id_.at(id); }
  std::unordered_map<int, OrderRecord> by_id_;
};

// Pretends to be a file-backed store. Same port. App cannot tell.
class FakeFileOrderStore final : public ForStoringOrders {
 public:
  void save(const OrderRecord& o) override {
    lines_.push_back(std::to_string(o.id) + "," + o.sku);
    by_id_[o.id] = o;
  }
  OrderRecord load(int id) override { return by_id_.at(id); }
  std::vector<std::string> lines_;
  std::unordered_map<int, OrderRecord> by_id_;
};

class RecordingAudit final : public ForRecordingAudit {
 public:
  void record(const std::string& event) override { events_.push_back(event); }
  std::vector<std::string> events_;
};

int main() {
  // --- Run A: in-memory adapter plugged into ForStoringOrders ---
  {
    InMemoryOrderStore store;
    RecordingAudit audit;
    PlaceOrderApp app(store, audit);

    const int id = app.place("SKU-1");
    assert(id == 1);
    assert(store.load(1).sku == "SKU-1");
    assert(audit.events_.size() == 1);
    assert(audit.events_[0] == "order_placed:1");
  }

  // --- Run B: swap the storage TECHNOLOGY; same port, same app code ---
  {
    FakeFileOrderStore store;
    RecordingAudit audit;
    PlaceOrderApp app(store, audit);

    const int id = app.place("SKU-2");
    assert(id == 1);
    assert(store.load(1).sku == "SKU-2");
    assert(store.lines_.size() == 1);           // file adapter's private detail
    assert(store.lines_[0] == "1,SKU-2");
    assert(audit.events_[0] == "order_placed:1");
  }

  // Evidence: one purposeful conversation (ForStoringOrders), two adapters.
  // Had we named ports ForMysql / ForFile, the app would have two dependencies
  // for one conversation — that's the smell this lesson trains you to reject.
  return 0;
}
