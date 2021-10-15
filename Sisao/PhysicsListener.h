#pragma once
#include "box2d/b2_world.h"
#include "box2d/b2_world_callbacks.h"
#include <unordered_map>

template<class Derived>
class PhysicsListener :
    public b2ContactListener {

    static std::unordered_map<int32_t, PhysicsListener<Derived>*> subscribers;
    const int32_t subscriber_id = 0;

    void BeginContact(b2Contact*) override;
    void EndContact(b2Contact*) override;

protected:
    static uint32_t gen_listener_id() {
        static uint32_t i = 0;
        return i++;
    }

    virtual void begin_overlap(b2Contact* func); // noop
    virtual void end_overlap(b2Contact* func); // noop

public:
    PhysicsListener() : subscriber_id(gen_listener_id()) {
        subscribers.emplace(subscriber_id, this);
    }

    ~PhysicsListener() {
        subscribers.erase(subscriber_id);
    }
};

template<class Derived>
std::unordered_map<int32_t, PhysicsListener<Derived>*> PhysicsListener<Derived>::subscribers;

template<class Derived>
inline void PhysicsListener<Derived>::BeginContact(b2Contact* contact) {
    for (auto const& x : subscribers) {
        static_cast<Derived*>(x.second)->begin_overlap(contact);
    }
}

template<class Derived>
inline void PhysicsListener<Derived>::EndContact(b2Contact* contact) {
    for (auto const& x : subscribers) {
        static_cast<Derived*>(x.second)->end_overlap(contact);
    }
}

template<class Derived>
inline void PhysicsListener<Derived>::begin_overlap(b2Contact* func) {}

template<class Derived>
inline void PhysicsListener<Derived>::end_overlap(b2Contact* func) {}
