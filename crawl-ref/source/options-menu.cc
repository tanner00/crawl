#include "AppHdr.h"

#include "libutil.h"
#include "message.h"
#include "options-menu.h"
#include "options.h"
#include "ui.h"
#include "options.h"
#include "game-options.h"

using namespace ui;

vector<string> booleanOptions = {"newgame_after_quit", "centre_on_scroll", "symmetric_scroll", "explore_greedy", "mouse_input", "sounds_on", "ability_menu"};

// @TODO: Make work with things that are not bools
formatted_string formatted_option(size_t option_index, bool value, bool highlighted) {
    string color = highlighted ? "yellow" : "white";
    string value_string = value ? "true" : "false";
    // @TODO: make the true / false represent current game state
    return formatted_string::parse_string("<" + color + ">" + booleanOptions[option_index] + "</" + color + ">: " + value_string);
}

class OptionsMenu : public Widget
{
private:
    shared_ptr<Box> m_root;
    vector<shared_ptr<Text>> option_texts;
    bool has_allocated = false;
    size_t selected_option = 0;

    void on_show()
    {
        on_hotkey_event([this](const KeyEvent& ev)
        {
            const auto keyn = ev.key();

            if (key_is_escape(keyn) || keyn == CK_MOUSE_CMD)
            {
                done = true;
                return done;
            } else if (keyn == CK_ENTER) {
                auto bool_option = dynamic_cast<BoolGameOption*>(Options.options_by_name[booleanOptions[selected_option]]);
                bool_option->toggle();

                option_texts[selected_option]->set_text(formatted_option(selected_option, bool_option->get(), true));
            } else if (keyn == CK_DOWN) {
                // booleanOptions.size():
                // will need to be changed when we have more options / different menus for options
                if (selected_option < booleanOptions.size() - 1) {
                    ++selected_option;
                }

                auto last_bool_option = dynamic_cast<BoolGameOption*>(Options.options_by_name[booleanOptions[selected_option - 1]]);
                auto new_bool_option = dynamic_cast<BoolGameOption*>(Options.options_by_name[booleanOptions[selected_option]]);

                option_texts[selected_option - 1]->set_text(formatted_option(selected_option - 1, last_bool_option->get(), false));
                option_texts[selected_option]->set_text(formatted_option(selected_option, new_bool_option->get(), true));
            } else if (keyn == CK_UP) {
                if (selected_option > 0) {
                    --selected_option;
                }

                auto last_bool_option = dynamic_cast<BoolGameOption*>(Options.options_by_name[booleanOptions[selected_option + 1]]);
                auto new_bool_option = dynamic_cast<BoolGameOption*>(Options.options_by_name[booleanOptions[selected_option]]);

                option_texts[selected_option + 1]->set_text(formatted_option(selected_option + 1, last_bool_option->get(), false));
                option_texts[selected_option]->set_text(formatted_option(selected_option, new_bool_option->get(), true));
            }

            return false;
        });
    }

    void _allocate_region()
    {
        m_root->allocate_region(m_region);

        if (!has_allocated)
        {
            has_allocated = true;
            on_show();
        }
    }

    void _render()
    {
        m_root->render();
    }
public:
    bool done = false;

    OptionsMenu()
    {
        m_root = make_shared<Box>(Box::VERT);
        add_internal_child(m_root);
        m_root->set_cross_alignment(Widget::Align::STRETCH);

        // @TODO: Make work with all options
        for (size_t i = 0; i < booleanOptions.size(); ++i) {
            auto bool_option = dynamic_cast<BoolGameOption*>(Options.options_by_name[booleanOptions[i]]);
            formatted_string formatted = formatted_option(i, bool_option->get(), i == 0);

            auto option_text = make_shared<Text>(formatted);
            option_text->set_margin_for_crt(0, 0, 1, 0);
            option_text->set_margin_for_sdl(0, 0, 10, 0);

            option_texts.push_back(option_text);
            m_root->add_child(option_text);
        }
    }
};

void display_options_menu() {
    auto options_menu_ui = make_shared<OptionsMenu>();
    auto popup = make_shared<ui::Popup>(options_menu_ui);

    ui::run_layout(move(popup), options_menu_ui->done);
}