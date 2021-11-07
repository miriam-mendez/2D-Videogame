#pragma once
#include "Object.h"
#include <vector>

class Lever :
    public Object {
    enum State {
        ACTIVATE,
        DEACTIVATE
    };
    bool requires_sync_with_linked = true;

    bool in_range = false;
    std::vector<Object::uuid_t> links;
    const int switch_delay = 500;
    int current_delay = 0;

    virtual void begin_overlap(b2Contact* contact);
    virtual void end_overlap(b2Contact* contact);
    // checks if one of the ids references this lever and the other is a player
    bool valid_contact(Object::uuid_t id1, Object::uuid_t id2);

    void send_to_linked(bool activated);
public:
    Lever(Object::uuid_t uuid) : Object(uuid) {}

    virtual void init(b2World* physics, ShaderProgram& shaderProgram,
                      int orientation, bool active, std::vector<Object::uuid_t> const& linked);
    void update(int deltaTime) override;
};

