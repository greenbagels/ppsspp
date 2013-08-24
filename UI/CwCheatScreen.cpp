// Copyright (c) 2012- PPSSPP Project.

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, version 2.0 or later versions.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License 2.0 for more details.

// A copy of the GPL 2.0 should have been included with the program.
// If not, see http://www.gnu.org/licenses/

// Official git repository and contact information can be found at
// https://github.com/hrydgard/ppsspp and http://www.ppsspp.org/.

#include "android/app-android.h"
#include "base/logging.h"

#include "gfx_es2/glsl_program.h"
#include "gfx_es2/gl_state.h"
#include "gfx_es2/fbo.h"

#include "input/input_state.h"
#include "ui/ui.h"
#include "i18n/i18n.h"

#include "Common/KeyMap.h"

#include "Core/Config.h"
#include "Core/CoreTiming.h"
#include "Core/CoreParameter.h"
#include "Core/Core.h"
#include "Core/Host.h"
#include "Core/System.h"
#include "GPU/GPUState.h"
#include "GPU/GPUInterface.h"
#include "Core/HLE/sceCtrl.h"
#include "Core/HLE/sceDisplay.h"
#include "Core/Debugger/SymbolMap.h"

#include "UI/OnScreenDisplay.h"
#include "UI/ui_atlas.h"
#include "UI/GamepadEmu.h"
#include "UI/UIShader.h"

#include "UI/MainScreen.h"
#include "UI/EmuScreen.h"
#include "UI/GameInfoCache.h"
#include "UI/MiscScreens.h"
#include "UI/CwCheatScreen.h"


extern void DrawBackground(float alpha);
static CWCheatEngine *cheatEngine2;

std::vector<std::string> CwCheatScreen::CreateCodeList() {
	std::vector<std::string> cheatList, formattedList;
	cheatEngine2 = new CWCheatEngine();
	cheatList = cheatEngine2->GetCodesList();

	for (size_t i = 0; i < cheatList.size(); i++) {
		if (cheatList[i].substr(0, 3) == "_C1") {
			formattedList.push_back(cheatList[i].substr(3));
			enableCheat[i] = true;
		}
		if (cheatList[i].substr(0, 3) == "_C0") {
			formattedList.push_back(cheatList[i].substr(3));
			enableCheat[i] = false;
		}

	}

	return formattedList;
}
void CwCheatScreen::CreateViews() {
	using namespace UI;
	std::vector<std::string> formattedList;
	I18NCategory *k = GetI18NCategory("CwCheats");
	formattedList = CreateCodeList();
	root_ = new LinearLayout(ORIENT_HORIZONTAL);

	LinearLayout *leftColumn = new LinearLayout(ORIENT_VERTICAL, new LinearLayoutParams(200, FILL_PARENT));
	leftColumn->Add(new Choice(k->T("Back")))->OnClick.Handle<UIScreen>(this, &UIScreen::OnBack);


	ScrollView *rightScroll = new ScrollView(ORIENT_VERTICAL, new LinearLayoutParams(1.0f));
	rightScroll->SetScrollToTop(false);
	LinearLayout *rightColumn = new LinearLayout(ORIENT_VERTICAL, new LinearLayoutParams(1.0f));
	rightScroll->Add(rightColumn);
	
	root_->Add(leftColumn);
	root_->Add(rightScroll);
	rightColumn->Add(new ItemHeader(k->T("Cheats")));
	for (size_t i = 0; i < formattedList.size(); i++) {
		const char * name = formattedList[i].c_str();
		rightColumn->Add(new CheckBox(&enableCheat[i], k->T(name)))->OnClick.Handle(this, &CwCheatScreen::OnCheckBox);;
		}
}
UI::EventReturn CwCheatScreen::OnCheckBox(UI::EventParams &params) {
	return UI::EVENT_DONE;
}