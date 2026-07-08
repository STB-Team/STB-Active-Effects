#include "WidgetActiveEffects.h"
using namespace RE;

WidgetActiveEffects::WidgetActiveEffects()
{
  const auto scale_form_manager = BSScaleformManager::GetSingleton();

  logger::info("Construct menu");

  inputContext  = Context::kNone;
  depthPriority = 0;

  menuFlags.set(UI_MENU_FLAGS::kAlwaysOpen);
  menuFlags.set(UI_MENU_FLAGS::kRequiresUpdate);
  menuFlags.set(UI_MENU_FLAGS::kAllowSaving);

  if (uiMovie) {
    uiMovie->SetMouseCursorCount(0);
  }

  scale_form_manager->LoadMovieEx(this, MENU_PATH, [](GFxMovieDef* def) -> void {
    def->SetState(GFxState::StateType::kLog, make_gptr<WidgetLogger>().get());
  });
}

void WidgetActiveEffects::register_()
{
  if (const auto ui = UI::GetSingleton()) {
    ui->Register(MENU_NAME, creator);
    logger::info("Register menu");
    show();
  }
}

void WidgetActiveEffects::show()
{
  if (const auto message_queue = UIMessageQueue::GetSingleton()) {
	if (auto ui = UI::GetSingleton(); !ui->GetMenu(TweenMenu::MENU_NAME) &&
										  !ui->GetMenu(ContainerMenu::MENU_NAME) && !ui->GetMenu(MapMenu::MENU_NAME) &&
										  !ui->GetMenu(InventoryMenu::MENU_NAME) && !ui->GetMenu(GiftMenu::MENU_NAME)) {
		logger::debug("Show menu");
		message_queue->AddMessage(MENU_NAME, UI_MESSAGE_TYPE::kShow, nullptr);
	}
  }
  logger::info("show (kShow) - widget menu REOPENED");
}

void WidgetActiveEffects::hide()
{
  if (const auto message_queue = UIMessageQueue::GetSingleton()) {
    logger::debug("Hide menu");
    message_queue->AddMessage(MENU_NAME, UI_MESSAGE_TYPE::kHide, nullptr);
  }
  logger::info("hide (kHide) - widget menu CLOSED");
}
#include "nlohmann/json.hpp"
using json = nlohmann::json;
namespace fs = std::filesystem;
static bool InfoLoad = false;

  TESForm* ReadText(const std::string& info, TESDataHandler* data)
  {
	size_t pos = info.find('~');
	if (pos == std::string::npos || pos == 0 || pos == info.length() - 1) {
		logger::error("Invalid info format: {}", info);
		return nullptr;
	}

	std::string esmName = info.substr(0, pos);
	std::string hexStr = info.substr(pos + 1);


	try {
		int hexValue = std::stoi(hexStr, nullptr, 16);

		if (auto form = data->LookupForm(hexValue, esmName)) {
			return form;
		} else {
			logger::error("Form not found: Hex={}, ESM={}", hexValue, esmName);
			return nullptr;
		}
	} catch (const std::invalid_argument& e) {
		e;
		logger::error("Invalid hex value: {}", hexStr);
		return nullptr;
	} catch (const std::out_of_range& e) {
		e;
		logger::error("Hex value out of range: {}", hexStr);
		return nullptr;
	}
  }

void CheckInI()
{
	if (const auto ui = UI::GetSingleton();
		ui && ui->GetMenu(WidgetActiveEffects::MENU_NAME) && ui->GetMenu(WidgetActiveEffects::MENU_NAME)->uiMovie) {
		const GFxValue scalex = Data::scalexAeff_;
		const GFxValue scale = Data::scaleAeff_;
		const GFxValue scaley = Data::scaleyAeff_;
		const GFxValue Alpha = Data::AlphaAeff_;
		const GFxValue preset01 = (int)Data::ParamAeff01_;
		const GFxValue preset02 = (int)Data::ParamAeff02_;
		const GFxValue preset03 = (int)Data::ParamAeff03_;
		const GFxValue preset04 = (int)Data::ParamAeff04_;
		auto widget = ui->GetMenu(WidgetActiveEffects::MENU_NAME);
		const GFxValue Visible = Data::VisibleAeff_;
		const GFxValue Right = Data::SetRightAeff_;
		const GFxValue Down = Data::SetDownAeff_;
		const GFxValue Max = Data::SetMaxRowAeff_;
		const GFxValue TimeFormat = Data::TimeFormat_;
		const GFxValue Spacing = Data::Spacing_;
		const GFxValue dur = Data::AeffDuration_;
		const GFxValue hort = "hor";
		const GFxValue vert = "vert";
		const GFxValue Group = Data::GroupAeff_;
		if (Data::VertAeff_ == 1)
			widget->uiMovie->Invoke("widget.setDirection", nullptr, &hort, 1);
		else
			widget->uiMovie->Invoke("widget.setDirection", nullptr, &vert, 1);
		widget->uiMovie->Invoke("widget.setGroup", nullptr, &Group, 1);
		widget->uiMovie->Invoke("widget.setTimeFormat", nullptr, &TimeFormat, 1);
		widget->uiMovie->Invoke("widget.setSpacing", nullptr, &Spacing, 1);
		widget->uiMovie->Invoke("widget.hideDuration", nullptr, &dur, 1);
		widget->uiMovie->Invoke("widget.setVisible", nullptr, &Visible, 1);
		widget->uiMovie->Invoke("widget.setParam1", nullptr, &preset01, 1);
		widget->uiMovie->Invoke("widget.setParam2", nullptr, &preset02, 1);
		widget->uiMovie->Invoke("widget.setParam3", nullptr, &preset03, 1);
		widget->uiMovie->Invoke("widget.setParam4", nullptr, &preset04, 1);
		widget->uiMovie->Invoke("widget.setScale", nullptr, &scale, 1);
		widget->uiMovie->Invoke("widget.setPosX", nullptr, &scalex, 1);
		widget->uiMovie->Invoke("widget.setPosY", nullptr, &scaley, 1);
		widget->uiMovie->Invoke("widget.setAlpha", nullptr, &Alpha, 1);
		widget->uiMovie->Invoke("widget.setRight", nullptr, &Right, 1);
		widget->uiMovie->Invoke("widget.setDown", nullptr, &Down, 1);
		widget->uiMovie->Invoke("widget.setMaxRow", nullptr, &Max, 1);
		if (!InfoLoad) {
			auto datah = TESDataHandler::GetSingleton();
			if (datah) {
				json data;
				std::ifstream ifs;
				if (fs::exists("Data/SKSE/Plugins/STBActiveEffectsInfo") &&
					fs::is_directory("Data/SKSE/Plugins/STBActiveEffectsInfo")) {
					for (const auto& entry : fs::directory_iterator("Data/SKSE/Plugins/STBActiveEffectsInfo")) {
						if (entry.is_regular_file() && entry.path().extension() == ".json") {
								std::ifstream ifs2(entry.path());
							if (ifs2.is_open()) {
								ifs2 >> data;
								ifs2.close();

								if (data.contains("effects")) {
									const auto& effects = data["effects"];
									for (const auto& effect : effects) {
										std::string InfoEfs;
										if (!effect[0].empty()) {
											for (size_t i = 0; i < effect[0].size(); ++i) {
												if (auto form = ReadText(effect[0][i].get<std::string>(), datah)) {
													InfoEfs += decimalToHex(std::to_string(form->formID));
													if (i < effect[0].size() - 1)
														InfoEfs += ",";
												} else if (effect[0][i].get<std::string>() == " ") {
													InfoEfs += " ";									
														InfoEfs += ",";								
												} else {
													InfoEfs += "None";             
													if (i < effect[0].size() - 1)  
														InfoEfs += ",";
												}
											}
										}

										std::string InfoSpells;
										if (!effect[1].empty()) {
											for (size_t i = 0; i < effect[1].size(); ++i) {
												if (auto form = ReadText(effect[1][i].get<std::string>(), datah)) {
													InfoSpells += decimalToHex(std::to_string(form->formID));
													if (i < effect[1].size() - 1)
														InfoSpells += ",";
												} else if (effect[1][i].get<std::string>() == " ") {  
													InfoSpells += " ";                               
													if (i < effect[1].size() - 1)                     
														InfoSpells += ",";                            
												} else {
													InfoSpells += "None";  
													if (i < effect[1].size() - 1)
														InfoSpells += ",";
												}
												
											}
										}

										const GFxValue result{ InfoEfs + "|" + InfoSpells + "|" + effect[2].get<std::string>() +
															   "|" + effect[3].get<std::string>() + "|" +
															   effect[4].get<std::string>() + "|" + effect[5].get<std::string>() +
															   "|" + effect[6].get<std::string>() };

										logger::info(" {}",
											InfoEfs + "|" + InfoSpells + "|" + effect[2].get<std::string>() + "|" +
												effect[3].get<std::string>() + "|" + effect[4].get<std::string>() + "|" +
												effect[5].get<std::string>() + "|" + effect[6].get<std::string>());

										if (widget && widget->uiMovie) {
											widget->uiMovie->Invoke("widget.setEffectsData", nullptr, &result, 1);
										}
									}
								}
							}
						}
					}
					InfoLoad = true;
				}
				std::string filePath = "Data/SKSE/Plugins/effectsData.json";
				ifs.open(filePath);

				if (ifs.is_open()) {
						ifs >> data;
						ifs.close();
						if (data.contains("effects")) {
							const auto& effects = data["effects"];

							for (const auto& effect : effects) {
									std::string InfoEfs;
									if (!effect[0].empty()) {
										for (size_t i = 0; i < effect[0].size(); ++i) {
											if (auto form = ReadText(effect[0][i].get<std::string>(), datah)) {
												InfoEfs += decimalToHex(std::to_string(form->formID));
												if (i < effect[0].size() - 1)
													InfoEfs += ",";
											} else if (effect[0][i].get<std::string>() == " ") {  
												InfoEfs += " ";                                   
												if (i < effect[0].size() - 1)                     
													InfoEfs += ",";                               
											} else {
												InfoEfs += "None";                
												if (i < effect[0].size() - 1)  
													InfoEfs += ",";       
											}
										}
									}

									std::string InfoSpells;
									if (!effect[1].empty()) {
										for (size_t i = 0; i < effect[1].size(); ++i) {
											if (auto form = ReadText(effect[1][i].get<std::string>(), datah)) {
												InfoSpells += decimalToHex(std::to_string(form->formID));
												if (i < effect[1].size() - 1)
													InfoSpells += ",";
											} else if (effect[1][i].get<std::string>() == " ") {  
												InfoSpells += " ";                                
												if (i < effect[1].size() - 1)                     
											InfoSpells += ",";                                    
											} else {
												InfoSpells += "None";  
												if (i < effect[1].size() - 1)
											    InfoSpells += ",";     
											}
										}
									}

									std::string effectData = InfoEfs + "|" + InfoSpells + "|" + effect[2].get<std::string>() +
								                             "|" + effect[3].get<std::string>() + "|" +
								                             effect[4].get<std::string>() + "|" + effect[5].get<std::string>();
									logger::info("Sending effect data2: {}", effectData);

									GFxValue result;
									result.SetString(effectData.c_str());
									widget->uiMovie->Invoke("widget.setEffectsData", nullptr, &result, 1);
								
							}
						}
				}
				
			}
		}
		}
}

std::string decimalToHex(const std::string& decimalStr)
{
  long long decimalValue = std::stoll(decimalStr);  

  std::stringstream ss;
  ss << std::hex << std::uppercase
	 << decimalValue; 

  return ss.str();  
}
const char* ActorValueToString(ActorValue av)
{
  using AV = ActorValue;

  switch (av) {
  case AV::kOneHanded:
		return "ONEHANDED";
  case AV::kMagicka:
		return "MAGICKA";
  case AV::kWaterBreathing:
		return "WATERBREATHING";
  case AV::kSneakingModifier:
		return "SNEAKMOD";
  case AV::kTwoHanded:
		return "TWOHANDED";
  case AV::kStamina:
		return "STAMINA";
  case AV::kWaterWalking:
		return "WATERWALKING";
  case AV::kAlchemyModifier:
		return "ALCHEMYMOD";
  case AV::kArchery:
		return "MARKSMAN";
  case AV::kHealRate:
		return "HEALRATE";
  case AV::kWardPower:
		return "WARDPOWER";
  case AV::kSpeechcraftModifier:
		return "SPEECHCRAFTMOD";
  case AV::kBlock:
		return "BLOCK";
  case AV::kMagickaRate:
		return "MAGICKARATE";
  case AV::kArmorPerks:
		return "ARMORPERKS";
  case AV::kAlterationModifier:
		return "ALTERATIONMOD";
  case AV::kSmithing:
		return "SMITHING";
  case AV::kStaminaRate:
		return "STAMINARATE";
  case AV::kBowSpeedBonus:
		return "BOWSPEEDBONUS";
  case AV::kConjurationModifier:
		return "CONJURATIONMOD";
  case AV::kHeavyArmor:
		return "HEAVYARMOR";
  case AV::kSpeedMult:
		return "SPEEDMULT";
  case AV::kAbsorbChance:
		return "ABSORBCHANCE";
  case AV::kDestructionModifier:
		return "DESTRUCTIONMOD";
  case AV::kLightArmor:
		return "LIGHTARMOR";
  case AV::kCarryWeight:
		return "CARRYWEIGHT";
  case AV::kWeaponSpeedMult:
		return "WEAPONSPEEDMULT";
  case AV::kIllusionModifier:
		return "ILLUSIONMOD";
  case AV::kPickpocket:
		return "PICKPOCKET";
  case AV::kMeleeDamage:
		return "MELEEDAMAGE";
  case AV::kShoutRecoveryMult:
		return "SHOUTRECOVERYMULT";
  case AV::kRestorationModifier:
		return "RESTORATIONMOD";
  case AV::kLockpicking:
		return "LOCKPICKING";
  case AV::kUnarmedDamage:
		return "UNARMEDDAMAGE";
  case AV::kBowStaggerBonus:
		return "BOWSTAGGERBONUS";
  case AV::kEnchantingModifier:
		return "ENCHANTINGMOD";
  case AV::kSneak:
		return "SNEAK";
  case AV::kDamageResist:
		return "DAMAGERESIST";
  case AV::kMovementNoiseMult:
		return "MOVEMENTNOISEMULT";
  case AV::kOneHandedSkillAdvance:
		return "ONEHANDEDSKILLADVANCE";
  case AV::kAlchemy:
		return "ALCHEMY";
  case AV::kPoisonResist:
		return "POISONRESIST";
  case AV::kOneHandedModifier:
		return "ONEHANDEDMOD";
  case AV::kSpeech:
		return "SPEECH";
  case AV::kResistFire:
		return "FIRERESIST";
  case AV::kTwoHandedModifier:
		return "TWOHANDEDMOD";
  case AV::kAlteration:
		return "ALTERATION";
  case AV::kResistShock:
		return "ELECTRICRESIST";
  case AV::kMarksmanModifier:
		return "MARKSMANMOD";
  case AV::kConjuration:
		return "CONJURATION";
  case AV::kResistFrost:
		return "FROSTRESIST";
  case AV::kBlockModifier:
		return "BLOCKMOD";
  case AV::kDestruction:
		return "DESTRUCTION";
  case AV::kResistMagic:
		return "MAGICKRESIST";
  case AV::kSmithingModifier:
		return "SMITHINGMOD";
  case AV::kIllusion:
		return "ILLUSION";
  case AV::kResistDisease:
		return "DISEASERESIST";
  case AV::kHeavyArmorModifier:
		return "HEAVYARMORMOD";
  case AV::kRestoration:
		return "RESTORATION";
  case AV::kParalysis:
		return "PARALYSIS";
  case AV::kLightArmorModifier:
		return "LIGHTARMORMOD";
  case AV::kEnchanting:
		return "ENCHANTING";
  case AV::kInvisibility:
		return "INVISIBILITY";
  case AV::kPickpocketModifier:
		return "PICKPOCKETMOD";
  case AV::kHealth:
		return "HEALTH";
  case AV::kNightEye:
		return "NIGHTEYE";
  case AV::kLockpickingModifier:
		return "LOCKPICKINGMOD";
  case AV::kLockpickingPowerModifier:
		return "LOCKPICKINGPOWERMOD";
  case AV::kBlockPowerModifier:
		return "BLOCKPOWERMOD";
  case AV::kPickpocketPowerModifier:
		return "PICKPOCKETPOWERMOD";
  case AV::kSmithingPowerModifier:
		return "SMITHINGPOWERMOD";
  case AV::kAlchemyPowerModifier:
		return "ALCHEMYPOWERMOD";
  case AV::kAlchemySkillAdvance:
		return "ALCHEMYSKILLADVANCE";
  case AV::kSpeechcraftPowerModifier:
		return "SPEECHCRAFTPOWERMOD";
  case AV::kLightArmorPowerModifier:
		return "LightArmorPOWERMOD";
  case AV::kHeavyArmorPowerModifier:
		return "HeavyArmorPOWERMOD";
  case AV::kSneakingPowerModifier:
		return "SneakPOWERMOD";
  case AV::kAlterationPowerModifier:
		return "ALTERATIONPOWERMOD";
  case AV::kConjurationPowerModifier:
		return "CONJURATIONPOWERMOD";
  case AV::kDestructionPowerModifier:
		return "DESTRUCTIONPOWERMOD";
  case AV::kIllusionPowerModifier:
		return "ILLUSIONPOWERMOD";
  case AV::kRestorationPowerModifier:
		return "RESTORATIONPOWERMOD";
  case AV::kEnchantingPowerModifier:
		return "ENCHANTINGPOWERMOD";
  case AV::kOneHandedPowerModifier:
		return "ONEHANDEDPOWERMOD";
  case AV::kTwoHandedPowerModifier:
		return "TWOHANDEDPOWERMOD";
  case AV::kMarksmanPowerModifier:
		return "MARKSMANPOWERMOD";
  case AV::kHealRateMult:
		return "HEALRATEMULT";
  case AV::kMagickaRateMult:
		return "MAGICKARATEMULT";
  case AV::kStaminaRateMult:
		return "STAMINARATEMULT";
  default:
		return " ";
  }
}

const char* ArchetypeToString(EffectArchetype archetype)
{
  using Archetype = EffectArchetype;  // shorthand

  switch (archetype) {
  case Archetype::kInvisibility:
		return "Invisibility";
  case Archetype::kNightEye:
		return "NightEye";
  case Archetype::kLight:
		return "LIGHT";
  case Archetype::kCloak:
		return "CLOAK";
  case Archetype::kBoundWeapon:
		return "BoundWeapon";
  case Archetype::kSummonCreature:
		return "SummonCreature";
  case Archetype::kParalysis:
		return "Paralysis";
  case Archetype::kSlowTime:
		return "SlowTime";
  case Archetype::kEnhanceWeapon:
		return "EnhanceWeapon";
  case Archetype::kEtherealize:
		return "Etherealize";
  default:
		return " ";
  }
}
void WidgetActiveEffects::update()
{
  const auto ui = UI::GetSingleton();
  if (!ui || ui->GameIsPaused()) {
    return;
  }
  const auto widgeteffect = ui->GetMenu(MENU_NAME);
  if (!widgeteffect || !widgeteffect->uiMovie) {
    return;
  }
  const auto player = PlayerCharacter::GetSingleton();
  if (!player) 
    return;
  
  auto list = player->AsMagicTarget()->GetActiveEffectList();
  if (!list)
	return;
  std::vector <std::string> Strings;
  std::string result;
  using flag = EffectSetting::EffectSettingData::Flag;
	 for (auto ef : *list)
	if (ef && ef->effect && ef->effect->baseEffect && ef->spell &&
		!ef->flags.any(ActiveEffect::Flag::kInactive) && ef->duration > 0 &&
		!ef->effect->baseEffect->data.flags.any(flag::kHideInUI)) {
			auto eff = ef->effect->baseEffect;
		std::string name2 = decimalToHex(std::to_string(ef->spell->formID));
		std::string name1 = decimalToHex(std::to_string(eff->formID));
		std::string text = std::to_string((uint64_t)ef);
		std::string NowDur = std::to_string(ef->duration - ef->elapsedSeconds);

		int check = 0;
		if (eff->data.flags.any(flag::kDetrimental)) {
				if (eff->data.flags.any(flag::kRecover))
				check = 1;
				else if (!eff->data.flags.any(flag::kRecover) && (eff->data.primaryAV == ActorValue::kHealth ||
						 eff->data.primaryAV == ActorValue::kMagicka || eff->data.primaryAV == ActorValue::kStamina))
				check = 2;
		} else if (!eff->data.flags.any(flag::kRecover) &&
				   (eff->data.primaryAV == ActorValue::kHealth || eff->data.primaryAV == ActorValue::kMagicka ||
					   eff->data.primaryAV == ActorValue::kStamina)) {
				check = 5;
		}
		auto arch = eff->data.archetype;
		if ((arch == EffectSetting::Archetype::kSummonCreature || arch == EffectSetting::Archetype::kBoundWeapon) && 
			eff->data.associatedSkill == ActorValue::kConjuration && ef->spell->GetSpellType() == MagicSystem::SpellType::kSpell)
			check = 3;
		else if (arch == EffectSetting::Archetype::kSummonCreature || arch == EffectSetting::Archetype::kBoundWeapon)
			check = 4;
		auto av = " ";
		if (arch != EffectSetting::Archetype::kValueModifier && arch != EffectSetting::Archetype::kPeakValueModifier &&
			arch != EffectSetting::Archetype::kDualValueModifier && arch != EffectSetting::Archetype::kAbsorb)
			av = ArchetypeToString(arch);
		else
			av = ActorValueToString(eff->data.primaryAV);
		std::string str = name1 + "," + name2 + "," + std::to_string((int)ef->magnitude) + "," + NowDur + "," +
			              std::to_string((int)ef->duration) + "," + text + "," + std::to_string(check) + "," + av;
		Strings.push_back(str);

	}
	 bool check = false;
	 for (auto& s : Strings)
	if (check) {
		result += "|" + s;
	} else {
		result += s;
		check = true;
	}
	

     const GFxValue effect{ result };
	 widgeteffect->uiMovie->Invoke("widget.Update", nullptr, &effect, 1);
}

void WidgetActiveEffects::AdvanceMovie(const float interval, const uint32_t current_time)
{
  logger::debug("AdvanceMovie");
	 updateTimer += interval;

	 if (updateTimer >= 0.1f) {
	update();
	updateTimer = 0.0f;
	 }
	 
  IMenu::AdvanceMovie(interval, current_time);
}

void WidgetActiveEffects::toggle_visibility(const bool mode)
{
  // No caching: the per-frame Update hook calls toggle_visibility(true) every frame in
  // normal play, which re-asserts SetVisible(true). A dedup cache would let a stale
  // hidden state stick and the widget could vanish for good.
  const auto ui = UI::GetSingleton();
  if (!ui) {
    return;
  }

  const auto overlay_menu = ui->GetMenu(MENU_NAME);
  if (!overlay_menu || !overlay_menu->uiMovie) {
    return;
  }

  overlay_menu->uiMovie->SetVisible(mode);
}
