#include <iostream>

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
//   - Flicker: has 250 millisecond cast time, does 60 damage non-crit
//     and 100 damage crit, with 40% chance of critting. Has no cooldown.
//     Reduces the cooldown of Dragon Tongue by 1 sec on every cast.
//   In addition, there is a resource called Focus, starting at 10
//   units. One unit is regenerated every second when it is not at
//   its maximum. Every cast of Lunar Slash triggers regeneration of
//   3 units of Focus immediately after cast, in addition to 3 units
//   every second for 6 seconds. Every Dragon Tongue costs 2 units
//   when conflagration is down and 1 unit when conflagration is up.
//   Every Flicker costs 1 unit.


struct BMResources : public Resources {

    int focus = 10;

    bool conflagrationUp() const {
        return conflagration;
    }

    void wait(int time) {
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

    Resources* copy() const {
        BMResources* newResources = new BMResources();
        newResources->focus = focus;
        newResources->focusRegenOffset = focusRegenOffset;
        newResources->conflagration = conflagration;
        newResources->conflagrationTimeLeft = conflagrationTimeLeft;
        return newResources;
    }

    void notify(LunarSlash* ls) {
        conflagration = true;
        conflagrationTimeLeft = 3000;
    }

    void notify(DragonTongue* dt) {
        
    }

    void notify(Flicker* fl) {

    }

    private:
        int focusRegenOffset = 0;
        bool conflagration;
        int conflagrationTimeLeft = 0;

}

class LunarSlash : public Skill {



};

class DragonTongue : public Skill {

}

class Flicker : public Skill {

}

class B : public Skill {

    int cd = 0;
    int remainingBuffs = 0;

    void notify(Skill* from) override;
    void notifyResources() override {}
    void useSkill() override {cd = 1000; if (remainingBuffs > 0) {remainingBuffs--;}}

    Skill* copy() const override {
        B* copyB = new B{};
        copyB->cd = cd;
        copyB->remainingBuffs = remainingBuffs;
        return copyB;
    }

    public:

    bool isReady() const override {return cd == 0;}
    int timeUntilReady() const override {return cd;}
    void wait(int time) override {cd = cd < time ? 0 : cd - time;}

    int getDamage() const override {if (remainingBuffs > 0) {return 90;} else {return 30;}}
    int getCastTime() const override {return 400;}
};

class A : public Skill {

    int cd = 0;

    void notify(Skill* from) override {
        if (dynamic_cast<B*>(from)) {
            cd = cd < 3000 ? 0 : cd - 3000;
        }
    }
    void notifyResources() override {}
    void useSkill() override {cd = 18000;}

    Skill* copy() const override {
        A* copyA = new A{};
        copyA->cd = cd;
        return copyA;
    }

    public:

    bool isReady() const override {return cd == 0;}
    int timeUntilReady() const override {return cd;}
    void wait(int time) override {cd = cd < time ? 0 : cd - time;}

    int getDamage() const override {return 60;}
    int getCastTime() const override {return 500;}
};

void B::notify(Skill* from) {
    if (dynamic_cast<A*>(from)) {
        remainingBuffs = 3;
    }
}

int main() {

}
