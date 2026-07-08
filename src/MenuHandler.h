#pragma once

// Marks that the widget needs its settings re-pushed once its movie has finished
// loading (loading screens). See ConfigureHudIfNeeded.
void RequestHudReconfig();

// Called every frame from the player Update hook. Once the widget movie is loaded,
// (re)shows + configures it exactly once, then clears the request. This is what makes
// the widget appear after `coc`/new game, where the movie is still loading when the
// loading screen closes.
void ConfigureHudIfNeeded();

struct MenuHandler final : public RE::BSTEventSink<RE::MenuOpenCloseEvent> {
public:
  [[nodiscard]] static auto get_singleton() noexcept -> MenuHandler*;
  static auto               register_() -> void;

  auto ProcessEvent(const RE::MenuOpenCloseEvent*               event,
                    RE::BSTEventSource<RE::MenuOpenCloseEvent>* event_source)
      -> RE::BSEventNotifyControl override;
};
