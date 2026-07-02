#pragma once
#include <SimpleIni.h>
std::string decimalToHex(const std::string& decimalStr);
void CheckInI();
struct Data
{
private:
	Data() = default;
	

public:
	[[nodiscard]] static auto get_singleton() noexcept -> Data&
	{
		static Data instance;
		return instance;
	}
	static inline bool enable = false;
	static inline bool hidden = true;
	static inline float scalexAeff_{ 100 };
	static inline float scaleAeff_{ 100 };
	static inline float scaleyAeff_{ 100 };
	static inline float AeffDuration_{ 400 };
	static inline int VertAeff_{ 1 };
	static inline bool ParamAeff01_{ 1 };
	static inline bool ParamAeff02_{ 1 };
	static inline bool ParamAeff03_{ 1 };
	static inline bool ParamAeff04_{ 1 };
	static inline float AlphaAeff_{ 100 };
	static inline bool VisibleAeff_{ true };
	static inline int SetRightAeff_{ 1 };
	static inline int SetDownAeff_{ 1 };
	static inline int SetMaxRowAeff_{ 10 };
	static inline float SildersScale_{ 1 };
	static inline bool LoggerEffects_ = false;
	static inline int TimeFormat_{ 0 };
	static inline int Spacing_{ 10 };
	static inline bool GroupAeff_{ false };
	static inline const wchar_t* PosAeffWidgetX{ L"fPosAeffWidgetX" };
	static inline const wchar_t* section{ L"Main" };
	static inline const wchar_t* sectiondata{ L"Settings" };
	static inline const wchar_t* LoggerEffects{ L"DebugEffects" };
	static inline const wchar_t* OpenMenu{ L"OpenMenu" };
	static inline const wchar_t* sectionbase{ L"Default" };
	static inline const wchar_t* VertAeff{ L"iVertAeffWidget" };
	static inline const wchar_t* AeffDuration{ L"fAeffDuration" };
	static inline const wchar_t* path_to_ini{ L"Data/SKSE/Plugins/STB_ActiveEffects.ini" };
	static inline const wchar_t* ScaleAeffWidget{ L"fScaleAeffWidget" };
	static inline const wchar_t* PosAeffWidgetY{ L"fPosAeffWidgetY" };
	static inline const wchar_t* AlphaAeff{ L"fSetAlphaAeff" };
	static inline const wchar_t* ParamAeff01{ L"iParamAeffWidget01" };
	static inline const wchar_t* ParamAeff02{ L"iParamAeffWidget02" };
	static inline const wchar_t* ParamAeff03{ L"iParamAeffWidget03" };
	static inline const wchar_t* ParamAeff04{ L"iParamAeffWidget04" };
	static inline const wchar_t* VisibleAeff{ L"SetAeffVisible" };
	static inline const wchar_t* SetDownAeff{ L"iSetDownAeffWidget" };
	static inline const wchar_t* SetRightAeff{ L"iSetRightAeffWidget" };
	static inline const wchar_t* SetMaxRowAeff{ L"iSetMaxRowAeffWidget" };
	static inline const wchar_t* SildersScale { L"fSildersScale" };
	static inline const wchar_t* TimeFormat{ L"iTimeFormat" };
	static inline const wchar_t* Spacing{ L"iSpacing" };
	static inline const wchar_t* GroupAeff{ L"GroupAeffWidget" };
	void LoadToIni()
	{
		const auto read_settings = [this](CSimpleIni& ini, const wchar_t* path) -> void {
			ini.LoadFile(path);

				ini.SetDoubleValue(section, PosAeffWidgetX, scalexAeff_);
			ini.SetDoubleValue(section, AeffDuration, AeffDuration_);
			ini.SetLongValue(section, VertAeff, VertAeff_);

			ini.SetDoubleValue(section, PosAeffWidgetY, scaleyAeff_);
			ini.SetDoubleValue(section, ScaleAeffWidget, scaleAeff_);
			ini.SetBoolValue(section, ParamAeff01, ParamAeff01_);
			ini.SetBoolValue(section, ParamAeff02, ParamAeff02_);
			ini.SetBoolValue(section, ParamAeff03, ParamAeff03_);
			ini.SetBoolValue(section, ParamAeff04, ParamAeff04_);
			ini.SetDoubleValue(section, AlphaAeff, AlphaAeff_);
			ini.SetBoolValue(section, VisibleAeff, VisibleAeff_);
			ini.SetLongValue(section, SetRightAeff, SetRightAeff_);
			ini.SetLongValue(section, SetDownAeff, SetDownAeff_);
			ini.SetLongValue(section, SetMaxRowAeff, SetMaxRowAeff_);
			ini.SetDoubleValue(section, SildersScale, SildersScale_);
			ini.SetLongValue(section, TimeFormat, TimeFormat_);
			ini.SetLongValue(section, Spacing, Spacing_);
			ini.SetBoolValue(section, GroupAeff, GroupAeff_);
			
		};

		CSimpleIni ini;

		read_settings(ini, path_to_ini);

		ini.SaveFile(path_to_ini);
	}
	void LoadToBaseIni()
	{
		const auto read_settings = [this](CSimpleIni& ini, const wchar_t* path) -> void {
			ini.LoadFile(path);

			ini.SetDoubleValue(sectionbase, PosAeffWidgetX, scalexAeff_);
			ini.SetDoubleValue(sectionbase, AeffDuration, AeffDuration_);
			ini.SetLongValue(sectionbase, VertAeff, VertAeff_);

			ini.SetDoubleValue(sectionbase, PosAeffWidgetY, scaleyAeff_);
			ini.SetDoubleValue(sectionbase, ScaleAeffWidget, scaleAeff_);
			ini.SetBoolValue(sectionbase, ParamAeff01, ParamAeff01_);
			ini.SetBoolValue(sectionbase, ParamAeff02, ParamAeff02_);
			ini.SetBoolValue(sectionbase, ParamAeff03, ParamAeff03_);
			ini.SetBoolValue(sectionbase, ParamAeff04, ParamAeff04_);
			ini.SetDoubleValue(sectionbase, AlphaAeff, AlphaAeff_);
			ini.SetBoolValue(sectionbase, VisibleAeff, VisibleAeff_);
			ini.SetLongValue(sectionbase, SetRightAeff, SetRightAeff_);
			ini.SetLongValue(sectionbase, SetDownAeff, SetDownAeff_);
			ini.SetLongValue(sectionbase, SetMaxRowAeff, SetMaxRowAeff_);
			ini.SetDoubleValue(sectionbase, SildersScale, SildersScale_);
			ini.SetLongValue(sectionbase, TimeFormat, TimeFormat_);
			ini.SetLongValue(sectionbase, Spacing, Spacing_);
			ini.SetBoolValue(sectionbase, GroupAeff, GroupAeff_);
		};
		CSimpleIni ini;

		read_settings(ini, path_to_ini);

		ini.SaveFile(path_to_ini);
	}
	auto load(bool based) -> void
	{
		if (based) {
			const auto read_double = [this](const CSimpleIni& ini, const wchar_t* key, float& value) -> void {
				if (ini.GetValue(sectionbase, key)) {
					value = static_cast<float>(ini.GetDoubleValue(sectionbase, key));
				}
			};
			const auto read_int = [this](const CSimpleIni& ini, const wchar_t* key, int& value) -> void {
				if (ini.GetValue(sectionbase, key)) {
					value = ini.GetLongValue(sectionbase, key);
				}
			};
			const auto read_bool = [this](const CSimpleIni& ini, const wchar_t* key, bool& value) -> void {
				if (ini.GetValue(sectionbase, key)) {
					value = ini.GetBoolValue(sectionbase, key);
				}
			};
			const auto read_settings = [this, read_int, read_bool, read_double](CSimpleIni& ini, const wchar_t* path) -> void {
				ini.LoadFile(path);

				read_double(ini, PosAeffWidgetX, scalexAeff_);
				read_double(ini, AeffDuration, AeffDuration_);
				read_int(ini, VertAeff, VertAeff_);
				read_double(ini, PosAeffWidgetY, scaleyAeff_);
				read_double(ini, ScaleAeffWidget, scaleAeff_);
				read_bool(ini, ParamAeff01, ParamAeff01_);
				read_bool(ini, ParamAeff02, ParamAeff02_);
				read_bool(ini, ParamAeff03, ParamAeff03_);
				read_bool(ini, ParamAeff04, ParamAeff04_);
				read_double(ini, AlphaAeff, AlphaAeff_);
				read_bool(ini, VisibleAeff, VisibleAeff_);
				read_int(ini, SetRightAeff, SetRightAeff_);
				read_int(ini, SetDownAeff, SetDownAeff_);
				read_int(ini, SetMaxRowAeff, SetMaxRowAeff_);
				read_double(ini, SildersScale, SildersScale_);
				read_int(ini, TimeFormat, TimeFormat_);
				read_int(ini, Spacing, Spacing_);
				read_bool(ini, GroupAeff, GroupAeff_);
				LoggerEffects_ = ini.GetBoolValue(sectiondata, LoggerEffects);

			};
			CSimpleIni ini;
			ini.SetUnicode();

			read_settings(ini, path_to_ini);
		} else {
			const auto read_double = [this](const CSimpleIni& ini, const wchar_t* key, float& value) -> void {
				if (ini.GetValue(section, key)) {
					value = static_cast<float>(ini.GetDoubleValue(section, key));
				}
			};
			const auto read_int = [this](const CSimpleIni& ini, const wchar_t* key, int& value) -> void {
				if (ini.GetValue(section, key)) {
					value = ini.GetLongValue(section, key);
				}
			};
			const auto read_bool = [this](const CSimpleIni& ini, const wchar_t* key, bool& value) -> void {
				if (ini.GetValue(section, key)) {
					value = ini.GetBoolValue(section, key);
				}
			};
			const auto read_settings = [this, read_int, read_bool, read_double](CSimpleIni& ini, const wchar_t* path) -> void {
				ini.LoadFile(path);
				read_double(ini, PosAeffWidgetX, scalexAeff_);
				read_double(ini, AeffDuration, AeffDuration_);
				read_int(ini, VertAeff, VertAeff_);
				read_double(ini, PosAeffWidgetY, scaleyAeff_);
				read_double(ini, ScaleAeffWidget, scaleAeff_);
				read_bool(ini, ParamAeff01, ParamAeff01_);
				read_bool(ini, ParamAeff02, ParamAeff02_);
				read_bool(ini, ParamAeff03, ParamAeff03_);
				read_bool(ini, ParamAeff04, ParamAeff04_);
				read_double(ini, AlphaAeff, AlphaAeff_);
				read_bool(ini, VisibleAeff, VisibleAeff_);
				read_int(ini, SetRightAeff, SetRightAeff_);
				read_int(ini, SetDownAeff, SetDownAeff_);
				read_int(ini, SetMaxRowAeff, SetMaxRowAeff_);
				read_double(ini, SildersScale, SildersScale_);
				read_int(ini, TimeFormat, TimeFormat_);
				read_int(ini, Spacing, Spacing_);
				read_bool(ini, GroupAeff, GroupAeff_);
				LoggerEffects_ = ini.GetBoolValue(sectiondata, LoggerEffects);
		
			};

			CSimpleIni ini;

			ini.SetUnicode();
			read_settings(ini, path_to_ini);
		}
	}
};
	struct WidgetActiveEffects final : RE::IMenu
{
public:
  static constexpr const char* MENU_PATH = "STBActiveEffects";
  static constexpr const char* MENU_NAME = "STBActiveEffects";

  WidgetActiveEffects();

  static void register_();
  static void show();
  static void hide();
  static void update();
  static void toggle_visibility(bool mode);

  static auto creator()->RE::stl::owner<RE::IMenu*> { return new WidgetActiveEffects(); }

  void AdvanceMovie(float interval, uint32_t current_time) override;

private:
  float updateTimer = 0.0f;
  class WidgetLogger final : public RE::GFxLog
  {
  public:
    void LogMessageVarg(LogMessageType, const char* fmt, const std::va_list arg_list) override {
      std::string format(fmt ? fmt : "");
      while (!format.empty() && format.back() == '\n') {
        format.pop_back();
      }

      std::va_list args;
      va_copy(args, arg_list);
      std::vector<char> buf(
          static_cast<std::size_t>(std::vsnprintf(0, 0, format.c_str(), arg_list) + 1));
      std::vsnprintf(buf.data(), buf.size(), format.c_str(), args);
      va_end(args);

      logger::info("{}"sv, buf.data());
    }
  };
};
