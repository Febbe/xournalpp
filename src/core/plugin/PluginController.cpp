#include "PluginController.h"
#ifdef ENABLE_PLUGINS

#include <algorithm>
#include <filesystem>

#include "control/Control.h"
#include "gui/GladeSearchpath.h"
#include "gui/dialog/PluginDialog.h"
#include "util/StringUtils.h"

#include "Plugin.h"
#include "config-features.h"


namespace {
auto load_awailable_plugins_from(fs::path const& path, Control* control) -> std::vector<std::unique_ptr<Plugin>> {
    if (!fs::is_directory(path)) {
        g_info("Skipping Plugin path, it is not a directory: %s", path.string().c_str());
        return {};
    }
    g_info("Loading plugins from: %s", path.string().c_str());

    std::vector<std::unique_ptr<Plugin>> returner;
    try {
        for (auto const& f: fs::directory_iterator(path)) {
            const auto& pluginPath = f.path();
            try {
                auto plugin = std::make_unique<Plugin>(control, pluginPath.filename().string(), pluginPath);
                if (!plugin || !plugin->isValid()) {
                    g_warning("Error loading plugin \"%s\"", f.path().string().c_str());
                    continue;
                }
                plugin->setEnabled(plugin->isDefaultEnabled());
                returner.emplace_back(std::move(plugin));
            } catch (std::exception const& e) {
                g_warning("Error loading plugin \"%s\": %s", f.path().string().c_str(), e.what());
            }
        }
    } catch (fs::filesystem_error const& e) {
        g_warning("Could not open plugin dir: \"%s\": %s", path.string().c_str(), e.what());
    }
    return returner;
}

auto emplace_sorted_if_not_exists(std::vector<std::unique_ptr<Plugin>>& plugins, std::unique_ptr<Plugin> plugin)
        -> void {
    auto const& path = plugin->getPath();
    auto endit = std::end(plugins);
    auto it = std::find_if(std::begin(plugins), endit,
                           [&path](auto const& p) { return fs::canonical(p->getPath()) == fs::canonical(path); });
    if (it != endit) {
        return;
    }
    plugins.emplace_back(std::move(plugin));
    std::sort(begin(plugins), end(plugins), [](auto const& lhs, auto const& rhs) {
        auto tieer = [](auto&& p) { return std::tie(p->getName(), p->getPath()); };
        return tieer(lhs) < tieer(rhs);
    });
}
}  // namespace
#endif


PluginController::PluginController(Control* control): control(control) {
#ifdef ENABLE_PLUGINS
    // Todo(fabian) move those search paths into PathUtils
    auto searchPath = control->getGladeSearchPath()->getFirstSearchPath();
    auto searchPaths = {fs::weakly_canonical(searchPath /= "../plugins"),  //
                        fs::weakly_canonical(searchPath /= "/plugins"),    //
                        Util::getConfigSubfolder("plugins")};

    for (auto&& path: searchPaths) {
        for (auto&& plugin: load_awailable_plugins_from(path, control)) {
            emplace_sorted_if_not_exists(plugins, std::move(plugin));
        }
    }

    Settings* settings = control->getSettings();
    // enable plugins
    std::vector<std::string> pluginEnabled = StringUtils::split(settings->getPluginEnabled(), ',');
    for (auto&& plugin_path: pluginEnabled) {
        if (auto iter = std::find_if(begin(plugins), end(plugins),
                                     [&plugin_path](auto&& plugin) { return plugin->getPath() == plugin_path; });
            iter != end(plugins)) {
            (*iter)->setEnabled(true);
        }
    }
    // disable plugins
    std::vector<std::string> pluginDisabled = StringUtils::split(settings->getPluginDisabled(), ',');
    for (auto&& plugin_path: pluginDisabled) {
        if (auto iter = std::find_if(begin(plugins), end(plugins),
                                     [&plugin_path](auto&& plugin) { return plugin->getPath() == plugin_path; });
            iter != end(plugins)) {
            (*iter)->setEnabled(false);
        }
    }

    for (auto&& plugin: plugins) {
        if (!plugin->isEnabled()) {
            continue;
        }
        plugin->loadScript();
    }
#endif
}

void PluginController::registerToolbar() {
#ifdef ENABLE_PLUGINS
    for (auto&& p: this->plugins) { p->registerToolbar(); }
#endif
}

void PluginController::showPluginManager() const {
    PluginDialog dlg(control->getGladeSearchPath(), control->getSettings());
    dlg.loadPluginList(this);
    dlg.show(control->getGtkWindow());
}

void PluginController::registerMenu() {
#ifdef ENABLE_PLUGINS
    GtkWidget* menuPlugin = control->getWindow()->get("menuPlugin");
    for (auto&& p: this->plugins) { p->registerMenu(control->getGtkWindow(), menuPlugin); }
    gtk_widget_show_all(menuPlugin);

#else
    // If plugins are disabled - disable menu also
    GtkWidget* menuitemPlugin = control->getWindow()->get("menuitemPlugin");
    gtk_widget_hide(menuitemPlugin);
#endif
}

auto PluginController::getPlugins() const -> std::vector<Plugin*> {
    std::vector<Plugin*> pl;
    pl.reserve(plugins.size());
    std::transform(begin(plugins), end(plugins), std::back_inserter(pl), [](auto&& plugin) { return plugin.get(); });
    return pl;
}
