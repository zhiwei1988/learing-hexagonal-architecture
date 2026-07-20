// Lesson 3 — Configurable dependency & the composition root.
//
// Claim verified here: the *configurator* (Cockburn Budapest 2023; aka
// Composition Root — Seemann) is the only place that names concrete adapters.
// The application holds non-owning references to ports. The same app type is
// wired once for a "production-shaped" adapter and once for a test double —
// application source unchanged.
//
// C++ ownership rule this file proves:
//   - Composition root OWNS adapters (stack / unique_ptr).
//   - App HOLDS ForX& (non-owning). App never new's an adapter.
//
// Build & run:
//   c++ -std=c++20 -Wall -Wextra -o lesson3_composition lesson3_composition.cc \
//     && ./lesson3_composition

#include <cassert>
#include <memory>
#include <string>
#include <unordered_map>

// ---------- Ports: owned by the application (declarations live with the app) ----------

struct OrderRecord {
  int id;
  std::string sku;
};

struct ForStoringOrders {
  virtual ~ForStoringOrders() = default;
  virtual void save(const OrderRecord&) = 0;
  virtual OrderRecord load(int id) = 0;
};

struct ForPlacingOrders {
  virtual ~ForPlacingOrders() = default;
  virtual int place(const std::string& sku) = 0;
};

// ---------- Inside the hexagon: depends on ports only, never owns adapters ----------

class PlaceOrderApp final : public ForPlacingOrders {
 public:
  // Constructor injection of a *required* interface (driven port).
  // Reference = "I use this conversation; I do not own its lifetime."
  explicit PlaceOrderApp(ForStoringOrders& store) : store_(store), next_id_(1) {}

  int place(const std::string& sku) override {
    OrderRecord order{next_id_++, sku};
    store_.save(order);
    return order.id;
  }

  // Deliberately absent (would be the smell this lesson rejects):
  //   PlaceOrderApp() : store_(*new PostgresOrderStore()) {}
  //   #include <libpq-fe.h> anywhere in this translation unit.

 private:
  ForStoringOrders& store_;  // non-owning
  int next_id_;
};

// ---------- Adapters: technology names allowed HERE ----------

class InMemoryOrderStore final : public ForStoringOrders {
 public:
  void save(const OrderRecord& o) override { by_id_[o.id] = o; }
  OrderRecord load(int id) override { return by_id_.at(id); }

 private:
  std::unordered_map<int, OrderRecord> by_id_;
};

// "Production-shaped" — name carries the technology; still just an adapter.
class PostgresOrderStore final : public ForStoringOrders {
 public:
  void save(const OrderRecord& o) override {
    // Pretend libpq wrote a row. Real driver stays out of the lesson binary.
    rows_written_++;
    by_id_[o.id] = o;
  }
  OrderRecord load(int id) override { return by_id_.at(id); }
  int rows_written() const { return rows_written_; }

 private:
  int rows_written_ = 0;
  std::unordered_map<int, OrderRecord> by_id_;
};

// ---------- Composition roots: the ONLY places that name concrete adapters ----------

// Production entry — Cockburn's "configurator" / Seemann's Composition Root.
void compose_production() {
  auto store = std::make_unique<PostgresOrderStore>();  // root OWNS
  PlaceOrderApp app(*store);                            // app BORROWS

  const int id = app.place("SKU-PROD");
  assert(id == 1);
  assert(store->load(1).sku == "SKU-PROD");
  assert(store->rows_written() == 1);
  // store dies after app — root controls lifetime. App never delete'd anything.
}

// Test entry — also a composition root (same privilege as main).
void compose_test() {
  InMemoryOrderStore store;  // stack ownership is fine
  PlaceOrderApp app(store);

  const int id = app.place("SKU-TEST");
  assert(id == 1);
  assert(store.load(1).sku == "SKU-TEST");
}

int main() {
  compose_production();
  compose_test();
  // Evidence: PlaceOrderApp's source mentions neither Postgres nor InMemory.
  // Only the two composition roots decide which adapter plugs into ForStoringOrders.
  return 0;
}
