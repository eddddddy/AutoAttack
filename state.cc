#include <vector>
#include <unordered_map>
#include <memory>
#include <utility>

#include "skill.h"
#include "resources.h"
#include "state.h"

void State::setSkills(std::vector<std::unique_ptr<Skill>>&& skills) {
    this->skills = std::move(skills);
}

void State::setResources(std::unique_ptr<Resources>&& resources) {
    this->resources = std::move(resources);
}

State* State::copy(std::unordered_map<Skill*, Skill*>& copied) const {
    State* stateCopy = new State;

    stateCopy->resources = std::unique_ptr<Resources>{resources->copy()};
    Resources* newResources = stateCopy->resources.get();

    for (auto it = skills.begin(); it != skills.end(); ++it) {
        Skill* skill = (*it).get();
        std::unique_ptr<Skill> newSkill{skill->deepCopy(copied)};
        newSkill->setResources(newResources);
        stateCopy->skills.emplace_back(std::move(newSkill));
    }

    return stateCopy;
}

std::vector<Skill*> State::getAvailableSkills() const {
    std::vector<Skill*> availableSkills;
    for (unsigned i = 0; i < skills.size(); i++) {
        if (skills[i]->isReady()) availableSkills.emplace_back(skills[i].get());
    }
    return availableSkills;
}

void State::useSkill(Skill* skill, int time) {
    if (skill) {
        skill->use();
        for (auto it = skills.begin(); it != skills.end(); ++it) {
            if ((*it).get() != skill) (*it)->wait(time);
        }
    } else {
        for (auto it = skills.begin(); it != skills.end(); ++it) {
            (*it)->wait(time);
        }
    }
    resources->wait(time);
}

int State::getWaitTime() const {
    int time = 3600000;
    for (unsigned i = 0; i < skills.size(); i++) {
        Skill* skill = skills[i].get();
        if (!skill->isReady() && skill->timeUntilReady() < time) time = skill->timeUntilReady();
    }
    Resources* r = resources.get();
    if (r->timeUntilNextUpdate() < time) time = r->timeUntilNextUpdate();

    return time;
}
