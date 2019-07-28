#include "game_object.h"
#include "geo2d.h"

#include "test_runner.h"

#include <vector>
#include <memory>

using namespace std;

#define COLIDE(object)                                     \
    bool object::CollideWith(const Unit& that) const {     \
        return geo2d::Collide(geo, that.Geo());            \
    }                                                      \
    bool object::CollideWith(const Building& that) const { \
        return geo2d::Collide(geo, that.Geo());            \
    }                                                      \
    bool object::CollideWith(const Tower& that) const {    \
        return geo2d::Collide(geo, that.Geo());            \
    }                                                      \
    bool object::CollideWith(const Fence& that) const {    \
        return geo2d::Collide(geo, that.Geo());            \
    }                                                      \
                                                           \

template <typename T>
struct Colider : GameObject {
    bool Collide(const GameObject& that) const final {
        return that.CollideWith(static_cast<const T&>(*this));
    }
};

class Unit final : public Colider<Unit> {
public:
    explicit Unit(geo2d::Point position) : geo(position) {}
    geo2d::Point Geo() const {
        return geo;
    }

    bool CollideWith(const Unit& that) const override;
    bool CollideWith(const Building& that) const override;
    bool CollideWith(const Tower& that) const override;
    bool CollideWith(const Fence& that) const override;
private:
    geo2d::Point geo;
};

class Building final : public Colider<Building> {
public:
    explicit Building(geo2d::Rectangle geometry) : geo(geometry) {}
    geo2d::Rectangle Geo() const {
        return geo;
    }

    bool CollideWith(const Unit& that) const override;
    bool CollideWith(const Building& that) const override;
    bool CollideWith(const Tower& that) const override;
    bool CollideWith(const Fence& that) const override;
private:
    geo2d::Rectangle geo;
};

class Tower final : public Colider<Tower> {
public:
    explicit Tower(geo2d::Circle geometry) : geo(geometry) {}
    geo2d::Circle Geo() const {
        return geo;
    }

    bool CollideWith(const Unit& that) const override;
    bool CollideWith(const Building& that) const override;
    bool CollideWith(const Tower& that) const override;
    bool CollideWith(const Fence& that) const override;
private:
    geo2d::Circle geo;
};

class Fence final : public Colider<Fence> {
public:
    explicit Fence(geo2d::Segment geometry) : geo(geometry) {}
    geo2d::Segment Geo() const {
        return geo;
    }

    bool CollideWith(const Unit& that) const override;
    bool CollideWith(const Building& that) const override;
    bool CollideWith(const Tower& that) const override;
    bool CollideWith(const Fence& that) const override;
private:
    geo2d::Segment geo;
};

COLIDE(Unit)
COLIDE(Fence)
COLIDE(Tower)
COLIDE(Building)

bool Collide(const GameObject& first, const GameObject& second) {
    return first.Collide(second);
}

void TestAddingNewObjectOnMap() {
using namespace geo2d;

    const vector<shared_ptr<GameObject>> game_map = {
            make_shared<Unit>(Point{3, 3}),
            make_shared<Unit>(Point{5, 5}),
            make_shared<Unit>(Point{3, 7}),
            make_shared<Fence>(Segment{{7, 3}, {9, 8}}),
            make_shared<Tower>(Circle{Point{9, 4}, 1}),
            make_shared<Tower>(Circle{Point{10, 7}, 1}),
            make_shared<Building>(Rectangle{{11, 4}, {14, 6}})
    };

    for (size_t i = 0; i < game_map.size(); ++i) {
        Assert(
                Collide(*game_map[i], *game_map[i]),
                "An object doesn't collide with itself: " + to_string(i)
        );

        for (size_t j = 0; j < i; ++j) {
            Assert(
                    !Collide(*game_map[i], *game_map[j]),
                    "Unexpected collision found " + to_string(i) + ' ' + to_string(j)
            );
        }
    }

    auto new_warehouse = make_shared<Building>(Rectangle{{4, 3}, {9, 6}});
    ASSERT(!Collide(*new_warehouse, *game_map[0]));
    ASSERT( Collide(*new_warehouse, *game_map[1]));
    ASSERT(!Collide(*new_warehouse, *game_map[2]));
    ASSERT( Collide(*new_warehouse, *game_map[3]));
    ASSERT( Collide(*new_warehouse, *game_map[4]));
    ASSERT(!Collide(*new_warehouse, *game_map[5]));
    ASSERT(!Collide(*new_warehouse, *game_map[6]));

    auto new_defense_tower = make_shared<Tower>(Circle{{8, 2}, 2});
    ASSERT(!Collide(*new_defense_tower, *game_map[0]));
    ASSERT(!Collide(*new_defense_tower, *game_map[1]));
    ASSERT(!Collide(*new_defense_tower, *game_map[2]));
    ASSERT( Collide(*new_defense_tower, *game_map[3]));
    ASSERT( Collide(*new_defense_tower, *game_map[4]));
    ASSERT(!Collide(*new_defense_tower, *game_map[5]));
    ASSERT(!Collide(*new_defense_tower, *game_map[6]));
}

int main() {
    TestRunner tr;
    RUN_TEST(tr, TestAddingNewObjectOnMap);
    return 0;
}
