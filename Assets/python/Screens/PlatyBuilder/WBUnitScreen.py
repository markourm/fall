from CvPythonExtensions import *
import CvUtil
import ScreenInput
import CvScreenEnums
import WBPromotionScreen
import WBPlayerScreen
import WBTeamScreen
import WBPlotScreen
import WBEventScreen
import WBPlayerUnits
import CvPlatyBuilderScreen
import WBInfoScreen
import Popup
gc = CyGlobalContext()

iChange = 1
iCopyType = 0
iOwnerType = 0
iChangeType = 0
iEditType = 0
iCommandUnitType = 0
iSelectedClass = -2
iCargoType = 1#Magister##bCargo = True
iUnitType = 1#Magister##bUnitType = True

class WBUnitScreen:

	def __init__(self, main):
		self.top = main
		self.iTable_Y = 110
		self.iScriptH = 90

	def interfaceScreen(self, pUnitX):
		screen = CyGInterfaceScreen( "WBUnitScreen", CvScreenEnums.WB_UNIT)
		
		global pUnit
		global pPlot

		pUnit = pUnitX
		pPlot = pUnit.plot()
		iWidth = screen.getXResolution()/5 - 20
		
		screen.setRenderInterfaceOnly(True)
		screen.addPanel( "MainBG", u"", u"", True, False, -10, -10, screen.getXResolution() + 20, screen.getYResolution() + 20, PanelStyles.PANEL_STYLE_MAIN )
		screen.showScreen(PopupStates.POPUPSTATE_IMMEDIATE, False)

		screen.setText("UnitExit", "Background", "<font=4>" + CyTranslator().getText("TXT_KEY_PEDIA_SCREEN_EXIT", ()).upper() + "</font>", CvUtil.FONT_RIGHT_JUSTIFY, screen.getXResolution() - 30, screen.getYResolution() - 42, -0.1, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_CLOSE_SCREEN, -1, -1 )
		
		screen.addDropDownBoxGFC("OwnerType", 20, self.iTable_Y - 90, iWidth, WidgetTypes.WIDGET_GENERAL, -1, -1, FontTypes.GAME_FONT)
		screen.addPullDownString("OwnerType", CyTranslator().getText("TXT_KEY_WB_CITY_ALL", ()), 0, 0, 0 == iOwnerType)
		screen.addPullDownString("OwnerType", CyTranslator().getText("TXT_KEY_WB_OWNER", ()), 1, 1, 1 == iOwnerType)
		screen.addPullDownString("OwnerType", CyTranslator().getText("TXT_KEY_PITBOSS_TEAM", ()), 2, 2, 2 == iOwnerType)

		screen.addDropDownBoxGFC("CopyType", 20, self.iTable_Y - 60, iWidth, WidgetTypes.WIDGET_GENERAL, -1, -1, FontTypes.GAME_FONT)
		screen.addPullDownString("CopyType", CyTranslator().getText("TXT_KEY_WB_CITY_ALL", ()), 0, 0, 0 == iCopyType)
		screen.addPullDownString("CopyType", CyTranslator().getText("TXT_KEY_SPACE_SHIP_SCREEN_TYPE_BUTTON", ()), 1, 1, 1 == iCopyType)
		screen.addPullDownString("CopyType", CyTranslator().getText("TXT_KEY_PEDIA_CATEGORY_UNIT_COMBAT", ()), 2, 2, 2 == iCopyType)
		screen.addPullDownString("CopyType", CyTranslator().getText("TXT_KEY_PEDIA_DOMAIN", ()), 3, 3, 3 == iCopyType)
		screen.addPullDownString("CopyType", CyTranslator().getText("TXT_KEY_WB_GROUP", ()), 4, 4, 4 == iCopyType)

		screen.addDropDownBoxGFC("EditType", 20, self.iTable_Y - 30, iWidth, WidgetTypes.WIDGET_GENERAL, -1, -1, FontTypes.GAME_FONT)
		screen.addPullDownString("EditType", CyTranslator().getText("TXT_KEY_WB_SINGLE_PLOT", ()), 0, 0, iEditType == 0)
		screen.addPullDownString("EditType", CyTranslator().getText("TXT_KEY_WB_AREA_PLOTS", ()), 1, 1, iEditType == 1)
		screen.addPullDownString("EditType", CyTranslator().getText("TXT_KEY_WB_ALL_PLOTS", ()), 2, 2, iEditType == 2)

		screen.addDropDownBoxGFC("CurrentPage", 20, screen.getYResolution() - 42, iWidth, WidgetTypes.WIDGET_GENERAL, -1, -1, FontTypes.GAME_FONT)
		screen.addPullDownString("CurrentPage", CyTranslator().getText("TXT_KEY_WB_UNIT_DATA", ()), 0, 0, True)
		screen.addPullDownString("CurrentPage", CyTranslator().getText("TXT_KEY_PEDIA_CATEGORY_PROMOTION", ()), 1, 1, False)
		screen.addPullDownString("CurrentPage", CyTranslator().getText("TXT_KEY_WB_PLAYER_DATA", ()), 2, 2, False)
		screen.addPullDownString("CurrentPage", CyTranslator().getText("TXT_KEY_WB_TEAM_DATA", ()), 3, 3, False)
		screen.addPullDownString("CurrentPage", CyTranslator().getText("TXT_KEY_WB_PLOT_DATA", ()), 4, 4, False)
		screen.addPullDownString("CurrentPage", CyTranslator().getText("TXT_KEY_CONCEPT_EVENTS", ()), 5, 5, False)
		screen.addPullDownString("CurrentPage", CyTranslator().getText("TXT_KEY_PEDIA_CATEGORY_UNIT", ()) + " + " + CyTranslator().getText("TXT_KEY_CONCEPT_CITIES", ()), 6, 6, False)
		screen.addPullDownString("CurrentPage", CyTranslator().getText("TXT_KEY_INFO_SCREEN", ()), 11, 11, False)

		iX = screen.getXResolution()/5 + 20
		screen.addDropDownBoxGFC("ChangeType", iX, screen.getYResolution() - 42, iWidth, WidgetTypes.WIDGET_GENERAL, -1, -1, FontTypes.GAME_FONT)
		screen.addPullDownString("ChangeType", CyTranslator().getText("TXT_KEY_WB_XLEVEL", ()), 0, 0, 0 == iChangeType)
		screen.addPullDownString("ChangeType", CyTranslator().getText("INTERFACE_PANE_EXPERIENCE", ()), 1, 1, 1 == iChangeType)
		screen.addPullDownString("ChangeType", CyTranslator().getText("TXT_KEY_DEMO_SCREEN_STRENGTH_TEXT", ()), 2, 2, 2 == iChangeType)
		screen.addPullDownString("ChangeType", CyTranslator().getText("TXT_KEY_WB_DAMAGE", ()), 3, 3, 3 == iChangeType)
		screen.addPullDownString("ChangeType", CyTranslator().getText("TXT_KEY_WB_MOVES", ()), 4, 4, 4 == iChangeType)
		screen.addPullDownString("ChangeType", CyTranslator().getText("TXT_KEY_WB_IMMOBILE_TIMER", ()), 5, 5, 5 == iChangeType)
		screen.addPullDownString("ChangeType", CyTranslator().getText("TXT_KEY_WB_PROMOTION_READY", ()), 6, 6, 6 == iChangeType)
		screen.addPullDownString("ChangeType", CyTranslator().getText("TXT_KEY_WB_MADE_ATTACK", ()), 7, 7, 7 == iChangeType)
		screen.addPullDownString("ChangeType", CyTranslator().getText("TXT_KEY_WB_MADE_INTERCEPT", ()), 8, 8, 8 == iChangeType)
		screen.addPullDownString("ChangeType", CyTranslator().getText("TXT_KEY_WB_DIRECTION", ()), 9, 9, 9 == iChangeType)
		screen.addPullDownString("ChangeType", CyTranslator().getText("TXT_KEY_WB_UNIT_AI", ()), 10, 10, 10 == iChangeType)
		screen.addPullDownString("ChangeType", CyTranslator().getText("TXT_KEY_WB_CARGO_SPACE", ()), 11, 11, 11 == iChangeType)
		screen.addPullDownString("ChangeType", CyTranslator().getText("TXT_KEY_WB_SCRIPT_DATA", ()), 12, 12, 12 == iChangeType)
#Magister Start
		screen.addPullDownString("ChangeType", CyTranslator().getText("INTERFACE_PANE_STRENGTH_DEFENSE", ()), 13, 13, 13 == iChangeType)
		screen.addPullDownString("ChangeType", CyTranslator().getText("TXT_KEY_WB_DURATION", ()), 14, 14, 14 == iChangeType)
		screen.addPullDownString("ChangeType", CyTranslator().getText("TXT_KEY_WB_SCRIPT_DATA", ()), 15, 15, 15 == iChangeType)
		screen.addPullDownString("ChangeType", CyTranslator().getText("TXT_KEY_WB_HAS_CAST", ()), 16, 16, 16 == iChangeType)
		screen.addPullDownString("ChangeType", CyTranslator().getText("TXT_KEY_WB_SUMMONER", ()), 17, 17, 17 == iChangeType)
		screen.addPullDownString("ChangeType", CyTranslator().getText("TXT_KEY_WB_IS_PERMANENT_SUMMON", ()), 18, 18, 18 == iChangeType)
		screen.addPullDownString("ChangeType", CyTranslator().getText("TXT_KEY_PROMOTION_AVATAR", ()), 19, 19, 19 == iChangeType)
		screen.addPullDownString("ChangeType", CyTranslator().getText("TXT_KEY_WB_SCENARIO_COUNTER_UNIT", ()), 20, 20, 20 == iChangeType)
#Magister Stop

		iX += iWidth/2#Magister		iX += iWidth
		sText = CyTranslator().getText("[COLOR_SELECTED_TEXT]", ()) + "<font=3b>" + CyTranslator().getText("TXT_KEY_WB_COPY_ALL", (CyTranslator().getText("TXT_KEY_PEDIA_CATEGORY_UNIT", ()),)) + "</color></font>"
		screen.setText("CopyStats", "Background", sText, CvUtil.FONT_CENTER_JUSTIFY, iX, screen.getYResolution() - 72, -0.1, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)#Magister		screen.setText("CopyStats", "Background", sText, CvUtil.FONT_LEFT_JUSTIFY, iX, screen.getYResolution() - 42, -0.1, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
		
		global lUnitAI
		lUnitAI = []
		for i in xrange(UnitAITypes.NUM_UNITAI_TYPES):
			sText = gc.getUnitAIInfo(i).getDescription()
			sList = ""
			while len(sText):
				sText = sText[sText.find("_") +1:]
				sText = sText.lower()
				sText = sText.capitalize()
				if sText.find("_") == -1:
					sList += sText
					break
				else:
					sList += sText[:sText.find("_")] + " "
					sText = sText[sText.find("_") +1:]
			lUnitAI.append([sList, i])
		lUnitAI.sort()

##Magister Start
		global lUnitReligion
		lUnitReligion = []
		sText = CyTranslator().getText("TXT_KEY_NO_RELIGION",())
		lUnitReligion.append([sText, -1])
		for i in xrange(gc.getNumReligionInfos()):
			sText = gc.getReligionInfo(i).getDescription()
			lUnitReligion.append([sText, i])

		global lSpells
		lSpells = []
		for i in xrange(gc.getNumSpellInfos()):
			sText = gc.getSpellInfo(i).getDescription()
			lSpells.append([sText, i])
##Magister Stop

		self.placeStats()
		self.placeDirection()
		self.sortUnits()
		self.placeUnitType()
		self.placeCargo()
		self.placeScript()

	def placeMap(self):
		screen = CyGInterfaceScreen("WBUnitScreen", CvScreenEnums.WB_UNIT)
		iWidth = screen.getXResolution()/5 - 40
		iY = self.iTable_Y
		iMapHeight = min((screen.getYResolution()/2 - self.iScriptH - 35 - iY), iWidth * 2/3)
		iMapWidth = iMapHeight * 3/2
		screen.addPlotGraphicGFC("PlotView", screen.getXResolution()/2 - iMapWidth/2, iY, iMapWidth, iMapHeight, pUnit.plot(), 250, True, WidgetTypes.WIDGET_GENERAL, -1, -1)

	def placeDirection(self):
		screen = CyGInterfaceScreen("WBUnitScreen", CvScreenEnums.WB_UNIT)
		iY = screen.getYResolution()/2
		screen.setLabel("UnitDirectionText", "Background", "<font=3b>" + CyTranslator().getText("TXT_KEY_WB_DIRECTION", ()) + "</font>", CvUtil.FONT_CENTER_JUSTIFY, screen.getXResolution()/2, iY, -0.1, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)

		iY += 20#Magister##		iY += 30
		iHeight = 3*24 + 2
		screen.addTableControlGFC("WBUnitDirection", 3, screen.getXResolution()/2 - iHeight/2, iY, iHeight, iHeight, False, True, 24, 24, TableStyles.TABLE_STYLE_EMPTY)
		for i in xrange(3):
			screen.setTableColumnHeader("WBUnitDirection", i, "", iHeight/3)
			screen.appendTableRow("WBUnitDirection")
		screen.setTableText("WBUnitDirection", 1,0 , " ", "", WidgetTypes.WIDGET_PYTHON, 1030, 0, CvUtil.FONT_CENTER_JUSTIFY)
		screen.setTableText("WBUnitDirection", 2,0 , " ", "", WidgetTypes.WIDGET_PYTHON, 1030, 1, CvUtil.FONT_CENTER_JUSTIFY)
		screen.setTableText("WBUnitDirection", 2,1 , " ", "", WidgetTypes.WIDGET_PYTHON, 1030, 2, CvUtil.FONT_CENTER_JUSTIFY)
		screen.setTableText("WBUnitDirection", 2,2 , " ", "", WidgetTypes.WIDGET_PYTHON, 1030, 3, CvUtil.FONT_CENTER_JUSTIFY)
		screen.setTableText("WBUnitDirection", 1,2 , " ", "", WidgetTypes.WIDGET_PYTHON, 1030, 4, CvUtil.FONT_CENTER_JUSTIFY)
		screen.setTableText("WBUnitDirection", 0,2 , " ", "", WidgetTypes.WIDGET_PYTHON, 1030, 5, CvUtil.FONT_CENTER_JUSTIFY)
		screen.setTableText("WBUnitDirection", 0,1 , " ", "", WidgetTypes.WIDGET_PYTHON, 1030, 6, CvUtil.FONT_CENTER_JUSTIFY)
		screen.setTableText("WBUnitDirection", 0,0 , " ", "", WidgetTypes.WIDGET_PYTHON, 1030, 7, CvUtil.FONT_CENTER_JUSTIFY)
		screen.setTableText("WBUnitDirection", 1,1 , " ", "", WidgetTypes.WIDGET_PYTHON, 1030, -1, CvUtil.FONT_CENTER_JUSTIFY)

		if pUnit.getFacingDirection() == DirectionTypes.DIRECTION_NORTH:
			screen.setTableText("WBUnitDirection", 1,0 , "", pUnit.getButton(), WidgetTypes.WIDGET_PYTHON, 1030, 0, CvUtil.FONT_CENTER_JUSTIFY)
		elif pUnit.getFacingDirection() == DirectionTypes.DIRECTION_NORTHEAST:
			screen.setTableText("WBUnitDirection", 2,0 , "", pUnit.getButton(), WidgetTypes.WIDGET_PYTHON, 1030, 1, CvUtil.FONT_CENTER_JUSTIFY)
		elif pUnit.getFacingDirection() == DirectionTypes.DIRECTION_EAST:
			screen.setTableText("WBUnitDirection", 2,1 , "", pUnit.getButton(), WidgetTypes.WIDGET_PYTHON, 1030, 2, CvUtil.FONT_CENTER_JUSTIFY)
		elif pUnit.getFacingDirection() == DirectionTypes.DIRECTION_SOUTHEAST:
			screen.setTableText("WBUnitDirection", 2,2 , "", pUnit.getButton(), WidgetTypes.WIDGET_PYTHON, 1030, 3, CvUtil.FONT_CENTER_JUSTIFY)
		elif pUnit.getFacingDirection() == DirectionTypes.DIRECTION_SOUTH:
			screen.setTableText("WBUnitDirection", 1,2 , "", pUnit.getButton(), WidgetTypes.WIDGET_PYTHON, 1030, 4, CvUtil.FONT_CENTER_JUSTIFY)
		elif pUnit.getFacingDirection() == DirectionTypes.DIRECTION_SOUTHWEST:
			screen.setTableText("WBUnitDirection", 0,2 , "", pUnit.getButton(), WidgetTypes.WIDGET_PYTHON, 1030, 5, CvUtil.FONT_CENTER_JUSTIFY)
		elif pUnit.getFacingDirection() == DirectionTypes.DIRECTION_WEST:
			screen.setTableText("WBUnitDirection", 0,1 , "", pUnit.getButton(), WidgetTypes.WIDGET_PYTHON, 1030, 6, CvUtil.FONT_CENTER_JUSTIFY)
		elif pUnit.getFacingDirection() == DirectionTypes.DIRECTION_NORTHWEST:
			screen.setTableText("WBUnitDirection", 0,0 , "", pUnit.getButton(), WidgetTypes.WIDGET_PYTHON, 1030, 7, CvUtil.FONT_CENTER_JUSTIFY)
		elif pUnit.getFacingDirection() == DirectionTypes.NO_DIRECTION:
			screen.setTableText("WBUnitDirection", 1,1 , "", pUnit.getButton(), WidgetTypes.WIDGET_PYTHON, 1030, -1, CvUtil.FONT_CENTER_JUSTIFY)

		iWidth = screen.getXResolution()/5 - 40
		iX = screen.getXResolution()/2 - iWidth/2
		iY += iHeight
		screen.addDropDownBoxGFC("UnitAIType", iX, iY, iWidth, WidgetTypes.WIDGET_GENERAL, -1, -1, FontTypes.GAME_FONT)
		for item in lUnitAI:
			screen.addPullDownString("UnitAIType", item[0], item[1], item[1], item[1] == pUnit.getUnitAIType())

#Magister Start
		iY += 30
		iRel = pUnit.getReligion()
		screen.addDropDownBoxGFC("UnitReligionType", iX+30, iY, iWidth-30, WidgetTypes.WIDGET_GENERAL, -1, -1, FontTypes.GAME_FONT)
		for item in lUnitReligion:
			screen.addPullDownString("UnitReligionType", item[0], item[1], item[1], item[1] == iRel)
		button = CyArtFileMgr().getInterfaceArtInfo("INTERFACE_BUTTONS_CANCEL").getPath()
		if iRel > -1:
			button = gc.getReligionInfo(iRel).getButton()
		screen.addDDSGFC("ReligionPic", button, iX, iY, 30, 30, WidgetTypes.WIDGET_HELP_RELIGION, iRel, -1)
#Magister Stop

		iY += 30
		sColor = CyTranslator().getText("[COLOR_WARNING_TEXT]", ())
		if pUnit.isPromotionReady():
			sColor = CyTranslator().getText("[COLOR_POSITIVE_TEXT]", ())
		sText = CyTranslator().getText("TXT_KEY_WB_PROMOTION_READY", ())
		screen.setText("PromotionReadyText", "Background", "<font=3>" + sColor + sText + "</color></font>", CvUtil.FONT_CENTER_JUSTIFY, screen.getXResolution()/2, iY + 1, -0.1, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)

		iY += 25#Magister		iY += 30
		sColor = CyTranslator().getText("[COLOR_WARNING_TEXT]", ())
		if pUnit.isMadeAttack():
			sColor = CyTranslator().getText("[COLOR_POSITIVE_TEXT]", ())
		sText = CyTranslator().getText("TXT_KEY_WB_MADE_ATTACK", ())
		screen.setText("MadeAttackText", "Background", "<font=3>" + sColor + sText + "</color></font>", CvUtil.FONT_CENTER_JUSTIFY, screen.getXResolution()/2, iY + 1, -0.1, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)

		iY += 25#Magister		iY += 30
		sColor = CyTranslator().getText("[COLOR_WARNING_TEXT]", ())
		if pUnit.isMadeInterception():
			sColor = CyTranslator().getText("[COLOR_POSITIVE_TEXT]", ())
		sText = CyTranslator().getText("TXT_KEY_WB_MADE_INTERCEPT", ())
		screen.setText("MadeInterceptionText", "Background", "<font=3>" + sColor + sText + "</color></font>", CvUtil.FONT_CENTER_JUSTIFY, screen.getXResolution()/2, iY + 1, -0.1, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)

#Magister Start
		iY += 25#Magister
		sColor = CyTranslator().getText("[COLOR_WARNING_TEXT]", ())
		if pUnit.isHasCasted():
			sColor = CyTranslator().getText("[COLOR_POSITIVE_TEXT]", ())
		sText = CyTranslator().getText("TXT_KEY_WB_HAS_CAST", ())
		screen.setText("HasCasted", "Background", "<font=3>" + sColor + sText + "</color></font>", CvUtil.FONT_CENTER_JUSTIFY, screen.getXResolution()/2, iY + 1, -0.1, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)

		iY += 25#Magister
		sColor = CyTranslator().getText("[COLOR_WARNING_TEXT]", ())
		if pUnit.isImmortal():
			sColor = CyTranslator().getText("[COLOR_POSITIVE_TEXT]", ())
		sText = CyTranslator().getText("TXT_KEY_WB_IS_IMMORTAL", ())
		screen.setText("IsImmortal", "Background", "<font=3>" + sColor + sText + "</color></font>", CvUtil.FONT_CENTER_JUSTIFY, screen.getXResolution()/2, iY + 1, -0.1, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)

		iY += 25#Magister
		sColor = CyTranslator().getText("[COLOR_WARNING_TEXT]", ())
		if pUnit.isAvatarOfCivLeader():
			sColor = CyTranslator().getText("[COLOR_POSITIVE_TEXT]", ())
		sText = CyTranslator().getText("TXT_KEY_PROMOTION_AVATAR", ())
		screen.setText("IsAvatar", "Background", "<font=3>" + sColor + sText + "</color></font>", CvUtil.FONT_CENTER_JUSTIFY, screen.getXResolution()/2, iY + 1, -0.1, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)

		iY += 25#Magister
		sColor = CyTranslator().getText("[COLOR_WARNING_TEXT]", ())
		if pUnit.isPermanentSummon():
			sColor = CyTranslator().getText("[COLOR_POSITIVE_TEXT]", ())
		sText = CyTranslator().getText("TXT_KEY_WB_IS_PERMANENT_SUMMON", ())
		screen.setText("IsPermanentSummon", "Background", "<font=3>" + sColor + sText + "</color></font>", CvUtil.FONT_CENTER_JUSTIFY, screen.getXResolution()/2, iY + 1, -0.1, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)

		if -1 < pUnit.getScenarioCounter() < gc.getNumUnitInfos() and pUnit.getScenarioCounter() != pUnit.getUnitType():
			iY += 25#Magister
			sColor = CyTranslator().getText("[COLOR_WARNING_TEXT]", ())
			sText = CyTranslator().getText("TXT_KEY_WB_SCENARIO_COUNTER_UNIT", ())
			screen.setText("SwitchToAlternateType", "Background", "<font=3>" + sColor + sText + "</color></font>", CvUtil.FONT_CENTER_JUSTIFY, screen.getXResolution()/2, iY + 1, -0.1, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
#Magister Stop

	def placeScript(self):
		screen = CyGInterfaceScreen("WBUnitScreen", CvScreenEnums.WB_UNIT)
		iWidth = screen.getXResolution()/5 - 40
		iX = screen.getXResolution()/2 - iWidth/2
		iY = screen.getYResolution()/2 - self.iScriptH - 35
		sText = CyTranslator().getText("[COLOR_SELECTED_TEXT]", ()) + "<font=3b>" + CyTranslator().getText("TXT_KEY_WB_SCRIPT_DATA", ()) + "</color></font>"
		screen.setText("UnitEditScriptData", "Background", sText, CvUtil.FONT_CENTER_JUSTIFY, screen.getXResolution()/2, iY, -0.1, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
		screen.addPanel( "ScriptPanel", "", "", False, False, iX, iY + 25, iWidth, self.iScriptH, PanelStyles.PANEL_STYLE_IN)
		screen.addMultilineText("GameScriptDataText", pUnit.getScriptData(), iX, iY + 25, iWidth, self.iScriptH, WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)

	def placeStats(self):
		screen = CyGInterfaceScreen("WBUnitScreen", CvScreenEnums.WB_UNIT)
		sText = CyTranslator().getText("[COLOR_SELECTED_TEXT]", ()) + "<font=4b>" + pUnit.getName() + "</color></font>"
		screen.setText("UnitScreenHeader", "Background", sText, CvUtil.FONT_CENTER_JUSTIFY, screen.getXResolution()/2, 20, -0.1, FontTypes.GAME_FONT, WidgetTypes.WIDGET_UNIT_NAME, -1, -1)
		sText = u"<font=3b>%s ID: %d, %s ID: %d</font>" %(CyTranslator().getText("TXT_KEY_WB_UNIT", ()), pUnit.getID(), CyTranslator().getText("TXT_KEY_WB_GROUP", ()), pUnit.getGroupID())
		screen.setLabel("UnitScreenHeaderB", "Background", sText, CvUtil.FONT_CENTER_JUSTIFY, screen.getXResolution()/2, 50, -0.1, FontTypes.GAME_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
		sText = "<font=3b>%s, X: %d, Y: %d</font>" %(CyTranslator().getText("TXT_KEY_WB_LATITUDE",(pPlot.getLatitude(),)), pPlot.getX(), pPlot.getY())
		screen.setLabel("UnitLocation", "Background", sText, CvUtil.FONT_CENTER_JUSTIFY, screen.getXResolution()/2, 70, -0.1, FontTypes.GAME_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)

		iX = screen.getXResolution()/5 + 20
		iY = self.iTable_Y
		iWidth = screen.getXResolution()/5 - 20

		screen.addDropDownBoxGFC("UnitOwner", iX, iY, iWidth, WidgetTypes.WIDGET_GENERAL, -1, -1, FontTypes.GAME_FONT)
		for iPlayerX in xrange(gc.getMAX_PLAYERS()):
			pPlayerX = gc.getPlayer(iPlayerX)
			if pPlayerX.isEverAlive():
				sName = pPlayerX.getName()
				if not pPlayerX.isAlive():
					sName = "*" + sName
				screen.addPullDownString("UnitOwner", sName, iPlayerX, iPlayerX, iPlayerX == pUnit.getOwner())

		iY += 30
		screen.addDropDownBoxGFC("ChangeBy", iX, iY, iWidth, WidgetTypes.WIDGET_GENERAL, -1, -1, FontTypes.GAME_FONT)
		i = 1
		while i < 100001:
			screen.addPullDownString("ChangeBy", "(+/-) " + str(i), i, i, iChange == i)
			if str(i)[0] == "1":
				i *= 5
			else:
				i *= 2

		iY += 30
		screen.addDropDownBoxGFC("CommandUnits", iX, iY, iWidth, WidgetTypes.WIDGET_GENERAL, -1, -1, FontTypes.GAME_FONT)
		screen.addPullDownString("CommandUnits", CyTranslator().getText("TXT_KEY_WB_UNIT", ()), 0, 0, iCommandUnitType == 0)
		screen.addPullDownString("CommandUnits", CyTranslator().getText("TXT_KEY_SPACE_SHIP_SCREEN_TYPE_BUTTON", ()), 1, 1, iCommandUnitType == 1)
		screen.addPullDownString("CommandUnits", CyTranslator().getText("TXT_KEY_PEDIA_CATEGORY_UNIT_COMBAT", ()), 2, 2, iCommandUnitType == 2)
		screen.addPullDownString("CommandUnits", CyTranslator().getText("TXT_KEY_PEDIA_DOMAIN", ()), 3, 3, 3 == iCommandUnitType == 3)
		screen.addPullDownString("CommandUnits", CyTranslator().getText("TXT_KEY_WB_GROUP", ()), 4, 4, iCommandUnitType == 4)
		screen.addPullDownString("CommandUnits", CyTranslator().getText("TXT_KEY_WB_CITY_ALL", ()), 5, 5, iCommandUnitType == 5)

		iY += 30
		screen.addDropDownBoxGFC("Commands", iX, iY, iWidth, WidgetTypes.WIDGET_GENERAL, -1, -1, FontTypes.GAME_FONT)
		screen.addPullDownString("Commands", CyTranslator().getText("TXT_KEY_WB_COMMANDS", ()), 0, 0, True)
		screen.addPullDownString("Commands", CyTranslator().getText("TXT_KEY_OPTIONS_RESET", ()), 1, 1, False)
		screen.addPullDownString("Commands", CyTranslator().getText("TXT_KEY_MISSION_MOVE_TO", ()), 2, 2, False)
		screen.addPullDownString("Commands", CyTranslator().getText("TXT_KEY_MAP_SCRIPT_COPY", ()), 3, 3, False)
		screen.addPullDownString("Commands", CyTranslator().getText("TXT_KEY_ESPIONAGE_DESTROY_UNIT", ()), 4, 4, False)

		iY += 30
		screen.setButtonGFC("UnitLevelPlus", "", "", iX, iY, 24, 24, WidgetTypes.WIDGET_PYTHON, 1030, -1, ButtonStyles.BUTTON_STYLE_CITY_PLUS)
		screen.setButtonGFC("UnitLevelMinus", "", "", iX + 25, iY, 24, 24, WidgetTypes.WIDGET_PYTHON, 1031, -1, ButtonStyles.BUTTON_STYLE_CITY_MINUS)
		sText = CyTranslator().getText("TXT_KEY_WB_XLEVEL", ()) + ": " + str(pUnit.getLevel())
		screen.setLabel("UnitLevelText", "Background", "<font=3>" + sText + "</font>", CvUtil.FONT_LEFT_JUSTIFY, iX + 55, iY + 1, -0.1, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)

		iY += 30
		screen.setButtonGFC("UnitExperiencePlus", "", "", iX, iY, 24, 24, WidgetTypes.WIDGET_PYTHON, 1030, -1, ButtonStyles.BUTTON_STYLE_CITY_PLUS)
		screen.setButtonGFC("UnitExperienceMinus", "", "", iX + 25, iY, 24, 24, WidgetTypes.WIDGET_PYTHON, 1031, -1, ButtonStyles.BUTTON_STYLE_CITY_MINUS)
		sText = CyTranslator().getText("INTERFACE_PANE_EXPERIENCE", ()) + ": " + str(pUnit.getExperience())
		screen.setLabel("UnitExperienceText", "Background", "<font=3>" + sText + "</font>", CvUtil.FONT_LEFT_JUSTIFY, iX + 55, iY + 1, -0.1, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)

		iY += 30
		screen.setButtonGFC("UnitBaseStrPlus", "", "", iX, iY, 24, 24, WidgetTypes.WIDGET_PYTHON, 1030, -1, ButtonStyles.BUTTON_STYLE_CITY_PLUS)
		screen.setButtonGFC("UnitBaseStrMinus", "", "", iX + 25, iY, 24, 24, WidgetTypes.WIDGET_PYTHON, 1031, -1, ButtonStyles.BUTTON_STYLE_CITY_MINUS)
		sText = CyTranslator().getText("INTERFACE_PANE_STRENGTH", ()) + " " + str(pUnit.baseCombatStr()) + CyTranslator().getText("[ICON_STRENGTH]", ())
		screen.setLabel("UnitBaseStrText", "Background", "<font=3>" + sText + "</font>", CvUtil.FONT_LEFT_JUSTIFY, iX + 55, iY + 1, -0.1, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)

#Magister Start
		iY += 30
		screen.setButtonGFC("UnitBaseDefStrPlus", "", "", iX, iY, 24, 24, WidgetTypes.WIDGET_PYTHON, 1030, -1, ButtonStyles.BUTTON_STYLE_CITY_PLUS)
		screen.setButtonGFC("UnitBaseDefStrMinus", "", "", iX + 25, iY, 24, 24, WidgetTypes.WIDGET_PYTHON, 1031, -1, ButtonStyles.BUTTON_STYLE_CITY_MINUS)
		sText = CyTranslator().getText("INTERFACE_PANE_STRENGTH_DEFENSE", ()) + ": " + str(pUnit.baseCombatStrDefense()) + CyTranslator().getText("[ICON_DEFENSE]", ())
		screen.setLabel("UnitBaseDefStrText", "Background", "<font=3>" + sText + "</font>", CvUtil.FONT_LEFT_JUSTIFY, iX + 55, iY + 1, -0.1, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
#Magister Stop

		iY += 30
		sText = CyTranslator().getText("INTERFACE_PANE_AIR_STRENGTH", ()) + " " + str(pUnit.airBaseCombatStr())
		screen.setLabel("UnitAirStrText", "Background", "<font=3>" + sText + "</font>", CvUtil.FONT_LEFT_JUSTIFY, iX + 55, iY + 1, -0.1, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)

		iY += 30
		screen.setButtonGFC("UnitDamagePlus", "", "", iX, iY, 24, 24, WidgetTypes.WIDGET_PYTHON, 1030, -1, ButtonStyles.BUTTON_STYLE_CITY_PLUS)
		screen.setButtonGFC("UnitDamageMinus", "", "", iX + 25, iY, 24, 24, WidgetTypes.WIDGET_PYTHON, 1031, -1, ButtonStyles.BUTTON_STYLE_CITY_MINUS)
		sText = CyTranslator().getText("TXT_KEY_WB_DAMAGE", ()) + ": " + str(pUnit.getDamage())
		screen.setLabel("UnitDamageText", "Background", "<font=3>" + sText + "</font>", CvUtil.FONT_LEFT_JUSTIFY, iX + 55, iY + 1, -0.1, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)

		iY += 30
		screen.setButtonGFC("UnitMovesLeftPlus", "", "", iX, iY, 24, 24, WidgetTypes.WIDGET_PYTHON, 1030, -1, ButtonStyles.BUTTON_STYLE_CITY_PLUS)
		screen.setButtonGFC("UnitMovesLeftMinus", "", "", iX + 25, iY, 24, 24, WidgetTypes.WIDGET_PYTHON, 1031, -1, ButtonStyles.BUTTON_STYLE_CITY_MINUS)
		sText = u"%s: %.2f" %(CyTranslator().getText("TXT_KEY_WB_MOVES", ()), float(pUnit.movesLeft()) / gc.getDefineINT("MOVE_DENOMINATOR")) + CyTranslator().getText("[ICON_MOVES]", ())
		screen.setLabel("UnitMovesLeftText", "Background", "<font=3>" + sText + "</font>", CvUtil.FONT_LEFT_JUSTIFY, iX + 55, iY + 1, -0.1, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)

		iY += 30
		screen.setButtonGFC("UnitImmobilePlus", "", "", iX, iY, 24, 24, WidgetTypes.WIDGET_PYTHON, 1030, -1, ButtonStyles.BUTTON_STYLE_CITY_PLUS)
		screen.setButtonGFC("UnitImmobileMinus", "", "", iX + 25, iY, 24, 24, WidgetTypes.WIDGET_PYTHON, 1031, -1, ButtonStyles.BUTTON_STYLE_CITY_MINUS)
		sText = CyTranslator().getText("TXT_KEY_WB_IMMOBILE_TIMER", ()) + ": " + str(pUnit.getImmobileTimer())
		screen.setLabel("UnitImmobileText", "Background", "<font=3>" + sText + "</font>", CvUtil.FONT_LEFT_JUSTIFY, iX + 55, iY + 1, -0.1, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)

#Magister Start
		iY += 30
		screen.setButtonGFC("UnitDurationPlus", "", "", iX, iY, 24, 24, WidgetTypes.WIDGET_PYTHON, 1030, -1, ButtonStyles.BUTTON_STYLE_CITY_PLUS)
		screen.setButtonGFC("UnitDurationMinus", "", "", iX + 25, iY, 24, 24, WidgetTypes.WIDGET_PYTHON, 1031, -1, ButtonStyles.BUTTON_STYLE_CITY_MINUS)
		sText = CyTranslator().getText("TXT_KEY_WB_DURATION", ()) + ": " + str(pUnit.getDuration())
		screen.setLabel("UnitDurationText",  "Background", "<font=3>" + sText + "</font>", CvUtil.FONT_LEFT_JUSTIFY, iX + 55, iY + 1, -0.1, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
#Magister Stop


	def placeUnitType(self):
		screen = CyGInterfaceScreen("WBUnitScreen", CvScreenEnums.WB_UNIT)

		iX = screen.getXResolution() * 4/5
		iY = self.iTable_Y - 90
		iWidth = screen.getXResolution()/5 - 20

		iY += 30
		screen.addDropDownBoxGFC("LeaderType", iX, iY, iWidth, WidgetTypes.WIDGET_GENERAL, -1, -1, FontTypes.GAME_FONT)
#Magister Start
		screen.addPullDownString("LeaderType", CyTranslator().getText("TXT_KEY_FOREIGN_ADVISOR_LEADER",()), 0, 0, iUnitType == 0)
		screen.addPullDownString("LeaderType", CyTranslator().getText("TXT_KEY_SPACE_SHIP_SCREEN_TYPE_BUTTON",()), 1, 1, iUnitType == 1)
		screen.addPullDownString("LeaderType", CyTranslator().getText("TXT_KEY_WB_SCENARIO_COUNTER_UNIT",()), 2, 2, iUnitType == 2)
#Magister Stop

		iY += 30
		screen.addDropDownBoxGFC("CombatClass", iX, iY, iWidth, WidgetTypes.WIDGET_GENERAL, -1, -1, FontTypes.GAME_FONT)
		screen.addPullDownString("CombatClass", CyTranslator().getText("TXT_KEY_WB_CITY_ALL",()), -2, -2, -2 == iSelectedClass)
		screen.addPullDownString("CombatClass", CyTranslator().getText("TXT_PEDIA_NON_COMBAT",()), -1, -1, -1 == iSelectedClass)
		for iCombatClass in xrange(gc.getNumUnitCombatInfos()):
			screen.addPullDownString("CombatClass", gc.getUnitCombatInfo(iCombatClass).getDescription(), iCombatClass, iCombatClass, iCombatClass == iSelectedClass)
#Magister Start
		screen.addPullDownString("CombatClass", CyTranslator().getText("TXT_KEY_UNLIMITED_UNITS", ()), -3, -3, -3 == iSelectedClass)
		screen.addPullDownString("CombatClass", CyTranslator().getText("TXT_KEY_NATIONAL_UNITS", ()), -4, -4, -4 == iSelectedClass)
		screen.addPullDownString("CombatClass", CyTranslator().getText("TXT_KEY_TEAM_UNITS", ()), -5, -5, -5 == iSelectedClass)
		screen.addPullDownString("CombatClass", CyTranslator().getText("TXT_KEY_WORLD_UNITS", ()), -6, -6, -6 == iSelectedClass)
		screen.addPullDownString("CombatClass", CyTranslator().getText("TXT_KEY_FREE_UNITS", ()), -7, -7, -7 == iSelectedClass)
		screen.addPullDownString("CombatClass", CyTranslator().getText("TXT_KEY_PEDIA_CATEGORY_ITEMS", ()), -8, -8, -8 == iSelectedClass)
		screen.addPullDownString("CombatClass", CyTranslator().getText("TXT_KEY_TIER_#", (1,)), -9, -9, -9 == iSelectedClass)
		screen.addPullDownString("CombatClass", CyTranslator().getText("TXT_KEY_TIER_#", (2,)), -10, -10, -10 == iSelectedClass)
		screen.addPullDownString("CombatClass", CyTranslator().getText("TXT_KEY_TIER_#", (3,)), -11, -11, -11 == iSelectedClass)
		screen.addPullDownString("CombatClass", CyTranslator().getText("TXT_KEY_TIER_#", (4,)), -12, -12, -12 == iSelectedClass)
#Magister Stop

		iY += 30
		iHeight = (screen.getYResolution() - iY - 40) /24 * 24 + 2
		screen.addTableControlGFC("WBUnitType", 1, iX, iY, iWidth, iHeight, False, False, 24, 24, TableStyles.TABLE_STYLE_STANDARD)
		screen.setTableColumnHeader("WBUnitType", 0, "", iWidth)

		lUnitType = []
		for i in xrange(gc.getNumUnitInfos()):
			UnitInfo = gc.getUnitInfo(i)
			iUnitClass = UnitInfo.getUnitClassType ()
##			if UnitInfo.getUnitCombatType() != iSelectedClass and iSelectedClass > -2: continue	#Magister

			if CvPlatyBuilderScreen.bHideInactive:
				iCivilization = gc.getPlayer(pUnit.getOwner()).getCivilizationType()
				if gc.getCivilizationInfo(iCivilization).getCivilizationUnits(UnitInfo.getUnitClassType()) != i: continue
##			lUnitType.append([UnitInfo.getDescription(), i])	#Magister

#Magister Start
			if iSelectedClass > -2:
				if UnitInfo.getUnitCombatType() == iSelectedClass:
					lUnitType.append([UnitInfo.getDescription(), i])
			elif iSelectedClass == -2:
				lUnitType.append([UnitInfo.getDescription(), i])
			elif iSelectedClass == -3:
				if not (isWorldUnitClass(iUnitClass) or isNationalUnitClass(iUnitClass) or isTeamUnitClass(iUnitClass)):
					lUnitType.append([UnitInfo.getDescription(), i])
			elif iSelectedClass == -4:
				if isNationalUnitClass(iUnitClass):
					lUnitType.append([UnitInfo.getDescription(), i])
			elif iSelectedClass == -5:
				if isTeamUnitClass(iUnitClass):
					lUnitType.append([UnitInfo.getDescription(), i])
			elif iSelectedClass == -6:
				if isWorldUnitClass(iUnitClass):
					lUnitType.append([UnitInfo.getDescription(), i])
			elif iSelectedClass == -7:
				if UnitInfo.getPrereqCiv() > -1 or gc.getUnitClassInfo(iUnitClass).getDefaultUnitIndex () != i:
					lUnitType.append([UnitInfo.getDescription(), i])
			elif iSelectedClass == -8:
				if UnitInfo.isObject():
					lUnitType.append([UnitInfo.getDescription(), i])
			elif iSelectedClass == -9:
				if UnitInfo.getTier() == 1:
					lUnitType.append([UnitInfo.getDescription(), i])
			elif iSelectedClass == -10:
				if UnitInfo.getTier() == 2:
					lUnitType.append([UnitInfo.getDescription(), i])
			elif iSelectedClass == -11:
				if UnitInfo.getTier() == 3:
					lUnitType.append([UnitInfo.getDescription(), i])
			elif iSelectedClass == -12:
				if UnitInfo.getTier() == 4:
					lUnitType.append([UnitInfo.getDescription(), i])
#Magister Stop

		lUnitType.sort()

		for item in lUnitType:
			iRow = screen.appendTableRow("WBUnitType")
			sColor = CyTranslator().getText("[COLOR_WARNING_TEXT]", ())
#Magister Start
			if iUnitType == 1:
				if pUnit.getUnitType() == item[1]:
					sColor = CyTranslator().getText("[COLOR_POSITIVE_TEXT]", ())
			elif iUnitType == 0:
				if pUnit.getLeaderUnitType() == item[1]:
					sColor = CyTranslator().getText("[COLOR_POSITIVE_TEXT]", ())
			elif iUnitType == 2:#magister
				if pUnit.getScenarioCounter() == item[1]:
					sColor = CyTranslator().getText("[COLOR_POSITIVE_TEXT]", ())
#Magister Stop
			screen.setTableText("WBUnitType", 0, iRow, "<font=3>" + sColor + item[0] + "</font></color>", gc.getUnitInfo(item[1]).getButton(), WidgetTypes.WIDGET_PYTHON, 8202, item[1], CvUtil.FONT_LEFT_JUSTIFY)

	def sortUnits(self):
		screen = CyGInterfaceScreen("WBUnitScreen", CvScreenEnums.WB_UNIT)
		global lUnits
		lUnits = []
		for iPlayerX in xrange(gc.getMAX_PLAYERS()):
			if iOwnerType == 1 and iPlayerX != pUnit.getOwner(): continue
			if iOwnerType == 2 and iPlayerX != pUnit.getTeam(): continue
			pPlayerX = gc.getPlayer(iPlayerX)
			if pPlayerX.isAlive():
				(loopUnit, iter) = pPlayerX.firstUnit(False)
				while(loopUnit):
					bCopy = True
					if iEditType == 0:
						if loopUnit.getX() != pUnit.getX() or loopUnit.getY() != pUnit.getY():
							bCopy = False
					elif iEditType == 1:
						if loopUnit.plot().getArea() != pUnit.plot().getArea():
							bCopy = False
					if iCopyType == 1:
						if loopUnit.getUnitType() != pUnit.getUnitType():
							bCopy = False
					elif iCopyType == 2:
						if loopUnit.getUnitCombatType() != pUnit.getUnitCombatType():
							bCopy = False
					elif iCopyType == 3:
						if loopUnit.getDomainType() != pUnit.getDomainType():
							bCopy = False
					elif iCopyType == 4:
						if loopUnit.getGroupID() != pUnit.getGroupID():
							bCopy = False
					if bCopy:
						lUnits.append([loopUnit.getOwner(), loopUnit.getID()])
					(loopUnit, iter) = pPlayerX.nextUnit(iter, False)
		lUnits.sort()
		self.placeCurrentUnit()
		
	def placeCurrentUnit(self):
		screen = CyGInterfaceScreen("WBUnitScreen", CvScreenEnums.WB_UNIT)

		iX = 20
		iWidth = screen.getXResolution()/5 - 20
		iHeight = (screen.getYResolution() - self.iTable_Y - 42) /24 * 24 + 2
		screen.addTableControlGFC("WBCurrentUnit", 3, iX, self.iTable_Y, iWidth, iHeight, False, False, 24, 24, TableStyles.TABLE_STYLE_STANDARD)
		screen.setTableColumnHeader("WBCurrentUnit", 0, "", 24)
		screen.setTableColumnHeader("WBCurrentUnit", 1, "", 24)
		screen.setTableColumnHeader("WBCurrentUnit", 2, "", iWidth - 48)

		for i in lUnits:
			pPlayerX = gc.getPlayer(i[0])
			pUnitX = pPlayerX.getUnit(i[1])
			if pUnitX.isNone(): continue
			iRow = screen.appendTableRow("WBCurrentUnit")
			sText = pUnitX.getName()
			if len(pUnitX.getNameNoDesc()):
				sText = pUnitX.getNameNoDesc()
			sColor = CyTranslator().getText("[COLOR_WARNING_TEXT]", ())
			if pUnitX.getOwner() == pUnit.getOwner():
				if pUnitX.getID() == pUnit.getID():
					sColor = CyTranslator().getText("[COLOR_POSITIVE_TEXT]", ())
				elif pUnitX.getGroupID() == pUnit.getGroupID():
					sColor = CyTranslator().getText("[COLOR_YELLOW]", ())
			screen.setTableText("WBCurrentUnit", 2, iRow, "<font=3>" + sColor + sText + "</font></color>", pUnitX.getButton(), WidgetTypes.WIDGET_PYTHON, 8300 + i[0], i[1], CvUtil.FONT_LEFT_JUSTIFY)
			iLeader = pPlayerX.getLeaderType()
			iCiv = pUnitX.getCivilizationType()
			screen.setTableText("WBCurrentUnit", 0, iRow, "", gc.getCivilizationInfo(iCiv).getButton(), WidgetTypes.WIDGET_PYTHON, 7872, iCiv, CvUtil.FONT_LEFT_JUSTIFY )
			screen.setTableText("WBCurrentUnit", 1, iRow, "", gc.getLeaderHeadInfo(iLeader).getButton(), WidgetTypes.WIDGET_PYTHON, 7876, iLeader, CvUtil.FONT_LEFT_JUSTIFY )

	def placeCargo(self):
		screen = CyGInterfaceScreen("WBUnitScreen", CvScreenEnums.WB_UNIT)
		iX = screen.getXResolution()*3/5
		iWidth = screen.getXResolution()/5 - 20
		screen.addDropDownBoxGFC("CargoType", iX, self.iTable_Y - 30, iWidth, WidgetTypes.WIDGET_GENERAL, -1, -1, FontTypes.GAME_FONT)
#Magister Start
		screen.addPullDownString("CargoType", CyTranslator().getText("TXT_KEY_UNIT_TRANSPORT", ()), 0, 0, iCargoType == 0)
		screen.addPullDownString("CargoType", CyTranslator().getText("TXT_KEY_WB_CARGO_SPACE", ()), 1, 1, iCargoType == 1)
		screen.addPullDownString("CargoType", CyTranslator().getText("TXT_KEY_WB_SUMMON", ()), 2, 2, iCargoType == 2)
		screen.addPullDownString("CargoType", CyTranslator().getText("TXT_KEY_WB_SUMMONER", ()), 3, 3, iCargoType == 3)
		screen.addPullDownString("CargoType", CyTranslator().getText("TXT_KEY_WB_SPELL", ()), 4, 4, iCargoType == 4)
#Magister Stop

		iHeight = (screen.getYResolution() - self.iTable_Y - 42) /24 * 24 + 2

		screen.addTableControlGFC("WBCargoUnits", 1, iX, self.iTable_Y, iWidth, iHeight, False, False, 24, 24, TableStyles.TABLE_STYLE_STANDARD)
		screen.setTableColumnHeader("WBCargoUnits", 0, "", iWidth)
		screen.hide("UnitCargoPlus")
		screen.hide("UnitCargoMinus")
		screen.hide("CargoSpaceHeader")

		if iCargoType == 1:#Magister
			screen.setButtonGFC("UnitCargoPlus", "", "", iX, screen.getYResolution() - 42, 24, 24, WidgetTypes.WIDGET_PYTHON, 1030, -1, ButtonStyles.BUTTON_STYLE_CITY_PLUS)
			screen.setButtonGFC("UnitCargoMinus", "", "", iX + 25, screen.getYResolution() - 42, 24, 24, WidgetTypes.WIDGET_PYTHON, 1031, -1, ButtonStyles.BUTTON_STYLE_CITY_MINUS)
			sText = CyTranslator().getText("TXT_KEY_WB_CARGO_SPACE", ()) + " (" + str(pUnit.getCargo()) + "/" + str(pUnit.cargoSpace()) + ")"
			screen.setLabel("CargoSpaceHeader", "Background", "<font=3b>" + sText + "</font>", CvUtil.FONT_LEFT_JUSTIFY, iX + 50, screen.getYResolution() - 41, -0.1, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1 )

			if pUnit.cargoSpace() > 0:
				for i in xrange(pPlot.getNumUnits()):
					pUnitX = pPlot.getUnit(i)
					if pUnitX.isNone(): continue
					if pUnitX.getID() == pUnit.getID(): continue
					if pUnit.domainCargo() > -1:
						if pUnitX.getDomainType() != pUnit.domainCargo(): continue
					if pUnit.specialCargo() > -1:
						if pUnitX.getSpecialUnitType() != pUnit.specialCargo(): continue
					iPlayerX = pUnitX.getOwner()
					if iPlayerX != pUnit.getOwner(): continue
					iRow = screen.appendTableRow("WBCargoUnits")
					sText = pUnitX.getName()
					if len(pUnitX.getNameNoDesc()):
						sText = pUnitX.getNameNoDesc()
					sColor = CyTranslator().getText("[COLOR_WARNING_TEXT]", ())
					if pUnitX.isCargo():
						sColor = CyTranslator().getText("[COLOR_YELLOW]", ())
					if pUnitX.getTransportUnit().getID() == pUnit.getID():
						sColor = CyTranslator().getText("[COLOR_POSITIVE_TEXT]", ())
					screen.setTableText("WBCargoUnits", 0, iRow, "<font=3>" + sColor + sText + "</font></color>", pUnitX.getButton(), WidgetTypes.WIDGET_PYTHON, 8300 + iPlayerX, pUnitX.getID(), CvUtil.FONT_LEFT_JUSTIFY)
		elif iCargoType == 0:#Magister
			for i in xrange(pPlot.getNumUnits()):
				pUnitX = pPlot.getUnit(i)
				if pUnitX.isNone(): continue
				if pUnitX.getID() == pUnit.getID(): continue
				if pUnitX.cargoSpace() < 1: continue
				if pUnitX.domainCargo() > -1:
					if pUnit.getDomainType() != pUnitX.domainCargo(): continue
				if pUnitX.specialCargo() > -1:
					if pUnit.getSpecialUnitType() != pUnitX.specialCargo(): continue
				iPlayerX = pUnitX.getOwner()
				if iPlayerX != pUnit.getOwner(): continue
				iRow = screen.appendTableRow("WBCargoUnits")
				sText = pUnitX.getName()
				if len(pUnitX.getNameNoDesc()):
					sText = pUnitX.getNameNoDesc()
				sText += " (" + str(pUnitX.getCargo()) + "/" + str(pUnitX.cargoSpace()) + ")"
				sColor = CyTranslator().getText("[COLOR_YELLOW]", ())
				if pUnitX.isFull():
					sColor = CyTranslator().getText("[COLOR_WARNING_TEXT]", ())
				if pUnit.getTransportUnit().getID() == pUnitX.getID():
					sColor = CyTranslator().getText("[COLOR_POSITIVE_TEXT]", ())
				screen.setTableText("WBCargoUnits", 0, iRow, "<font=3>" + sColor + sText + "</font></color>", pUnitX.getButton(), WidgetTypes.WIDGET_PYTHON, 8300 + iPlayerX, pUnitX.getID(), CvUtil.FONT_LEFT_JUSTIFY)
#Magister
		elif iCargoType == 2:
			iPlayerX = pUnit.getOwner()
			pPlayer = gc.getPlayer(iPlayerX)
			unitID = pUnit.getID()
			(pUnitX, iter) = pPlayer.firstUnit(False)
			while(pUnitX):
				if not (pUnitX.isNone() or pUnitX.isDead() or pUnitX.getID() == unitID):
					iRow = screen.appendTableRow("WBCargoUnits")
					sText = pUnitX.getName()
					if len(pUnitX.getNameNoDesc()):
						sText = pUnitX.getNameNoDesc()
					sColor =  CyTranslator().getText("[COLOR_WARNING_TEXT]", ())
					if pUnitX.getSummoner() == unitID:
						sColor = CyTranslator().getText("[COLOR_POSITIVE_TEXT]", ())
					screen.setTableText("WBCargoUnits", 0, iRow, "<font=3>" + sColor + sText + "</font></color>", pUnitX.getButton(), WidgetTypes.WIDGET_PYTHON, 8300 + iPlayerX, pUnitX.getID(), CvUtil.FONT_LEFT_JUSTIFY)
				(pUnitX, iter) = pPlayer.nextUnit(iter, False)

		elif iCargoType == 3:
			iPlayerX = pUnit.getOwner()
			pPlayer = gc.getPlayer(iPlayerX)
			unitID = pUnit.getID()
			summonerID = pUnit.getSummoner()
			(pUnitX, iter) = pPlayer.firstUnit(False)
			while(pUnitX):
				if not (pUnitX.isNone() or pUnitX.isDead() or pUnitX.getID() == unitID):
					iRow = screen.appendTableRow("WBCargoUnits")
					sText = pUnitX.getName()
					if len(pUnitX.getNameNoDesc()):
						sText = pUnitX.getNameNoDesc()
					sColor =  CyTranslator().getText("[COLOR_WARNING_TEXT]", ())
					if summonerID == pUnitX.getID():
						sColor = CyTranslator().getText("[COLOR_POSITIVE_TEXT]", ())
					screen.setTableText("WBCargoUnits", 0, iRow, "<font=3>" + sColor + sText + "</font></color>", pUnitX.getButton(), WidgetTypes.WIDGET_PYTHON, 8300 + iPlayerX, pUnitX.getID(), CvUtil.FONT_LEFT_JUSTIFY)
				(pUnitX, iter) = pPlayer.nextUnit(iter, False)

		elif iCargoType == 4:
			for item in lSpells:
				if pUnit.canCast(item[1], False):
					sColor = CyTranslator().getText("[COLOR_YELLOW]", ())
					iRow = screen.appendTableRow("WBCargoUnits")
					ItemInfo = gc.getSpellInfo(item[1])
					screen.setTableText("WBCargoUnits", 0, iRow, "<font=3>" + sColor + item[0] + "</font></color>", ItemInfo.getButton(), WidgetTypes.WIDGET_PYTHON, 9001, item[1], CvUtil.FONT_LEFT_JUSTIFY)

#Magister Stop

	def handleInput(self, inputClass):
		screen = CyGInterfaceScreen( "WBUnitScreen", CvScreenEnums.WB_UNIT)
		global iChange
		global iChangeType
		global iEditType
		global iCopyType
		global iOwnerType
		global iCommandUnitType
		global iSelectedClass
		global iCargoType#Magister##		global bCargo
		global iUnitType#Magister##		global bUnitType

		if inputClass.getFunctionName() == "ChangeBy":
			iChange = screen.getPullDownData("ChangeBy", screen.getSelectedPullDownID("ChangeBy"))

		elif inputClass.getFunctionName() == "CurrentPage":
			iIndex = screen.getPullDownData("CurrentPage", screen.getSelectedPullDownID("CurrentPage"))
			if iIndex == 1:
				WBPromotionScreen.WBPromotionScreen().interfaceScreen(pUnit)
			elif iIndex == 2:
				WBPlayerScreen.WBPlayerScreen().interfaceScreen(pUnit.getOwner())
			elif iIndex == 3:
				WBTeamScreen.WBTeamScreen().interfaceScreen(pUnit.getTeam())
			elif iIndex == 4:
				WBPlotScreen.WBPlotScreen().interfaceScreen(pPlot)
			elif iIndex == 5:
				WBEventScreen.WBEventScreen().interfaceScreen(pPlot)
			elif iIndex == 6:
				WBPlayerUnits.WBPlayerUnits().interfaceScreen(pUnit.getOwner())
			elif iIndex == 11:
				WBInfoScreen.WBInfoScreen().interfaceScreen(pUnit.getOwner())

		elif inputClass.getFunctionName() == "CargoType":
			iCargoType = screen.getSelectedPullDownID("CargoType")#Magister			bCargo = not bCargo
			self.placeCargo()

		elif inputClass.getFunctionName() == "ChangeType":
			iChangeType = screen.getPullDownData("ChangeType", screen.getSelectedPullDownID("ChangeType"))

		elif inputClass.getFunctionName() == "OwnerType":
			iOwnerType = screen.getPullDownData("OwnerType", screen.getSelectedPullDownID("OwnerType"))
			self.sortUnits()

		elif inputClass.getFunctionName() == "EditType":
			iEditType = screen.getPullDownData("OwnerType", screen.getSelectedPullDownID("EditType"))
			self.sortUnits()

		elif inputClass.getFunctionName() == "CopyType":
			iCopyType = screen.getPullDownData("CopyType", screen.getSelectedPullDownID("CopyType"))
			self.sortUnits()

		elif inputClass.getFunctionName() == "UnitScreenHeader":
			popup = Popup.PyPopup(CvUtil.EventEditUnitName, EventContextTypes.EVENTCONTEXT_ALL)
			popup.setUserData((pUnit.getID(), pUnit.getOwner()))
			popup.setBodyString(CyTranslator().getText("TXT_KEY_RENAME_UNIT", ()))
			popup.createEditBox(pUnit.getNameNoDesc())
			popup.setEditBoxMaxCharCount(25)
			popup.launch()

		elif inputClass.getFunctionName() == "WBCurrentUnit":
			iPlayer = inputClass.getData1() - 8300
			self.interfaceScreen(gc.getPlayer(iPlayer).getUnit(inputClass.getData2()))

		elif inputClass.getFunctionName() == "UnitOwner":
			self.changeOwner(screen.getPullDownData("UnitOwner", screen.getSelectedPullDownID("UnitOwner")))

		elif inputClass.getFunctionName() == "LeaderType":
			iUnitType = screen.getSelectedPullDownID("LeaderType")#Magister			bUnitType = not bUnitType
			self.placeUnitType()

		elif inputClass.getFunctionName() == "CombatClass":
			iSelectedClass = screen.getPullDownData("CombatClass", screen.getSelectedPullDownID("CombatClass"))
			self.placeUnitType()

		elif inputClass.getFunctionName() == "WBUnitType":
			self.changeUnitType(pUnit, inputClass.getData2(), iUnitType)#Magister			self.changeUnitType(pUnit, inputClass.getData2(), bUnitType)

		elif inputClass.getFunctionName().find("UnitLevel") > -1:
			if inputClass.getData1() == 1030:
				pUnit.setLevel(pUnit.getLevel() + iChange)
			elif inputClass.getData1() == 1031:
				pUnit.setLevel(max(0, pUnit.getLevel() - iChange))
			self.placeStats()

		elif inputClass.getFunctionName().find("UnitExperience") > -1:
			if inputClass.getData1() == 1030:
				pUnit.changeExperience(iChange, -1, False, False, False)
			elif inputClass.getData1() == 1031:
				pUnit.changeExperience(- min(iChange, pUnit.getExperience()), -1, False, False, False)
			self.placeStats()

		elif inputClass.getFunctionName().find("UnitBaseStr") > -1:
			if inputClass.getData1() == 1030:
				pUnit.setBaseCombatStr(pUnit.baseCombatStr() + iChange)
			elif inputClass.getData1() == 1031:
				pUnit.setBaseCombatStr(max(0, pUnit.baseCombatStr() - iChange))
			self.placeStats()

		elif inputClass.getFunctionName().find("UnitDamage") > -1:
			if inputClass.getData1() == 1030:
				pUnit.changeDamage(iChange, -1)
			elif inputClass.getData1() == 1031:
				pUnit.changeDamage(- min(iChange, pUnit.getDamage()), -1)
			self.placeStats()

		elif inputClass.getFunctionName().find("UnitMovesLeft") > -1:
			if inputClass.getData1() == 1030:
				pUnit.changeMoves(- iChange * gc.getDefineINT("MOVE_DENOMINATOR"))
			elif inputClass.getData1() == 1031:
				pUnit.changeMoves(min(iChange * gc.getDefineINT("MOVE_DENOMINATOR"), pUnit.movesLeft()))
			self.placeStats()

		elif inputClass.getFunctionName().find("UnitImmobile") > -1:
			if inputClass.getData1() == 1030:
				pUnit.setImmobileTimer(pUnit.getImmobileTimer() + iChange)
			elif inputClass.getData1() == 1031:
				pUnit.setImmobileTimer(max(0, pUnit.getImmobileTimer() - iChange))
			self.placeStats()

		elif inputClass.getFunctionName().find("UnitCargo") > -1:
			if inputClass.getData1() == 1030:
				pUnit.changeCargoSpace(iChange)
			elif inputClass.getData1() == 1031:
				pUnit.changeCargoSpace(- min(iChange, pUnit.cargoSpace()))
			self.placeCargo()

		elif inputClass.getFunctionName() == "WBCargoUnits":
			if iCargoType == 1:#Magister
				iPlayerX = inputClass.getData1() - 8300
				pUnitX = gc.getPlayer(iPlayerX).getUnit(inputClass.getData2())
				if pUnitX.getTransportUnit().getID() == pUnit.getID():
					pUnitX.setTransportUnit(CyUnit())
				else:
					if not pUnit.isFull():
						pUnitX.setTransportUnit(pUnit)
				self.interfaceScreen(pUnit)
			elif iCargoType == 0:#Magister
				iPlayerX = inputClass.getData1() - 8300
				pUnitX = gc.getPlayer(iPlayerX).getUnit(inputClass.getData2())
				if pUnit.getTransportUnit().getID() == pUnitX.getID():
					pUnit.setTransportUnit(CyUnit())
				else:
					if not pUnitX.isFull():
						pUnit.setTransportUnit(pUnitX)
				self.interfaceScreen(pUnit)

#Magister Start
			elif iCargoType == 3:
				iPlayerX = inputClass.getData1() - 8300
				pUnitX = gc.getPlayer(iPlayerX).getUnit(inputClass.getData2())
				if pUnit.getSummoner() == pUnitX.getID():
					pUnit.setSummoner(-1)
				else:
					pUnit.setSummoner(pUnitX.getID())
				self.interfaceScreen(pUnit)

			elif iCargoType == 2:
				iPlayerX = inputClass.getData1() - 8300
				pUnitX = gc.getPlayer(iPlayerX).getUnit(inputClass.getData2())
				if pUnitX.getSummoner() == pUnit.getID():
					pUnitX.setSummoner(-1)
				else:
					pUnitX.setSummoner(pUnit.getID())
				self.interfaceScreen(pUnit)

			elif iCargoType == 4:
				pUnit.cast(inputClass.getData2())
				if pUnit == None or  pUnit is None or pUnit.isDead() or pUnit.isDelayedDeath():
					screen.hideScreen()#if the spell kills the caster, leaving the screen open causes problems
					return
				else:
					self.interfaceScreen(pUnit)
#Magister Stop

		elif inputClass.getFunctionName() == "PromotionReadyText":
			pUnit.setPromotionReady(not pUnit.isPromotionReady())
			self.placeDirection()

		elif inputClass.getFunctionName() == "MadeAttackText":
			pUnit.setMadeAttack(not pUnit.isMadeAttack())
			self.placeDirection()

		elif inputClass.getFunctionName() == "MadeInterceptionText":
			pUnit.setMadeInterception(not pUnit.isMadeInterception())
			self.placeDirection()

#Magister Start
		elif inputClass.getFunctionName().find("UnitBaseDefStr") > -1:
			if inputClass.getData1() == 1030:
				pUnit.setBaseCombatStrDefense(pUnit.baseCombatStrDefense() + iChange)
			elif inputClass.getData1() == 1031:
				pUnit.setBaseCombatStrDefense(max(0, pUnit.baseCombatStrDefense() - iChange))
			self.placeStats()

		elif inputClass.getFunctionName().find("UnitDuration") > -1:
			if inputClass.getData1() == 1030:
				pUnit.setDuration(pUnit.getDuration() + iChange)
			elif inputClass.getData1() == 1031:
				pUnit.setDuration(max(0, pUnit.getDuration() - iChange))
			self.placeStats()

		elif inputClass.getFunctionName() == "UnitReligionType":
			pUnit.setReligion(screen.getPullDownData("UnitReligionType", screen.getSelectedPullDownID("UnitReligionType")))
			self.placeDirection()

		elif inputClass.getFunctionName() == "HasCasted":
			pUnit.setHasCasted(not pUnit.isHasCasted())
			self.placeDirection()
			if iCargoType == 4:
				self.placeCargo()

		elif inputClass.getFunctionName() == "IsImmortal":
			if pUnit.isImmortal():
				while pUnit.isImmortal():
					pUnit.changeImmortal(-1)
			else:
				pUnit.changeImmortal(1)
			self.placeDirection()

		elif inputClass.getFunctionName() == "IsAvatar":
			pUnit.setAvatarOfCivLeader(not pUnit.isAvatarOfCivLeader())
			self.placeDirection()

		elif inputClass.getFunctionName() == "IsPermanentSummon":
			pUnit.setPermanentSummon(not pUnit.isPermanentSummon())
			self.placeDirection()

		elif inputClass.getFunctionName() == "SwitchToAlternateType":
			self.changeUnitType(pUnit, pUnit.getScenarioCounter(), 1)
			self.placeDirection()
#Magister Stop

		if inputClass.getFunctionName() == "UnitAIType":
			pUnit.setUnitAIType(screen.getPullDownData("UnitAIType", screen.getSelectedPullDownID("UnitAIType")))

		elif inputClass.getFunctionName() == "WBUnitDirection":
			self.changeDirection(inputClass.getData2(), pUnit)
			self.placeDirection()

		elif inputClass.getFunctionName() == "UnitEditScriptData":
			popup = Popup.PyPopup(3333, EventContextTypes.EVENTCONTEXT_ALL)
			popup.setHeaderString(CyTranslator().getText("TXT_KEY_WB_SCRIPT", ()))
			popup.setUserData((pUnit.getOwner(), pUnit.getID()))
			popup.createEditBox(pUnit.getScriptData())
			popup.launch()
			return

		elif inputClass.getFunctionName() == "CopyStats":
			self.handleCopyAll()

		elif inputClass.getFunctionName() == "CommandUnits":
			iCommandUnitType = screen.getPullDownData("CommandUnits", screen.getSelectedPullDownID("CommandUnits"))

		elif inputClass.getFunctionName() == "Commands":
			iIndex = screen.getPullDownData("Commands", screen.getSelectedPullDownID("Commands"))
			lUnits = []
			if iCommandUnitType == 0:
				lUnits.append(pUnit)
			else:
				for i in xrange(pPlot.getNumUnits()):
					pUnitX = pPlot.getUnit(i)
					if pUnitX.isNone(): continue
					if pUnitX.getOwner() != pUnit.getOwner(): continue
					if iCommandUnitType == 1:
						if pUnitX.getUnitType() != pUnit.getUnitType(): continue
					elif iCommandUnitType == 2:
						if pUnitX.getUnitCombatType() != pUnit.getUnitCombatType(): continue
					elif iCommandUnitType == 3:
						if pUnitX.getDomainType() != pUnit.getDomainType(): continue
					elif iCommandUnitType == 4:
						if pUnitX.getGroupID() != pUnit.getGroupID(): continue
					lUnits.append(pUnitX)
			bRefresh = False
			for pUnitX in lUnits:
				bRefresh = self.doAllCommands(pUnitX, iIndex)
			screen.hideScreen()
			if bRefresh and pPlot.getNumUnits() > 0:
				self.interfaceScreen(pPlot.getUnit(0))
		return 1

	def doAllCommands(self, pUnitX, iIndex):
		if iIndex == 1:
			Info = gc.getUnitInfo(pUnit.getUnitType())
			pUnitX.setBaseCombatStr(Info.getCombat())
			pUnitX.setDamage(0, -1)
			pUnitX.setMoves(0)
			pUnitX.setImmobileTimer(0)
			pUnitX.setPromotionReady(False)
			pUnitX.setMadeAttack(False)
			pUnitX.setMadeInterception(False)
#Magister Start
			pUnitX.setBaseCombatStrDefense(Info.getCombatDefense())
			pUnitX.setDuration(0)
			pUnitX.setHasCasted(False)
			pUnitX.setReligion(Info.getReligionType())
			pUnitX.changeImmortal(-1)
##			pUnitX.setSummoner(-1)
##			pUnitX.setPermanentSummon(False)
##			pUnitX.setAvatarOfCivLeader(False)
#Magister Stop

			self.changeDirection(DirectionTypes.DIRECTION_SOUTH, pUnitX)
			pUnitX.setUnitAIType(Info.getDefaultUnitAIType())
			pUnitX.changeCargoSpace(Info.getCargoSpace() - pUnitX.cargoSpace())
			pUnitX.setScriptData("")
			return True
		elif iIndex == 2:
			self.top.m_lMoveUnit.append(pUnitX.getID())
			self.top.iPlayerAddMode = "MoveUnit"
			self.top.m_iCurrentPlayer = pUnitX.getOwner()
			return False
		elif iIndex == 3:
			for i in xrange(iChange + 1):
				pNewUnit = gc.getPlayer(pUnitX.getOwner()).initUnit(pUnitX.getUnitType(), pUnitX.getX(), pUnitX.getY(), UnitAITypes.NO_UNITAI, DirectionTypes.NO_DIRECTION)
				pNewUnit.convert(pUnitX)
				pNewUnit.setBaseCombatStr(pUnitX.baseCombatStr())
				pNewUnit.changeCargoSpace(pUnitX.cargoSpace() - pNewUnit.cargoSpace())
				pNewUnit.setImmobileTimer(pUnitX.getImmobileTimer())
				pNewUnit.setScriptData(pUnitX.getScriptData())
#Magister Start
				pNewUnit.setDuration(pUnitX.getDuration())
				pNewUnit.setHasCasted(pUnitX.isHasCasted())
				pNewUnit.setSummoner(pUnitX.getSummoner())
				pNewUnit.setPermanentSummon(pUnitX.isPermanentSummon())
				pNewUnit.setReligion(pUnitX.getReligion())
				pNewUnit.setBaseCombatStrDefense(pUnitX.baseCombatStrDefense())
				pNewUnit.setUnitArtStyleType(pUnitX.getUnitArtStyleType())
#Magister Stop
			pUnitX.kill(False, -1)
			return True
		elif iIndex == 4:
			pUnitX.kill(False, -1)
			return (pPlot.getNumUnits() > 0)
		return False

	def changeDirection(self, iNewDirection, pUnitX):
		if iNewDirection == -1: return
		iOldDirection = pUnitX.getFacingDirection()
		if iNewDirection == iOldDirection: return
		if iOldDirection > iNewDirection:
			for i in xrange(iOldDirection - iNewDirection):
				pUnitX.rotateFacingDirectionCounterClockwise()
		else:
			for i in xrange(iNewDirection - iOldDirection):
				pUnitX.rotateFacingDirectionClockwise()

	def changeOwner(self, iPlayer):
		pNewUnit = gc.getPlayer(iPlayer).initUnit(pUnit.getUnitType(), pUnit.getX(), pUnit.getY(), UnitAITypes.NO_UNITAI, DirectionTypes.NO_DIRECTION)
##Magister Start
		##When duplicating Adventurers and other great people, I don't want the double to get the free promotion that the named ones get
		for iProm in range(gc.getNumPromotionInfos()):
			if pNewUnit.isHasPromotion(iProm):
				pNewUnit.setHasPromotion(iProm, False)
##Magister Stop
		pNewUnit.convert(pUnit)
		pNewUnit.setBaseCombatStr(pUnit.baseCombatStr())
		pNewUnit.changeCargoSpace(pUnit.cargoSpace() - pNewUnit.cargoSpace())
		pNewUnit.setImmobileTimer(pUnit.getImmobileTimer())
		pNewUnit.setScriptData(pUnit.getScriptData())
##Magister Start
		pNewUnit.setDuration(pUnit.getDuration())
		pNewUnit.setBaseCombatStrDefense(pUnit.baseCombatStrDefense())
		pNewUnit.setReligion(pUnit.getReligion())
		pNewUnit.setSummoner(pUnit.getSummoner())
		pNewUnit.setHasCasted(pUnit.isHasCasted())
		pNewUnit.setScenarioCounter(pUnit.getScenarioCounter())
##Magister Stop
		pUnit.kill(False, -1)
		self.interfaceScreen(pNewUnit)

	def changeUnitType(self, pUnit, iUnitType, iListType):##Magister	def changeUnitType(self, pUnit, iUnitType, bUnitType):
		if iListType == 1:##Magister 		if bUnitType:
			pNewUnit = gc.getPlayer(pUnit.getOwner()).initUnit(iUnitType, pUnit.getX(), pUnit.getY(), UnitAITypes.NO_UNITAI, DirectionTypes.NO_DIRECTION)
##Magister Start
			pNewUnit.setImmobileTimer(pUnit.getImmobileTimer())
			pNewUnit.setSummoner(pUnit.getSummoner())
			pNewUnitID = pNewUnit.getID()
			summonerID = pUnit.getID()
			pPlayer = gc.getPlayer(pUnit.getOwner())
			(loopUnit, iter) = pPlayer.firstUnit(False)
			while(loopUnit):
				if not loopUnit.isDead(): #is the unit alive and valid?
					if loopUnit.getSummoner() == summonerID:#is it a summon of pUnit?
						loopUnit.setSummoner(pNewUnitID)
				(loopUnit, iter) = pPlayer.nextUnit(iter, False)
##Magister Stop
			pNewUnit.convert(pUnit)
			pNewUnit.setScriptData("PlatyUnit" + pUnit.getScriptData())
			pUnit.kill(False, -1)
			for i in xrange(pPlot.getNumUnits()):
				pUnitX = pPlot.getUnit(i)
				if pUnitX.getScriptData().find("PlatyUnit") == 0:
					pUnitX.setScriptData(pUnitX.getScriptData()[9:])
					break
			self.interfaceScreen(pUnitX)
		elif iListType == 0:#Magister		else:
			if pUnit.getLeaderUnitType() == iUnitType:
				pUnit.setLeaderUnitType(-1)
			else:
				pUnit.setLeaderUnitType(iUnitType)
			self.interfaceScreen(pUnit)
#Magister Start
		elif iListType == 2:
			if pUnit.getScenarioCounter() == iUnitType:
				pUnit.setScenarioCounter(-1)
			else:
				pUnit.setScenarioCounter(iUnitType)
			self.interfaceScreen(pUnit)
#Magister Stop

	def handleCopyAll(self):
		for i in lUnits:
			loopUnit = gc.getPlayer(i[0]).getUnit(i[1])
			if loopUnit.isNone(): continue
			if iChangeType == 0:
				loopUnit.setLevel(pUnit.getLevel())
			elif iChangeType == 1:
				loopUnit.setExperience(pUnit.getExperience(), -1)
			elif iChangeType == 2:
				loopUnit.setBaseCombatStr(pUnit.baseCombatStr())
			elif iChangeType == 3:
				loopUnit.setDamage(pUnit.getDamage(), -1)
			elif iChangeType == 4:
				loopUnit.setMoves(pUnit.getMoves())
			elif iChangeType == 5:
				loopUnit.setImmobileTimer(pUnit.getImmobileTimer())
			elif iChangeType == 6:
				loopUnit.setPromotionReady(pUnit.isPromotionReady())
			elif iChangeType == 7:
				loopUnit.setMadeAttack(pUnit.isMadeAttack())
			elif iChangeType == 8:
				loopUnit.setMadeInterception(pUnit.isMadeInterception())
			elif iChangeType == 9:
				self.changeDirection(pUnit.getFacingDirection(), loopUnit)
			elif iChangeType == 10:
				loopUnit.setUnitAIType(pUnit.getUnitAIType())
			elif iChangeType == 11:
				loopUnit.changeCargoSpace(pUnit.cargoSpace() - loopUnit.cargoSpace())
			elif iChangeType == 12:
				loopUnit.setScriptData(pUnit.getScriptData())

#Magister Start
			elif iChangeType == 13:
				loopUnit.setBaseCombatStrDefense(pUnit.baseCombatStrDefense())
			elif iChangeType == 14:
				loopUnit.setDuration(pUnit.getDuration())
			elif iChangeType == 15:
				loopUnit.setReligion(pUnit.getReligion())
			elif iChangeType == 16:
				loopUnit.setHasCasted(pUnit.isHasCasted())
			elif iChangeType == 17:
				loopUnit.setSummoner(pUnit.getSummoner())
			elif iChangeType == 18:
				loopUnit.setPermanentSummon(pUnit.isPermanentSummon())
			elif iChangeType == 19:
				loopUnit.setAvatarOfCivLeader(pUnit.isAvatarOfCivLeader())
			elif iChangeType == 20:
				loopUnit.setScenarioCounter(pUnit.getScenarioCounter())
#Magister Stop


	def update(self, fDelta):
		self.placeMap()
		return 1