#ifndef CoreTable_hpp
#define CoreTable_hpp

#include "sqlpp11/sqlpp11.h"

#define object_detailed_struct(name, var, type)                                     \
    struct var {                                                                    \
        struct _alias_t {                                                           \
            static constexpr const char _literal[] = name;                          \
            using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;  \
            template <typename T>                                                   \
            struct _member_t {                                                      \
                T var;                                                              \
                T &operator()() {                                                   \
                    return var;                                                     \
                }                                                                   \
                const T &operator()() const {                                       \
                    return var;                                                     \
                }                                                                   \
            };                                                                      \
        };                                                                          \
        using _traits = sqlpp::make_traits<type>;                                   \
    }

#define object_struct(name, type) object_detailed_struct(#name, name, type)

#define database_table(name, ...)                                                   \
    struct name : sqlpp::table_t<name, ##__VA_ARGS__> {                             \
        using _value_type = sqlpp::no_value_t;                                      \
        struct _alias_t {                                                           \
            static constexpr const char _literal[] = #name;                         \
            using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;  \
            template <typename T>                                                   \
            struct _member_t {                                                      \
                T _table;                                                           \
                T &operator()() {                                                   \
                    return _table;                                                  \
                }                                                                   \
                const T &operator()() const {                                       \
                    return _table;                                                  \
                }                                                                   \
            };                                                                      \
        };                                                                          \
    }

#endif
