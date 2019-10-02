#include <vector>
#include <unordered_map>

#include "skill.h"

Skill::~Skill() {}

void Skill::notifyObservers() {
    for (Skill* skill : observers) {
        skill->notify(this);
    }
}

void Skill::setResources(Resources* resources) {
    this->resources = resources;
}

void Skill::addObserver(Skill* skill) {
    observers.emplace_back(skill);
}

void Skill::use() {
    if (!isReady()) {throw std::terminate;}
    useSkill();
    notifyObservers();
    notifyResources();
}

Skill* Skill::deepCopy(std::unordered_map<Skill*, Skill*>& copied) {
    if (copied.find(this) == copied.end()) {
        Skill* newSkill = copy();
        copied[this] = newSkill;
        for (Skill* ob : observers) {
            newSkill->addObserver(ob->deepCopy(copied));
        }
        return newSkill;
    } else {
        return copied[this];
    }
}
