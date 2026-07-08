#include "MenuHandler.h"
#include "WidgetActiveEffects.h"

using namespace RE;
auto MenuHandler::get_singleton() noexcept -> MenuHandler* {
  static MenuHandler instance;
  return std::addressof(instance);
}

auto MenuHandler::register_() -> void {
  if (const auto ui = UI::GetSingleton()) {
    ui->AddEventSink(get_singleton());
  }
}

// Set true on every loading screen. The player Update hook re-applies show()+CheckInI
// once the widget movie has finished its async LoadMovieEx, then clears it. Starts true
// so the first entry into the world configures the widget (coc / new game race).
static bool g_hudNeedsConfig = true;

void RequestHudReconfig()
{
  g_hudNeedsConfig = true;
  logger::info("RequestHudReconfig");
}

void ConfigureHudIfNeeded()
{
  if (!g_hudNeedsConfig) {
    return;
  }
  const auto ui = RE::UI::GetSingleton();
  if (!ui) {
    return;
  }
  const auto widget = ui->GetMenu(WidgetActiveEffects::MENU_NAME);
  if (!widget || !widget->uiMovie) {
    static int waitLog = 0;
    if ((waitLog++ % 120) == 0) {
      logger::info("ConfigureHudIfNeeded waiting: menu={} movie={}", static_cast<bool>(widget),
        widget ? static_cast<bool>(widget->uiMovie) : false);
    }
    return;
  }
  // coc / new game never fire kPostLoadGame, so the always-open overlay is never
  // (re)shown after the world loads. Re-send kShow once here, mirroring kPostLoadGame.
  // There is no kHide anywhere in the flow, so this cannot cause the old churn bug.
  WidgetActiveEffects::show();
  CheckInI();
  WidgetActiveEffects::toggle_visibility(true);
  g_hudNeedsConfig = false;
  logger::info("ConfigureHudIfNeeded: movie ready -> show + CheckInI done");
}

// Is any HUD-blocking menu (other than a_except) still open? DialogueMenu counts:
// while the player is in dialogue the widget stays hidden.
static bool AnyBlockingMenuOpen(RE::UI* ui, const RE::BSFixedString& a_except)
{
  const auto stillOpen = [&](std::string_view a_menu) {
    return !(a_except == a_menu) && ui->IsMenuOpen(a_menu);
  };
  return stillOpen(InventoryMenu::MENU_NAME) || stillOpen(CraftingMenu::MENU_NAME) ||
         stillOpen(BarterMenu::MENU_NAME) || stillOpen(TweenMenu::MENU_NAME) ||
         stillOpen(GiftMenu::MENU_NAME) || stillOpen(ContainerMenu::MENU_NAME) ||
         stillOpen(MagicMenu::MENU_NAME) || stillOpen(StatsMenu::MENU_NAME) ||
         stillOpen(JournalMenu::MENU_NAME) || stillOpen(LockpickingMenu::MENU_NAME) ||
         stillOpen(SleepWaitMenu::MENU_NAME) || stillOpen(RaceSexMenu::MENU_NAME) ||
         stillOpen(MapMenu::MENU_NAME) || stillOpen(DialogueMenu::MENU_NAME);
}

static bool IsBlockingMenu(const RE::BSFixedString& a_name)
{
  return a_name == InventoryMenu::MENU_NAME || a_name == CraftingMenu::MENU_NAME ||
         a_name == BarterMenu::MENU_NAME || a_name == TweenMenu::MENU_NAME ||
         a_name == GiftMenu::MENU_NAME || a_name == ContainerMenu::MENU_NAME ||
         a_name == MagicMenu::MENU_NAME || a_name == StatsMenu::MENU_NAME ||
         a_name == JournalMenu::MENU_NAME || a_name == LockpickingMenu::MENU_NAME ||
         a_name == SleepWaitMenu::MENU_NAME || a_name == RaceSexMenu::MENU_NAME ||
         a_name == MapMenu::MENU_NAME;
}

// Restore visibility on the NEXT UI frame: inside the close event the closing menu can
// still report IsMenuOpen()==true (stale), so deciding here would get skipped.
static void QueueHudRestore()
{
  const auto task = SKSE::GetTaskInterface();
  if (!task) {
    return;
  }
  task->AddTask([]() {
    auto ui = RE::UI::GetSingleton();
    if (!ui) {
      return;
    }
    if (!AnyBlockingMenuOpen(ui, RE::BSFixedString{})) {
      WidgetActiveEffects::toggle_visibility(true);
      CheckInI();
    }
  });
}

auto MenuHandler::ProcessEvent(const MenuOpenCloseEvent* event, BSTEventSource<MenuOpenCloseEvent>*) -> BSEventNotifyControl
{
  if (!event) {
    return BSEventNotifyControl::kContinue;
  }
  auto ui = RE::UI::GetSingleton();
  if (!ui) {
    return BSEventNotifyControl::kContinue;
  }

  const auto& name = event->menuName;

  // Loading screens / F9 quickload. CheckInI() runs unconditionally; movies may still
  // be loading, so RequestHudReconfig lets the Update hook re-apply once they are ready.
  if (name == LoadingMenu::MENU_NAME) {
    CheckInI();
    RequestHudReconfig();
    if (event->opening) {
      WidgetActiveEffects::toggle_visibility(false);
    } else {
      WidgetActiveEffects::show();  // kShow now (like the original / kPostLoadGame); coc relies on this
      WidgetActiveEffects::toggle_visibility(true);
      QueueHudRestore();
    }
    return BSEventNotifyControl::kContinue;
  }

  // Full-screen / HUD-blocking menus (including Map, so opening Map from the Tween menu
  // no longer strands the widget): hide while open, restore once the last one closes.
  // Visibility only - the always-open overlay is never closed via kHide.
  if (IsBlockingMenu(name)) {
    if (event->opening) {
      WidgetActiveEffects::toggle_visibility(false);
    } else {
      QueueHudRestore();
    }
    return BSEventNotifyControl::kContinue;
  }

  // Transient overlays and dialogue: visual toggle only.
  if (name == CursorMenu::MENU_NAME || name == MessageBoxMenu::MENU_NAME ||
      name == FaderMenu::MENU_NAME || name == DialogueMenu::MENU_NAME) {
    if (event->opening) {
      WidgetActiveEffects::toggle_visibility(false);
    } else {
      QueueHudRestore();
    }
  }

  return BSEventNotifyControl::kContinue;
}
