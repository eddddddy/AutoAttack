import abc
from copy import copy
import math


MAX_TIME = 3600000


class Skill:

    def __init__(self, name):
        self.name = name
        self.others = None
        self.resources = None

    def __notify_others(self):
        for other in self.others:
            if other.name != self.name:
                other.notify(self)

    def __notify_resources(self):
        self.resources.notify(self)

    def use(self):
        if not self.is_ready():
            raise RuntimeError("Skill not yet ready to use")

        self._use()
        self.__notify_others()
        self.__notify_resources()

    def __copy__(self):
        new_obj = self.copy()
        new_obj.name = self.name
        return new_obj

    def __eq__(self, other):
        if self.name != other.name:
            return False

        return self.key() == other.key()

    def __hash__(self):
        return hash((self.name, *self.key()))

    @abc.abstractmethod
    def notify(self, used):
        raise NotImplementedError

    @abc.abstractmethod
    def is_ready(self):
        raise NotImplementedError

    @abc.abstractmethod
    def time_until_ready(self):
        raise NotImplementedError

    @abc.abstractmethod
    def wait(self, time):
        raise NotImplementedError

    @abc.abstractmethod
    def get_damage(self):
        raise NotImplementedError

    @abc.abstractmethod
    def get_cast_time(self):
        raise NotImplementedError

    @abc.abstractmethod
    def _use(self):
        raise NotImplementedError

    @abc.abstractmethod
    def is_deterministic(self):
        raise NotImplementedError

    @abc.abstractmethod
    def copy(self):
        raise NotImplementedError

    @abc.abstractmethod
    def key(self):
        raise NotImplementedError


class Resources:

    def __copy__(self):
        return self.copy()

    def __eq__(self, other):
        if type(self) != type(other):
            return False

        return self.key() == other.key()

    def __hash__(self):
        return hash(self.key())

    @abc.abstractmethod
    def time_until_update(self):
        raise NotImplementedError

    @abc.abstractmethod
    def wait(self, time):
        raise NotImplementedError

    @abc.abstractmethod
    def notify(self, used):
        raise NotImplementedError

    @abc.abstractmethod
    def copy(self):
        raise NotImplementedError

    @abc.abstractmethod
    def key(self):
        raise NotImplementedError


class State:

    def __init__(self, skills, resources):
        self.skills = skills
        self.resources = resources

        for skill in self.skills:
            skill.others = self.skills
            skill.resources = self.resources

        self.skill_name_map = {skill.name: skill for skill in self.skills}

    def print_lines(self):
        lines = ["State: "]
        for skill in self.skills:
            lines[-1] += repr(skill)
            lines.append("       ")

        lines[-1] += repr(self.resources)
        return lines

    def __copy__(self):
        new_resources = copy(self.resources)
        new_skills = [copy(skill) for skill in self.skills]

        return State(new_skills, new_resources)

    def __eq__(self, other):
        zipped_skills = zip(self.skills, other.skills)
        return all(skill == other_skill for skill, other_skill in zipped_skills) \
               and self.resources == other.resources

    def __hash__(self):
        h = 0
        for skill in self.skills:
            h = h ^ hash(skill)

        return h ^ hash(self.resources)

    def get_available_skills(self):
        available = []
        for skill in self.skills:
            if skill.is_ready():
                available.append(skill)
        return available

    def use(self, skill=None, time=None):
        if skill is not None:
            self.skill_name_map[skill.name].use()
            for other in self.skills:
                if other.name != skill.name:
                    other.wait(time)
        else:
            for skill in self.skills:
                skill.wait(time)
        self.resources.wait(time)

    def is_deterministic(self):
        return all(skill.is_deterministic() for skill in self.skills)

    def get_wait_time(self):
        time = MAX_TIME
        for skill in self.skills:
            if not skill.is_ready() and skill.time_until_ready() < time:
                time = skill.time_until_ready()

        if 0 < self.resources.time_until_update() < time:
            time = self.resources.time_until_update()

        return time


class Node:

    def __init__(self, state):
        self.state = state
        self.parent = None
        self.children = []
        self.nb = 0

        self.__init_children()

    def print_state(self, cpuct=1, depth=0):
        lines = self.state.print_lines()

        if self.parent is not None:
            if self.parent.skill is not None:
                line = self.parent.skill.name
            else:
                line = "Wait"
            line = f"{line} (value={self.parent.get_value(cpuct)})"
            lines.insert(0, line)

        prefix = "            " * depth
        print("\n".join([prefix + line for line in lines]))

        for edge in self.children:
            for child in edge.children:
                child.print_state(cpuct=cpuct, depth=depth + 1)

    def __init_children(self):
        available = self.state.get_available_skills()
        for skill in available:
            self.children.append(Edge(self, skill=skill))

        self.children.append(Edge(self, time=self.state.get_wait_time()))

    def playout(self, cpuct):
        # selection
        curr_node = self
        while True:
            edge = max(curr_node.children, key=lambda e: e.get_value(cpuct))

            if curr_node.state.is_deterministic():
                if not edge.children:
                    new_state = copy(curr_node.state)
                    new_state.use(skill=edge.skill, time=edge.time)
                    break
                else:
                    curr_node = list(edge.children.values())[0]
            else:
                new_state = copy(curr_node.state)
                new_state.use(skill=edge.skill, time=edge.time)

                if new_state in edge.children:
                    curr_node = edge.children[new_state]
                else:
                    break

        # expansion
        new_node = Node(new_state)
        new_node.parent = edge
        edge.children[new_state] = new_node

        # backprop
        curr_edge = edge
        cum_damage, cum_time = 0, 0
        while curr_edge:
            cum_damage += curr_edge.get_skill_damage()
            cum_time += curr_edge.time
            dps = cum_damage / cum_time
            curr_edge.add_value(dps)
            curr_edge.parent.nb += 1
            curr_edge = curr_edge.parent.parent

    def current_best(self):
        rotation = []
        curr_node = self
        while True:
            edge = max(curr_node.children, key=lambda e: e.n)
            if edge.skill is not None:
                rotation.append(edge.skill.name)

            curr_node = max(edge.children.values(), key=lambda n: n.nb, default=None)
            if curr_node is None:
                break

        first_edge = max(self.children, key=lambda n: n.n)
        return rotation, (first_edge.q, first_edge.q_wma, first_edge.q_exp)


class Edge:

    def __init__(self, parent, skill=None, time=None):
        self.n = 0
        self.w = 0
        self.q = 0
        self.parent = parent

        # exponential moving average instead of simple average
        self.alpha = 0.9997
        self.q_exp = 0

        # weighted moving average
        self.top = 0
        self.q_wma = 0

        # stores state -> node
        self.children = {}

        self.total_skill_damage = 0
        self.num_damage_calls = 0

        if skill is not None:
            self.p = skill.get_damage() / skill.get_cast_time()
            self.skill = skill
            self.time = skill.get_cast_time()
        else:
            self.p = 0.001
            self.skill = None
            self.time = time

    def add_value(self, value):
        self.n += 1
        self.w += value
        self.q = self.w / self.n

        self.q_exp = self.q_exp * self.alpha + (1 - self.alpha) * value

        self.top = self.top + self.n * value
        self.q_wma = self.top * 2 / self.n / (self.n + 1)

        if self.skill is not None:
            self.p = self.get_average_skill_damage() / self.skill.get_cast_time()

    def get_value(self, cpuct):
        return self.q + cpuct * self.p * math.sqrt(self.parent.nb) / (1 + self.n)

    def get_skill_damage(self):
        if self.skill is not None:
            damage = self.skill.get_damage()
            self.total_skill_damage += damage
            self.num_damage_calls += 1
            return damage
        return 0

    def get_average_skill_damage(self):
        if self.num_damage_calls > 0:
            return self.total_skill_damage / self.num_damage_calls
        return 0
