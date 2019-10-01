#ifndef _SKILL_H_
#define _SKILL_H_

#include <vector>
#include <unordered_map>

struct Resources;

class Skill {

    private:
        std::vector<Skill*> observers;
        void notifyObservers();

        virtual void notify(Skill* from) = 0;

        virtual void notifyResources() = 0;

        virtual void useSkill() = 0;

        // the copy must not have its resources field set or
        //   contain any observers
        virtual Skill* copy() const = 0;

    protected:
        Resources* resources;

    public:
        virtual ~Skill();

        void setResources(Resources* resources);
        void addObserver(Skill* skill);

        void use();

        virtual bool isReady() const = 0;
        virtual int timeUntilReady() const = 0;
        virtual void wait(int time) = 0;

        // only call if ready
        virtual int getDamage() const = 0;    // need not be deterministic (e.g. crits)
        virtual int getCastTime() const = 0;  // must be fully deterministic based on state

        Skill* deepCopy(std::unordered_map<Skill*, Skill*>& copied);
};

#endif
