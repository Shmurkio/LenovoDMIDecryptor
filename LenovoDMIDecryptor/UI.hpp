#pragma once

#include <Windows.h>
#include "AppState.hpp"

namespace UI
{
    auto DrawMainWindow(HWND hWnd, APP_STATE& State) -> void;

	// Title bar layout constants.
    constexpr const float TITLE_BAR_HEIGHT_OFFSET						= 8.0f;
	constexpr const ImU32 TITLE_BAR_COLOR								= IM_COL32(35, 35, 35, 255);
	constexpr const ImU32 TITLE_BAR_LINE_COLOR							= IM_COL32(70, 70, 70, 255);
	constexpr const float TITLE_BAR_BUTTON_WIDTH						= 32.0f;
	constexpr const float TITLE_BAR_TEXT_PADDING_X						= 6.0f;
	constexpr const float TITLE_BAR_TEXT_PADDING_Y						= 0.5f;
	constexpr const ImU32 TITLE_BAR_BUTTON_COLOR						= IM_COL32(35, 35, 35, 255);
	constexpr const ImU32 TITLE_BAR_BUTTON_HOVER_COLOR					= IM_COL32(70, 70, 70, 255);
	constexpr const ImU32 TITLE_BAR_BUTTON_ACTIVE_COLOR					= IM_COL32(70, 70, 70, 255);

	// Menu bar layout constants.
	constexpr const float MENU_BAR_HEIGHT_OFFSET						= TITLE_BAR_HEIGHT_OFFSET;
	constexpr const ImU32 MENU_BAR_COLOR								= IM_COL32(45, 45, 45, 255);
	constexpr const ImU32 MENU_BAR_LINE_COLOR							= IM_COL32(70, 70, 70, 255);
	constexpr const ImU32 MENU_BAR_BUTTON_COLOR							= IM_COL32(45, 45, 45, 255);
	constexpr const ImU32 MENU_BAR_BUTTON_HOVER_COLOR					= IM_COL32(70, 70, 70, 255);
	constexpr const ImU32 MENU_BAR_BUTTON_ACTIVE_COLOR					= IM_COL32(70, 70, 70, 255);
	constexpr const float MENU_BAR_BUTTON_PADDING_X						= 4.0f;
	constexpr const float MENU_BAR_BUTTON_PADDING_Y						= 0.5f;
	constexpr const float MENU_BAR_BUTTON_ADDITIONAL_WIDTH				= 12.0f;
	constexpr const ImU32 MENU_BAR_DROPDOWN_COLOR						= IM_COL32(45, 45, 45, 255);
	constexpr const ImU32 MENU_BAR_DROPDOWN_HOVER_COLOR					= IM_COL32(70, 70, 70, 255);
	constexpr const ImU32 MENU_BAR_DROPDOWN_ACTIVE_COLOR				= IM_COL32(70, 70, 70, 255);

	// About window layout constants.
	constexpr const float ABOUT_WINDOW_WIDTH							= 500.0f;
	constexpr const float ABOUT_WINDOW_HEIGHT							= 320.0f;
	constexpr const ImU32 ABOUT_WINDOW_BORDER_COLOR						= TITLE_BAR_LINE_COLOR;
	constexpr const float ABOUT_WINDOW_PADDING_X						= TITLE_BAR_TEXT_PADDING_X;
	constexpr const float ABOUT_WINDOW_PADDING_Y						= 6.0f;
	constexpr const float ABOUT_WINDOW_SPACING							= 4.0f;
	constexpr const float ABOUT_WINDOW_TITLE_BAR_HEIGHT_OFFSET			= TITLE_BAR_HEIGHT_OFFSET;
	constexpr const ImU32 ABOUT_WINDOW_TITLE_BAR_COLOR					= TITLE_BAR_COLOR;
	constexpr const ImU32 ABOUT_WINDOW_TITLE_BAR_LINE_COLOR				= TITLE_BAR_LINE_COLOR;
	constexpr const float ABOUT_WINDOW_CLOSE_BUTTON_WIDTH				= 28.0f;
	constexpr const ImU32 ABOUT_WINDOW_CLOSE_BUTTON_COLOR				= TITLE_BAR_BUTTON_COLOR;
	constexpr const ImU32 ABOUT_WINDOW_CLOSE_BUTTON_HOVER_COLOR			= TITLE_BAR_BUTTON_HOVER_COLOR;
	constexpr const ImU32 ABOUT_WINDOW_CLOSE_BUTTON_ACTIVE_COLOR		= TITLE_BAR_BUTTON_ACTIVE_COLOR;
	constexpr const float ABOUT_WINDOW_TITLE_TEXT_PADDING_X				= 8.0f;
	constexpr const float ABOUT_WINDOW_TITLE_TEXT_PADDING_Y				= 0.5f;
	constexpr const float ABOUT_WINDOW_LABEL_OFFSET						= 160.0f;
	constexpr const ImU32 ABOUT_WINDOW_TEXT_LINK_OPENED_COLOR			= IM_COL32(160, 120, 255, 255);
	constexpr const ImU32 ABOUT_WINDOW_TEXT_LINK_DEFAULT_COLOR			= IM_COL32(100, 150, 255, 255);

	// Structure panel layout constants.
	constexpr const float STRUCTURE_PANEL_RELATIVE_WIDTH				= 0.7f;
	constexpr const float STRUCTURE_PANEL_OFFSET_LEFT					= 12.0f;
	constexpr const float STRUCTURE_PANEL_OFFSET_TOP					= 12.0f;
	constexpr const float STRUCTURE_PANEL_OFFSET_BOTTOM					= 12.0f;
	constexpr const float STRUCTURE_PANEL_PADDING_X						= TITLE_BAR_TEXT_PADDING_X;
	constexpr const float STRUCTURE_PANEL_PADDING_Y						= 8.0f;
	constexpr const float STRUCTURE_PANEL_TITLE_TEXT_PADDING_X			= TITLE_BAR_TEXT_PADDING_X;
	constexpr const float STRUCTURE_PANEL_TITLE_TEXT_PADDING_Y			= TITLE_BAR_TEXT_PADDING_Y;
	constexpr const float STRUCTURE_PANEL_TITLE_BAR_HEIGHT_OFFSET		= TITLE_BAR_HEIGHT_OFFSET;
	constexpr const ImU32 STRUCTURE_PANEL_COLOR							= IM_COL32(45, 45, 45, 255);
	constexpr const ImU32 STRUCTURE_PANEL_BORDER_COLOR					= IM_COL32(70, 70, 70, 255);
	constexpr const ImU32 STRUCTURE_PANEL_TITLE_BAR_COLOR				= IM_COL32(45, 45, 45, 255);
	constexpr const ImU32 STRUCTURE_PANEL_TITLE_BAR_LINE_COLOR			= MENU_BAR_LINE_COLOR;
	constexpr const ImU32 STRUCTURE_PANEL_SCROLLBAR_COLOR				= IM_COL32(40, 40, 40, 255);
	constexpr const ImU32 STRUCTURE_PANEL_SCROLLBAR_GRAB_COLOR			= IM_COL32(85, 85, 85, 255);
	constexpr const ImU32 STRUCTURE_PANEL_SCROLLBAR_GRAB_HOVER_COLOR	= IM_COL32(105, 105, 105, 255);
	constexpr const ImU32 STRUCTURE_PANEL_SCROLLBAR_GRAB_ACTIVE_COLOR	= IM_COL32(125, 125, 125, 255);
	constexpr const ImU32 STRUCTURE_PANEL_HEADER_COLOR					= IM_COL32(45, 75, 110, 255);
	constexpr const ImU32 STRUCTURE_PANEL_HEADER_HOVER_COLOR			= IM_COL32(70, 70, 70, 255);
	constexpr const ImU32 STRUCTURE_PANEL_HEADER_ACTIVE_COLOR			= IM_COL32(70, 70, 70, 255);
	constexpr const ImU32 STRUCTURE_PANEL_TEXT_PRIORITY_COLOR			= IM_COL32(170, 140, 255, 255);
	constexpr const ImU32 STRUCTURE_PANEL_SMBIOS_NAMESPACE_COLOR		= IM_COL32(100, 200, 255, 255);
	constexpr const ImU32 STRUCTURE_PANEL_OTHER_NAMESPACE_COLOR			= IM_COL32(150, 150, 150, 255);

	// State panel layout constants.
	constexpr const float STATE_PANEL_RELATIVE_WIDTH					= 1.0f - STRUCTURE_PANEL_RELATIVE_WIDTH;
	constexpr const float STATE_PANEL_OFFSET_RIGHT						= STRUCTURE_PANEL_OFFSET_LEFT;
	constexpr const float STATE_PANEL_OFFSET_TOP						= STRUCTURE_PANEL_OFFSET_TOP;
	constexpr const float STATE_PANEL_HEIGHT							= 182.0f;
	constexpr const float STATE_PANEL_PADDING_X							= TITLE_BAR_TEXT_PADDING_X;
	constexpr const float STATE_PANEL_PADDING_Y							= 8.0f;
	constexpr const float STATE_PANEL_TITLE_TEXT_PADDING_X				= TITLE_BAR_TEXT_PADDING_X;
	constexpr const float STATE_PANEL_TITLE_TEXT_PADDING_Y				= TITLE_BAR_TEXT_PADDING_Y;
	constexpr const float STATE_PANEL_TITLE_BAR_HEIGHT_OFFSET			= TITLE_BAR_HEIGHT_OFFSET;
	constexpr const ImU32 STATE_PANEL_COLOR								= STRUCTURE_PANEL_COLOR;
	constexpr const ImU32 STATE_PANEL_BORDER_COLOR						= STRUCTURE_PANEL_BORDER_COLOR;
	constexpr const ImU32 STATE_PANEL_TITLE_BAR_COLOR					= STRUCTURE_PANEL_TITLE_BAR_COLOR;
	constexpr const ImU32 STATE_PANEL_TITLE_BAR_LINE_COLOR				= STRUCTURE_PANEL_TITLE_BAR_LINE_COLOR;
	constexpr const ImU32 STATE_PANEL_CHECKBOX_COLOR					= STRUCTURE_PANEL_SCROLLBAR_COLOR;
	constexpr const ImU32 STATE_PANEL_CHECKBOX_HOVER_COLOR				= STRUCTURE_PANEL_HEADER_HOVER_COLOR;
	constexpr const ImU32 STATE_PANEL_CHECKBOX_ACTIVE_COLOR				= STRUCTURE_PANEL_HEADER_ACTIVE_COLOR;
	constexpr const ImU32 STATE_PANEL_CHECKBOX_CHECKMARK_COLOR			= IM_COL32(255, 255, 255, 255);
}