#include "reverb_ui.hpp"
#include "util.hpp"

namespace {

gboolean room_size_enum_to_int(GValue* value,
                               GVariant* variant,
                               gpointer user_data) {
    auto v = g_variant_get_string(variant, nullptr);

    if (v == std::string("Small")) {
        g_value_set_int(value, 0);
    } else if (v == std::string("Medium")) {
        g_value_set_int(value, 1);
    } else if (v == std::string("Large")) {
        g_value_set_int(value, 2);
    } else if (v == std::string("Tunnel-like")) {
        g_value_set_int(value, 3);
    } else if (v == std::string("Large/smooth")) {
        g_value_set_int(value, 4);
    } else if (v == std::string("Experimental")) {
        g_value_set_int(value, 5);
    }

    return true;
}

GVariant* int_to_room_size_enum(const GValue* value,
                                const GVariantType* expected_type,
                                gpointer user_data) {
    int v = g_value_get_int(value);

    if (v == 0) {
        return g_variant_new_string("Small");
    } else if (v == 1) {
        return g_variant_new_string("Medium");
    } else if (v == 2) {
        return g_variant_new_string("Large");
    } else if (v == 3) {
        return g_variant_new_string("Tunnel-like");
    } else if (v == 4) {
        return g_variant_new_string("Large/smooth");
    } else {
        return g_variant_new_string("Experimental");
    }
}

}  // namespace

ReverbUi::ReverbUi(BaseObjectType* cobject,
                   const Glib::RefPtr<Gtk::Builder>& refBuilder,
                   const std::string& settings_name)
    : Gtk::Grid(cobject), PluginUiBase(refBuilder, settings_name) {
    name = "reverb";

    // loading glade widgets

    builder->get_widget("room_size", room_size);
    builder->get_widget("preset_room", preset_room);
    builder->get_widget("preset_empty_walls", preset_empty_walls);
    builder->get_widget("preset_ambience", preset_ambience);
    builder->get_widget("preset_large_empty_hall", preset_large_empty_hall);
    builder->get_widget("preset_disco", preset_disco);
    builder->get_widget("preset_large_occupied_hall",
                        preset_large_occupied_hall);
    builder->get_widget("preset_default", preset_default);

    get_object("input_gain", input_gain);
    get_object("output_gain", output_gain);
    get_object("predelay", predelay);
    get_object("decay_time", decay_time);
    get_object("diffusion", diffusion);
    get_object("amount", amount);
    get_object("dry", dry);
    get_object("hf_damp", hf_damp);
    get_object("bass_cut", bass_cut);
    get_object("treble_cut", treble_cut);

    // gsettings bindings

    auto flag = Gio::SettingsBindFlags::SETTINGS_BIND_DEFAULT;

    settings->bind("input-gain", input_gain, "value", flag);
    settings->bind("output-gain", output_gain, "value", flag);
    settings->bind("predelay", predelay, "value", flag);
    settings->bind("decay-time", decay_time, "value", flag);
    settings->bind("diffusion", diffusion, "value", flag);
    settings->bind("amount", amount, "value", flag);
    settings->bind("dry", dry, "value", flag);
    settings->bind("hf-damp", hf_damp, "value", flag);
    settings->bind("bass-cut", bass_cut, "value", flag);
    settings->bind("treble-cut", treble_cut, "value", flag);

    g_settings_bind_with_mapping(settings->gobj(), "room-size",
                                 room_size->gobj(), "active",
                                 G_SETTINGS_BIND_DEFAULT, room_size_enum_to_int,
                                 int_to_room_size_enum, nullptr, nullptr);

    init_presets_buttons();

    settings->set_boolean("post-messages", true);
}

ReverbUi::~ReverbUi() {
    settings->set_boolean("post-messages", false);
}

std::shared_ptr<ReverbUi> ReverbUi::create(std::string settings_name) {
    auto builder = Gtk::Builder::create_from_resource(
        "/com/github/wwmm/pulseeffects/reverb.glade");

    ReverbUi* grid = nullptr;

    builder->get_widget_derived("widgets_grid", grid, settings_name);

    return std::shared_ptr<ReverbUi>(grid);
}

void ReverbUi::init_presets_buttons() {
    preset_room->signal_clicked().connect([=]() {
        decay_time->set_value(0.445945);
        hf_damp->set_value(5508.46);
        room_size->set_active(4);
        diffusion->set_value(0.54);
        amount->set_value(util::linear_to_db(0.469761));
        dry->set_value(util::linear_to_db(1));
        predelay->set_value(25);
        bass_cut->set_value(257.65);
        treble_cut->set_value(20000);
    });

    preset_empty_walls->signal_clicked().connect([=]() {
        decay_time->set_value(0.505687);
        hf_damp->set_value(3971.64);
        room_size->set_active(4);
        diffusion->set_value(0.17);
        amount->set_value(util::linear_to_db(0.198884));
        dry->set_value(util::linear_to_db(1));
        predelay->set_value(13);
        bass_cut->set_value(240.453);
        treble_cut->set_value(3303.47);
    });

    preset_ambience->signal_clicked().connect([=]() {
        decay_time->set_value(1.10354);
        hf_damp->set_value(2182.58);
        room_size->set_active(4);
        diffusion->set_value(0.69);
        amount->set_value(util::linear_to_db(0.291183));
        dry->set_value(util::linear_to_db(1));
        predelay->set_value(6.5);
        bass_cut->set_value(514.079);
        treble_cut->set_value(4064.15);
    });

    preset_large_empty_hall->signal_clicked().connect([=]() {
        decay_time->set_value(2.00689);
        hf_damp->set_value(20000);
        amount->set_value(util::linear_to_db(0.366022));
        settings->reset("room-size");
        settings->reset("diffusion");
        settings->reset("dry");
        settings->reset("predelay");
        settings->reset("bass-cut");
        settings->reset("treble-cut");
    });

    preset_disco->signal_clicked().connect([=]() {
        decay_time->set_value(1);
        hf_damp->set_value(3396.49);
        amount->set_value(util::linear_to_db(0.269807));
        settings->reset("room-size");
        settings->reset("diffusion");
        settings->reset("dry");
        settings->reset("predelay");
        settings->reset("bass-cut");
        settings->reset("treble-cut");
    });

    preset_large_occupied_hall->signal_clicked().connect([=]() {
        decay_time->set_value(1.45397);
        hf_damp->set_value(9795.58);
        amount->set_value(util::linear_to_db(0.184284));
        settings->reset("room-size");
        settings->reset("diffusion");
        settings->reset("dry");
        settings->reset("predelay");
        settings->reset("bass-cut");
        settings->reset("treble-cut");
    });

    preset_default->signal_clicked().connect([=]() {
        settings->reset("decay-time");
        settings->reset("hf-damp");
        settings->reset("amount");
        settings->reset("room-size");
        settings->reset("diffusion");
        settings->reset("dry");
        settings->reset("predelay");
        settings->reset("bass-cut");
        settings->reset("treble-cut");
    });
}

void ReverbUi::reset() {
    settings->reset("state");
    settings->reset("input-gain");
    settings->reset("output-gain");
    settings->reset("room-size");
    settings->reset("decay-time");
    settings->reset("hf-damp");
    settings->reset("diffusion");
    settings->reset("amount");
    settings->reset("dry");
    settings->reset("predelay");
    settings->reset("bass-cut");
    settings->reset("treble-cut");
}