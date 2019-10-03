#ifndef _STATE_H_
#define _STATE_H_

#include <vector>
#include <unordered_map>
#include <memory>

class Skill;
class Resources;

class State final {

    private:
        std::vector<std::unique_ptr<Skill>> skills;
        std::unique_ptr<Resources> resources;

    public:

        // Set all the skills, stealing ownership of the vector and
        //   every pointer it stores. The skills in the vector must
        //   already have all of their observers set (if there are
        //   any) and their resources set to the same object. This
        //   method and the next must be called before any other
        //   method is called, otherwise behaviour is undefined.
        //   This method must be called only once, otherwise
        //   behaviour is also undefined.
        void setSkills(std::vector<std::unique_ptr<Skill>>&& skills);

        // Set the resources of the skills, stealing ownership of
        //   the pointer passed in. The resources passed in must
        //   be the resources of every skill in the skills vector.
        //   This method and the previoius must be called before
        //   any other method is called, otherwise behaviour is
        //   undefined. This method must be called only once,
        //   otherwise behaviour is also undefined.
        void setResources(std::unique_ptr<Resources>&& resources);

        // Return a pointer to a deep copy of the current Skill
        //   object. The copy will have all of its skills and
        //   resources in a different memory location, with their
        //   observers set to the new locations. The map passed
        //   in will be modified so that it maps the memory
        //   location of every old skill pointer to the new
        //   memory location of the corresponding pointer.
        State* copy(std::unordered_map<Skill*, Skill*>& copied) const;

        std::vector<Skill*> getAvailableSkills() const;

        // the skill must be nullptr, or otherwise must be ready and one
        //   of the pointers in the skills vector
        void useSkill(Skill* skill, int time);

        int getWaitTime() const;
};

#endif
