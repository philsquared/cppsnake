#include "keys.h"
#include "scape.h"

#include <iostream>
#include <thread>
#include <chrono>
#include <vector>
#include <random>

struct coords{ int x, y; };
inline auto operator==( coords const& l, coords const& r ) -> bool { return l.x == r.x && l.y == r.y; };

struct rect {
    int left, top, width, height, right, bottom;
    rect( int x, int y, int w, int h ) : left(x), top(y), width(w), height(h), right(x+w-1), bottom(y+h-1) {}
};

enum class direction{ up, down, left, right };

int main() {
    keys::line_buffering_off();

    using namespace scape;
    std::cout << screen::save << screen::clear;
    auto dims = screen::dimensions();

    rect stage( 1, 2, dims.width, dims.height-1 );
    auto width = static_cast<size_t>( stage.width );

    std::mt19937 rng;
    rng.seed( std::random_device()() );
    std::uniform_int_distribution<> rnd_x( stage.left+1, stage.right-1 ), rnd_y( stage.top+1, stage.bottom-1 );

    std::cout
            << bright_yellow.on_blue << move_to{ stage.left, 1 } << std::string( width, ' ' )
            << black.on_grey
            << move_to{ stage.left, stage.top } << std::string( width, '|' );
    for( int i = stage.top+1; i < stage.bottom; ++i )
        std::cout << move_to{ stage.left, i } << '_' << move_to{ stage.right, i } << '_';
    std::cout << move_to{ stage.left, stage.bottom } << std::string( width, '|' );
    std::cout << reset_colour << hide_cursor;

    coords head{ stage.width/2, stage.height/2 };

    direction dir = direction::up;

    std::vector<coords> snake( 10, {-1,-1} );
    std::vector<coords> rocks, gems;
    for( size_t i = 0; i < 20; ++i ) {
        coords rock = { rnd_x(rng), rnd_y(rng) };
        while( rock.x == head.x && rock.y <= head.y )
            rock = { rnd_x(rng), rnd_y(rng) };
        rocks.push_back( rock );
        std::cout << red.on_grey << move_to{ rock.x, rock.y } << "*";
    }
    for( size_t i = 0; i < 10; ++i ) {
        coords gem = { rnd_x(rng), rnd_y(rng) };
        gems.push_back( gem );
        std::cout << yellow.on_green << move_to{ gem.x, gem.y } << "@";
    }
    size_t head_index = 0;
    int iterations = 0;
    int score = 0;
    int frameTimeMs = 250;

    do {
        size_t back_index = head_index < snake.size()-1 ? head_index+1 : 0;

        auto const& back = snake[back_index];
        if( back.x > 0 )
            std::cout << move_to{ back.x,back.y } << " ";
        std::cout << move_to{ head.x,head.y } << "O" << flush;

        snake[head_index] = head;
        if( ++head_index == snake.size() )
            head_index = 0;

        std::this_thread::sleep_for( std::chrono::milliseconds( frameTimeMs ) );
        auto key = keys::get_key();
        if( key == keys::key::escape )
            break;
        switch( key.key_type ) {
            case keys::key::left_arrow: dir = direction::left; break;
            case keys::key::right_arrow: dir = direction::right; break;
            case keys::key::up_arrow: dir = direction::up; break;
            case keys::key::down_arrow: dir = direction::down; break;
            default: break;
        }

        switch( dir ) {
            case direction::up: --head.y; break;
            case direction::down: ++head.y; break;
            case direction::left: --head.x; break;
            case direction::right: ++head.x; break;
        }

        if( head.x == stage.left || head.x == stage.right || head.y == stage.top || head.y == stage.bottom ||
                std::find( snake.begin(), snake.end(), head ) != snake.end() ||
                std::find( rocks.begin(), rocks.end(), head ) != rocks.end() ) {
            std::cout
                << bright_yellow.on_red << " crash! "
                << bright_yellow.on_blue << move_to{ stage.left + stage.width/2-20, stage.bottom }
                << " game over, press ESCAPE twice to exit " << reset_colour << flush;
            do key = keys::get_key();
            while( key != keys::key::escape);
            break;
        }
        if( std::find( gems.begin(), gems.end(), head ) != gems.end() )
            score += 100;

        iterations++;
        score += 10;

        if( iterations % 15 == 0 )
            snake.push_back( {-1,-1 } ); // make snake longer

        if( iterations % 50 == 0 && frameTimeMs > 50 )
            frameTimeMs *= 0.9; // speed up

        std::cout << bright_yellow.on_blue << move_to{ 2, 1 } << "score: " << score << "  " << reset_colour;
    }
    while(true);

    std::cout << screen::restore << show_cursor;
    keys::line_buffering_on();
}