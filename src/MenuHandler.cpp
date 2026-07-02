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

auto MenuHandler::ProcessEvent(const MenuOpenCloseEvent* event, BSTEventSource<MenuOpenCloseEvent>*) -> BSEventNotifyControl
{
  if (event) {
	if (auto ui = RE::UI::GetSingleton()) {
		bool otherMenuOpen = ui->IsMenuOpen(RE::InventoryMenu::MENU_NAME) || ui->IsMenuOpen(RE::CraftingMenu::MENU_NAME) ||
			                 ui->IsMenuOpen(RE::BarterMenu::MENU_NAME) || ui->IsMenuOpen(RE::TweenMenu::MENU_NAME) ||
			                 ui->IsMenuOpen(RE::GiftMenu::MENU_NAME) || ui->IsMenuOpen(RE::ContainerMenu::MENU_NAME) ||
			                 ui->IsMenuOpen(RE::MagicMenu::MENU_NAME) || ui->IsMenuOpen(RE::StatsMenu::MENU_NAME) ||
			                 ui->IsMenuOpen(RE::JournalMenu::MENU_NAME) || ui->IsMenuOpen(RE::LockpickingMenu::MENU_NAME) ||
			                 ui->IsMenuOpen(RE::SleepWaitMenu::MENU_NAME) || ui->IsMenuOpen(RE::RaceSexMenu::MENU_NAME) ||
			                 ui->IsMenuOpen(RE::FaderMenu::MENU_NAME);

		bool dialogueOpen = ui->IsMenuOpen(RE::DialogueMenu::MENU_NAME);

		if (event->menuName == RaceSexMenu::MENU_NAME || event->menuName == LockpickingMenu::MENU_NAME ||
			event->menuName == JournalMenu::MENU_NAME || event->menuName == ContainerMenu::MENU_NAME ||
			event->menuName == GiftMenu::MENU_NAME || event->menuName == BarterMenu::MENU_NAME ||
			event->menuName == CraftingMenu::MENU_NAME || event->menuName == TweenMenu::MENU_NAME ||
			event->menuName == MagicMenu::MENU_NAME || event->menuName == InventoryMenu::MENU_NAME) {
			if (event->opening) {
				WidgetActiveEffects::hide();
			} else if (!otherMenuOpen && !dialogueOpen) {
				WidgetActiveEffects::show();
			} else if (!otherMenuOpen && dialogueOpen) {
				WidgetActiveEffects::show();
				WidgetActiveEffects::toggle_visibility(false);
			}
		}

		if (event->menuName == LoadingMenu::MENU_NAME) {
			CheckInI();
			event->opening ? WidgetActiveEffects::hide() : WidgetActiveEffects::show();
		}

		if (event->menuName == MapMenu::MENU_NAME || event->menuName == CursorMenu::MENU_NAME ||
			event->menuName == MessageBoxMenu::MENU_NAME || event->menuName == FaderMenu::MENU_NAME) {
			WidgetActiveEffects::toggle_visibility(!event->opening);
		}

		if (event->menuName == DialogueMenu::MENU_NAME) {
			WidgetActiveEffects::toggle_visibility(!event->opening);
		}
	}
  }
  return BSEventNotifyControl::kContinue;
}
