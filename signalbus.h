//
// Created by Nico Russo on 4/28/26.
//

#ifndef LATTICE_SIGNALBUS_H
#define LATTICE_SIGNALBUS_H
#include <functional>

namespace Lattice {

    class SignalBus {
    public:
        template<typename Event>
        using Handler = std::function<void(const Event&)>;

        template<typename Event>
        int connect(Handler<Event> fn) {
            auto& vec = _handlers[typeid(Event)];
            int id = _nextID++;
            vec.emplace_back(id, [fn](const void* e) {
                fn(*static_cast<const Event*>(e));
            });
            return id;
        }

        template<typename Event>
        void emit(const Event& e) {
            const auto it = _handlers.find(typeid(Event));
            if (it == _handlers.end()) return;

            for (auto& [id, fn] : it->second)
                fn(&e);
        }

        template<typename Event>
        void disconnect(int id) {
            auto& vec = _handlers[typeid(Event)];
            vec.erase(std::remove_if(vec.begin(), vec.end(),
                [&](auto& p) { return p.first == id; }),
                vec.end());
        }

    private:
        using ErasedHandler = std::function<void(const void*)>;

        std::unordered_map<
            std::type_index,
            std::vector<std::pair<int, ErasedHandler>>
        > _handlers;

        int _nextID = 0;
    };

}

#endif //LATTICE_SIGNALBUS_H
