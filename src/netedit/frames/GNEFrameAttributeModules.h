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
/// @file    GNEFrameAttributeModules.h
/// @author  Pablo Alvarez Lopez
/// @date    Aug 2019
///
// Auxiliar class for GNEFrame Modules (only for attributes edition)
/****************************************************************************/
#pragma once
#include <config.h>

#include <netedit/elements/GNEAttributeCarrier.h>
#include <netedit/GNEViewNetHelper.h>
#include <utils/common/Parameterised.h>
#include <utils/foxtools/FXGroupBoxModule.h>
#include <utils/xml/CommonXMLStructure.h>

// ===========================================================================
// class declaration
// ===========================================================================

class GNEFrame;
class FlowEditor;

// ===========================================================================
// class definitions
// ===========================================================================

class GNEFrameAttributeModules {

public:
    // ===========================================================================
    // class declaration
    // ===========================================================================

    class AttributesEditor;
    class AttributesEditorFlow;

    // ===========================================================================
    // class AttributesEditorRow
    // ===========================================================================

    class AttributesEditorRow : protected FXHorizontalFrame {
        /// @brief FOX-declaration
        FXDECLARE(GNEFrameAttributeModules::AttributesEditorRow)

    public:
        /// @brief constructor
        AttributesEditorRow(AttributesEditor* attributeEditorParent, const GNEAttributeProperties& ACAttr, const std::string& value,
                            const bool attributeEnabled, const bool computed);

        /// @brief destroy AttributesCreatorRow (but don't delete)
        void destroy();

        /// @brief refresh current row
        void refreshAttributesEditorRow(const std::string& value, const bool forceRefresh, const bool attributeEnabled, const bool computed);

        /// @brief check if current attribute of TextField/ComboBox is valid
        bool isAttributesEditorRowValid() const;

        /// @name FOX-callbacks
        /// @{

        /// @brief try to set new attribute value
        long onCmdSetAttribute(FXObject*, FXSelector, void*);

        /// @brief called when user press a check button
        long onCmdSelectCheckButton(FXObject*, FXSelector, void*);

        /// @brief open model dialog for more comfortable attribute editing
        long onCmdOpenAttributeDialog(FXObject*, FXSelector, void*);
        /// @}

    protected:
        /// @brief default constructor
        AttributesEditorRow();

        /// @brief removed invalid spaces of Positions and shapes
        std::string stripWhitespaceAfterComma(const std::string& stringValue);

        /// @brief check junction merging
        bool mergeJunction(SumoXMLAttr attr, const std::vector<GNEAttributeCarrier*>& inspectedACs, const std::string& newVal) const;

    private:
        /// @brief pointer to AttributesEditor parent
        AttributesEditor* myAttributesEditorParent;

        /// @brief current AC Attribute
        const GNEAttributeProperties myACAttr;

        /// @brief flag to check if input element contains multiple values
        const bool myMultiple;

        /// @brief pointer to attribute label
        FXLabel* myAttributeLabel = nullptr;

        /// @brief pointer to attribute  menu check
        FXCheckButton* myAttributeCheckButton = nullptr;

        /// @brief pointer to buttonCombinableChoices
        FXButton* myAttributeButtonCombinableChoices = nullptr;

        /// @brief Button for open color editor
        FXButton* myAttributeColorButton = nullptr;

        /// @brief textField to modify the value of string attributes
        FXTextField* myValueTextField = nullptr;

        /// @brief pointer to combo box choices
        FXComboBox* myValueComboBoxChoices = nullptr;

        /// @brief pointer to menu check
        FXCheckButton* myValueCheckButton = nullptr;
    };

    // ===========================================================================
    // class AttributesEditor
    // ===========================================================================

    class AttributesEditor : public FXGroupBoxModule {
        /// @brief FOX-declaration
        FXDECLARE(GNEFrameAttributeModules::AttributesEditor)

    public:
        /// @brief constructor
        AttributesEditor(GNEFrame* inspectorFrameParent);

        /// @brief show attributes of multiple ACs
        void showAttributeEditorModule(bool includeExtended, bool forceAttributeEnabled);

        /// @brief hide attribute editor
        void hideAttributesEditorModule();

        /// @brief refresh attribute editor (only the valid values will be refresh)
        void refreshAttributeEditor(bool forceRefreshShape, bool forceRefreshPosition);

        /// @brief pointer to GNEFrame parent
        GNEFrame* getFrameParent() const;

        /// @name FOX-callbacks
        /// @{
        /// @brief Called when user press the help button
        long onCmdAttributesEditorHelp(FXObject*, FXSelector, void*);
        /// @}

    protected:
        /// @brief fox need this
        FOX_CONSTRUCTOR(AttributesEditor)

    private:
        /// @brief pointer to GNEFrame parent
        GNEFrame* myFrameParent;

        /// @brief FlowEditor modul
        FlowEditor* myAttributesEditorFlow = nullptr;

        /// @brief list of Attribute editor rows
        std::vector<AttributesEditorRow*> myAttributesEditorRows;

        /// @brief button for help
        FXButton* myHelpButton = nullptr;

        /// @brief flag used to mark if current edited ACs are bein edited including extended attribute
        bool myIncludeExtended;
    };

    // ===========================================================================
    // class AttributesEditorExtended
    // ===========================================================================

    class AttributesEditorExtended : public FXGroupBoxModule {
        /// @brief FOX-declaration
        FXDECLARE(GNEFrameAttributeModules::AttributesEditorExtended)

    public:
        /// @brief constructor
        AttributesEditorExtended(GNEFrame* frameParent);

        /// @brief destructor
        ~AttributesEditorExtended();

        /// @brief show AttributesEditorExtended modul
        void showAttributesEditorExtendedModule();

        /// @brief hide group box
        void hideAttributesEditorExtendedModule();

        /// @name FOX-callbacks
        /// @{
        /// @brief Called when open dialog button is clicked
        long onCmdOpenDialog(FXObject*, FXSelector, void*);
        /// @}

    protected:
        FOX_CONSTRUCTOR(AttributesEditorExtended)

    private:
        /// @brief pointer to Frame Parent
        GNEFrame* myFrameParent = nullptr;
    };

    // ===========================================================================
    // class GenericDataAttributes
    // ===========================================================================

    class GenericDataAttributes : public FXGroupBoxModule {
        /// @brief FOX-declaration
        FXDECLARE(GNEFrameAttributeModules::GenericDataAttributes)

    public:
        /// @brief constructor
        GenericDataAttributes(GNEFrame* frameParent);

        /// @brief destructor
        ~GenericDataAttributes();

        /// @brief show netedit attributes EditorCreator
        void showGenericDataAttributes();

        /// @brief hide netedit attributes EditorCreator
        void hideGenericDataAttributes();

        /// @brief refresh netedit attributes
        void refreshGenericDataAttributes();

        /// @brief get parameters as map
        const Parameterised::Map& getParametersMap() const;

        /// @brief get parameters as string
        std::string getParametersStr() const;

        /// @brief get parameters as vector of strings
        std::vector<std::pair<std::string, std::string> > getParameters() const;

        /// @brief set parameters
        void setParameters(const std::vector<std::pair<std::string, std::string> >& parameters);

        /// @brief pointer to frame parent
        GNEFrame* getFrameParent() const;

        /// @name FOX-callbacks
        /// @{
        /// @brief Called when user clicks over add parameter
        long onCmdEditParameters(FXObject*, FXSelector, void*);

        /// @brief Called when user udpate the parameter text field
        long onCmdSetParameters(FXObject*, FXSelector, void*);
        /// @}

    protected:
        FOX_CONSTRUCTOR(GenericDataAttributes)

    private:
        /// @brief pointer to frame parent
        GNEFrame* myFrameParent = nullptr;

        /// @brief pointer to current map of parameters
        Parameterised::Map myParameters;

        /// @brief text field for write parameters
        FXTextField* myTextFieldParameters = nullptr;

        /// @brief button for edit parameters using specific dialog
        FXButton* myButtonEditParameters = nullptr;
    };

    // ===========================================================================
    // class NeteditAttributes
    // ===========================================================================

    class NeteditAttributes : public FXGroupBoxModule {
        /// @brief FOX-declaration
        FXDECLARE(GNEFrameAttributeModules::NeteditAttributes)

    public:
        /// @brief constructor
        NeteditAttributes(GNEFrame* frameParent);

        /// @brief destructor
        ~NeteditAttributes();

        /// @brief show Netedit attributes modul
        void showNeteditAttributesModule(const GNETagProperties& tagValue);

        /// @brief hide Netedit attributes modul
        void hideNeteditAttributesModule();

        /// @brief fill valuesMap with netedit attributes
        bool getNeteditAttributesAndValues(CommonXMLStructure::SumoBaseObject* baseObject, const GNELane* lane) const;

        /// @name FOX-callbacks
        /// @{
        /// @brief Called when user changes some element of NeteditAttributes
        long onCmdSetNeteditAttribute(FXObject*, FXSelector, void*);

        /// @brief Called when user press the help button
        long onCmdHelp(FXObject*, FXSelector, void*);
        /// @}

    protected:
        FOX_CONSTRUCTOR(NeteditAttributes)

    private:
        /// @brief list of the reference points
        enum class AdditionalReferencePoint {
            LEFT,
            RIGHT,
            CENTER,
            INVALID
        };

        /// @brief obtain the Start position values of StoppingPlaces and E2 detector over the lane
        double setStartPosition(double positionOfTheMouseOverLane, double lengthOfAdditional) const;

        /// @brief obtain the End position values of StoppingPlaces and E2 detector over the lane
        double setEndPosition(double positionOfTheMouseOverLane, double lengthOfAdditional) const;

        /// @brief pointer to frame parent
        GNEFrame* myFrameParent;

        /// @brief match box with the list of reference points
        FXComboBox* myReferencePointMatchBox;

        /// @brief horizontal frame for length
        FXHorizontalFrame* myLengthFrame;

        /// @brief textField for length
        FXTextField* myLengthTextField;

        /// @brief horizontal frame for close polygon
        FXHorizontalFrame* myCloseShapeFrame;

        /// @brief checkbox to enable/disable close polygon
        FXCheckButton* myCloseShapeCheckButton;

        /// @brief horizontal frame for center view after creation frame
        FXHorizontalFrame* myCenterViewAfterCreationFrame;

        /// @brief checkbox to enable/disable center element after creation
        FXCheckButton* myCenterViewAfterCreationButton;

        /// @brief Button for help about the reference point
        FXButton* helpReferencePoint;

        /// @brief Flag to check if current length is valid
        bool myCurrentLengthValid;

        /// @brief actual additional reference point selected in the match Box
        AdditionalReferencePoint myActualAdditionalReferencePoint;
    };

    /// @brief return true if AC can be edited in the current supermode
    static bool isSupermodeValid(const GNEViewNet* viewNet, const GNEAttributeCarrier* AC);

    /// @brief return true if give ACAttr can be edited in the current supermode
    static bool isSupermodeValid(const GNEViewNet* viewNet, const GNEAttributeProperties& ACAttr);
};
