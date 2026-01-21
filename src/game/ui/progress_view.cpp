#include "game/ui/progress_view.hpp"
#include "game/values/hex_values.hpp"

namespace gameui {
/// Progress table text settings.
static const ui::TextSettings settings = {assets::font, 20, sf::Color::White,
                                          sf::Color::Black, 2.f};

/// Constructs a progress view element.
Progress::Progress() : dev::Section(gameui::settings) {
  // center panel on the screen
  bounds = {0.5as, 0.5as, 600px, 0ps};
  color.a *= 2;

  // add padding
  padding.setVertical(20);
  clear();

  // deactivate by default
  scissor = true;
  deactivate();
};

/// Constructs player info table.
void Progress::reconstruct(const std::vector<Region::Team> &teams) {
  // store team list
  this->teams = teams;

  // construct argument lists
  args = std::vector<ui::Text::List>(teams.size(), ui::Text::List{});

  // construct table
  for (size_t i = 0; i < teams.size(); i++) {
    // construct row labels
    ui::Text *arr[] = {line("gp.prog.pos"), extra("gp.prog.team", 0.25ps),
                       extra("gp.prog.name", 0.5ps),
                       extra("gp.prog.per", 0.75ps)};

    // configure labels
    for (ui::Text *label : arr) {
      label->size().x = 0.25ps;
      label->align = ui::Text::N;
      label->use(&args[i]);
    };

    // configure place label
    args[i]["pos"] = ext::str_int(i + 1);
    switch (i + 1) {
    case 1:
      arr[0]->setColor(sf::Color(0xef, 0xbf, 0x04));
      break; // gold
    case 2:
      arr[0]->setColor(sf::Color(0xc4, 0xc4, 0xc4));
      break; // silver
    case 3:
      arr[0]->setColor(sf::Color(0xce, 0x89, 0x46));
      break; // bronze
    default:
      break;
    };

    // configure default label values
    args[i]["team"] = "-";
    args[i]["name"] = "-";
    args[i]["percent"] = ext::str_percent(0, 1);
  };
};

/// Updates table data.
void Progress::update(const logic::TileCount &count,
                      const std::vector<Messages::Player> &players) {
  /// Label order.
  enum Type { Pos, Team, Name, Per, Count };

  // update each row
  size_t idx = 0;
  for (size_t i = 0; i < count.teams.size(); i++) {
    const auto &info = count.teams[i];

    // ignore if not displaying
    if (std::find(teams.begin(), teams.end(), info.team) == teams.cend())
      continue;

    // update team label
    lines[idx * Count + Team]->setColor(Values::hex_colors[info.team]);
    lines[idx * Count + Name]->setColor(Values::hex_colors[info.team]);
    args[idx]["team"] = Values::hex_names[info.team];

    // update player name
    for (const auto &player : players) {
      if (player.team == info.team) {
        args[idx]["name"] = player.name;
        break;
      };
    };

    // update percentage
    args[idx]["percent"] = ext::str_percent(info.tiles, count.total);
    idx++;
  };
};
}; // namespace gameui
