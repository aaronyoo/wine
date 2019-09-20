#ifndef LOGGER_HPP
#define LOGGER_HPP

namespace logger {
    void init();
    void msg_info(const char* format, ...);
    void panic(const char* foramt, ...);
}

#endif // LOGGER_HPP