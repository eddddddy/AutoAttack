#include <iostream>
#include <string>
#include <vector>
#include <random>
#include <memory>
#include <utility>

#include "skill.h"
#include "resources.h"
#include "node.h"

// Example (a simplified BM rotation): 3 skills called Lunar Slash, 
//   Dragon Tongue, and Flicker, with their effects as follows:
//   - Lunar Slash: Has 400 millisecond cast time, does 100 damage
//     non-crit and 180 damage crit, with 40% chance of critting.
//     Has 18 sec cooldown measured from the end of its cast. Triggers
//     conflagration for 3 secs measured from the end of its cast.
//   - Dragon Tongue: Has 400 millisecond cast time, does 120 damage
//     non-crit and 200 damage crit when conflagration is down, with
//     40% chance of critting, and does 180 damage non-crit and 320
//     damage crit when conflagration is up, with 60% chance of critting.
//     Has 6 sec cooldown measured from the end of its cast only if
//     conflagration is down. Reduces the cooldown of Lunar Slash by
//     2 secs on every cast.
//   - Flicker: has 250 millisecond cast time, does 40 damage non-crit
//     and 60 damage crit, with 40% chance of critting. Has no cooldown.
//     Reduces the cooldown of Dragon Tongue by 1 sec on every cast.
//   In addition, there is a resource called Focus, starting at 10
//   units. One unit is regenerated every second when it is not at
//   its maximum. Every cast of Lunar Slash triggers regeneration of
//   3 units of Focus immediately after cast, in addition to 3 units
//   every second for 6 seconds. Every Dragon Tongue costs 2 units
//   when conflagration is down and 1 unit when conflagration is up.
//   Every Flicker costs 1 unit.

class LunarSlash;
class DragonTongue;
class Flicker;

struct BMResources : public Resources {

    int focus = 10;
    bool conflagrationUp() const {return conflagration;}
    int timeUntilNextUpdate() const override {return 1000 - focusRegenOffset;}

    void wait(int time) override {
        if (conflagration) {
            conflagrationTimeLeft -= time;
            if (conflagrationTimeLeft <= 0) {
                conflagrationTimeLeft = 0;
                conflagration = false;
            }
        }
        if (focus < 10) {
            focusRegenOffset += time;
            if (focusRegenOffset >= 1000) {
                focus += 1;
                focusRegenOffset -= 1000;
                if (focus == 10) {
                    focusRegenOffset = 0;
                }
            }
        }
    }

    Resources* copy() const override {
        BMResources* newResources = new BMResources();
        newResources->focus = focus;
        newResources->focusRegenOffset = focusRegenOffset;
        newResources->conflagration = conflagration;
        newResources->conflagrationTimeLeft = conflagrationTimeLeft;
        newResources->timeSinceLastLS = timeSinceLastLS;
        return newResources;
    }

    void notify(LunarSlash* ls) {
        conflagration = true;
        conflagrationTimeLeft = 3000;
        timeSinceLastLS = 0;
    }

    void notify(DragonTongue* dt) {focus -= (conflagration ? 1 : 2);}
    void notify(Flicker* fl) {focus -= 1;}

    private:
        int focusRegenOffset = 0;
        bool conflagration = false;
        int conflagrationTimeLeft = 0;
        int timeSinceLastLS = 3600000;

};

class LunarSlash : public Skill {
    static std::mt19937 mt;
    static std::uniform_int_distribution<int> dist;

    int cd = 0;

    void notify(Skill* from) override;
    void notifyResources() override {static_cast<BMResources*>(resources)->notify(this);}
    void useSkill() override {cd = 18000;}
    Skill* copy() const override {LunarSlash* ls = new LunarSlash{}; ls->cd = cd; return ls;}

public:
    bool isReady() const override {return cd == 0;}
    int timeUntilReady() const override {return cd;}
    void wait(int time) override {cd = cd < time ? 0 : cd - time;}
    int getDamage() const override {return (dist(mt) >= 4) ? 180 : 100;}
    int getCastTime() const override {return 400;}
    std::string toString() const override {return "LS";}
};

class DragonTongue : public Skill {
    static std::mt19937 mt;
    static std::uniform_int_distribution<int> dist;

    int cd = 0;

    void notify(Skill* from) override;
    void notifyResources() override {static_cast<BMResources*>(resources)->notify(this);}
    void useSkill() override {if (!static_cast<BMResources*>(resources)->conflagrationUp()) cd = 6000;}
    Skill* copy() const override {DragonTongue* dt = new DragonTongue{}; dt->cd = cd; return dt;}

public:
    bool isReady() const override {
        if (static_cast<BMResources*>(resources)->conflagrationUp()) return static_cast<BMResources*>(resources)->focus >= 1;
        else return cd == 0 && static_cast<BMResources*>(resources)->focus >= 2;
    }
    int timeUntilReady() const override {
        BMResources* r = static_cast<BMResources*>(resources);
        if (r->conflagrationUp()) return r->focus >= 1 ? 0 : 3600000;
        else return r->focus >= 2 ? cd : 3600000;
    }
    void wait(int time) override {cd = cd < time ? 0 : cd - time;}
    int getDamage() const override {
        if (static_cast<BMResources*>(resources)->conflagrationUp()) return (dist(mt) >= 3) ? 320 : 180;
        else return (dist(mt) >= 4) ? 200 : 120;
    }
    int getCastTime() const override {return 400;}
    std::string toString() const override {return "DT";}
};

class Flicker : public Skill {
    static std::mt19937 mt;
    static std::uniform_int_distribution<int> dist;

    void notify(Skill* from) override {}
    void notifyResources() override {static_cast<BMResources*>(resources)->notify(this);}
    void useSkill() override {}
    Skill* copy() const override {return new Flicker{};}

public:
    bool isReady() const override {return static_cast<BMResources*>(resources)->focus >= 1;}
    int timeUntilReady() const override {return static_cast<BMResources*>(resources)->focus >= 1 ? 0 : 3600000;}
    void wait(int time) override {}
    int getDamage() const override {return (dist(mt) >= 4) ? 60 : 40;}
    int getCastTime() const override {return 250;}
    std::string toString() const override {return "FL";}
};

void LunarSlash::notify(Skill* from) {if (dynamic_cast<DragonTongue*>(from)) cd = cd < 2000 ? 0 : cd - 2000;}

void DragonTongue::notify(Skill* from) {
    if (dynamic_cast<Flicker*>(from)) cd = cd < 1000 ? 0 : cd - 1000;
    else if (dynamic_cast<LunarSlash*>(from)) cd = 0;
}

std::random_device rd;
std::mt19937 LunarSlash::mt = std::mt19937{rd()};
std::uniform_int_distribution<int> LunarSlash::dist = std::uniform_int_distribution<int>{1, 5};
std::mt19937 DragonTongue::mt = std::mt19937{rd()};
std::uniform_int_distribution<int> DragonTongue::dist = std::uniform_int_distribution<int>{1, 5};
std::mt19937 Flicker::mt = std::mt19937{rd()};
std::uniform_int_distribution<int> Flicker::dist = std::uniform_int_distribution<int>{1, 5};

int main(int argc, char* argv[]) {

    double cPUCT = 1;
    if (argc > 1) cPUCT = std::stod(std::string(argv[1]));

    std::vector<std::unique_ptr<Skill>> skills;
    std::unique_ptr<Resources> resources = std::make_unique<BMResources>();
    skills.emplace_back(std::make_unique<LunarSlash>());
    skills.emplace_back(std::make_unique<DragonTongue>());
    skills.emplace_back(std::make_unique<Flicker>());
    skills[0]->addObserver(skills[1].get());
    skills[1]->addObserver(skills[0].get());
    skills[2]->addObserver(skills[1].get());
    skills[0]->setResources(resources.get());
    skills[1]->setResources(resources.get());
    skills[2]->setResources(resources.get());
    auto state = std::make_unique<State>();
    state->setSkills(std::move(skills));
    state->setResources(std::move(resources));
    Node root;
    root.setState(std::move(state));

    for (int i = 0; i < 1000000000; i++) {
        root.playout(cPUCT);
        if (i % 100000 == 0) std::cout << root.currentBestPath() << std::endl;
    }
}
