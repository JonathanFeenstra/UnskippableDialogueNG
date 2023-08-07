/*
Copyright (C) 2023 Jonathan Feenstra

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

class DialogueMenuEx final : public RE::DialogueMenu
{
public:
	static void Install()
	{
		REL::Relocation<uintptr_t> vtbl(RE::VTABLE_DialogueMenu[0]);
		_AcceptFn = vtbl.write_vfunc(0x1, &AcceptEx);
	}

	void AcceptEx(CallbackProcessor* a_cbReg)
	{
		_AcceptFn(this, a_cbReg);
		fxDelegate->callbacks.Remove("SkipText");
		a_cbReg->Process("SkipText", [](const RE::FxDelegateArgs&) {
			// Empty
		});
	}

private:
	using AcceptFn = decltype(&DialogueMenu::Accept);

	inline static REL::Relocation<AcceptFn> _AcceptFn;
};

SKSEPluginLoad(const SKSE::LoadInterface* skse)
{
	SKSE::Init(skse);
	DialogueMenuEx::Install();
	return true;
}