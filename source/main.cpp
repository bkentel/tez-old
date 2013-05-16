#include "pch.hpp"
#include "platform/window.hpp"
#include "platform/d2d_renderer.hpp"

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

bklib::rect<unsigned> tile_index_to_rect(unsigned index) {
    tiles_per_row = tile_set_width / tile_size;
    
    unsigned x = index % tiles_per_row;
    unsigned y = index / tiles_per_row;
}

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

    bklib::window win;
    bklib::d2d_renderer renderer(win.handle());
    
    win.listen<window::event_type::on_keydown>([&](
        window& w, unsigned code, unsigned repeat, unsigned scan, bool was_down
    ) {
        //auto const mapping = mapper.get_mapping(code);
        //the_world.on_command(mapping);
    });

    win.listen<window::event_type::on_size>([&](window& w, unsigned width, unsigned height) {
        renderer.resize(width, height);
    });

    win.listen<window::event_type::on_paint>([&](window& w) {
        renderer.begin_draw();
        renderer.draw_bitmap(bklib::rect<int>(0, 0, 0, 0), bklib::rect<int>(0, 0, 255, 255));
        renderer.end_draw();
        
        ::ValidateRect(w.handle(), nullptr);
    });

    for (; !win.is_closed();) {
        win.do_events();
    }
}
