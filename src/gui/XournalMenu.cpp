#include "XournalMenu.h"

namespace {
auto getXournalMenuModelFile() -> GMenuItem* {
    auto* file_menu = g_menu_new();

    auto* file_menu_item = g_menu_item_new_submenu("_File", G_MENU_MODEL(file_menu));
    auto* new_item = g_menu_item_new("_New", "ACTION_NEW");
    g_menu_append_item(file_menu, new_item);

    auto* quit_item = g_menu_item_new("_Quit", "app.quit");
    g_menu_append_item(file_menu, quit_item);

    return file_menu_item;
}
}  // namespace


auto getXournalMenuModel() -> GMenuModel* {
    auto g_menu = g_menu_new();
    g_menu_append_item(g_menu, getXournalMenuModelFile());
    //    g_menu_freeze(g_menu);
    return G_MENU_MODEL(g_menu);
};