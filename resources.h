#ifndef _RESOURCES_H_
#define _RESOURCES_H_

struct Resources {

    virtual ~Resources() {};

    // Return the time remaining until the next change
    //   of the interface used by the skills (e.g.
    //   the time remaining until the next mana regen
    //   tick).
    virtual int timeUntilNextUpdate() const = 0;

    // Called when the resources object is set to wait
    //   the amount of time specified. Use this method
    //   to modify internal cooldowns or any other field
    //   dependant on time.
    virtual void wait(int time) = 0;

    // Return an exact (deep) copy of the object
    virtual Resources* copy() const = 0;
};

#endif
