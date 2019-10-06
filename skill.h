#ifndef _SKILL_H_
#define _SKILL_H_

#include <string>
#include <vector>
#include <unordered_map>

struct Resources;

class Skill {

    private:
        std::vector<Skill*> observers;
        void notifyObservers();

    protected:
        Resources* resources;

    public:

        // Set the resources of the skill
        void setResources(Resources* resources);

        // Set the given skill as an observer for the skill that
        //   this method is called on. The notify() method of the
        //   observer will be called if this skill is used.
        void addObserver(Skill* skill);

        // Use the current skill. The skill must be ready (i.e.
        //   the return value of isReady() must be true), otherwise
        //   the process will immediately terminate. This is a
        //   template method that does the following operations:
        //   useSkill() is called on this object, then every skill
        //   that is set as an observer of this skill will have
        //   its notify() method called, with the used skill passed
        //   as the parameter, then notifyResources() is called on
        //   this object.
        void use();

        // Return a pointer to a deep copy of the current Skill
        //   object. The copy will have all of its observers set
        //   in a new memory location, either newly allocated
        //   or defined in the map passed in, but will not have
        //   its resources set. The map passed in will be modified
        //   so that it maps the memory locations of the current
        //   skill object and its observers to the memory locations
        //   of the new skill object and its new observers.
        Skill* deepCopy(std::unordered_map<Skill*, Skill*>& copied);

    ///////////////////////////////////////////////////////////////
    // VIRTUAL METHODS FOR SUBCLASSING

    private: 

        // Called when a skill that the current skill is observing
        //   is used. Use this method to modify any internal fields.
        virtual void notify(Skill* from) = 0;

        // Called when use() is called on this skill. Use this
        //   method to modify the fields of the resources object.
        virtual void notifyResources() = 0;

        // Called when use() is called on this skill. Use this
        //   method to modify any internal fields.
        virtual void useSkill() = 0;

        // Return a pointer to an object of the same class that is
        //   an exact deep copy of the object it is called on. The
        //   object must only set fields defined directly in the
        //   subclass; that is, the object must not set its own
        //   observers or its own resources.
        virtual Skill* copy() const = 0;

    public:
        virtual ~Skill();

        // Return true if the skill is available for use in
        //   the current state, otherwise return false
        virtual bool isReady() const = 0;

        // Return the time remaining until the skill is next
        //   available for use. If the skill is currently
        //   available for use, this value is unspecified.
        virtual int timeUntilReady() const = 0;

        // Called when the current skill object is set to wait
        //   the amount of time specified. Use this method
        //   to modify internal cooldowns or any other field
        //   dependant on time.
        virtual void wait(int time) = 0;

        // Return the damage that a cast of this skill would cause.
        //   This value does not need to be deterministic given the
        //   the current internal state. If the skill is not ready
        //   to cast, this value is unspecified.
        virtual int getDamage() const = 0;

        // Return the time that this skill would take to finish
        //   execution (also known as the time of its animation lock).
        //   This value MUST be deterministic given the current
        //   internal state, otherwise any behaviour in this module
        //   would be undefined. If the skill is not ready to cast,
        //   this value is unspecified.
        virtual int getCastTime() const = 0;

        // Return a string representation of the current Skill
        //   object. This representation will be used to print
        //   the optimal rotations.
        virtual std::string toString() const = 0;
};

#endif
