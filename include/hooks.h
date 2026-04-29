//
// Created by Nico Russo on 4/28/26.
//

#ifndef LATTICE_HOOKS_H
#define LATTICE_HOOKS_H

namespace Lattice {
    struct Hooks {
        static inline void (*Start)() = nullptr;
        static inline void (*Update)(float) = nullptr;
        static inline void (*FixedUpdate)(float) = nullptr;
        static inline void (*LateUpdate)(float) = nullptr;
        static inline void (*Shutdown)() = nullptr;
    };
}

#define LATTICE_HOOK(name, ...)                                      \
    void name(__VA_ARGS__);                                          \
    namespace {                                                      \
        struct AutoRegister_##name {                                 \
            AutoRegister_##name() {                                  \
                Lattice::Hooks::name = &name;                        \
            }                                                        \
        } autoRegister_##name;                                       \
    }                                                                \
    void name(__VA_ARGS__)

#endif //LATTICE_HOOKS_H
