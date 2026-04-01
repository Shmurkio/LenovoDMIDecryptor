#include "UI.hpp"

#include "imgui.h"
#include "DMI.hpp"
#include "Cast.hpp"
#include "Util.hpp"

static auto IsEncrypted(Lenovo::PDMI_DATA DmiBlock) -> bool
{
	if (!DmiBlock)
	{
		return false;
	}

	auto Ptr = reinterpret_cast<uint8_t*>(DmiBlock);
	uint8_t LastByte = Ptr[Lenovo::DEFAULT_DMI_BLOCK_SIZE - 1];
	
	return (LastByte == DmiBlock->Header.Key);
}

struct UI_LAYOUT
{
	float TitleBarHeight;
	ImU32 TitleBarColor;
	ImU32 TitleBarLineColor;
	float TitleBarButtonWidth;
	ImU32 TitleBarButtonColor;
	ImU32 TitleBarButtonHoverColor;
	ImU32 TitleBarButtonActiveColor;
	ImVec2 TitleBarTextPadding;

	float MenuBarHeight;
	ImU32 MenuBarColor;
	ImU32 MenuBarLineColor;
	ImU32 MenuBarButtonColor;
	ImU32 MenuBarButtonHoverColor;
	ImU32 MenuBarButtonActiveColor;
	ImVec2 MenuBarButtonPadding;
	float MenuBarButtonAdditionalWidth;
	ImU32 MenuBarDropdownColor;
	ImU32 MenuBarDropdownHoverColor;
	ImU32 MenuBarDropdownActiveColor;

	float DmiPanelOffsetLeft;
	float DmiPanelOffsetTop;
	float DmiPanelOffsetBottom;
	float DmiPanelWidth;
	float DmiPanelTitleBarHeight;
	ImVec2 DmiPanelTitleBarTextPadding;
	ImU32 DmiPanelColor;
	ImU32 DmiPanelBorderColor;
	ImU32 DmiPanelTitleBarColor;
	ImU32 DmiPanelTitleBarLineColor;
	float DmiPanelTextHeight;
	ImVec2 DmiPanelContentTextPadding;
	float DmiPanelContentOffsetLeft;
	float DmiPanelContentOffsetTop;
	float DmiPanelContentOffsetRight;
	float DmiPanelContentOffsetBottom;
	ImU32 DmiPanelScrollbarColor;
	ImU32 DmiPanelScrollbarGrabColor;
	ImU32 DmiPanelScrollbarGrabHoverColor;
	ImU32 DmiPanelScrollbarGrabActiveColor;
	ImU32 DmiPanelHeaderColor;
	ImU32 DmiPanelHeaderHoverColor;
	ImU32 DmiPanelHeaderActiveColor;

	float StatusPanelOffsetRight;
	float StatusPanelOffsetTop;
	float StatusPanelWidth;
	float StatusPanelTitleBarHeight;
	ImVec2 StatusPanelTitleBarTextPadding;
	ImU32 StatusPanelColor;
	ImU32 StatusPanelBorderColor;
	ImU32 StatusPanelTitleBarColor;
	ImU32 StatusPanelTitleBarLineColor;
	float StatusPanelContentOffsetLeft;
	float StatusPanelContentOffsetTop;
	float StatusPanelContentOffsetRight;
	float StatusPanelContentOffsetBottom;
	ImVec2 StatusPanelContentTextPadding;
	ImU32 StatusPanelCheckboxColor;
	ImU32 StatusPanelCheckboxHoverColor;
	ImU32 StatusPanelCheckboxActiveColor;
	ImU32 StatusPanelCheckboxCheckmarkColor;

	float AboutWindowWidth;
	float AboutWindowHeight;
	float AboutWindowTitleBarHeight;
	ImVec2 AboutWindowTitleBarTextPadding;
	ImU32 AboutWindowTitleBarColor;
	ImU32 AboutWindowTitleBarLineColor;
	ImU32 AboutWindowColor;
	float AboutWindowTitleBarButtonWidth;
	ImVec2 AboutWindowContentTextPadding;
	ImU32 AboutWindowTitleBarButtonColor;
	ImU32 AboutWindowTitleBarButtonHoverColor;
	ImU32 AboutWindowTitleBarButtonActiveColor;
};

static UI_LAYOUT gLayout = { 0 };

static const float gTitleBarHeightOffset = 8.0f;
static const ImU32 gTitleBarColor = IM_COL32(35, 35, 35, 255);
static const ImU32 gTitleBarLineColor = IM_COL32(70, 70, 70, 255);
static const float gTitleBarButtonWidth = 32.0f;
static const float gTitleBarTextPaddingX = 6.0f;
static const float gTitleBarTextPaddingY = 0.5f;
static const ImU32 gTitleBarButtonColor = IM_COL32(35, 35, 35, 255);
static const ImU32 gTitleBarButtonHoverColor = IM_COL32(70, 70, 70, 255);
static const ImU32 gTitleBarButtonActiveColor = IM_COL32(70, 70, 70, 255);

static const float gMenuBarHeightOffset = 4.0f;
static const ImU32 gMenuBarColor = IM_COL32(45, 45, 45, 255);
static const ImU32 gMenuBarLineColor = IM_COL32(70, 70, 70, 255);
static const ImU32 gMenuBarButtonColor = IM_COL32(45, 45, 45, 255);
static const ImU32 gMenuBarButtonHoverColor = IM_COL32(70, 70, 70, 255);
static const ImU32 gMenuBarButtonActiveColor = IM_COL32(70, 70, 70, 255);
static const float gMenuBarButtonPaddingX = 4.0f;
static const float gMenuBarButtonPaddingY = 1.0f;
static const float gMenuBarButtonAdditionalWidth = 12.0f;
static const ImU32 gMenuBarDropdownColor = IM_COL32(45, 45, 45, 255);
static const ImU32 gMenuBarDropdownHoverColor = IM_COL32(70, 70, 70, 255);
static const ImU32 gMenuBarDropdownActiveColor = IM_COL32(70, 70, 70, 255);

static const float gAboutWindowTitleBarHeightOffset = gMenuBarHeightOffset;
static const ImU32 gAboutWindowTitleBarColor = gTitleBarColor;
static const ImU32 gAboutWindowTitleBarLineColor = gTitleBarLineColor;
static const ImU32 gAboutWindowColor = IM_COL32(48, 48, 48, 255);
static const float gAboutWindowTitleBarButtonWidth = gTitleBarButtonWidth;
static const float gAboutWindowTitleBarTextPaddingX = 4.0f;
static const float gAboutWindowTitleBarTextPaddingY = 1.0f;
static const float gAboutWindowWidth = 290.0f;
static const float gAboutWindowHeight = 165.0f;
static const float gAboutWindowContentTextPaddingX = 4.0f;
static const float gAboutWindowContentTextPaddingY = 1.0f;
static const ImU32 gAboutWindowTitleBarButtonColor = gAboutWindowTitleBarColor;
static const ImU32 gAboutWindowTitleBarButtonHoverColor = gTitleBarButtonHoverColor;
static const ImU32 gAboutWindowTitleBarButtonActiveColor = gTitleBarButtonActiveColor;

static const float gDmiPanelOffsetLeft = 12.0f;
static const float gDmiPanelOffsetTop = 12.0f;
static const float gDmiPanelOffsetBottom = 12.0f;
static const float gDmiPanelRelativeWidth = 0.7f;
static const float gDmiPanelTitleBarHeightOffset = 4.0f;
static const float gDmiPanelTitleBarTextPaddingX = 4.0f;
static const float gDmiPanelTitleBarTextPaddingY = 1.0f;
static const ImU32 gDmiPanelColor = IM_COL32(45, 45, 45, 255);
static const ImU32 gDmiPanelBorderColor = IM_COL32(70, 70, 70, 255);
static const ImU32 gDmiPanelTitleBarColor = IM_COL32(48, 48, 48, 255);
static const ImU32 gDmiPanelTitleBarLineColor = IM_COL32(80, 80, 80, 255);
static const float gDmiPanelContentOffsetLeft = 1.0f;
static const float gDmiPanelContentOffsetTop = 1.0f;
static const float gDmiPanelContentOffsetRight = 1.0f;
static const float gDmiPanelContentOffsetBottom = 1.0f;
static const ImU32 gDmiPanelScrollbarColor = IM_COL32(35, 35, 35, 255);
static const ImU32 gDmiPanelScrollbarGrabColor = IM_COL32(70, 70, 70, 255);
static const ImU32 gDmiPanelScrollbarGrabHoverColor = IM_COL32(90, 90, 90, 255);
static const ImU32 gDmiPanelScrollbarGrabActiveColor = IM_COL32(110, 110, 110, 255);
static const float gDmiPanelContentTextPaddingX = 4.0f;
static const float gDmiPanelContentTextPaddingY = 1.0f;
static const ImU32 gDmiPanelHeaderColor = IM_COL32(45, 75, 110, 255);
static const ImU32 gDmiPanelHeaderHoverColor = IM_COL32(70, 70, 70, 255);
static const ImU32 gDmiPanelHeaderActiveColor = IM_COL32(70, 70, 70, 255);

static const float gStatusPanelOffsetRight = gDmiPanelOffsetLeft;
static const float gStatusPanelOffsetTop = gDmiPanelOffsetTop;
static const float gStatusPanelRelativeWidth = 1.0f - gDmiPanelRelativeWidth;
static const float gStatusPanelTitleBarHeightOffset = gDmiPanelTitleBarHeightOffset;
static const float gStatusPanelTitleBarTextPaddingX = gDmiPanelTitleBarTextPaddingX;
static const float gStatusPanelTitleBarTextPaddingY = gDmiPanelTitleBarTextPaddingY;
static const ImU32 gStatusPanelColor = gDmiPanelColor;
static const ImU32 gStatusPanelBorderColor = gDmiPanelBorderColor;
static const ImU32 gStatusPanelTitleBarColor = gDmiPanelTitleBarColor;
static const ImU32 gStatusPanelTitleBarLineColor = gDmiPanelTitleBarLineColor;
static const float gStatusPanelContentOffsetLeft = gDmiPanelContentOffsetLeft;
static const float gStatusPanelContentOffsetTop = gDmiPanelContentOffsetTop;
static const float gStatusPanelContentOffsetRight = gDmiPanelContentOffsetRight;
static const float gStatusPanelContentOffsetBottom = gDmiPanelContentOffsetBottom;
static const float gStatusPanelContentTextPaddingX = gDmiPanelContentTextPaddingX;
static const float gStatusPanelContentTextPaddingY = gDmiPanelContentTextPaddingY;
static const ImU32 gStatusPanelCheckboxColor = IM_COL32(40, 40, 40, 255);
static const ImU32 gStatusPanelCheckboxHoverColor = gDmiPanelHeaderHoverColor;
static const ImU32 gStatusPanelCheckboxActiveColor = gDmiPanelHeaderHoverColor;
static const ImU32 gStatusPanelCheckboxCheckmarkColor = IM_COL32(255, 255, 255, 255);

static std::string gVersionString = "1.4.3";

static bool gOpenAboutPopup = false;

static auto BuildLayout(
	const float TitleBarHeightOffset,
	const ImU32 TitleBarColor,
	const ImU32 TitleBarLineColor,
	const float TitleBarButtonWidth,
	const float TitleBarTextPaddingX,
	const float TitleBarTextPaddingY,
	const ImU32 TitleBarButtonColor,
	const ImU32 TitleBarButtonHoverColor,
	const ImU32 TitleBarButtonActiveColor,
	const float MenuBarHeightOffset,
	const ImU32 MenuBarColor,
	const ImU32 MenuBarLineColor,
	const ImU32 MenuBarButtonColor,
	const ImU32 MenuBarButtonHoverColor,
	const ImU32 MenuBarButtonActiveColor,
	const float MenuBarButtonPaddingX,
	const float MenuBarButtonPaddingY,
	const float MenuBarButtonAdditionalWidth,
	const ImU32 MenuBarDropdownColor,
	const ImU32 MenuBarDropdownHoverColor,
	const ImU32 MenuBarDropdownActiveColor,
	const float DmiPanelOffsetLeft,
	const float DmiPanelOffsetTop,
	const float DmiPanelOffsetBottom,
	const float DmiPanelRelativeWidth,
	const float DmiPanelTitleBarHeightOffset,
	const float DmiPanelTitleBarTextPaddingX,
	const float DmiPanelTitleBarTextPaddingY,
	const ImU32 DmiPanelColor,
	const ImU32 DmiPanelBorderColor,
	const ImU32 DmiPanelTitleBarColor,
	const ImU32 DmiPanelTitleBarLineColor,
	const float DmiPanelContentOffsetLeft,
	const float DmiPanelContentOffsetTop,
	const float DmiPanelContentOffsetRight,
	const float DmiPanelContentOffsetBottom,
	const ImU32 DmiPanelScrollbarColor,
	const ImU32 DmiPanelScrollbarGrabColor,
	const ImU32 DmiPanelScrollbarGrabHoverColor,
	const ImU32 DmiPanelScrollbarGrabActiveColor,
	const float DmiPanelContentTextPaddingX,
	const float DmiPanelContentTextPaddingY,
	const ImU32 DmiPanelHeaderColor,
	const ImU32 DmiPanelHeaderHoverColor,
	const ImU32 DmiPanelHeaderActiveColor,
	const float StatusPanelOffsetRight,
	const float StatusPanelOffsetTop,
	const float StatusPanelRelativeWidth,
	const float StatusPanelTitleBarHeightOffset,
	const float StatusPanelTitleBarTextPaddingX,
	const float StatusPanelTitleBarTextPaddingY,
	const ImU32 StatusPanelColor,
	const ImU32 StatusPanelBorderColor,
	const ImU32 StatusPanelTitleBarColor,
	const ImU32 StatusPanelTitleBarLineColor,
	const float StatusPanelContentOffsetLeft,
	const float StatusPanelContentOffsetTop,
	const float StatusPanelContentOffsetRight,
	const float StatusPanelContentOffsetBottom,
	const float StatusPanelContentTextPaddingX,
	const float StatusPanelContentTextPaddingY,
	const ImU32 StatusPanelCheckboxColor,
	const ImU32 StatusPanelCheckboxHoverColor,
	const ImU32 StatusPanelCheckboxActiveColor,
	const ImU32 StatusPanelCheckboxCheckmarkColor,
	const float AboutWindowWidth,
	const float AboutWindowHeight,
	const float AboutWindowTitleBarHeightOffset,
	const float AboutWindowTitleBarTextPaddingX,
	const float AboutWindowTitleBarTextPaddingY,
	const ImU32 AboutWindowTitleBarColor,
	const ImU32 AboutWindowTitleBarLineColor,
	const ImU32 AboutWindowColor,
	const float AboutWindowTitleBarButtonWidth,
	const float AboutWindowContentTextPaddingX,
	const float AboutWindowContentTextPaddingY,
	const ImU32 AboutWindowTitleBarButtonColor,
	const ImU32 AboutWindowTitleBarButtonHoverColor,
	const ImU32 AboutWindowTitleBarButtonActiveColor
) -> void
{
	const auto TextLineHeight = ImGui::GetTextLineHeight();
	const auto WindowSize = ImGui::GetWindowSize();

	gLayout.TitleBarHeight = TextLineHeight + TitleBarHeightOffset;
	gLayout.TitleBarColor = TitleBarColor;
	gLayout.TitleBarLineColor = TitleBarLineColor;
	gLayout.TitleBarButtonWidth = TitleBarButtonWidth;
	gLayout.TitleBarTextPadding = ImVec2(TitleBarTextPaddingX, (gLayout.TitleBarHeight - TextLineHeight) * TitleBarTextPaddingY);
	gLayout.TitleBarButtonColor = TitleBarButtonColor;
	gLayout.TitleBarButtonHoverColor = TitleBarButtonHoverColor;
	gLayout.TitleBarButtonActiveColor = TitleBarButtonActiveColor;

	gLayout.MenuBarHeight = TextLineHeight + MenuBarHeightOffset;
	gLayout.MenuBarColor = MenuBarColor;
	gLayout.MenuBarLineColor = MenuBarLineColor;
	gLayout.MenuBarButtonColor = MenuBarButtonColor;
	gLayout.MenuBarButtonHoverColor = MenuBarButtonHoverColor;
	gLayout.MenuBarButtonActiveColor = MenuBarButtonActiveColor;
	gLayout.MenuBarButtonPadding = ImVec2(MenuBarButtonPaddingX, MenuBarButtonPaddingY);
	gLayout.MenuBarButtonAdditionalWidth = MenuBarButtonAdditionalWidth;
	gLayout.MenuBarDropdownColor = MenuBarDropdownColor;
	gLayout.MenuBarDropdownHoverColor = MenuBarDropdownHoverColor;
	gLayout.MenuBarDropdownActiveColor = MenuBarDropdownActiveColor;

	gLayout.DmiPanelOffsetLeft = DmiPanelOffsetLeft;
	gLayout.DmiPanelOffsetTop = DmiPanelOffsetTop;
	gLayout.DmiPanelOffsetBottom = DmiPanelOffsetBottom;
	gLayout.DmiPanelWidth = WindowSize.x * DmiPanelRelativeWidth - DmiPanelOffsetLeft;
	gLayout.DmiPanelTitleBarHeight = TextLineHeight + DmiPanelTitleBarHeightOffset;
	gLayout.DmiPanelTitleBarTextPadding = ImVec2(DmiPanelTitleBarTextPaddingX, DmiPanelTitleBarTextPaddingY);
	gLayout.DmiPanelColor = DmiPanelColor;
	gLayout.DmiPanelBorderColor = DmiPanelBorderColor;
	gLayout.DmiPanelTitleBarColor = DmiPanelTitleBarColor;
	gLayout.DmiPanelTitleBarLineColor = DmiPanelTitleBarLineColor;
	gLayout.DmiPanelContentOffsetLeft = DmiPanelContentOffsetLeft;
	gLayout.DmiPanelContentOffsetTop = DmiPanelContentOffsetTop;
	gLayout.DmiPanelContentOffsetRight = DmiPanelContentOffsetRight;
	gLayout.DmiPanelContentOffsetBottom = DmiPanelContentOffsetBottom;
	gLayout.DmiPanelScrollbarColor = DmiPanelScrollbarColor;
	gLayout.DmiPanelScrollbarGrabColor = DmiPanelScrollbarGrabColor;
	gLayout.DmiPanelScrollbarGrabHoverColor = DmiPanelScrollbarGrabHoverColor;
	gLayout.DmiPanelScrollbarGrabActiveColor = DmiPanelScrollbarGrabActiveColor;
	gLayout.DmiPanelContentTextPadding = ImVec2(DmiPanelContentTextPaddingX, DmiPanelContentTextPaddingY);
	gLayout.DmiPanelHeaderColor = DmiPanelHeaderColor;
	gLayout.DmiPanelHeaderHoverColor = DmiPanelHeaderHoverColor;
	gLayout.DmiPanelHeaderActiveColor = DmiPanelHeaderActiveColor;

	gLayout.StatusPanelOffsetRight = StatusPanelOffsetRight;
	gLayout.StatusPanelOffsetTop = StatusPanelOffsetTop;
	gLayout.StatusPanelWidth = (WindowSize.x * StatusPanelRelativeWidth) - (2 * StatusPanelOffsetRight);
	gLayout.StatusPanelTitleBarHeight = TextLineHeight + StatusPanelTitleBarHeightOffset;
	gLayout.StatusPanelTitleBarTextPadding = ImVec2(StatusPanelTitleBarTextPaddingX, StatusPanelTitleBarTextPaddingY);
	gLayout.StatusPanelColor = StatusPanelColor;
	gLayout.StatusPanelBorderColor = StatusPanelBorderColor;
	gLayout.StatusPanelTitleBarColor = StatusPanelTitleBarColor;
	gLayout.StatusPanelTitleBarLineColor = StatusPanelTitleBarLineColor;
	gLayout.StatusPanelContentOffsetLeft = StatusPanelContentOffsetLeft;
	gLayout.StatusPanelContentOffsetTop = StatusPanelContentOffsetTop;
	gLayout.StatusPanelContentOffsetRight = StatusPanelContentOffsetRight;
	gLayout.StatusPanelContentOffsetBottom = StatusPanelContentOffsetBottom;
	gLayout.StatusPanelContentTextPadding = ImVec2(StatusPanelContentTextPaddingX, StatusPanelContentTextPaddingY);
	gLayout.StatusPanelCheckboxColor = StatusPanelCheckboxColor;
	gLayout.StatusPanelCheckboxHoverColor = StatusPanelCheckboxHoverColor;
	gLayout.StatusPanelCheckboxActiveColor = StatusPanelCheckboxActiveColor;
	gLayout.StatusPanelCheckboxCheckmarkColor = StatusPanelCheckboxCheckmarkColor;

	gLayout.AboutWindowWidth = AboutWindowWidth;
	gLayout.AboutWindowHeight = AboutWindowHeight;
	gLayout.AboutWindowTitleBarHeight = TextLineHeight + AboutWindowTitleBarHeightOffset;
	gLayout.AboutWindowTitleBarTextPadding = ImVec2(AboutWindowTitleBarTextPaddingX, AboutWindowTitleBarTextPaddingY);
	gLayout.AboutWindowTitleBarColor = AboutWindowTitleBarColor;
	gLayout.AboutWindowTitleBarLineColor = AboutWindowTitleBarLineColor;
	gLayout.AboutWindowColor = AboutWindowColor;
	gLayout.AboutWindowTitleBarButtonWidth = AboutWindowTitleBarButtonWidth;
	gLayout.AboutWindowContentTextPadding = ImVec2(AboutWindowContentTextPaddingX, AboutWindowContentTextPaddingY);
	gLayout.AboutWindowTitleBarButtonColor = AboutWindowTitleBarButtonColor;
	gLayout.AboutWindowTitleBarButtonHoverColor = AboutWindowTitleBarButtonHoverColor;
	gLayout.AboutWindowTitleBarButtonActiveColor = AboutWindowTitleBarButtonActiveColor;
}

static auto DrawTitleBar(HWND hWnd, APP_STATE& State) -> void
{
	// Get the draw list, position and size of the current window.
	const auto WindowPos = ImGui::GetWindowPos();
	const auto WindowSize = ImGui::GetWindowSize();
	const auto DrawList = ImGui::GetWindowDrawList();

	// Draw a filled rectangle for the title bar.
	DrawList->AddRectFilled(
		WindowPos,
		ImVec2(WindowPos.x + WindowSize.x, WindowPos.y + gLayout.TitleBarHeight),
		gLayout.TitleBarColor
	);

	// Draw a line at the bottom of the title bar.
	DrawList->AddLine(
		ImVec2(WindowPos.x, WindowPos.y + gLayout.TitleBarHeight),
		ImVec2(WindowPos.x + WindowSize.x, WindowPos.y + gLayout.TitleBarHeight),
		gLayout.TitleBarLineColor
	);

	// Add an invisible button to the title bar area to handle dragging the window.
	ImGui::SetCursorPos(
		ImVec2(0.0f, 0.0f)
	);
	ImGui::InvisibleButton(
		"TitleBarDragZone",
		ImVec2(WindowSize.x - gLayout.TitleBarButtonWidth * 2.0f, gLayout.TitleBarHeight)
	);

	// Handle dragging.
	if (ImGui::IsItemActive() && ImGui::IsMouseDragging(ImGuiMouseButton_Left))
	{
		if (!State.Window.Dragging)
		{
			Window::BeginWindowDrag(hWnd, State.Window);
		}

		Window::UpdateWindowDrag(hWnd, State.Window);
	}
	else
	{
		Window::EndWindowDrag(State.Window);
	}

	// Build the title text.
	std::string TitleText = "Lenovo DMI Decryptor " + gVersionString;

	if (State.FileInfo.Loaded && !State.FileInfo.FileName.empty())
	{
		TitleText += " - ";
		TitleText += State.FileInfo.FileName;

		if (State.FileInfo.Dirty)
		{
			TitleText += "*";
		}
	}

	// Set the padding for the title text.
	ImGui::SetCursorPos(
		gLayout.TitleBarTextPadding
	);

	// Print the text on the title bar.
	ImGui::TextUnformatted(
		TitleText.c_str()
	);

	// Set the color for the title bar buttons.
	ImGui::PushStyleColor(ImGuiCol_Button, gLayout.TitleBarButtonColor);
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, gLayout.TitleBarButtonHoverColor);
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, gLayout.TitleBarButtonActiveColor);

	// Set the cursor position for the minimize button.
	ImGui::SetCursorPos(
		ImVec2(WindowSize.x - gLayout.TitleBarButtonWidth * 2.0f, 0.0f)
	);

	// Handle minimize button.
	if (ImGui::Button("-", ImVec2(gLayout.TitleBarButtonWidth, gLayout.TitleBarHeight)))
	{
		ShowWindow(hWnd, SW_MINIMIZE);
	}

	// Set the cursor position for the close button.
	ImGui::SetCursorPos(
		ImVec2(WindowSize.x - gLayout.TitleBarButtonWidth, 0.0f)
	);

	// Handle close button.
	if (ImGui::Button("X", ImVec2(gLayout.TitleBarButtonWidth, gLayout.TitleBarHeight)))
	{
		if (Window::HandleUnsavedChanges(hWnd, State.Window))
		{
			DestroyWindow(hWnd);
		}
	}

	// Pop the style colors for the buttons.
	ImGui::PopStyleColor(3);
}

static auto OpenImageFile(HWND hWnd, APP_STATE& State) -> bool
{
	if (!Window::HandleUnsavedChanges(hWnd, State.Window))
	{
		return false;
	}

	std::string FilePath;
	auto Result = File::SelectFile(FilePath);

	if (!Result)
	{
		return false;
	}

	Result = File::OpenFile(FilePath, State.FileInfo);

	if (!Result)
	{
		MessageBox(hWnd, _T("Failed to open the selected file."), _T("Error"), MB_ICONERROR);
		return false;
	}

	return Dmi::ReloadBlocks(hWnd, State);
}

static auto DrawAboutWindow() -> void
{
	if (gOpenAboutPopup)
	{
		ImGui::OpenPopup("AboutPopup");
		gOpenAboutPopup = false;
	}

	ImGui::SetNextWindowSize(
		ImVec2(gLayout.AboutWindowWidth, gLayout.AboutWindowHeight),
		ImGuiCond_Appearing
	);

	ImGui::SetNextWindowPos(
		ImGui::GetMainViewport()->GetCenter(),
		ImGuiCond_Appearing,
		ImVec2(0.5f, 0.5f)
	);

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::PushStyleColor(ImGuiCol_PopupBg, gLayout.AboutWindowColor);
	ImGui::PushStyleColor(ImGuiCol_Border, gLayout.DmiPanelBorderColor);

	if (ImGui::BeginPopupModal(
		"AboutPopup",
		nullptr,
		ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoSavedSettings))
	{
		auto WindowPos = ImGui::GetWindowPos();
		auto WindowSize = ImGui::GetWindowSize();
		auto DrawList = ImGui::GetWindowDrawList();

		DrawList->AddRectFilled(
			WindowPos,
			ImVec2(WindowPos.x + WindowSize.x, WindowPos.y + gLayout.AboutWindowTitleBarHeight),
			gLayout.AboutWindowTitleBarColor
		);

		DrawList->AddRectFilled(
			ImVec2(WindowPos.x, floorf(WindowPos.y + gLayout.AboutWindowTitleBarHeight)),
			ImVec2(WindowPos.x + WindowSize.x, floorf(WindowPos.y + gLayout.AboutWindowTitleBarHeight) + 1.0f),
			gLayout.AboutWindowTitleBarLineColor
		);

		ImGui::SetCursorPos(ImVec2(0.0f, 0.0f));
		ImGui::InvisibleButton(
			"AboutTitleBarDragZone",
			ImVec2(
				WindowSize.x - gLayout.AboutWindowTitleBarButtonWidth,
				gLayout.AboutWindowTitleBarHeight
			)
		);

		if (ImGui::IsItemActive() && ImGui::IsMouseDragging(ImGuiMouseButton_Left))
		{
			ImGui::SetWindowPos(ImVec2(
				WindowPos.x + ImGui::GetIO().MouseDelta.x,
				WindowPos.y + ImGui::GetIO().MouseDelta.y
			));

			WindowPos = ImGui::GetWindowPos();
			WindowSize = ImGui::GetWindowSize();
		}

		ImGui::SetCursorPos(gLayout.AboutWindowTitleBarTextPadding);
		ImGui::TextUnformatted("About");

		ImGui::PushStyleColor(ImGuiCol_Button, gLayout.AboutWindowTitleBarButtonColor);
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, gLayout.AboutWindowTitleBarButtonHoverColor);
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, gLayout.AboutWindowTitleBarButtonActiveColor);
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, gLayout.MenuBarButtonPadding);

		ImGui::SetCursorPos(ImVec2(
			WindowSize.x - gLayout.AboutWindowTitleBarButtonWidth,
			0.0f
		));

		if (ImGui::Button(
			"X",
			ImVec2(
				gLayout.AboutWindowTitleBarButtonWidth,
				gLayout.AboutWindowTitleBarHeight)))
		{
			ImGui::CloseCurrentPopup();
		}

		ImGui::PopStyleVar();
		ImGui::PopStyleColor(3);

		const float PaddingX = gLayout.AboutWindowContentTextPadding.x;

		ImGui::SetCursorPosY(gLayout.AboutWindowTitleBarHeight + ImGui::GetStyle().ItemSpacing.y);
		ImGui::Indent(PaddingX);

		const float WrapPos = ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x - PaddingX;
		ImGui::PushTextWrapPos(WrapPos);

		ImGui::Text("Lenovo DMI Decryptor %s", gVersionString.c_str());

		{
			const float StartX = ImGui::GetCursorScreenPos().x;
			const float EndX = ImGui::GetWindowPos().x + ImGui::GetWindowWidth() - PaddingX;
			const float Y = floorf(ImGui::GetCursorScreenPos().y);

			DrawList->AddRectFilled(
				ImVec2(StartX, Y),
				ImVec2(EndX, Y + 1.0f),
				gLayout.AboutWindowTitleBarLineColor
			);

			ImGui::Dummy(ImVec2(0.0f, ImGui::GetStyle().ItemSpacing.y * 0.5f));
		}

		ImGui::TextWrapped("Copyright © 2026, Shmurkio");
		ImGui::TextWrapped("License: MIT License");

		const char* Url = "https://github.com/Shmurkio/LenovoDMIDecryptor";
		const char* Label = "github.com/Shmurkio/LenovoDMIDecryptor";

		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.2f, 0.6f, 1.0f, 1.0f));
		ImGui::TextWrapped("%s", Label);
		ImGui::PopStyleColor();

		if (ImGui::IsItemHovered())
		{
			ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);

			const auto Min = ImGui::GetItemRectMin();
			const auto Max = ImGui::GetItemRectMax();
			const float Y = floorf(Max.y - 1.0f);

			DrawList->AddRectFilled(
				ImVec2(Min.x, Y),
				ImVec2(Max.x, Y + 1.0f),
				IM_COL32(100, 150, 255, 255)
			);

			ImGui::SetTooltip("%s", Url);
		}

		if (ImGui::IsItemClicked())
		{
			ShellExecuteA(nullptr, "open", Url, nullptr, nullptr, SW_SHOWNORMAL);
		}

		ImGui::TextWrapped("Discord: shmurkio");

		{
			const float StartX = ImGui::GetCursorScreenPos().x;
			const float EndX = ImGui::GetWindowPos().x + ImGui::GetWindowWidth() - PaddingX;
			const float Y = floorf(ImGui::GetCursorScreenPos().y);

			DrawList->AddRectFilled(
				ImVec2(StartX, Y),
				ImVec2(EndX, Y + 1.0f),
				gLayout.AboutWindowTitleBarLineColor
			);

			ImGui::Dummy(ImVec2(0.0f, ImGui::GetStyle().ItemSpacing.y * 0.5f));
		}

		ImGui::TextWrapped(
			"Disclaimer: I am not responsible for any damage, data loss, "
			"or other issues caused by the use of this software."
		);

		ImGui::PopTextWrapPos();
		ImGui::Unindent(PaddingX);

		ImGui::EndPopup();
	}

	ImGui::PopStyleColor(2);
	ImGui::PopStyleVar();
}

static auto SaveImageFile(HWND hWnd, APP_STATE& State) -> bool
{
	if (!File::SaveFile(State.FileInfo))
	{
		MessageBox(hWnd, _T("Failed to save file."), _T("Error"), MB_ICONERROR);
		return false;
	}

	return true;
}

static auto DrawMenuBar(HWND hWnd, APP_STATE& State) -> void
{
	// Get the draw list, position and size of the current window.
	const auto WindowPos = ImGui::GetWindowPos();
	const auto WindowSize = ImGui::GetWindowSize();
	auto DrawList = ImGui::GetWindowDrawList();

	// Draw a filled rectangle for the menu bar.
	DrawList->AddRectFilled(
		ImVec2(WindowPos.x, WindowPos.y + gLayout.TitleBarHeight),
		ImVec2(WindowPos.x + WindowSize.x, WindowPos.y + gLayout.TitleBarHeight + gLayout.MenuBarHeight),
		gLayout.MenuBarColor
	);

	// Draw a line at the bottom of the menu bar.
	DrawList->AddLine(
		ImVec2(WindowPos.x, WindowPos.y + gLayout.TitleBarHeight + gLayout.MenuBarHeight),
		ImVec2(WindowPos.x + WindowSize.x, WindowPos.y + gLayout.TitleBarHeight + gLayout.MenuBarHeight),
		gLayout.MenuBarLineColor
	);

	// Set the color for the menu bar buttons.
	ImGui::PushStyleColor(ImGuiCol_Button, gLayout.MenuBarButtonColor);
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, gLayout.MenuBarButtonHoverColor);
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, gLayout.MenuBarButtonActiveColor);

	// Set the padding for the menu bar buttons.
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, gLayout.MenuBarButtonPadding);

	// Set the cursor position for the "File" menu button.
	ImGui::SetCursorPos(ImVec2(0.0f, gLayout.TitleBarHeight));

	// Compute the width of the "File" button based on the text size and additional width.
	auto ButtonWidth = ImGui::CalcTextSize("File").x + gLayout.MenuBarButtonAdditionalWidth;

	// Handle "File" button.
	if (ImGui::Button("File", ImVec2(ButtonWidth, gLayout.MenuBarHeight)))
	{
		ImGui::OpenPopup("FileMenu");
	}

	// Compute the position for the "File" dropdown menu.
	const auto FileButtonMin = ImGui::GetItemRectMin();
	const auto FileButtonMax = ImGui::GetItemRectMax();
	const auto FileMenuPos = ImVec2(FileButtonMin.x, FileButtonMax.y);

	// Set the cursor position for the "Help" menu button.
	ImGui::SetCursorPos(ImVec2(ButtonWidth, gLayout.TitleBarHeight));

	// Compute the width of the "Help" button based on the text size and additional width.
	ButtonWidth = ImGui::CalcTextSize("Help").x + gLayout.MenuBarButtonAdditionalWidth;

	// Handle "Help" button.
	if (ImGui::Button("Help", ImVec2(ButtonWidth, gLayout.MenuBarHeight)))
	{
		ImGui::OpenPopup("HelpMenu");
	}

	// Compute the position for the "Help" dropdown menu.
	const auto HelpButtonMin = ImGui::GetItemRectMin();
	const auto HelpButtonMax = ImGui::GetItemRectMax();
	const auto HelpMenuPos = ImVec2(HelpButtonMin.x, HelpButtonMax.y);

	// Pop the style variable for the button padding.
	ImGui::PopStyleVar();

	// Pop the style colors for the buttons.
	ImGui::PopStyleColor(3);

	// Set the colors for the dropdown menu.
	ImGui::PushStyleColor(ImGuiCol_PopupBg, gLayout.MenuBarDropdownColor);
	ImGui::PushStyleColor(ImGuiCol_HeaderHovered, gLayout.MenuBarDropdownHoverColor);
	ImGui::PushStyleColor(ImGuiCol_HeaderActive, gLayout.MenuBarDropdownActiveColor);

	// Set the position for the "File" dropdown menu to appear below the "File" button.
	ImGui::SetNextWindowPos(FileMenuPos, ImGuiCond_Appearing);

	if (ImGui::BeginPopup("FileMenu", ImGuiWindowFlags_NoMove))
	{
		if (ImGui::MenuItem("Open image file"))
		{
			OpenImageFile(hWnd, State);
		}

		if (ImGui::MenuItem("Save image file"))
		{
			SaveImageFile(hWnd, State);
		}

		ImGui::Separator();

		if (ImGui::MenuItem("Quit"))
		{
			if (Window::HandleUnsavedChanges(hWnd, State.Window))
			{
				DestroyWindow(hWnd);
			}
		}

		ImGui::EndPopup();
	}

	// Set the position for the "Help" dropdown menu to appear below the "Help" button.
	ImGui::SetNextWindowPos(HelpMenuPos, ImGuiCond_Appearing);

	if (ImGui::BeginPopup("HelpMenu", ImGuiWindowFlags_NoMove))
	{
		if (ImGui::MenuItem("About"))
		{
			gOpenAboutPopup = true;
		}

		ImGui::EndPopup();
	}

	ImGui::PopStyleColor(3);
}

static auto DrawDmiHeader(Lenovo::CDMI_HEADER& Header) -> void
{
	if (!ImGui::TreeNode("Header"))
	{
		return;
	}

	char Signature[5] = { 0 };
	memcpy(Signature, &Header.Signature, 4);

	ImGui::Text("Signature:");	ImGui::SameLine(300.0f);	ImGui::Text("%s", Signature);
	ImGui::Text("Generation:");	ImGui::SameLine(300.0f);	ImGui::Text("0x%08X", Header.Generation);
	ImGui::Text("Entries:");	ImGui::SameLine(300.0f);	ImGui::Text("0x%08X", Header.Entries);
	ImGui::Text("AccessFlag:");	ImGui::SameLine(300.0f);	ImGui::Text("0x%02X", Header.AccessFlag);
	ImGui::Text("Key:");		ImGui::SameLine(300.0f);	ImGui::Text("0x%02X", Header.Key);
	ImGui::Text("Checksum:");	ImGui::SameLine(300.0f);	ImGui::Text("0x%04X", Header.Checksum);

	ImGui::TreePop();
}

static auto DrawHexRow(const uint8_t* Data, size_t Size) -> void
{
	if (!Data || Size == 0)
	{
		return;
	}

	constexpr size_t BytesPerRow = 16;

	for (size_t Row = 0; Row < Size; Row += BytesPerRow)
	{
		char HexLine[128] = { 0 };
		char AsciiLine[17] = { 0 };

		size_t HexPos = 0;
		size_t Col = 0;

		for (; Col < BytesPerRow && (Row + Col) < Size; ++Col)
		{
			const auto Byte = Data[Row + Col];

			if (Col == 8)
			{
				HexPos += sprintf_s(HexLine + HexPos, sizeof(HexLine) - HexPos, " ");
			}

			HexPos += sprintf_s(
				HexLine + HexPos,
				sizeof(HexLine) - HexPos,
				"%02X ",
				Byte
			);

			AsciiLine[Col] = (Byte >= 32 && Byte <= 126)
				? static_cast<char>(Byte)
				: '.';
		}

		for (; Col < BytesPerRow; ++Col)
		{
			if (Col == 8)
			{
				HexPos += sprintf_s(HexLine + HexPos, sizeof(HexLine) - HexPos, " ");
			}

			HexPos += sprintf_s(
				HexLine + HexPos,
				sizeof(HexLine) - HexPos,
				"   "
			);
		}

		ImGui::Text("%04zX | %s| %s", Row, HexLine, AsciiLine);
	}
}

static auto DrawDmiEntry(Lenovo::PDMI_DATA_ENTRY Entry, uint32_t Index) -> Lenovo::PDMI_DATA_ENTRY
{
	if (!Entry)
	{
		return nullptr;
	}
	
	char EntryLabel[64] = { 0 };
	snprintf(EntryLabel, sizeof(EntryLabel), "Entry[0x%08X]", Index);

	if (ImGui::TreeNode(EntryLabel))
	{
		if (ImGui::TreeNode("Key"))
		{
			if (ImGui::TreeNode("Signature"))
			{
				DrawHexRow(Entry->Key.Signature, 0x0E);
				ImGui::TreePop();
			}

			ImGui::Text("FieldId:");	ImGui::SameLine(300.0f);	ImGui::Text("0x%04X", Entry->Key.FieldId);
			ImGui::TreePop();
		}

		ImGui::Text("DataSize:");	ImGui::SameLine(300.0f);	ImGui::Text("0x%08X", Entry->DataSize);
		ImGui::Text("Flags:");		ImGui::SameLine(300.0f);	ImGui::Text("0x%02X", Entry->Flags);
		ImGui::Text("Unknown1:");	ImGui::SameLine(300.0f);	ImGui::Text("0x%02X", Entry->Unknown1);
		ImGui::Text("Unknown2:");	ImGui::SameLine(300.0f);	ImGui::Text("0x%04X", Entry->Unknown2);

		if (ImGui::TreeNode("Data"))
		{
			DrawHexRow(Entry->Data, Entry->DataSize);
			ImGui::TreePop();
		}

		ImGui::TreePop();
	}

	auto NextEntry = Cast::To<Lenovo::PDMI_DATA_ENTRY>(Cast::To<uint8_t*>(Entry) + offsetof(Lenovo::DMI_DATA_ENTRY, Data) + Entry->DataSize);
	return NextEntry;
}

static auto DrawDmiBody(Lenovo::PDMI_DATA Block, uint32_t DmiBlockSize) -> void
{
	if (!ImGui::TreeNode("Body"))
	{
		return;
	}

	if (!IsEncrypted(Block)) // Only if decrypted
	{
		if (ImGui::TreeNode("Entries"))
		{
			auto Entry = reinterpret_cast<Lenovo::PDMI_DATA_ENTRY>(Block->Entry);

			for (uint32_t i = 0; i < Block->Header.Entries; ++i)
			{
				auto NextEntry = DrawDmiEntry(Entry, i);
				Entry = NextEntry;
			}

			ImGui::TreePop();
		}
	}

	ImGui::TreePop();
}

static auto DrawDmiBlock(const char* Name, Lenovo::PDMI_DATA Block, uint32_t Offset, uint32_t DmiBlockSize) -> void
{
	if (!Block)
	{
		return;
	}

	if (!ImGui::TreeNode(Name))
	{
		return;
	}

	ImGui::Text("Offset:");
	ImGui::SameLine(300.0f);
	ImGui::Text("0x%08X", Offset);

	ImGui::Text("Size:");
	ImGui::SameLine(300.0f);
	ImGui::Text("0x%08X", DmiBlockSize);

	DrawDmiHeader(Block->Header);
	DrawDmiBody(Block, DmiBlockSize);

	ImGui::TreePop();
}

auto DrawDmiPanel(APP_STATE& State) -> void
{
	const auto WindowPos = ImGui::GetWindowPos();
	const auto WindowSize = ImGui::GetWindowSize();
	const auto DrawList = ImGui::GetWindowDrawList();

	const float PanelX = WindowPos.x + gLayout.DmiPanelOffsetLeft;
	const float PanelY = WindowPos.y + gLayout.TitleBarHeight + gLayout.MenuBarHeight + gLayout.DmiPanelOffsetTop;
	const float PanelWidth = gLayout.DmiPanelWidth;
	const float PanelHeight = WindowSize.y - gLayout.TitleBarHeight - gLayout.MenuBarHeight - gLayout.DmiPanelOffsetTop - gLayout.DmiPanelOffsetBottom;

	const auto PanelMin = ImVec2(PanelX, PanelY);
	const auto PanelMax = ImVec2(PanelX + PanelWidth, PanelY + PanelHeight);

	const auto TitleBarMin = PanelMin;
	const auto TitleBarMax = ImVec2(PanelMax.x, PanelMin.y + gLayout.DmiPanelTitleBarHeight);

	// Draw the panel background.
	DrawList->AddRectFilled(
		PanelMin,
		PanelMax,
		gLayout.DmiPanelColor
	);

	// Draw the panel border.
	DrawList->AddRect(
		PanelMin,
		PanelMax,
		gLayout.DmiPanelBorderColor
	);

	// Draw the title bar background.
	DrawList->AddRectFilled(
		TitleBarMin,
		TitleBarMax,
		gLayout.DmiPanelTitleBarColor
	);

	// Draw the title bar line.
	DrawList->AddLine(
		ImVec2(TitleBarMin.x, TitleBarMax.y),
		ImVec2(TitleBarMax.x, TitleBarMax.y),
		gLayout.DmiPanelTitleBarLineColor
	);

	// Set the padding for the title text.
	ImGui::SetCursorPos(
		ImVec2(
			gLayout.DmiPanelOffsetLeft + gLayout.DmiPanelTitleBarTextPadding.x,
			gLayout.TitleBarHeight + gLayout.MenuBarHeight + gLayout.DmiPanelOffsetTop + gLayout.DmiPanelTitleBarTextPadding.y
		)
	);

	// Print the title text on the title bar.
	ImGui::TextUnformatted("DMI Information");

	// Content area below the title bar.
	const float ContentX = PanelMin.x + gLayout.DmiPanelContentOffsetLeft;
	const float ContentY = TitleBarMax.y + gLayout.DmiPanelContentOffsetTop;
	const float ContentWidth = PanelWidth - gLayout.DmiPanelContentOffsetLeft - gLayout.DmiPanelContentOffsetRight;
	const float ContentHeight = PanelHeight - gLayout.DmiPanelTitleBarHeight - gLayout.DmiPanelContentOffsetTop - gLayout.DmiPanelContentOffsetBottom;

	// Place the child into the content area.
	ImGui::SetCursorPos(
		ImVec2(
			gLayout.DmiPanelOffsetLeft + gLayout.DmiPanelContentOffsetLeft,
			gLayout.TitleBarHeight + gLayout.MenuBarHeight + gLayout.DmiPanelOffsetTop + gLayout.DmiPanelTitleBarHeight + gLayout.DmiPanelContentOffsetTop
		)
	);

	// Set the colors for the scrollbars.
	ImGui::PushStyleColor(ImGuiCol_ScrollbarBg, gLayout.DmiPanelScrollbarColor);
	ImGui::PushStyleColor(ImGuiCol_ScrollbarGrab, gLayout.DmiPanelScrollbarGrabColor);
	ImGui::PushStyleColor(ImGuiCol_ScrollbarGrabHovered, gLayout.DmiPanelScrollbarGrabHoverColor);
	ImGui::PushStyleColor(ImGuiCol_ScrollbarGrabActive, gLayout.DmiPanelScrollbarGrabActiveColor);

	// Set the border color to transparent for the child region.
	ImGui::PushStyleColor(ImGuiCol_Border, IM_COL32(0, 0, 0, 0));

	// Set the padding for the content text.
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, gLayout.DmiPanelContentTextPadding);

	// Set the color for the headers in the content area.
	ImGui::PushStyleColor(ImGuiCol_Header, gLayout.DmiPanelHeaderColor);
	ImGui::PushStyleColor(ImGuiCol_HeaderHovered, gLayout.DmiPanelHeaderHoverColor);
	ImGui::PushStyleColor(ImGuiCol_HeaderActive, gLayout.DmiPanelHeaderActiveColor);

	// Use a child region for scrolling.
	if (ImGui::BeginChild(
		"DmiPanelScroll",
		ImVec2(ContentWidth, ContentHeight),
		true,
		ImGuiWindowFlags_AlwaysVerticalScrollbar)
	)
	{
		if (State.FileInfo.Loaded)
		{
			auto Offset = Cast::To<uint32_t>(Cast::To<uint8_t*>(State.DmiBlock1) - State.FileInfo.Buffer);
			if (State.DmiBlock1Found) DrawDmiBlock("DMI Block 1", State.DmiBlock1, Offset, State.DmiBlockSize);
			Offset = Cast::To<uint32_t>(Cast::To<uint8_t*>(State.DmiBlock2) - State.FileInfo.Buffer);
			if (State.DmiBlock2Found) DrawDmiBlock("DMI Block 2", State.DmiBlock2, Offset, State.DmiBlockSize);
		}
	}

	// Pop the style variable for the content text padding.
	ImGui::PopStyleVar();

	// Pop the style colors for the scrollbars, headers, and the border.
	ImGui::PopStyleColor(8);

	// End the child region.
	ImGui::EndChild();
}

static auto DrawStatusPanelContent(HWND hWnd, APP_STATE& State, bool MeasureOnly) -> void
{
	auto Block1Encrypted = IsEncrypted(State.DmiBlock1);
	auto Block2Encrypted = IsEncrypted(State.DmiBlock2);

	const auto FileLoaded = State.FileInfo.Loaded;
	const auto Block1Found = FileLoaded && State.DmiBlock1Found;
	const auto Block2Found = FileLoaded && State.DmiBlock2Found;

	auto Block1ChecksumInvalid = false;
	auto Block2ChecksumInvalid = false;

	if (Block1Found)
	{
		auto Checksum = Util::GetChecksum16(
			Cast::To<uint8_t*>(&State.DmiBlock1->Entry),
			State.DmiBlockSize - sizeof(Lenovo::DMI_HEADER)
		);
		if (Checksum != State.DmiBlock1->Header.Checksum) Block1ChecksumInvalid = true;
	}

	if (Block2Found)
	{
		auto Checksum = Util::GetChecksum16(
			Cast::To<uint8_t*>(&State.DmiBlock2->Entry),
			State.DmiBlockSize - sizeof(Lenovo::DMI_HEADER)
		);
		if (Checksum != State.DmiBlock2->Header.Checksum) Block2ChecksumInvalid = true;
	}

	const auto Block1Clickable = Block1Found;
	const auto Block2Clickable = Block2Found;

	// Block 1.
	if (MeasureOnly)
	{
		ImGui::Dummy(ImVec2(ImGui::GetFrameHeight(), ImGui::GetFrameHeight()));
	}
	else
	{
		ImGui::BeginDisabled(!Block1Clickable);

		if (ImGui::Checkbox("##Block1Encrypted", &Block1Encrypted))
		{
			auto Body = Cast::To<uint8_t*>(State.DmiBlock1) + sizeof(Lenovo::DMI_HEADER);
			auto BodySize = static_cast<size_t>(State.DmiBlockSize - sizeof(Lenovo::DMI_HEADER));

			if (!Util::XorBuffer(Body, BodySize, State.DmiBlock1->Header.Key))
			{
				MessageBox(hWnd, _T("Failed to XOR DMI Block 1."), _T("Error"), MB_ICONERROR);
			}
			else
			{
				State.DmiBlock1->Header.Checksum = Util::GetChecksum16(
					Cast::To<uint8_t*>(&State.DmiBlock1->Entry),
					State.DmiBlockSize - sizeof(Lenovo::DMI_HEADER)
				);
				State.FileInfo.Dirty = true;
			}
		}

		ImGui::EndDisabled();
	}

	ImGui::SameLine();
	ImGui::TextUnformatted("Block 1 encrypted");

	if (FileLoaded && !State.DmiBlock1Found)
	{
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(1.0f, 0.25f, 0.25f, 1.0f), "Not found");
	}
	else if (Block1ChecksumInvalid)
	{
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(1.0f, 0.25f, 0.25f, 1.0f), "Checksum invalid");
	}

	// Block 2.
	if (MeasureOnly)
	{
		ImGui::Dummy(ImVec2(ImGui::GetFrameHeight(), ImGui::GetFrameHeight()));
	}
	else
	{
		ImGui::BeginDisabled(!Block2Clickable);

		if (ImGui::Checkbox("##Block2Encrypted", &Block2Encrypted))
		{
			auto Body = Cast::To<uint8_t*>(State.DmiBlock2) + sizeof(Lenovo::DMI_HEADER);
			auto BodySize = static_cast<size_t>(State.DmiBlockSize - sizeof(Lenovo::DMI_HEADER));

			if (!Util::XorBuffer(Body, BodySize, State.DmiBlock2->Header.Key))
			{
				MessageBox(hWnd, _T("Failed to XOR DMI Block 2."), _T("Error"), MB_ICONERROR);
			}
			else
			{
				State.DmiBlock2->Header.Checksum = Util::GetChecksum16(
					Cast::To<uint8_t*>(&State.DmiBlock2->Entry),
					State.DmiBlockSize - sizeof(Lenovo::DMI_HEADER)
				);
				State.FileInfo.Dirty = true;
			}
		}

		ImGui::EndDisabled();
	}

	ImGui::SameLine();
	ImGui::TextUnformatted("Block 2 encrypted");

	if (FileLoaded && !State.DmiBlock2Found)
	{
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(1.0f, 0.25f, 0.25f, 1.0f), "Not found");
	}
	else if (Block2ChecksumInvalid)
	{
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(1.0f, 0.25f, 0.25f, 1.0f), "Checksum invalid");
	}
}

static auto DrawStatusPanel(HWND hWnd, APP_STATE& State) -> void
{
	const auto WindowPos = ImGui::GetWindowPos();
	const auto WindowSize = ImGui::GetWindowSize();
	const auto DrawList = ImGui::GetWindowDrawList();

	const float PanelX = WindowPos.x + WindowSize.x - gLayout.StatusPanelOffsetRight - gLayout.StatusPanelWidth;
	const float PanelY = WindowPos.y + gLayout.TitleBarHeight + gLayout.MenuBarHeight + gLayout.StatusPanelOffsetTop;
	const float PanelWidth = gLayout.StatusPanelWidth;

	// Measure the content height first.
	float ContentHeight = 0.0f;
	{
		const auto OldCursorPos = ImGui::GetCursorPos();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, gLayout.StatusPanelContentTextPadding);

		// Put measurement off-screen so it doesn't affect the visible layout.
		ImGui::SetCursorPos(ImVec2(-10000.0f, -10000.0f));

		ImGui::BeginGroup();
		DrawStatusPanelContent(hWnd, State, true);
		ImGui::EndGroup();

		ContentHeight = ImGui::GetItemRectSize().y;

		ImGui::SetCursorPos(OldCursorPos);
		ImGui::PopStyleVar();
	}

	const float ChildHeight = ContentHeight + (gLayout.StatusPanelContentTextPadding.y * 2.0f);

	const float PanelHeight = gLayout.StatusPanelTitleBarHeight + gLayout.StatusPanelContentOffsetTop + ChildHeight + gLayout.StatusPanelContentOffsetBottom;

	const auto PanelMin = ImVec2(PanelX, PanelY);
	const auto PanelMax = ImVec2(PanelX + PanelWidth, PanelY + PanelHeight);

	const auto TitleBarMin = PanelMin;
	const auto TitleBarMax = ImVec2(PanelMax.x, PanelMin.y + gLayout.StatusPanelTitleBarHeight);

	// Draw the panel background.
	DrawList->AddRectFilled(
		PanelMin,
		PanelMax,
		gLayout.StatusPanelColor
	);

	// Draw the panel border.
	DrawList->AddRect(
		PanelMin,
		PanelMax,
		gLayout.StatusPanelBorderColor
	);

	// Draw the title bar background.
	DrawList->AddRectFilled(
		TitleBarMin,
		TitleBarMax,
		gLayout.StatusPanelTitleBarColor
	);

	// Draw the title bar line.
	DrawList->AddLine(
		ImVec2(TitleBarMin.x, TitleBarMax.y),
		ImVec2(TitleBarMax.x, TitleBarMax.y),
		gLayout.StatusPanelTitleBarLineColor
	);

	// Draw the title text.
	ImGui::SetCursorPos(
		ImVec2(PanelX + gLayout.StatusPanelTitleBarTextPadding.x, PanelY + gLayout.StatusPanelTitleBarTextPadding.y)
	);

	ImGui::TextUnformatted("Status");

	// Content area below the title bar.
	const float ChildWidth = PanelWidth - gLayout.StatusPanelContentOffsetLeft - gLayout.StatusPanelContentOffsetRight;

	//const float ChildHeight = ContentHeight;

	// Place the child in the content area.
	ImGui::SetCursorPos(
		ImVec2(WindowSize.x - gLayout.StatusPanelOffsetRight - gLayout.StatusPanelWidth + gLayout.StatusPanelContentOffsetLeft, gLayout.TitleBarHeight + gLayout.MenuBarHeight + gLayout.StatusPanelOffsetTop + gLayout.StatusPanelTitleBarHeight + gLayout.StatusPanelContentOffsetTop)
	);

	// Style for the status content.
	ImGui::PushStyleColor(ImGuiCol_FrameBg, gLayout.StatusPanelCheckboxColor);
	ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, gLayout.StatusPanelCheckboxHoverColor);
	ImGui::PushStyleColor(ImGuiCol_FrameBgActive, gLayout.StatusPanelCheckboxActiveColor);
	ImGui::PushStyleColor(ImGuiCol_CheckMark, gLayout.StatusPanelCheckboxCheckmarkColor);
	ImGui::PushStyleColor(ImGuiCol_Border, IM_COL32(0, 0, 0, 0));
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, gLayout.StatusPanelContentTextPadding);

	if (ImGui::BeginChild(
		"StatusPanelContent",
		ImVec2(ChildWidth, ChildHeight),
		true,
		ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse
	))
	{
		DrawStatusPanelContent(hWnd, State, false);
	}

	ImGui::EndChild();

	ImGui::PopStyleVar();
	ImGui::PopStyleColor(5);
}

auto UI::DrawMainWindow(HWND hWnd, APP_STATE& State) -> void
{
	// Set the main window to cover the entire viewport.
	auto Viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(Viewport->Pos);
	ImGui::SetNextWindowSize(Viewport->Size);

	// Set the window flags to disable title bar, resizing, moving, and collapsing.
	const auto Flags =
		ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoCollapse;

	// Begin the main window with the specified flags.
	ImGui::Begin("MainWindow", nullptr, Flags);

	// Build the layout of the UI.
	BuildLayout(
		gTitleBarHeightOffset,
		gTitleBarColor,
		gTitleBarLineColor,
		gTitleBarButtonWidth,
		gTitleBarTextPaddingX,
		gTitleBarTextPaddingY,
		gTitleBarButtonColor,
		gTitleBarButtonHoverColor,
		gTitleBarButtonActiveColor,
		gMenuBarHeightOffset,
		gMenuBarColor,
		gMenuBarLineColor,
		gMenuBarButtonColor,
		gMenuBarButtonHoverColor,
		gMenuBarButtonActiveColor,
		gMenuBarButtonPaddingX,
		gMenuBarButtonPaddingY,
		gMenuBarButtonAdditionalWidth,
		gMenuBarDropdownColor,
		gMenuBarDropdownHoverColor,
		gMenuBarDropdownActiveColor,
		gDmiPanelOffsetLeft,
		gDmiPanelOffsetTop,
		gDmiPanelOffsetBottom,
		gDmiPanelRelativeWidth,
		gDmiPanelTitleBarHeightOffset,
		gDmiPanelTitleBarTextPaddingX,
		gDmiPanelTitleBarTextPaddingY,
		gDmiPanelColor,
		gDmiPanelBorderColor,
		gDmiPanelTitleBarColor,
		gDmiPanelTitleBarLineColor,
		gDmiPanelContentOffsetLeft,
		gDmiPanelContentOffsetTop,
		gDmiPanelContentOffsetRight,
		gDmiPanelContentOffsetBottom,
		gDmiPanelScrollbarColor,
		gDmiPanelScrollbarGrabColor,
		gDmiPanelScrollbarGrabHoverColor,
		gDmiPanelScrollbarGrabActiveColor,
		gDmiPanelContentTextPaddingX,
		gDmiPanelContentTextPaddingY,
		gDmiPanelHeaderColor,
		gDmiPanelHeaderHoverColor,
		gDmiPanelHeaderActiveColor,
		gStatusPanelOffsetRight,
		gStatusPanelOffsetTop,
		gStatusPanelRelativeWidth,
		gStatusPanelTitleBarHeightOffset,
		gStatusPanelTitleBarTextPaddingX,
		gStatusPanelTitleBarTextPaddingY,
		gStatusPanelColor,
		gStatusPanelBorderColor,
		gStatusPanelTitleBarColor,
		gStatusPanelTitleBarLineColor,
		gStatusPanelContentOffsetLeft,
		gStatusPanelContentOffsetTop,
		gStatusPanelContentOffsetRight,
		gStatusPanelContentOffsetBottom,
		gStatusPanelContentTextPaddingX,
		gStatusPanelContentTextPaddingY,
		gStatusPanelCheckboxColor,
		gStatusPanelCheckboxHoverColor,
		gStatusPanelCheckboxActiveColor,
		gStatusPanelCheckboxCheckmarkColor,
		gAboutWindowWidth,
		gAboutWindowHeight,
		gAboutWindowTitleBarHeightOffset,
		gAboutWindowTitleBarTextPaddingX,
		gAboutWindowTitleBarTextPaddingY,
		gAboutWindowTitleBarColor,
		gAboutWindowTitleBarLineColor,
		gAboutWindowColor,
		gAboutWindowTitleBarButtonWidth,
		gAboutWindowContentTextPaddingX,
		gAboutWindowContentTextPaddingY,
		gAboutWindowTitleBarButtonColor,
		gAboutWindowTitleBarButtonHoverColor,
		gAboutWindowTitleBarButtonActiveColor
	);

	// Draw the title bar of the window.
	DrawTitleBar(hWnd, State);

	// Draw the menu bar of the window.
	DrawMenuBar(hWnd, State);

	// Draw the DMI panel.
	DrawDmiPanel(State);

	// Draw the status panel.
	DrawStatusPanel(hWnd, State);

	// Draw the about window if it's open.
	DrawAboutWindow();

	// End the main window.
	ImGui::End();
}