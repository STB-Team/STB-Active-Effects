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

// Apply the config once we are safely in the world. Returns true when there is nothing
// left to do. Never touches the movie while a LoadingMenu is up (that crashes Scaleform);
// the post-load fade is fine.
static bool TryApplyHudConfig()
{
  if (!g_hudNeedsConfig) {
    return true;
  }
  const auto ui = RE::UI::GetSingleton();
  if (!ui) {
    return false;
  }
  if (ui->IsMenuOpen(RE::LoadingMenu::MENU_NAME)) {
    return false;  // still loading - wait
  }
  const auto widget = ui->GetMenu(WidgetActiveEffects::MENU_NAME);
  if (!widget || !widget->uiMovie) {
    return false;  // movie still loading
  }
  WidgetActiveEffects::show();
  CheckInI();
  WidgetActiveEffects::toggle_visibility(true);
  g_hudNeedsConfig = false;
  logger::info("HUD configured");
  return true;
}

// Fallback path, driven every frame from the PlayerCharacter::Update hook.
void ConfigureHudIfNeeded()
{
  TryApplyHudConfig();
}

// Primary path: polled through the SKSE task queue, started the moment the loading screen
// closes. The task queue keeps ticking during the post-load fade-in, whereas the player
// Update hook is paused then - so this applies the config immediately instead of ~1s later.
// Bounded so it can never spin forever.
static void PollHudConfig(int attempts)
{
  if (TryApplyHudConfig()) {
    return;
  }
  if (attempts <= 0) {
    return;  // give up; the Update hook keeps trying as a fallback
  }
  if (const auto task = SKSE::GetTaskInterface()) {
    task->AddTask([attempts]() { PollHudConfig(attempts - 1); });
  }
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
      WidgetActiveEffects::toggle_visibility(true);  // visibility only. Do NOT CheckInI
                     // from a deferred task: its setParam/frame invokes crash Scaleform
                     // when the movie hasn't advanced yet on a cold load.
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

  // Loading screens / F9 quickload. Do NOT CheckInI here: the movie is mid-transition and
  // a frame rebuild crashes Scaleform (load-time CTD). Just flag a reconfig;
  // ConfigureHudIfNeeded applies it once we are back in the world.
  if (name == LoadingMenu::MENU_NAME) {
    RequestHudReconfig();
    if (event->opening) {
      WidgetActiveEffects::toggle_visibility(false);
    } else {
      WidgetActiveEffects::show();  // kShow (safe: just queues a UI message); coc relies on this
      WidgetActiveEffects::toggle_visibility(true);
      PollHudConfig(600);  // apply config as soon as the load ends, via the task queue
                           // (ticks during the fade, unlike the player Update hook)
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
