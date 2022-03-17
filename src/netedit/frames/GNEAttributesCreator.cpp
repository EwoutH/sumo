/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2022 German Aerospace Center (DLR) and others.
// This program and the accompanying materials are made available under the
// terms of the Eclipse Public License 2.0 which is available at
// https://www.eclipse.org/legal/epl-2.0/
// This Source Code may also be made available under the following Secondary
// Licenses when the conditions for such availability set forth in the Eclipse
// Public License 2.0 are satisfied: GNU General Public License, version 2
// or later which is available at
// https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
// SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later
/****************************************************************************/
/// @file    GNEFrameAttributeModules.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Aug 2019
///
// Auxiliar class for GNEFrame Modules (only for attributes edition)
/****************************************************************************/
#include <config.h>

#include <netedit/GNENet.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewNet.h>
#include <netedit/dialogs/GNEAllowDisallow.h>
#include <netedit/dialogs/GNESingleParametersDialog.h>
#include <utils/common/StringTokenizer.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/windows/GUIAppEnum.h>

#include "GNEAttributesCreator.h"
#include "GNEAttributesCreatorRow.h"
#include "GNEFlowEditor.h"


// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(AttributesCreator) AttributesCreatorMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_RESET,  AttributesCreator::onCmdReset),
    FXMAPFUNC(SEL_COMMAND,  MID_HELP,       AttributesCreator::onCmdHelp),
};

// Object implementation
FXIMPLEMENT(AttributesCreator, FXGroupBoxModule, AttributesCreatorMap, ARRAYNUMBER(AttributesCreatorMap))


// ===========================================================================
// method definitions
// ===========================================================================

AttributesCreator::AttributesCreator(GNEFrame* frameParent) :
    FXGroupBoxModule(frameParent->getContentFrame(), "Internal attributes"),
    myFrameParent(frameParent),
    myTemplateAC(nullptr) {
    // resize myAttributesCreatorRows
    myAttributesCreatorRows.resize(GNEAttributeCarrier::MAXNUMBEROFATTRIBUTES, nullptr);
    // create myFlowEditor
    myFlowEditor = new FlowEditor(frameParent->getViewNet(), frameParent->getContentFrame());
    // create reset and help button
    myFrameButtons = new FXHorizontalFrame(getCollapsableFrame(), GUIDesignAuxiliarHorizontalFrame);
    myResetButton = new FXButton(myFrameButtons, "", GUIIconSubSys::getIcon(GUIIcon::RESET), this, MID_GNE_RESET, GUIDesignButtonIcon);
    new FXButton(myFrameButtons, "Help", nullptr, this, MID_HELP, GUIDesignButtonRectangular);
}


AttributesCreator::~AttributesCreator() {}


void
AttributesCreator::showAttributesCreatorModule(GNEAttributeCarrier* templateAC, const std::vector<SumoXMLAttr>& hiddenAttributes) {
    // destroy all rows
    for (auto& row : myAttributesCreatorRows) {
        // destroy and delete all rows
        if (row != nullptr) {
            row->destroy();
            delete row;
            row = nullptr;
        }
    }
    if (templateAC) {
        // set current template AC and hidden attributes
        myTemplateAC = templateAC;
        myHiddenAttributes = hiddenAttributes;
        // refresh rows (new rows will be created)
        refreshRows(true);
        // enable reset
        myResetButton->enable();
        // show
        show();
    }
    else {
        throw ProcessError("invalid templateAC in showAttributesCreatorModule");
    }
}


void
AttributesCreator::hideAttributesCreatorModule() {
    // hide attributes creator flow
    myFlowEditor->hideFlowEditor();
    // hide modul
    hide();
}


GNEFrame*
AttributesCreator::getFrameParent() const {
    return myFrameParent;
}


void
AttributesCreator::getAttributesAndValues(CommonXMLStructure::SumoBaseObject* baseObject, bool includeAll) const {
    // get standard parameters
    for (const auto& row : myAttributesCreatorRows) {
        if (row && row->getAttrProperties().getAttr() != SUMO_ATTR_NOTHING) {
            const auto& attrProperties = row->getAttrProperties();
            // flag for row enabled
            const bool rowEnabled = row->isAttributesCreatorRowEnabled();
            // flag for default attributes
            const bool hasDefaultStaticValue = !attrProperties.hasDefaultValue() || (attrProperties.getDefaultValue() != row->getValue());
            // flag for enablitables attributes
            const bool isFlowDefinitionAttribute = attrProperties.isFlowDefinition();
            // flag for Terminatel attributes
            const bool isActivatableAttribute = attrProperties.isActivatable() && row->getAttributeCheckButtonCheck();
            // check if flags configuration allow to include values
            if (rowEnabled && (includeAll || hasDefaultStaticValue || isFlowDefinitionAttribute || isActivatableAttribute)) {
                // add attribute depending of type
                if (attrProperties.isInt()) {
                    const int intValue = GNEAttributeCarrier::canParse<int>(row->getValue()) ? GNEAttributeCarrier::parse<int>(row->getValue()) : GNEAttributeCarrier::parse<int>(attrProperties.getDefaultValue());
                    baseObject->addIntAttribute(attrProperties.getAttr(), intValue);
                }
                else if (attrProperties.isFloat()) {
                    const double doubleValue = GNEAttributeCarrier::canParse<double>(row->getValue()) ? GNEAttributeCarrier::parse<double>(row->getValue()) : GNEAttributeCarrier::parse<double>(attrProperties.getDefaultValue());
                    baseObject->addDoubleAttribute(attrProperties.getAttr(), doubleValue);
                }
                else if (attrProperties.isBool()) {
                    const bool boolValue = GNEAttributeCarrier::canParse<bool>(row->getValue()) ? GNEAttributeCarrier::parse<bool>(row->getValue()) : GNEAttributeCarrier::parse<bool>(attrProperties.getDefaultValue());
                    baseObject->addBoolAttribute(attrProperties.getAttr(), boolValue);
                }
                else if (attrProperties.isposition()) {
                    const Position positionValue = GNEAttributeCarrier::canParse<Position>(row->getValue()) ? GNEAttributeCarrier::parse<Position>(row->getValue()) : GNEAttributeCarrier::parse<Position>(attrProperties.getDefaultValue());
                    baseObject->addPositionAttribute(attrProperties.getAttr(), positionValue);
                }
                else if (attrProperties.isSUMOTime()) {
                    const SUMOTime timeValue = GNEAttributeCarrier::canParse<SUMOTime>(row->getValue()) ? GNEAttributeCarrier::parse<SUMOTime>(row->getValue()) : GNEAttributeCarrier::parse<SUMOTime>(attrProperties.getDefaultValue());
                    baseObject->addTimeAttribute(attrProperties.getAttr(), timeValue);
                }
                else if (attrProperties.isColor()) {
                    const RGBColor colorValue = GNEAttributeCarrier::canParse<RGBColor>(row->getValue()) ? GNEAttributeCarrier::parse<RGBColor>(row->getValue()) : GNEAttributeCarrier::parse<RGBColor>(attrProperties.getDefaultValue());
                    baseObject->addColorAttribute(attrProperties.getAttr(), colorValue);
                }
                else if (attrProperties.isList()) {
                    if (attrProperties.isposition()) {
                        const PositionVector positionVectorValue = GNEAttributeCarrier::canParse<PositionVector>(row->getValue()) ? GNEAttributeCarrier::parse<PositionVector>(row->getValue()) : GNEAttributeCarrier::parse<PositionVector>(attrProperties.getDefaultValue());
                        baseObject->addPositionVectorAttribute(attrProperties.getAttr(), positionVectorValue);
                    }
                    else {
                        const std::vector<std::string> stringVectorValue = GNEAttributeCarrier::canParse<std::vector<std::string> >(row->getValue()) ? GNEAttributeCarrier::parse<std::vector<std::string> >(row->getValue()) : GNEAttributeCarrier::parse<std::vector<std::string> >(attrProperties.getDefaultValue());
                        baseObject->addStringListAttribute(attrProperties.getAttr(), stringVectorValue);
                    }
                }
                else {
                    baseObject->addStringAttribute(attrProperties.getAttr(), row->getValue());
                }
            }
        }
    }
    // add extra flow attributes (only will updated if myFlowEditor is shown)
    if (myFlowEditor->shownFlowEditor()) {
        myFlowEditor->getFlowAttributes(baseObject);
    }
}


GNEAttributeCarrier*
AttributesCreator::getCurrentTemplateAC() const {
    return myTemplateAC;
}


void
AttributesCreator::showWarningMessage(std::string extra) const {
    std::string errorMessage;
    // show warning box if input parameters aren't invalid
    if (extra.size() == 0) {
        errorMessage = "Invalid input parameter of " + myTemplateAC->getTagProperty().getTagStr();
    }
    else {
        errorMessage = "Invalid input parameter of " + myTemplateAC->getTagProperty().getTagStr() + ": " + extra;
    }
    // set message in status bar
    myFrameParent->getViewNet()->setStatusBarText(errorMessage);
    // Write Warning in console if we're in testing mode
    WRITE_DEBUG(errorMessage);
}


void
AttributesCreator::refreshAttributesCreator() {
    // just refresh row without creating new rows
    if (shown() && myTemplateAC) {
        refreshRows(false);
    }
}


void
AttributesCreator::disableAttributesCreator() {
    // disable all rows
    for (const auto& row : myAttributesCreatorRows) {
        if (row) {
            row->disableRow();
        }
    }
    // also disable reset
    myResetButton->disable();
}


bool
AttributesCreator::areValuesValid() const {
    // iterate over standar parameters
    for (const auto& attribute : myTemplateAC->getTagProperty()) {
        // Return false if error message of attriuve isn't empty
        if (myAttributesCreatorRows.at(attribute.getPositionListed()) && !myAttributesCreatorRows.at(attribute.getPositionListed())->isAttributeValid()) {
            return false;
        }
    }
    // check flow attributes
    if (myFlowEditor->shownFlowEditor()) {
        return myFlowEditor->areFlowValuesValid();
    }
    return true;
}


long
AttributesCreator::onCmdReset(FXObject*, FXSelector, void*) {
    if (myTemplateAC) {
        myTemplateAC->resetDefaultValues();
        refreshRows(false);
    }
    return 1;
}


long
AttributesCreator::onCmdHelp(FXObject*, FXSelector, void*) {
    // open Help attributes dialog
    myFrameParent->openHelpAttributesDialog(myTemplateAC);
    return 1;
}


void
AttributesCreator::refreshRows(const bool createRows) {
    // declare a flag to show Flow editor
    bool showFlowEditor = false;
    // iterate over tag attributes and create AttributesCreatorRows for every attribute
    for (const auto& attribute : myTemplateAC->getTagProperty()) {
        // declare falg to check conditions for show attribute
        bool showAttribute = true;
        // check that only non-unique attributes (except ID) are created (And depending of includeExtendedAttributes)
        if (attribute.isUnique() && (attribute.getAttr() != SUMO_ATTR_ID)) {
            showAttribute = false;
        }
        // check if attribute must stay hidden
        if (std::find(myHiddenAttributes.begin(), myHiddenAttributes.end(), attribute.getAttr()) != myHiddenAttributes.end()) {
            showAttribute = false;
        }
        // check if attribute is a flow definitionattribute
        if (attribute.isFlowDefinition()) {
            showAttribute = false;
            showFlowEditor = true;
        }
        // check special case for vaporizer IDs
        if ((attribute.getAttr() == SUMO_ATTR_ID) && (attribute.getTagPropertyParent().getTag() == SUMO_TAG_VAPORIZER)) {
            showAttribute = false;
        }
        // check special case for VType IDs in vehicle Frame
        if ((attribute.getAttr() == SUMO_ATTR_TYPE) && (myFrameParent->getViewNet()->getEditModes().isCurrentSupermodeDemand()) &&
            (myFrameParent->getViewNet()->getEditModes().demandEditMode == DemandEditMode::DEMAND_VEHICLE)) {
            showAttribute = false;
        }
        // show attribute depending of showAttribute flag
        if (showAttribute) {
            // check if we have to create a new row
            if (createRows) {
                myAttributesCreatorRows.at(attribute.getPositionListed()) = new AttributesCreatorRow(this, attribute);
            }
            else {
                myAttributesCreatorRows.at(attribute.getPositionListed())->refreshRow();
            }
        }
    }
    // reparent help button (to place it at bottom)
    myFrameButtons->reparent(getCollapsableFrame());
    // recalc
    recalc();
    // check if flow editor has to be shown
    if (showFlowEditor) {
        myFlowEditor->showFlowEditor({ myTemplateAC });
    }
    else {
        myFlowEditor->hideFlowEditor();
    }
}

/****************************************************************************/
