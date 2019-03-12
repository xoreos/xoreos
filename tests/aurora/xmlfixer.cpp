/* xoreos - A reimplementation of BioWare's Aurora engine
 *
 * xoreos is the legal property of its developers, whose names
 * can be found in the AUTHORS file distributed with this source
 * distribution.
 *
 * xoreos is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * xoreos is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with xoreos. If not, see <http://www.gnu.org/licenses/>.
 */

/** @file
 *  Unit test for the NWN2 XML validation fix
 */

#include "gtest/gtest.h"

#include "src/common/util.h"
#include "src/common/error.h"
#include "src/common/memreadstream.h"

#include "src/aurora/xmlfixer.h"

// Invalid NWN2 XML
static const char *kDataInvalid =
	"abc<?xml version=\"1.0\" encoding=\"NWN2UI\">\n"
	"<!-- Copyright 2006 Obsidian Entertainment, Inc. -->\n"
	"\n"
	"\t<!--\n"
	"\t\t-- double dash\n"
	"\t-->\n"
	"\n"
	"<UIScene name=\"FOOBAR\" fadeout=\"0.3\" fadein=\"0.3\" x=0 y=0 fullscreen=true\n"
	"\t width=SCREEN_WIDTH height=SCREEN_HEIGHT\n"
	"\t OnAdd=UIScene_OnAdd_SetUpServerOptions( \n"
	"\t OnRemove=UIScene_OnRemove_SaveServerOptions()\n"
	"\t priority=\"SCENE_FE_FULLSCREEN\" \n"
	"\t /> <!-- Appended comment -->\n"
	"\n"
	"\t\t<UIText name=\"IDENTIFY_TEXT\" width=PARENT_WIDTH height=PARENT_HEIGHT align=center valign=middle uppercase=truefontfamily=\"Default\" style=\"bold\" />\n"
	"\n"
	"\t<UIText name=\"PreHorizonB\" strref=\"181357\"\" x=5 y=368 width=90\" height=16 valign=\"middle align=LEFT fontfamily=\"Special_Font\" style=\"1\"  />\n"
	"\n"
	"\t</UIButton>\n"
	"\t<UIButton name=\"CAMERA_ZOOM_OUT\" x=\"0\" y=\"0\" width=\"35\" height=\"34\" buttontype=\"radio\" groupid=\"2\" groupmemberid=\"1\" DefaultToolTip=\"181294\"\n"
	"\t\t\tOnSelected=UIButton_Input_Move3DCamera(\"TEMP_CHARACTER_SCENE\",\"PLAYER_CREATURE\",\"SET_POSITION\",\"-1.25\",\"0.0\",\"1.3\",\"-0.90\",\"4.0\",\"1.1\",\"0.5\")\n"
	"\t\t\tOnToolTip=UIObject_Tooltip_DisplayObject(OBJECT_X,OBJECT_Y,SCREEN_TOOLTIP_2,ALIGN_NONE,ALIGN_NONE,0,0,ALIGN_LEFT\") >\n"
	"\n"
	"<!--\n"
	"(W) for ?whisper?\n"
	"-->\n"
	"\n"
	"</UIButton>\n"
	"<UIButton name=\"LOCAL SAY\" buttontype=radio groupid=1 groupmemberid=1 \n"
	"\tOnSelected=UIButton_Input_SetChatMode(0) style=\"CHAT_MODE_BUTTON\"    text=\"L\"\n"
	"\tOnToolTip=UIObject_Tooltip_DisplayTooltipStringRef(184736,\"OBJECT_X\",\"OBJECT_Y\",\"SCREEN_TOOLTIP_2\") />\n"
	"\n"
	"\t<UIButton name=\"DUNGEON MASTER\" buttontype=\"radio\" groupid=\"1\" groupmemberid=\"4\" \n"
	"\t\tOnSelected=UIButton_Input_SetChatMode(3) style=\"CHAT_MODE_BUTTON\"  text=\"D\"\n"
	"\t\tOnToolTip=UIObject_Tooltip_DisplayTooltipStringRef(184739,\"OBJECT_X\",OBJECT_Y,\"SCREEN_TOOLTIP_2\") />\n"
	"\n";

// Valid XML
static const char *kDataValid =
	"<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
	"<Root>\n"
	"<UIScene name=\"FOOBAR\" fadeout=\"0.3\" fadein=\"0.3\" x=\"0\" y=\"0\" fullscreen=\"true\" width=\"SCREEN_WIDTH\" height=\"SCREEN_HEIGHT\" OnAdd=\"UIScene_OnAdd_SetUpServerOptions()\" OnRemove=\"UIScene_OnRemove_SaveServerOptions()\" priority=\"SCENE_FE_FULLSCREEN\" />\n"
	"<UIText name=\"IDENTIFY_TEXT\" width=\"PARENT_WIDTH\" height=\"PARENT_HEIGHT\" align=\"center\" valign=\"middle\" uppercase=\"true\" fontfamily=\"Default\" style=\"bold\" />\n"
	"<UIText name=\"PreHorizonB\" strref=\"181357\" x=\"5\" y=\"368\" width=\"90\" height=\"16\" valign=\"middle\" align=\"LEFT\" fontfamily=\"Special_Font\" style=\"1\" />\n"
	"<!-- </UIButton>-->\n"
	"<UIButton name=\"CAMERA_ZOOM_OUT\" x=\"0\" y=\"0\" width=\"35\" height=\"34\" buttontype=\"radio\" groupid=\"2\" groupmemberid=\"1\" DefaultToolTip=\"181294\" OnSelected=\"UIButton_Input_Move3DCamera(&quot;TEMP_CHARACTER_SCENE&quot;,&quot;PLAYER_CREATURE&quot;,&quot;SET_POSITION&quot;,&quot;-1.25&quot;,&quot;0.0&quot;,&quot;1.3&quot;,&quot;-0.90&quot;,&quot;4.0&quot;,&quot;1.1&quot;,&quot;0.5&quot;)\" OnToolTip=\"UIObject_Tooltip_DisplayObject(&quot;OBJECT_X&quot;,&quot;OBJECT_Y&quot;,&quot;SCREEN_TOOLTIP_2&quot;,&quot;ALIGN_NONE&quot;,&quot;ALIGN_NONE&quot;,&quot;0&quot;,&quot;0&quot;,&quot;ALIGN_LEFT&quot;)\" >\n"
	"</UIButton>\n"
	"<UIButton name=\"LOCAL SAY\" buttontype=\"radio\" groupid=\"1\" groupmemberid=\"1\" OnSelected=\"UIButton_Input_SetChatMode(&quot;0&quot;)\" style=\"CHAT_MODE_BUTTON\" text=\"L\" OnToolTip=\"UIObject_Tooltip_DisplayTooltipStringRef(&quot;184736&quot;,&quot;OBJECT_X&quot;,&quot;OBJECT_Y&quot;,&quot;SCREEN_TOOLTIP_2&quot;)\" />\n"
	"<UIButton name=\"DUNGEON MASTER\" buttontype=\"radio\" groupid=\"1\" groupmemberid=\"4\" OnSelected=\"UIButton_Input_SetChatMode(&quot;3&quot;)\" style=\"CHAT_MODE_BUTTON\" text=\"D\" OnToolTip=\"UIObject_Tooltip_DisplayTooltipStringRef(&quot;184739&quot;,&quot;OBJECT_X&quot;,&quot;OBJECT_Y&quot;,&quot;SCREEN_TOOLTIP_2&quot;)\" />\n"
	"</Root>\n";

GTEST_TEST(xmlFixer, fixXMLStream) {
	Common::SeekableReadStream *invalid = new Common::MemoryReadStream(kDataInvalid);

	Aurora::XMLFixer converter;
	Common::SeekableReadStream *valid = converter.fixXMLStream(*invalid);
	ASSERT_EQ(valid->size(), strlen(kDataValid));

	for (size_t i = 0; i < strlen(kDataValid); i++) {
		EXPECT_EQ(valid->readByte(), kDataValid[i]) << "At index " << i;
	}

	delete invalid;
	delete valid;
}

