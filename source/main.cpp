#include "pch.hpp"
#include "platform/window.hpp"
#include "platform/d2d_renderer.hpp"
#include "tez/map_layout.hpp"
#include "tez/room_generator.hpp"

//#include "targa.hpp"
//#include "quad_tree.hpp"

//#include "keyboard.hpp"
//
//#include "entity.hpp"
//#include "level.hpp"
//
//#include "tile_map.hpp"

//////////////////////////////////////////////////////////////////////////////////
//class player : public entity {
//public:
//    player() : entity(0, 0) {}
//private:
//    std::string name_;
//};
//
//////////////////////////////////////////////////////////////////////////////////
//class world {
//public:
//    world() {
//        player_.attach(level_);
//    }
//
//    void on_command(command_type command) {
//        switch (command) {
//        case command_type::direction_n :
//            player_.move_to(direction::north);
//            break;
//        case command_type::direction_s :
//            player_.move_to(direction::south);
//            break;
//        case command_type::direction_e :
//            player_.move_to(direction::east);
//            break;
//        case command_type::direction_w :
//            player_.move_to(direction::west);
//            break;
//        }
//    }
//private:
//    level  level_;
//    player player_;
//};
//
//
//////////////////////////////////////////////////////////////////////////////////


int main() {
    using bklib::window;

    //key_mapper mapper;

    //mapper.set_mapping(key_combo(VK_UP),      command_type::direction_n);
    //mapper.set_mapping(key_combo(VK_DOWN),    command_type::direction_s);
    //mapper.set_mapping(key_combo(VK_RIGHT),   command_type::direction_e);
    //mapper.set_mapping(key_combo(VK_LEFT),    command_type::direction_w);

    //mapper.set_mapping(key_combo(VK_NUMPAD7), command_type::direction_nw);
    //mapper.set_mapping(key_combo(VK_NUMPAD8), command_type::direction_n);
    //mapper.set_mapping(key_combo(VK_NUMPAD9), command_type::direction_ne);
    //mapper.set_mapping(key_combo(VK_NUMPAD4), command_type::direction_w);
    //mapper.set_mapping(key_combo(VK_NUMPAD5), command_type::direction_here);
    //mapper.set_mapping(key_combo(VK_NUMPAD6), command_type::direction_e);
    //mapper.set_mapping(key_combo(VK_NUMPAD1), command_type::direction_sw);
    //mapper.set_mapping(key_combo(VK_NUMPAD2), command_type::direction_s);
    //mapper.set_mapping(key_combo(VK_NUMPAD3), command_type::direction_se);

    //world the_world;

    std::default_random_engine engine(::GetTickCount());
    auto random = bklib::make_random_wrapper(engine);

    auto gen_simple   = tez::simple_room_generator(random);
    auto gen_compound = tez::compound_room_generator(random);

    auto const make_map = [&] {
        tez::map_layout layout(random);

        for (int i = 0; i < 20; ++i) {
            if (i % 4 == 0) {
                layout.add_room(gen_compound.generate());
            } else {
                layout.add_room(gen_simple.generate());
            }
        }
    
        layout.normalize();
        return layout.make_map();
    };

    auto test_map = make_map();

    bklib::window win;
    bklib::d2d_renderer renderer(win.handle());
    
    win.listen<window::event_type::on_keydown>([&](
        window& w, unsigned code, unsigned repeat, unsigned scan, bool was_down
    ) {
        static int delta = 3;

        switch (code) {
        case VK_SPACE :
            test_map = make_map();
            break;
        case VK_ADD :
            renderer.zoom_in();
            break;
        case VK_SUBTRACT :
            renderer.zoom_out();
            break;
        case VK_UP :
            renderer.translate(0, delta);
            break;
        case VK_DOWN :
            renderer.translate(0, -delta);
            break;
        case VK_LEFT :
            renderer.translate(delta, 0);
            break;
        case VK_RIGHT :
            renderer.translate(-delta, 0);
            break;
        }

        InvalidateRect(w.handle(), nullptr, FALSE);
        //auto const mapping = mapper.get_mapping(code);
        //the_world.on_command(mapping);
    });

    win.listen<window::event_type::on_size>([&](window& w, unsigned width, unsigned height) {
        renderer.resize(width, height);
    });

    win.listen<window::event_type::on_paint>([&](window& w) {
        renderer.begin_draw();
        
        for (unsigned y = 0; y < test_map.height(); ++y) {
            for (unsigned x = 0; x < test_map.width(); ++x) {
                unsigned index = 0;
                
                switch (test_map.at(x, y).type) {
                case tez::tile_category::empty :    index = 0; break;
                case tez::tile_category::wall :     index = 2; break;
                case tez::tile_category::ceiling :  index = 24; break;
                case tez::tile_category::floor :    index = 1; break;
                case tez::tile_category::door :     index = 32; break;
                case tez::tile_category::corridor : index = 20; break;
                }
                
                renderer.draw_bitmap(index, bklib::make_point(x, y));
            }
        }
        
        
        renderer.end_draw();
        
        ::ValidateRect(w.handle(), nullptr);
    });

    for (; !win.is_closed();) {
        win.do_events();
    }
}
