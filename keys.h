#ifndef SNAKE_KEYS_H_INCLUDED
#define SNAKE_KEYS_H_INCLUDED

#include <iosfwd>

namespace keys {
    void line_buffering_off();
    void line_buffering_on();

    struct key {
        enum type : char {
            none,
            unknown,
            printable,

            escape = 27,
            up_arrow = 65,
            down_arrow = 66,
            right_arrow = 67,
            left_arrow = 68,

            open_square_bracket = 91
        };

        type key_type;
        char character;

        key( type key_type ) : key_type( key_type ), character( 0 ) {}
        key( char character ) : key_type( printable ), character( character ) {}

        explicit operator bool() const { return key_type != none; }
        auto operator == ( char c ) const -> bool {
            return character == c;
        }
        auto operator == ( type t ) const -> bool {
            return key_type == t;
        }
        auto operator != ( char c ) const -> bool {
            return character != c;
        }
        auto operator != ( type t ) const -> bool {
            return key_type != t;
        }

        friend std::ostream& operator <<( std::ostream& os, key key );
    };

    auto keys_available() -> bool;
    auto get_key() -> key;

} // namespace keys

#endif //SNAKE_KEYS_H_INCLUDED
