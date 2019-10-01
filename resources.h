#ifndef _RESOURCES_H_
#define _RESOURCES_H_

struct Resources {
    virtual ~Resources() {};
    virtual void wait(int time) = 0;
    virtual Resources* copy() const = 0;
};

#endif
