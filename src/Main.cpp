/*
Copyright (C) 2023, 2025 Jonathan Feenstra

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

class SkipTextFunctionHandler final : public RE::GFxFunctionHandler
{
public:
	static void Install(const RE::DialogueMenu* a_dialogueMenu)
	{
		RE::GFxValue dialogueMenu_mc;
		if (a_dialogueMenu->uiMovie->GetVariable(&dialogueMenu_mc, "_root.DialogueMenu_mc")) {
			auto handler = RE::make_gptr<SkipTextFunctionHandler>();
			RE::GFxValue skipTextOriginal;
			dialogueMenu_mc.GetMember("SkipText", &skipTextOriginal);
			dialogueMenu_mc.SetMember("SkipTextOriginal", skipTextOriginal);

			RE::GFxValue skipTextNew;
			a_dialogueMenu->uiMovie->CreateFunction(&skipTextNew, handler.get());
			dialogueMenu_mc.SetMember("SkipText", skipTextNew);
		}
	}

	void Call(Params& a_params) override
	{
		RE::GFxValue topicList;
		if (!a_params.thisPtr->GetMember("TopicList", &topicList))
			return;

		RE::GFxValue entriesA;
		if (!topicList.GetMember("EntriesA", &entriesA))
			return;

		if (entriesA.GetArraySize() == 0) {
			// this can happen when talking to an NPC that's bleeding out (e.g. after a brawl), preventing any dialogue progress except for skipping/cancelling
			a_params.thisPtr->Invoke("SkipTextOriginal", nullptr, a_params.args, a_params.argCount);
		}
	}
};

class OnCancelPressFunctionHandler final : public RE::GFxFunctionHandler
{
public:
	static void Install(const RE::DialogueMenu* a_dialogueMenu)
	{
		RE::GFxValue dialogueMenu_mc;
		if (a_dialogueMenu->uiMovie->GetVariable(&dialogueMenu_mc, "_root.DialogueMenu_mc")) {
			auto handler = RE::make_gptr<OnCancelPressFunctionHandler>();
			RE::GFxValue onCancelPressNew;
			a_dialogueMenu->uiMovie->CreateFunction(&onCancelPressNew, handler.get());
			dialogueMenu_mc.SetMember("onCancelPress", onCancelPressNew);
		}
	}

	void Call(Params& a_params) override
	{
		a_params.thisPtr->Invoke("StartHideMenu", nullptr, nullptr, 0);
	}
};

class MenuOpenCloseEventSink final : public RE::BSTEventSink<RE::MenuOpenCloseEvent>
{
public:
	static void Install()
	{
		const auto singleton = GetSingleton();
		if (const auto ui = RE::UI::GetSingleton()) {
			ui->AddEventSink(singleton);
		}
	}

	static MenuOpenCloseEventSink* GetSingleton()
	{
		static MenuOpenCloseEventSink singleton;
		return &singleton;
	}

	RE::BSEventNotifyControl ProcessEvent(const RE::MenuOpenCloseEvent* a_event, RE::BSTEventSource<RE::MenuOpenCloseEvent>*) override
	{
		if (a_event->menuName == RE::DialogueMenu::MENU_NAME && a_event->opening) {
			const auto dialogueMenu = RE::UI::GetSingleton()->GetMenu<RE::DialogueMenu>().get();
			if (dialogueMenu) {
				SkipTextFunctionHandler::Install(dialogueMenu);
				OnCancelPressFunctionHandler::Install(dialogueMenu);
			}
		}

		return RE::BSEventNotifyControl::kContinue;
	}

	MenuOpenCloseEventSink() = default;

private:
	MenuOpenCloseEventSink(const MenuOpenCloseEventSink&) = delete;
	MenuOpenCloseEventSink(MenuOpenCloseEventSink&&) = delete;
	~MenuOpenCloseEventSink() = default;
	MenuOpenCloseEventSink& operator=(const MenuOpenCloseEventSink&) = delete;
	MenuOpenCloseEventSink& operator=(MenuOpenCloseEventSink&&) = delete;
};

SKSEPluginLoad(const SKSE::LoadInterface* skse)
{
	SKSE::Init(skse);
	MenuOpenCloseEventSink::Install();
	return true;
}