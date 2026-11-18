// license:BSD-3-Clause
/*
 * libretro-same: MAME DIP switches as core options.
 *
 * After the machine starts, every DIP switch of the system becomes a core option in a
 * "DIP Switches" category, and MAME's own "DIP Switches" menu is hidden (mainmenu.cpp),
 * so the frontend's menu is the only place to change them. The core option value wins
 * over the value saved in MAME's cfg file.
 *
 * Usage (libretro.cpp):
 *   same_dip_create_options()        in retro_load_game(), once the machine has started
 *   same_dip_update(options_changed) in retro_run(), before running the frame
 */

#ifndef SAME_DIPSWITCHES_H
#define SAME_DIPSWITCHES_H

#ifdef SAME_CORE

#include <cctype>
#include <string>
#include <vector>

struct same_dip_option
{
	std::string key;                    // core option key
	std::string desc;                   // DIP switch name
	std::string port_tag;               // the field is found again by port tag and mask
	ioport_value mask;
	std::vector<std::string> names;     // setting names (core option values)
	std::vector<ioport_value> values;   // setting values
	std::string default_name;
};

static std::vector<same_dip_option> same_dips;
static std::vector<retro_core_option_v2_category> same_option_cats;
static std::vector<retro_core_option_v2_definition> same_option_defs;
static running_machine *same_dip_machine = nullptr;

/* "Left Difficulty Switch" -> "left_difficulty_switch" */
static std::string same_dip_key_part(const char *name)
{
	std::string key;
	for (const char *c = name; *c; c++)
	{
		if (isalnum((unsigned char)*c))
			key += (char)tolower((unsigned char)*c);
		else if (!key.empty() && key.back() != '_')
			key += '_';
	}
	while (!key.empty() && key.back() == '_')
		key.pop_back();
	return key;
}

/* Registers the regular core options plus one option per DIP switch. */
static void same_dip_create_options(void)
{
	running_machine *machine = mame_machine_manager::instance()->machine();
	if (!machine)
		return;

	same_dips.clear();
	for (auto &port : machine->ioport().ports())
	{
		for (ioport_field const &field : port.second->fields())
		{
			if (field.type() != IPT_DIPSWITCH || field.settings().empty())
				continue;

			same_dip_option dip;
			dip.desc     = field.name();
			dip.port_tag = port.second->tag();
			dip.mask     = field.mask();
			dip.key      = std::string(CORE_NAME "_dip_") + same_dip_key_part(dip.desc.c_str());

			for (const ioport_setting &setting : field.settings())
			{
				if (dip.names.size() == RETRO_NUM_CORE_OPTION_VALUES_MAX - 1)
					break;
				dip.names.push_back(setting.name());
				dip.values.push_back(setting.value());
				if (setting.value() == field.defvalue())
					dip.default_name = setting.name();
			}
			if (dip.default_name.empty())
				dip.default_name = dip.names[0];

			// Two switches with the same name (e.g. one from a slot device): add the port and mask
			for (const same_dip_option &other : same_dips)
				if (other.key == dip.key)
				{
					dip.key += "_" + same_dip_key_part(dip.port_tag.c_str()) + "_" + string_format("%x", dip.mask);
					break;
				}

			same_dips.push_back(dip);
		}
	}

	if (same_dips.empty())
		return;

	// Categories: the regular ones plus "DIP Switches"
	same_option_cats.clear();
	for (retro_core_option_v2_category *cat = option_cats_us; cat->key; cat++)
		same_option_cats.push_back(*cat);
	same_option_cats.push_back({ "dipswitches", "DIP Switches", "MAME DIP switches of the system." });
	same_option_cats.push_back({ NULL, NULL, NULL });

	// Definitions: the regular ones plus one per DIP switch
	same_option_defs.clear();
	for (retro_core_option_v2_definition *def = option_defs_us; def->key; def++)
		same_option_defs.push_back(*def);
	for (const same_dip_option &dip : same_dips)
	{
		retro_core_option_v2_definition def = {};
		def.key           = dip.key.c_str();
		def.desc          = dip.desc.c_str();
		def.category_key  = "dipswitches";
		def.default_value = dip.default_name.c_str();
		for (size_t i = 0; i < dip.names.size(); i++)
			def.values[i] = { dip.names[i].c_str(), NULL };
		same_option_defs.push_back(def);
	}
	same_option_defs.push_back({});

	retro_core_options_v2 options = { same_option_cats.data(), same_option_defs.data() };
	unsigned version = 0;
	if (environ_cb(RETRO_ENVIRONMENT_GET_CORE_OPTIONS_VERSION, &version) && version >= 2)
		environ_cb(RETRO_ENVIRONMENT_SET_CORE_OPTIONS_V2, &options);

	same_dip_machine = nullptr;   // apply them on the next same_dip_update()
}

/* Applies the DIP switch options when they change or when MAME creates a new machine
   (hard reset), which starts again from the cfg file values. */
static void same_dip_update(bool options_changed)
{
	running_machine *machine = mame_machine_manager::instance()->machine();
	if (!machine || same_dips.empty() || (!options_changed && machine == same_dip_machine))
		return;
	same_dip_machine = machine;

	for (const same_dip_option &dip : same_dips)
	{
		struct retro_variable var = { dip.key.c_str(), NULL };
		if (!environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) || !var.value)
			continue;

		auto port = machine->ioport().ports().find(dip.port_tag);
		ioport_field *field = (port != machine->ioport().ports().end()) ? port->second->field(dip.mask) : nullptr;
		if (!field)
			continue;

		for (size_t i = 0; i < dip.names.size(); i++)
			if (dip.names[i] == var.value)
			{
				ioport_field::user_settings settings;
				field->get_user_settings(settings);
				settings.value = dip.values[i];
				field->set_user_settings(settings);
				break;
			}
	}
}

#endif // SAME_CORE

#endif
