# C Raytracer

Another raytracer, but in C this time.
Actually, this is not the main project, the interesting part of this code is the python C-bundling system that enables me to write bootleg template macros, like this option macro:


```c
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
```

This doesn't work in a multi-file `gcc` C project since you will get a 'multiple definitions' error for some reason. But it works in single-file projects.
So, I wrote a script in Python that converts any C project into a single file, like a Javascript bundler.

This saves ~4 bytes per file, but really it just makes compiling the final program a lot easier. I know that `Cmake` can be maintained more or less automatically, but it's still nicer to just call a single command to build stuff.

Well, two in this case, but I'm thinking about migrating the bundling script into a C program as well...

![Some nice orbs](https://github.com/ContainedBlargh/craytracer/assets/18627721/0ede5003-fa24-4480-b2ec-af09659d51c7)
