using namespace RE;
#include "string.h"
#include "WidgetActiveEffects.h"
#include "MenuHandler.h"  // ConfigureHudIfNeeded (used by the Update hook below)

#include <windows.h>

std::string formIdToHexString(uint32_t formId)
{
	std::stringstream ss;
	ss << std::hex << std::uppercase << formId; 
	std::string fullHex = ss.str();

	if (fullHex.length() < 8) {
		fullHex = std::string(8 - fullHex.length(), '0') + fullHex;
	}

	return fullHex.substr(2);
}

class EfAddP
{
public:
	static void Hook()
	{
		_EffectAddP = REL::Relocation<uintptr_t>(VTABLE_PlayerCharacter[4]).write_vfunc(0x8, EffectAddP);
	}

private:
	static void EffectAddP(MagicTarget* target, ActiveEffect* ef)
	{
		if (ef && ef->effect && ef->effect->baseEffect && ef->spell &&
			ef->effect->baseEffect->sourceFiles.array && !ef->effect->baseEffect->sourceFiles.array->empty() &&
			ef->spell->sourceFiles.array && !ef->spell->sourceFiles.array->empty()) {
			auto effect = ef->effect->baseEffect;
			if (!effect->data.flags.any(EffectSetting::EffectSettingData::Flag::kHideInUI)) {
			std::string name1 = effect->GetName();
			std::string name2 = ef->spell->GetName();
			std::string name3 = effect->sourceFiles.array->front()->fileName;
			std::string name4 = formIdToHexString(effect->formID);
			std::string name5 = ef->spell->sourceFiles.array->front()->fileName;
			std::string name6 = formIdToHexString(ef->spell->formID);
			std::string result = name1 + "|" + name2 + "|" + name3 + "|" + name4 + "|" + name5 + "|" + name6;

			std::ifstream input_file("Data/SKSE/Plugins/widgetEffects.txt");
			std::string line;
			bool exists = false;

			while (std::getline(input_file, line)) {
				if (line == result) {
					exists = true;
					break;
				}
			}
			input_file.close(); 

			if (!exists) {
				std::ofstream output_file("Data/SKSE/Plugins/widgetEffects.txt", std::ios::app);
				if (output_file.is_open()) {
					output_file << "\n"; 
					output_file << result;  
					output_file.close();    
				}
			}
			}
		}
		_EffectAddP(target, ef);
	}
	static inline REL::Relocation<decltype(EffectAddP)> _EffectAddP;
};
class PlayerUpdate
{
public:
	static void Hook() { _Update = REL::Relocation<uintptr_t>(VTABLE_PlayerCharacter[0]).write_vfunc(0xad, Update); }

private:
	static void Update(PlayerCharacter* player, float delta)
	{
		if (auto camera = PlayerCamera::GetSingleton()) {
			if (camera->currentState == camera->cameraStates[CameraState::kAutoVanity] ||
				camera->currentState == camera->cameraStates[CameraState::kVATS] ||
				camera->currentState == camera->cameraStates[CameraState::kFree])
				WidgetActiveEffects::toggle_visibility(false);
			else if (auto ui = UI::GetSingleton();
					 ui && !ui->IsMenuOpen(RE::MessageBoxMenu::MENU_NAME) && !ui->IsMenuOpen(RE::DialogueMenu::MENU_NAME) &&
					 !ui->IsMenuOpen(RE::CursorMenu::MENU_NAME) && !ui->IsMenuOpen(RE::MapMenu::MENU_NAME) &&
					 !ui->IsMenuOpen(RE::FaderMenu::MENU_NAME) && !ui->IsMenuOpen(RE::LoadingMenu::MENU_NAME)) {
				// Configure the widget once its movie has loaded (coc / new game race),
				// then keep it visible in normal play.
				ConfigureHudIfNeeded();
				WidgetActiveEffects::toggle_visibility(true);
			}
		}
		_Update(player, delta);
	}

	static inline REL::Relocation<decltype(Update)> _Update;
};

#include "MenuHandler.h"
#include "SKSEMenuFramework.h"
#include <string>


namespace SKSE_Menu
{
	using namespace ImGuiMCP;

	void RenderTooltip(const char* desc)
	{
		if (IsItemHovered()) {
			BeginTooltip();
			PushTextWrapPos(GetFontSize() * 35.0f);
			TextUnformatted(desc);
			PopTextWrapPos();
			EndTooltip();
		}
	}

void __stdcall ShowImgui()
	{
		auto& Data = Data::get_singleton();
		bool changed = false;

		if (Checkbox("Enable Widget", &Data.VisibleAeff_))
			changed = true;

		if (Data.VisibleAeff_) {
			if (SliderFloat("X Position", &Data.scalexAeff_, -200 * Data.SildersScale_, 1480 * Data.SildersScale_))
				changed = true;

			if (SliderFloat("Y Position", &Data.scaleyAeff_, -200 * Data.SildersScale_, 920 * Data.SildersScale_))
				changed = true;			

			if (SliderFloat("Size", &Data.scaleAeff_, 0, 100))
				changed = true;

			if (SliderFloat("Opacity", &Data.AlphaAeff_, 0, 100))
				changed = true;
			
			if (SliderFloat("Hide effects with duration above (in minutes)", &Data.AeffDuration_, 0, 400))
				changed = true;
			Text("Time Format:");
			if (RadioButton("HH:MM:SS", &Data.TimeFormat_, 0))
				changed = true;
			SameLine();
			if (RadioButton("Auto HH:MM:SS / MM:SS", &Data.TimeFormat_, 1))
				changed = true;
			SameLine();
			if (RadioButton("MM:SS (Up to 99 min)", &Data.TimeFormat_, 2))
				changed = true;
			SameLine();
			if (RadioButton("MM:SS (No limit)", &Data.TimeFormat_, 3))
				changed = true;

			if (SliderInt("Icons per column", &Data.SetMaxRowAeff_, 1, 20))
				changed = true;
			if (SliderInt("Spacing", &Data.Spacing_, 10, 100))
				changed = true;
			if (RadioButton("Direction: Horizontal", &Data.VertAeff_, 1))
				changed = true;
			SameLine();
			if (RadioButton("Direction: Vertical", &Data.VertAeff_, 2))
				changed = true;
			if (Checkbox("Group effects", &Data.GroupAeff_))
				changed = true;
			if (Checkbox("Show duration bar", &Data.ParamAeff01_))
				changed = true;
			SameLine();
			if (Checkbox("Show time", &Data.ParamAeff02_))
				changed = true;
			SameLine();
			if (Checkbox("Show magnitude", &Data.ParamAeff03_))
				changed = true;
			SameLine();
			if (Checkbox("Show source", &Data.ParamAeff04_))
				changed = true;

			if (RadioButton("Appear from right", &Data.SetRightAeff_, 1))
				changed = true;
			SameLine();
			if (RadioButton("Appear from left", &Data.SetRightAeff_, -1))
				changed = true;
			if (RadioButton("Appear from top", &Data.SetDownAeff_, 1))
				changed = true;
			SameLine();
			if (RadioButton("Appear from bottom", &Data.SetDownAeff_, -1))
				changed = true;
			if (InputFloat("Scale sliders for Pos Y and X", &Data.SildersScale_))
				changed = true;
			SetNextItemWidth(50);
		}
		static int clicked = 0;
		if (Button("Restore default settings"))
			clicked++;
		if (clicked & 1) {
			Data.load(true);
			clicked--;
			CheckInI();
			Data.LoadToIni();
		}
		static int clicked2 = 0;
		if (Button("Save settings"))
			clicked2++;
		if (clicked2 & 1) {
			Data.LoadToBaseIni();
			clicked2--;
		}
			Separator();

		if (changed) {
			CheckInI();
			Data.LoadToIni();
		}
	}
	 void register_skse_menu()
	{
		if (!SKSEMenuFramework::IsInstalled()) {
			logger::warn("SKSEMenuFramework not installed");
			return;
		}

		static constexpr auto main_title = "STB Active Effects";
		SKSEMenuFramework::SetSection(main_title);

		static constexpr auto settings_title = "General";
		SKSEMenuFramework::AddSectionItem(settings_title, ShowImgui);

	}
}


	void MessageHandler(SKSE::MessagingInterface::Message* a_message)
{
	switch (a_message->type) {
	case SKSE::MessagingInterface::kDataLoaded:
		WidgetActiveEffects::register_();
		WidgetActiveEffects::show();
		MenuHandler::register_();
		PlayerUpdate::Hook();
		Data::get_singleton().load(false);
		if (Data::LoggerEffects_)
			EfAddP::Hook();
		SKSE_Menu::register_skse_menu();
		
		break;
	case SKSE::MessagingInterface::kNewGame: 
			Data::get_singleton().load(false);
			WidgetActiveEffects::show();
			break;
	case SKSE::MessagingInterface::kPostLoadGame:
			Data::get_singleton().load(false);
			CheckInI();
			WidgetActiveEffects::show();
			break;
		}
	}

	extern "C" DLLEXPORT bool SKSEAPI SKSEPlugin_Query(const SKSE::QueryInterface* a_skse, SKSE::PluginInfo* a_info)
{
	a_info->infoVersion = SKSE::PluginInfo::kVersion;
	a_info->name = "STB_ActiveEffects";
	a_info->version = 1;

	if (a_skse->IsEditor()) {
		logger::critical("Loaded in editor, marking as incompatible"sv);
		return false;
	}

	return true;
}

extern "C" DLLEXPORT constinit auto SKSEPlugin_Version = []() {
	SKSE::PluginVersionData v;

	v.PluginVersion(1);
	v.PluginName("STB_ActiveEffects");
	v.AuthorName("STB");
	v.UsesAddressLibrary(true);
	v.CompatibleVersions({ SKSE::RUNTIME_SSE_LATEST });
	v.HasNoStructUse(true);

	return v;
}();

void InitializeLog()
{
	auto path = logger::log_directory();
	if (!path) {
		stl::report_and_fail("Failed to find standard logging directory"sv);
	}

	*path /= fmt::format(FMT_STRING("{}.log"), Version::PROJECT);
	auto sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(path->string(), true);

	auto log = std::make_shared<spdlog::logger>("global log"s, std::move(sink));

	log->set_level(spdlog::level::info);
	log->flush_on(spdlog::level::info);

	spdlog::set_default_logger(std::move(log));
	spdlog::set_pattern("[%l] %v"s);
}

extern "C" DLLEXPORT bool SKSEAPI SKSEPlugin_Load(const SKSE::LoadInterface* a_skse)
{
	InitializeLog();
	SKSE::Init(a_skse);
	SKSE::AllocTrampoline(1 << 9);
	auto messaging = SKSE::GetMessagingInterface();
	if (!messaging->RegisterListener("SKSE", MessageHandler)) {
	return false;
	}
	
	return true;
}



