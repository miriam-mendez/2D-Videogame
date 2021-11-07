#pragma once
#include "Object.h"
class Flag :
    public Object {

    bool inverted = false;
    bool in_range = false;
    bool part_sound_played = false;
    std::string level;
    const int timer = 700; // time the flag needs to be active to change level
    int remaining_time = timer;
    enum State {
        RISE,
        FALL
    };

    virtual void begin_overlap(b2Contact* contact);
    virtual void end_overlap(b2Contact* contact);
    // checks if one of the ids references this flag and the other is a player
    bool valid_contact(Object::uuid_t id1, Object::uuid_t id2);

public:
    Flag(Object::uuid_t uuid) : Object(uuid) {}

    virtual void init(b2World* physics, ShaderProgram& shaderProgram,
                      bool inverted, std::string const& level);
    void update(int deltaTime) override;
};

