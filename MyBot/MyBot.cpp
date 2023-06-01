#include "data.hpp"

// Documentation : https://dpp.dev/index.html

int main()
{
	// Ids, Bot Token, WoWAddon string.
	const dpp::snowflake MESSAGE_ID { M_MESSAGE_ID };
	const dpp::snowflake CHANNEL_ID { M_CHANNEL_ID };
	const std::string BOT_TOKEN { M_BOT_TOKEN };
	const std::string ADDON_DUMP { M_ADDON_DUMP };

	uint64_t intents = dpp::i_default_intents | dpp::i_message_content;
	dpp::cluster bot(BOT_TOKEN, intents);
	bot.on_log(dpp::utility::cout_logger());

	bot.on_ready([&bot, CHANNEL_ID, MESSAGE_ID, ADDON_DUMP](const dpp::ready_t& event) {
		bot.message_get(MESSAGE_ID, CHANNEL_ID, [&bot, CHANNEL_ID, MESSAGE_ID, ADDON_DUMP](auto callback) {

			// Collect the data from the WoWAddon and return it into something usable.
			const auto [bankData, guildBankMap, gold, lastScanned] = bankCleanse(ADDON_DUMP);

			// Sets to differentiate the items by.
			const std::set<std::string> ENCHANTING_SET = {
				// Enchanting Mats.
				"Abyss Crystal",
				"Dream Shard",
				"Greater Cosmic Essence",
				"Infinite Dust"
			};
			const std::set<std::string> FISH_FEAST_SET = {
				// Enchanting Mats.
				"Fish Feast",
				"Nettlefish",
				"Musselback Sculpin",
				"Glacial Salmon",
				"Northern Spices"
			};
			const std::set<std::string> GEMS_SET = {
				// Enchanting Mats.
				"Autumn's Glow",
				"Monarch Topaz",
				"Forest Emerald",
				"Scarlet Ruby",
				"Sky Sapphire",
				"Twilight Opal"
			};

			if (!callback.is_error()) {
				// Get the message I want to edit.
				dpp::message editedMessage = std::get<dpp::message>(callback.value);
				// Clear its previous content.
				editedMessage.embeds.clear();
				// Add a message at the start. How much gold the bank has currently.
				editedMessage.set_content(fmt::format(":coin: Current Gold Amount: {} :coin:", gold));

				// Create 3 Embeds. One for each collection of items I want to display.
				dpp::embed embed = dpp::embed().set_color(dpp::colors::dark_blue).set_thumbnail("https://wow.zamimg.com/images/wow/icons/large/inv_enchant_abysscrystal.jpg");
				embed.set_title("-----------------Enchanting Mats-----------------");

				dpp::embed embed2 = dpp::embed().set_color(dpp::colors::dark_green).set_thumbnail("https://wow.zamimg.com/images/wow/icons/large/inv_misc_fish_52.jpg");
				embed2.set_title("------------------Fish Feast Mats-----------------");

				dpp::embed embed3 = dpp::embed().set_color(dpp::colors::dark_red).set_thumbnail("https://wow.zamimg.com/images/wow/icons/large/inv_jewelcrafting_gem_04.jpg");
				embed3.set_title("--------------------Gem Mats---------------------").set_footer(dpp::embed_footer().set_text(lastScanned)); // Also adds footer containing Last Scanned.

				// Iterates through json file which contains the items I want to display. Sort them into the three groups of items and add fields for each one.
				for (auto it = bankData.begin(); it != bankData.end(); ++it) {
					if (ENCHANTING_SET.contains(it.key()) == true) {
						std::string formattedValue = fmt::format("```ansi\n\u001b[0;32m{}\n```", static_cast<std::string>(it.value())); // Formatted to add colour.
						embed.add_field(it.key(), formattedValue, true);
					}
					else if (FISH_FEAST_SET.contains(it.key()) == true) {
						std::string formattedValue = fmt::format("```ansi\n\u001b[0;32m{}\n```", static_cast<std::string>(it.value()));
						embed2.add_field(it.key(), formattedValue, true);
					}
					else if (GEMS_SET.contains(it.key()) == true) {
						std::string formattedValue = fmt::format("```ansi\n\u001b[0;32m{}\n```", static_cast<std::string>(it.value()));
						embed3.add_field(it.key(), formattedValue, true);
					}
				}

				editedMessage.add_embed(embed).add_embed(embed2).add_embed(embed3);
				bot.message_edit(editedMessage);

			}
			else {
				dpp::embed embedNew { dpp::embed().set_color(dpp::colors::pink).set_thumbnail("https://wow.zamimg.com/images/wow/icons/large/spell_misc_emotionsad.jpg").set_title("No Message To Edit") };
				bot.message_create(dpp::message(CHANNEL_ID, embedNew));
			}
		});

	});

	bot.start(dpp::st_wait);

	return 0;
}
