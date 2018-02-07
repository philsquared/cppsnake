#include "keys.h"

#include <termios.h>
#include <sys/ioctl.h>

#include <iostream>

namespace keys {
    const int STDIN_FILENO = 0;

    void line_buffering_off() {
        struct termios t;
        tcgetattr( STDIN_FILENO, &t );
        t.c_lflag &= ~ICANON;
        t.c_lflag &= ~ECHO;
        tcsetattr( STDIN_FILENO, TCSANOW, &t );
    }
    void line_buffering_on() {
        struct termios t;
        tcgetattr( STDIN_FILENO, &t );
        t.c_lflag |= ICANON;
        t.c_lflag |= ECHO;
        tcsetattr( STDIN_FILENO, TCSANOW, &t );
    }

    std::ostream& operator <<( std::ostream& os, key key ) {
        switch( key.key_type ) {
            case key::none: return os << "{no key}";
            case key::unknown: return os << "{unknown key}";
            case key::left_arrow: return os << "{left arrow}";
            case key::right_arrow: return os << "{right arrow}";
            case key::up_arrow: return os << "{up arrow}";
            case key::down_arrow: return os << "{down arrow}";
            case key::escape: return os << "{escape}";
            case key::printable: return os << key.character;
            default: return os << "{unrecognised key type}";
        }
    }

    auto keys_available() -> bool {
        int bytesWaiting;
        ioctl( STDIN_FILENO, FIONREAD, &bytesWaiting );
        return bytesWaiting > 0;
    }
    auto get_key() -> key {
        if( !keys_available() )
            return { key::none };
        key::type code;
        std::cin >> reinterpret_cast<char&>( code );
        if( code == key::escape ) {
            std::cin >> reinterpret_cast<char&>( code );
            if( code == key::open_square_bracket ) {
                std::cin >> reinterpret_cast<char&>( code );
                return { code };
            }
            else if( code == key::escape ){
                return { key::escape };
            }
            return { key::unknown };
        }
        else {
            return { static_cast<char>( code ) };
        }
    }

} // namespace keys