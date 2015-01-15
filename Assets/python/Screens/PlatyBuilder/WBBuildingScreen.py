from CvPythonExtensions import *
import CvUtil
import ScreenInput
import CvScreenEnums
import WBCityDataScreen
import WBCityEditScreen
import WBPlayerScreen
import WBTeamScreen
import WBPlotScreen
import WBEventScreen
import WBPlayerUnits
import WBReligionScreen
import WBCorporationScreen
import WBInfoScreen
import CvPlatyBuilderScreen
import CvEventManager
gc = CyGlobalContext()

iChangeType = 2
iSelectedClass = 0
bApplyAll = False

iBuildingFilter  = 0#Magister

class WBBuildingScreen:

	def interfaceScreen(self, pCityX):
		screen = CyGInterfaceScreen("WBBuildingScreen", CvScreenEnums.WB_BUILDING)
		global pCity
		global iPlayer

		pCity = pCityX
		iPlayer = pCity.getOwner()

		screen.setRenderInterfaceOnly(True)
		screen.addPanel("MainBG", u"", u"", True, False, -10, -10, screen.getXResolution() + 20, screen.getYResolution() + 20, PanelStyles.PANEL_STYLE_MAIN )
		screen.showScreen(PopupStates.POPUPSTATE_IMMEDIATE, False)
		
		screen.setLabel("BuildingHeader", "Background", u"<font=4b>" + CyTranslator().getText("TXT_KEY_PEDIA_CATEGORY_BUILDING", ()) + "</font>", CvUtil.FONT_CENTER_JUSTIFY, screen.getXResolution() * 5/8 - 10, screen.getYResolution()/2, -0.1, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
		screen.setLabel("WonderHeader", "Background", u"<font=4b>" + CyTranslator().getText("TXT_KEY_CONCEPT_WONDERS", ()) + "</font>", CvUtil.FONT_CENTER_JUSTIFY, screen.getXResolution() * 5/8 - 10, 20, -0.1, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
		sText = CyTranslator().getText("[COLOR_SELECTED_TEXT]", ()) + "<font=3b>" + CyTranslator().getText("TXT_KEY_WB_GRANT_AVAILABLE", ()) + "</color></font>"
		screen.setText("BuildingAvailable", "Background", sText, CvUtil.FONT_CENTER_JUSTIFY, screen.getXResolution() * 5/8 - 10, screen.getYResolution()/2 + 30, -0.1, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
		screen.setText("WonderAvailable", "Background", sText, CvUtil.FONT_CENTER_JUSTIFY, screen.getXResolution() * 5/8 - 10, 50, -0.1, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
		screen.setText("WBBuildingExit", "Background", "<font=4>" + CyTranslator().getText("TXT_KEY_PEDIA_SCREEN_EXIT", ()).upper() + "</font>", CvUtil.FONT_RIGHT_JUSTIFY, screen.getXResolution() - 30, screen.getYResolution() - 42, -0.1, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_CLOSE_SCREEN, -1, -1 )

		iHeight = (screen.getYResolution() - 42 - 50) / 24 * 24 + 2
		iWidth = screen.getXResolution()/4 - 40
		screen.addTableControlGFC( "CurrentCity", 1, 20, 80, iWidth, iHeight, False, False, 24, 24, TableStyles.TABLE_STYLE_STANDARD )
		screen.setTableColumnHeader("CurrentCity", 0, "", iWidth)

		pPlayer = gc.getPlayer(iPlayer)
		(loopCity, iter) = pPlayer.firstCity(False)
		while(loopCity):
			iRow = screen.appendTableRow("CurrentCity")
			sColor = CyTranslator().getText("[COLOR_WARNING_TEXT]", ())
			if loopCity.getID() == pCity.getID():
				sColor = CyTranslator().getText("[COLOR_POSITIVE_TEXT]", ())
			screen.setTableText("CurrentCity", 0, iRow, "<font=3>" + sColor + loopCity.getName() + "</font></color>", gc.getCivilizationInfo(pCity.getCivilizationType()).getButton(), WidgetTypes.WIDGET_PYTHON, 7200 + iPlayer, loopCity.getID(), CvUtil.FONT_LEFT_JUSTIFY )
			(loopCity, iter) = pPlayer.nextCity(iter, False)

#Magister Start
		screen.addDropDownBoxGFC("BuildingClass", screen.getXResolution()/4,  screen.getYResolution()/2, 150, WidgetTypes.WIDGET_GENERAL, -1, -1, FontTypes.GAME_FONT)
		screen.addPullDownString("BuildingClass", CyTranslator().getText("TXT_KEY_PEDIA_CATEGORY_BUILDING", ()), 0, 0, iBuildingFilter == 0)
		screen.addPullDownString("BuildingClass", CyTranslator().getText("TXT_KEY_CIV_BUILDINGS", ()), 1, 1, iBuildingFilter == 1)
		screen.addPullDownString("BuildingClass", CyTranslator().getText("TXT_KEY_REL_BUILDINGS", ()), 2, 2, iBuildingFilter == 2)
		screen.addPullDownString("BuildingClass", CyTranslator().getText("TXT_KEY_SPELL_BUILDINGS", ()), 3, 3, iBuildingFilter == 3)
		screen.addPullDownString("BuildingClass", CyTranslator().getText("TXT_KEY_PEDIA_CATEGORY_ITEMS", ()), 4, 4, iBuildingFilter == 4)
		screen.addPullDownString("BuildingClass", CyTranslator().getText("TXT_KEY_WB_SHOW_HIDDEN", ()), 5, 5, iBuildingFilter == 5)
		screen.addPullDownString("BuildingClass", CyTranslator().getText("TXT_KEY_ALL", ()), 6, 6, iBuildingFilter == 6)
#Magister Stop

		screen.addDropDownBoxGFC("WonderClass", screen.getXResolution()/4, 50, 150, WidgetTypes.WIDGET_GENERAL, -1, -1, FontTypes.GAME_FONT)
		screen.addPullDownString("WonderClass", CyTranslator().getText("TXT_KEY_WB_CITY_ALL", ()), 0, 0, iSelectedClass == 0)
		screen.addPullDownString("WonderClass", CyTranslator().getText("TXT_KEY_PEDIA_NATIONAL_WONDER", ()), 1, 1, iSelectedClass == 1)
		screen.addPullDownString("WonderClass", CyTranslator().getText("TXT_KEY_PEDIA_TEAM_WONDER", ()), 2, 2, iSelectedClass == 2)
		screen.addPullDownString("WonderClass", CyTranslator().getText("TXT_KEY_PEDIA_WORLD_WONDER", ()), 3, 3, iSelectedClass == 3)
		screen.addPullDownString("WonderClass", CyTranslator().getText("TXT_KEY_WB_SHRINE", ()), 4, 4, iSelectedClass == 4)#Magister

		screen.addDropDownBoxGFC("CurrentPage", 20, screen.getYResolution() - 42, iWidth, WidgetTypes.WIDGET_GENERAL, -1, -1, FontTypes.GAME_FONT)
		screen.addPullDownString("CurrentPage", CyTranslator().getText("TXT_KEY_WB_CITY_DATA", ()), 0, 0, False)
		screen.addPullDownString("CurrentPage", CyTranslator().getText("TXT_KEY_WB_CITY_DATA2", ()), 1, 1, False)
		screen.addPullDownString("CurrentPage", CyTranslator().getText("TXT_KEY_PEDIA_CATEGORY_BUILDING", ()), 2, 2, True)
		screen.addPullDownString("CurrentPage", CyTranslator().getText("TXT_KEY_WB_PLAYER_DATA", ()), 3, 3, False)
		screen.addPullDownString("CurrentPage", CyTranslator().getText("TXT_KEY_WB_TEAM_DATA", ()), 4, 4, False)
		screen.addPullDownString("CurrentPage", CyTranslator().getText("TXT_KEY_PEDIA_CATEGORY_RELIGION", ()), 8, 8, False)
		screen.addPullDownString("CurrentPage", CyTranslator().getText("TXT_KEY_CONCEPT_CORPORATIONS", ()), 9, 9, False)
		screen.addPullDownString("CurrentPage", CyTranslator().getText("TXT_KEY_PEDIA_CATEGORY_UNIT", ()) + " + " + CyTranslator().getText("TXT_KEY_CONCEPT_CITIES", ()), 5, 5, False)
		screen.addPullDownString("CurrentPage", CyTranslator().getText("TXT_KEY_WB_PLOT_DATA", ()), 6, 6, False)
		screen.addPullDownString("CurrentPage", CyTranslator().getText("TXT_KEY_CONCEPT_EVENTS", ()), 7, 7, False)
		screen.addPullDownString("CurrentPage", CyTranslator().getText("TXT_KEY_INFO_SCREEN", ()), 11, 11, False)

		sText = "<font=3b>" + CyTranslator().getText("TXT_KEY_WB_COPY_ALL", (CyTranslator().getText("TXT_KEY_CONCEPT_CITIES", ()),)) + "</font>"
		sColor = CyTranslator().getText("[COLOR_WARNING_TEXT]", ())
		if bApplyAll:
			sColor = CyTranslator().getText("[COLOR_POSITIVE_TEXT]", ())
		screen.setText("ApplyAll", "Background", sColor + sText + "</color>", CvUtil.FONT_LEFT_JUSTIFY, screen.getXResolution()/4, screen.getYResolution()/2 + 30, -0.1, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
		
		screen.addDropDownBoxGFC("ChangeType", screen.getXResolution() - 170, 20, 150, WidgetTypes.WIDGET_GENERAL, -1, -1, FontTypes.GAME_FONT)
		screen.addPullDownString("ChangeType", CyTranslator().getText("TXT_KEY_WB_MODIFY", ("",)), 2, 2, 2 == iChangeType)
		screen.addPullDownString("ChangeType", CyTranslator().getText("TXT_KEY_WB_CITY_ADD", ()), 1, 1, 1 == iChangeType)
		screen.addPullDownString("ChangeType", CyTranslator().getText("TXT_KEY_WB_CITY_REMOVE", ()), 0, 0, 0 == iChangeType)

		sText = CyTranslator().getText("[COLOR_SELECTED_TEXT]", ()) + "<font=4b>" + CyTranslator().getText("TXT_KEY_WB_CITY_ALL", ()) + " (+/-)</color></font>"
		screen.setText("BuildingAll", "Background", sText, CvUtil.FONT_RIGHT_JUSTIFY, screen.getXResolution() - 20, screen.getYResolution()/2 + 30, -0.1, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
		screen.setText("WonderAll", "Background", sText, CvUtil.FONT_RIGHT_JUSTIFY, screen.getXResolution() - 20, 50, -0.1, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)

		self.sortBuildings()

	def sortBuildings(self):
		global lBuilding
		global lNational
		global lTeam
		global lWorld
		global lWonder
		global lAll
		global lStandard
		global lUnique
		global lSpell
		global lReligion
		global lEquipment
		global lHidden

		global lShrines

		lStandard = []
		lBuilding = []
		lNational = []
		lTeam = []
		lWorld = []
		lWonder = []
		lAll = []
		lUnique = []
		lSpell = []
		lReligion = []
		lEquipment = []
		lHidden = []
		lShrines = []
		for i in xrange(gc.getNumBuildingInfos()):
			BuildingInfo = gc.getBuildingInfo(i)
			iBuildingClass = BuildingInfo.getBuildingClassType()

			if CvPlatyBuilderScreen.bHideInactive:
				if gc.getCivilizationInfo(pCity.getCivilizationType()).getCivilizationBuildings(iBuildingClass) != i: continue
			sDesc = BuildingInfo.getDescription()
			lAll.append([sDesc, i])
			if BuildingInfo.isGraphicalOnly():
				lHidden.append([sDesc, i])
			else:
				if isLimitedWonderClass(iBuildingClass):
					lWonder.append([sDesc, i])

					if BuildingInfo.getHolyCity() != -1:
						lShrines.append([sDesc, i])
					else:
						if isNationalWonderClass(iBuildingClass):
							lNational.append([sDesc, i])
						if isTeamWonderClass(iBuildingClass):
							lTeam.append([sDesc, i])
						if isWorldWonderClass(iBuildingClass):
							lWorld.append([sDesc, i])
				elif BuildingInfo.getProductionCost() > -1:
					lStandard.append([sDesc, i])
				if BuildingInfo.getPrereqCiv() != -1:
					lUnique.append([sDesc, i])
				if BuildingInfo.getReligionType() != -1:
					lReligion.append([sDesc, i])
				if BuildingInfo.isRequiresCaster():
					lSpell.append([sDesc, i])
				if BuildingInfo.isEquipment():
					lEquipment.append([sDesc, i])
		lNational.sort()
		lTeam.sort()
		lWorld.sort()
		lBuilding.sort()
		lWonder.sort()
		lUnique.sort()
		lSpell.sort()
		lReligion.sort()
		lEquipment.sort()
		lHidden.sort()
		self.placeBuildings()
		self.placeWonders()

	def placeBuildings(self):
		screen = CyGInterfaceScreen( "WBBuildingScreen", CvScreenEnums.WB_BUILDING)

#Magister Start
		if iBuildingFilter == 0:
			lBuilding = lStandard
		elif iBuildingFilter == 1:
			lBuilding = lUnique
		elif iBuildingFilter == 2:
			lBuilding = lReligion
		elif iBuildingFilter == 3:
			lBuilding = lSpell
		elif iBuildingFilter == 4:
			lBuilding = lEquipment
		elif iBuildingFilter == 5:
			lBuilding = lHidden
		elif iBuildingFilter == 6:
			lBuilding = lAll
#Magister Stop

		iWidth = screen.getXResolution() *3/4 - 20
		iMaxRows = (screen.getYResolution()/2 - 102) / 24
		nColumns = max(1, min(iWidth/180, (len(lBuilding) + iMaxRows - 1)/iMaxRows))
		iHeight = iMaxRows * 24 + 2
		screen.addTableControlGFC("WBBuilding", nColumns, screen.getXResolution()/4, screen.getYResolution()/2 + 60, iWidth, iHeight, False, False, 24, 24, TableStyles.TABLE_STYLE_STANDARD )
		for i in xrange(nColumns):
			screen.setTableColumnHeader( "WBBuilding", i, "", iWidth/nColumns)

		nRows = len(lBuilding)/ nColumns
		if len(lBuilding) % nColumns:
			nRows += 1
		for i in xrange(nRows):
			screen.appendTableRow("WBBuilding")

		for iCount in xrange(len(lBuilding)):
			item = lBuilding[iCount]
			iRow = iCount % nRows
			iColumn = iCount / nRows
			ItemInfo = gc.getBuildingInfo(item[1])
			sColor = CyTranslator().getText("[COLOR_WARNING_TEXT]", ())
			if pCity.getNumRealBuilding(item[1]):
				sColor = CyTranslator().getText("[COLOR_POSITIVE_TEXT]", ())
			elif pCity.isHasBuilding(item[1]):
				sColor = CyTranslator().getText("[COLOR_YELLOW]", ())
			screen.setTableText("WBBuilding", iColumn, iRow, "<font=3>" + sColor + item[0] + "</font></color>", ItemInfo.getButton(), WidgetTypes.WIDGET_HELP_BUILDING, item[1], 1, CvUtil.FONT_LEFT_JUSTIFY )

	def placeWonders(self):
		screen = CyGInterfaceScreen( "WBBuildingScreen", CvScreenEnums.WB_BUILDING)
		if iSelectedClass == 0:
			lWonders = lWonder#Magister
		elif iSelectedClass == 1:
			lWonders = lNational
		elif iSelectedClass == 2:
			lWonders = lTeam
		elif iSelectedClass == 3:
			lWonders = lWorld
		elif iSelectedClass == 4:
			lWonders = lShrines


		iWidth = screen.getXResolution() *3/4 - 20
		iMaxRows = (screen.getYResolution()/2 - 80) / 24
		nColumns = max(1, min(iWidth/180, (len(lWonders) + iMaxRows - 1)/iMaxRows))#Magister
		iHeight = iMaxRows * 24 + 2
		screen.addTableControlGFC("WBWonders", nColumns, screen.getXResolution()/4, 80, iWidth, iHeight, False, False, 24, 24, TableStyles.TABLE_STYLE_STANDARD )
		for i in xrange(nColumns):
			screen.setTableColumnHeader( "WBWonders", i, "", iWidth/nColumns)

		nRows = (len(lWonders) + nColumns - 1) / nColumns
		for i in xrange(nRows):
			screen.appendTableRow("WBWonders")

		for iCount in xrange(len(lWonders)):
			item = lWonders[iCount]
			iRow = iCount % nRows
			iColumn = iCount / nRows
			ItemInfo = gc.getBuildingInfo(item[1])
			sColor = CyTranslator().getText("[COLOR_WARNING_TEXT]", ())
			if pCity.getNumRealBuilding(item[1]):
				sColor = CyTranslator().getText("[COLOR_POSITIVE_TEXT]", ())
			elif pCity.isHasBuilding(item[1]):
				sColor = CyTranslator().getText("[COLOR_YELLOW]", ())
			screen.setTableText("WBWonders", iColumn, iRow, "<font=3>" + sColor + item[0] + "</font></color>", ItemInfo.getButton(), WidgetTypes.WIDGET_HELP_BUILDING, item[1], 1, CvUtil.FONT_LEFT_JUSTIFY )

	def handleInput (self, inputClass):
		screen = CyGInterfaceScreen( "WBBuildingScreen", CvScreenEnums.WB_BUILDING)
		global bApplyAll
		global iSelectedClass
		global iChangeType
		global iBuildingFilter#Magister

		if inputClass.getFunctionName() == "CurrentPage":
			iIndex = screen.getPullDownData("CurrentPage", screen.getSelectedPullDownID("CurrentPage"))
			if iIndex == 0:
				WBCityEditScreen.WBCityEditScreen().interfaceScreen(pCity)
			elif iIndex == 1:
				WBCityDataScreen.WBCityDataScreen().interfaceScreen(pCity)
			elif iIndex == 3:
				WBPlayerScreen.WBPlayerScreen().interfaceScreen(iPlayer)
			elif iIndex == 4:
				WBTeamScreen.WBTeamScreen().interfaceScreen(pCity.getTeam())
			elif iIndex == 5:
				WBPlayerUnits.WBPlayerUnits().interfaceScreen(iPlayer)
			elif iIndex == 6:
				WBPlotScreen.WBPlotScreen().interfaceScreen(pCity.plot())
			elif iIndex == 7:
				WBEventScreen.WBEventScreen().interfaceScreen(pCity.plot())
			elif iIndex == 8:
				WBReligionScreen.WBReligionScreen().interfaceScreen(iPlayer)
			elif iIndex == 9:
				WBCorporationScreen.WBCorporationScreen().interfaceScreen(iPlayer)
			elif iIndex == 11:
				WBInfoScreen.WBInfoScreen().interfaceScreen(iPlayer)

		elif inputClass.getFunctionName() == "ChangeType":
			iChangeType = screen.getPullDownData("ChangeType", screen.getSelectedPullDownID("ChangeType"))

		elif inputClass.getFunctionName() == "CurrentCity":
			self.interfaceScreen(gc.getPlayer(iPlayer).getCity(inputClass.getData2()))

		elif inputClass.getFunctionName() == "WBBuilding":
			bUpdate = self.editBuilding(inputClass.getData1(), gc.getPlayer(iPlayer), False, False)
			self.placeBuildings()
			if bUpdate:
				self.placeWonders()

		elif inputClass.getFunctionName() == "BuildingAvailable":

#Magister Start
			if iBuildingFilter == 0:
				lBuilding = lStandard
			elif iBuildingFilter == 1:
				lBuilding = lUnique
			elif iBuildingFilter == 2:
				lBuilding = lReligion
			elif iBuildingFilter == 3:
				lBuilding = lSpell
			elif iBuildingFilter == 4:
				lBuilding = lEquipment
			elif iBuildingFilter == 5:
				lBuilding = lHidden
			elif iBuildingFilter == 6:
				lBuilding = lAll
#Magister Stop

			bUpdate = False
			for item in lBuilding:
				bTemp = self.editBuilding(item[1], gc.getPlayer(iPlayer), True, False)
				bUpdate = bUpdate or bTemp
			self.placeBuildings()
			if bUpdate:
				self.placeWonders()

		elif inputClass.getFunctionName() == "BuildingAll":
#Magister Start
			if iBuildingFilter == 0:
				lBuilding = lStandard
			elif iBuildingFilter == 1:
				lBuilding = lUnique
			elif iBuildingFilter == 2:
				lBuilding = lReligion
			elif iBuildingFilter == 3:
				lBuilding = lSpell
			elif iBuildingFilter == 4:
				lBuilding = lEquipment
			elif iBuildingFilter == 5:
				lBuilding = lHidden
			elif iBuildingFilter == 6:
				lBuilding = lAll
#Magister Stop
			bUpdate = False
			for item in lBuilding:
				bTemp = self.editBuilding(item[1], gc.getPlayer(iPlayer), False, False)
				bUpdate = bUpdate or bTemp
			self.placeBuildings()
			if bUpdate:
				self.placeWonders()

		elif inputClass.getFunctionName() == "WonderClass":
			iSelectedClass = inputClass.getData()
			self.placeWonders()

#Magister Start
		elif inputClass.getFunctionName() == "BuildingClass":
			iBuildingFilter = inputClass.getData()
			self.placeBuildings()
#Magister Stop

		elif inputClass.getFunctionName() == "WBWonders":
			bUpdate = self.editBuilding(inputClass.getData1(), gc.getPlayer(iPlayer), False, True)
			self.placeWonders()
			if bUpdate:
				self.placeBuildings()

		elif inputClass.getFunctionName() == "WonderAvailable":
			bUpdate = False
			lList = lWorld
			if iSelectedClass == 0:
				lList = lWonder#Magister
			elif iSelectedClass == 1:
				lList = lNational
			elif iSelectedClass == 2:
				lList = lTeam
			for item in lList:
				bTemp = self.editBuilding(item[1], gc.getPlayer(iPlayer), True, True)
				bUpdate = bUpdate or bTemp
			self.placeWonders()
			if bUpdate:
				self.placeBuildings()

		elif inputClass.getFunctionName() == "WonderAll":
			bUpdate = False
			lList = lWorld
			if iSelectedClass == 0:
				lList = lWonder#Magister
			elif iSelectedClass == 1:
				lList = lNational
			elif iSelectedClass == 2:
				lList = lTeam
			for item in lList:
				bTemp = self.editBuilding(item[1], gc.getPlayer(iPlayer), False, True)
				bUpdate = bUpdate or bTemp
			self.placeWonders()
			if bUpdate:
				self.placeBuildings()

		elif inputClass.getFunctionName() == "ApplyAll":
			bApplyAll = not bApplyAll
			sText = u"<font=3b>" + CyTranslator().getText("TXT_KEY_WB_COPY_ALL", (CyTranslator().getText("TXT_KEY_CONCEPT_CITIES", ()),)) + "</font>"
			sColor = CyTranslator().getText("[COLOR_WARNING_TEXT]", ())
			if bApplyAll:
				sColor = CyTranslator().getText("[COLOR_POSITIVE_TEXT]", ())
			screen.modifyString("ApplyAll", sColor + sText + "</color>", 0)
		return 1

	def editBuilding(self, item, pPlayerX, bAvailable, bWonder):
		ItemInfo = gc.getBuildingInfo(item)
		iType = iChangeType or bAvailable
		if bApplyAll and not bWonder:
			(loopCity, iter) = pPlayerX.firstCity(False)
			while(loopCity):
				bModify = True
				if ItemInfo.isWater() and not loopCity.isCoastal(ItemInfo.getMinAreaSize()): bModify = False
				if ItemInfo.isRiver() and not loopCity.plot().isRiver(): bModify = False
				if bAvailable:
					if ItemInfo.isCapital(): bModify = False
					iHolyReligion = ItemInfo.getHolyCity()
					if iHolyReligion > -1 and not loopCity.isHolyCityByType(iHolyReligion): bModify = False
					if not loopCity.canConstruct(item, True, False, True): bModify = False
				if bModify:
					if iChangeType == 2 and not bAvailable:
						iType = not loopCity.getNumRealBuilding(item)
					self.doEffects(loopCity, item, iType)
				(loopCity, iter) = pPlayerX.nextCity(iter, False)
		else:
			if bAvailable:
				if ItemInfo.isCapital(): return
				iHolyReligion = ItemInfo.getHolyCity()
				if iHolyReligion > -1 and not pCity.isHolyCityByType(iHolyReligion): return
				if not pCity.canConstruct(item, True, False, True): return
			if iChangeType == 2 and not bAvailable:
				iType = not pCity.getNumRealBuilding(item)
			self.doEffects(pCity, item, iType)
		iFreeBuilding = ItemInfo.getFreeBuildingClass()
		if iFreeBuilding > -1:
			if bWonder != isLimitedWonderClass(iFreeBuilding):
				return True
		return False

	def doEffects(self, pCity, item, bAdd):
		bEffects = False
		if bAdd and CvPlatyBuilderScreen.bPython and pCity.getNumRealBuilding(item) == 0:
			bEffects = True
		pCity.setNumRealBuilding(item, bAdd)
		if bEffects:
			CvEventManager.CvEventManager().onBuildingBuilt([pCity, item])

	def update(self, fDelta):
		return 1