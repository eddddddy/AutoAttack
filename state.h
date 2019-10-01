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

        // deep copies the skills and resources
        State copy(std::unordered_map<Skill*, Skill*>& copied) const;

        std::vector<Skill*> getAvailableSkills() const;

        // the skill must be nullptr, or otherwise must be ready and one
        //   of the pointers in the skills vector
        void useSkill(Skill* skill);

        int getWaitTime() const;
};

#endif
