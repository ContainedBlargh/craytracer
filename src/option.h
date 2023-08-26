#ifndef OPTION_H
#define OPTION_H
/**
 * @file option.h
 * @author Jon Voigt Tøttrup (jvoi@itu.dk)
 *
 * @brief A module for defining and using Options made from pure macros 👌
 * @version 0.1
 * @date 2023-08-23
 *
 * @copyright WingCorp (c) 2023
 *
 */
typedef enum __attribute__((packed)) _OptionType
{
    NONE,
    SOME
} OptionType;

#define option_type(T)                                                                                                 \
    typedef struct __attribute__((packed)) _##T##Option                                                                \
    {                                                                                                                  \
        OptionType type;                                                                                               \
        T value;                                                                                                       \
    } T##Option;                                                                                                       \
    T##Option no_##T()                                                                                                 \
    {                                                                                                                  \
        return (T##Option){.type = NONE};                                                                              \
    }                                                                                                                  \
    T##Option some_##T(T value)                                                                                        \
    {                                                                                                                  \
        return (T##Option){.type = SOME, .value = value};                                                              \
    }

#define is_some(option) ((option.type) == SOME)
#define is_none(option) ((option.type) == NONE)

#endif