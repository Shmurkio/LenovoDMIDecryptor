#include "UI.hpp"

#include "imgui.h"
#include "DMI.hpp"
#include "Cast.hpp"
#include "Util.hpp"

struct TITLE_BAR_LAYOUT
{
	float	Height;
	ImU32	Color;
	ImU32	LineColor;
	float	ButtonWidth;
	ImU32	ButtonColor;
	ImU32	ButtonHoverColor;
	ImU32	ButtonActiveColor;
	ImVec2	TextPadding;
};

struct MENU_BAR_LAYOUT
{
	float	Height;
	ImU32	Color;
	ImU32	LineColor;
	ImU32	ButtonColor;
	ImU32	ButtonHoverColor;
	ImU32	ButtonActiveColor;
	ImVec2	ButtonPadding;
	float	ButtonAdditionalWidth;
	ImU32	DropdownColor;
	ImU32	DropdownHoverColor;
	ImU32	DropdownActiveColor;
};

struct ABOUT_WINDOW_LAYOUT
{
	float	Width;
	float	Height;
	ImU32	BorderColor;
	ImVec2	Padding;
	float	Spacing;
	float	LabelOffset;
	ImU32	TextLinkOpenedColor;
	ImU32	TextLinkDefaultColor;

	float	TitleBarHeight;
	ImU32	TitleBarColor;
	ImU32	TitleBarLineColor;
	float	CloseButtonWidth;
	ImU32	CloseButtonColor;
	ImU32	CloseButtonHoverColor;
	ImU32	CloseButtonActiveColor;
	ImVec2	TitleTextPadding;
};

struct STRUCTURE_PANEL_LAYOUT
{
	float	Width;
	float	Height;
	float	TitleBarHeight;
	ImVec2	Position;
	ImVec2	Padding;
	ImVec2	TitleTextPadding;
	ImU32	TitleBarColor;
	ImU32	TitleBarLineColor;
	ImU32	ScrollbarColor;
	ImU32	ScrollbarGrabColor;
	ImU32	ScrollbarGrabHoverColor;
	ImU32	ScrollbarGrabActiveColor;
	ImU32	Color;
	ImU32	HeaderColor;
	ImU32	HeaderHoverColor;
	ImU32	HeaderActiveColor;
	ImU32	TextPriorityColor;
	ImU32	SmbiosNamespaceColor;
	ImU32	OtherNamespaceColor;
};

struct STATE_PANEL_LAYOUT
{
	float	Width;
	float	Height;
	float	TitleBarHeight;
	ImVec2	Position;
	ImVec2	Padding;
	ImVec2	TitleTextPadding;
	ImU32	TitleBarColor;
	ImU32	TitleBarLineColor;
	ImU32	Color;
	ImU32	CheckboxColor;
	ImU32	CheckboxHoverColor;
	ImU32	CheckboxActiveColor;
	ImU32	CheckboxCheckmarkColor;
};

struct UI_LAYOUT
{
	TITLE_BAR_LAYOUT		TitleBar;
	MENU_BAR_LAYOUT			MenuBar;
	ABOUT_WINDOW_LAYOUT		AboutWindow;
	STRUCTURE_PANEL_LAYOUT	StructurePanel;
	STATE_PANEL_LAYOUT		StatePanel;
};

static UI_LAYOUT						gLayout = { 0 };
static std::string						gVersionString = "1.4.5";
static bool								gOpenAboutPopup = false;
static double							gDiscordCopiedTime = 0.0;
static std::unordered_set<std::string>	gCopiedDiscordUsers;
static std::unordered_set<std::string>	gOpenedUrls;
static const char*						gLastCopiedDiscord = nullptr;
static bool								gAboutPopupWasOpen = false;

static auto BuildTitleBar(
	float	HeightOffset,
	ImU32	Color,
	ImU32	LineColor,
	float	ButtonWidth,
	ImU32	ButtonColor,
	ImU32	ButtonHoverColor,
	ImU32	ButtonActiveColor,
	float	TextPaddingX,
	float	TextPaddingY
) -> void
{
	const auto TextLineHeight			= ImGui::GetTextLineHeight();
	const auto WindowSize				= ImGui::GetWindowSize();

	gLayout.TitleBar.Height				= TextLineHeight + HeightOffset;
	gLayout.TitleBar.Color				= Color;
	gLayout.TitleBar.LineColor			= LineColor;
	gLayout.TitleBar.ButtonWidth		= ButtonWidth;
	gLayout.TitleBar.ButtonColor		= ButtonColor;
	gLayout.TitleBar.ButtonHoverColor	= ButtonHoverColor;
	gLayout.TitleBar.ButtonActiveColor	= ButtonActiveColor;
	gLayout.TitleBar.TextPadding		= ImVec2(TextPaddingX, (gLayout.TitleBar.Height - TextLineHeight) * TextPaddingY);
}

static auto BuildMenuBar(
	float	HeightOffset,
	ImU32	Color,
	ImU32	LineColor,
	ImU32	ButtonColor,
	ImU32	ButtonHoverColor,
	ImU32	ButtonActiveColor,
	float	ButtonPaddingX,
	float	ButtonPaddingY,
	float	ButtonAdditionalWidth,
	ImU32	DropdownColor,
	ImU32	DropdownHoverColor,
	ImU32	DropdownActiveColor
) -> void
{
	const auto TextLineHeight				= ImGui::GetTextLineHeight();
	const auto WindowSize					= ImGui::GetWindowSize();

	gLayout.MenuBar.Height					= TextLineHeight + HeightOffset;
	gLayout.MenuBar.Color					= Color;
	gLayout.MenuBar.LineColor				= LineColor;
	gLayout.MenuBar.ButtonColor				= ButtonColor;
	gLayout.MenuBar.ButtonHoverColor		= ButtonHoverColor;
	gLayout.MenuBar.ButtonActiveColor		= ButtonActiveColor;
	gLayout.MenuBar.ButtonPadding			= ImVec2(ButtonPaddingX, (gLayout.MenuBar.Height - TextLineHeight) * ButtonPaddingY);
	gLayout.MenuBar.ButtonAdditionalWidth	= ButtonAdditionalWidth;
	gLayout.MenuBar.DropdownColor			= DropdownColor;
	gLayout.MenuBar.DropdownHoverColor		= DropdownHoverColor;
	gLayout.MenuBar.DropdownActiveColor		= DropdownActiveColor;
}

static auto BuildAboutWindow(
	float	Width,
	float	Height,
	ImU32	BorderColor,
	float	PaddingX,
	float	PaddingY,
	float	Spacing,
	float	TitleBarHeightOffset,
	ImU32	TitleBarColor,
	ImU32	TitleBarLineColor,
	float	CloseButtonWidth,
	ImU32	CloseButtonColor,
	ImU32	CloseButtonHoverColor,
	ImU32	CloseButtonActiveColor,
	float	TitleTextPaddingX,
	float	TitleTextPaddingY,
	float	LabelOffset,
	ImU32	TextLinkDefaultColor,
	ImU32	TextLinkOpenedColor
) -> void
{
	const auto TextLineHeight					= ImGui::GetTextLineHeight();

	gLayout.AboutWindow.Width					= Width;
	gLayout.AboutWindow.Height					= Height;
	gLayout.AboutWindow.BorderColor				= BorderColor;
	gLayout.AboutWindow.Padding					= ImVec2(PaddingX, PaddingY);
	gLayout.AboutWindow.Spacing					= Spacing;
	gLayout.AboutWindow.LabelOffset				= LabelOffset;
	gLayout.AboutWindow.TextLinkDefaultColor	= TextLinkDefaultColor;
	gLayout.AboutWindow.TextLinkOpenedColor		= TextLinkOpenedColor;

	gLayout.AboutWindow.TitleBarHeight			= TextLineHeight + TitleBarHeightOffset;
	gLayout.AboutWindow.TitleBarColor			= TitleBarColor;
	gLayout.AboutWindow.TitleBarLineColor		= TitleBarLineColor;
	gLayout.AboutWindow.CloseButtonWidth		= CloseButtonWidth;
	gLayout.AboutWindow.CloseButtonColor		= CloseButtonColor;
	gLayout.AboutWindow.CloseButtonHoverColor	= CloseButtonHoverColor;
	gLayout.AboutWindow.CloseButtonActiveColor	= CloseButtonActiveColor;
	gLayout.AboutWindow.TitleTextPadding		= ImVec2(TitleTextPaddingX, (gLayout.AboutWindow.TitleBarHeight - TextLineHeight) * TitleTextPaddingY);
}

static auto BuildStructurePanel(
	float	RelativeWidth,
	float	OffsetLeft,
	float	OffsetTop,
	float	OffsetBottom,
	float	PaddingX,
	float	PaddingY,
	float	TitleTextPaddingX,
	float	TitleTextPaddingY,
	float	TitleBarHeightOffset,
	ImU32	TitleBarColor,
	ImU32	TitleBarLineColor,
	ImU32	ScrollbarColor,
	ImU32	ScrollbarGrabColor,
	ImU32	ScrollbarGrabHoverColor,
	ImU32	ScrollbarGrabActiveColor,
	ImU32	Color,
	ImU32	HeaderColor,
	ImU32	HeaderHoverColor,
	ImU32	HeaderActiveColor,
	ImU32	TextPriorityColor,
	ImU32	SmbiosNamespaceColor,
	ImU32	OtherNamespaceColor
) -> void
{
	const auto TextLineHeight						= ImGui::GetTextLineHeight();
	const auto WindowPos							= ImGui::GetWindowPos();
	const auto WindowSize							= ImGui::GetWindowSize();

	gLayout.StructurePanel.Width					= WindowSize.x * RelativeWidth;
	gLayout.StructurePanel.Height					= WindowSize.y - gLayout.TitleBar.Height - gLayout.MenuBar.Height - OffsetTop - OffsetBottom;
	gLayout.StructurePanel.TitleBarHeight			= TextLineHeight + TitleBarHeightOffset;
	gLayout.StructurePanel.Position					= ImVec2(WindowPos.x + OffsetLeft, WindowPos.y + gLayout.TitleBar.Height + gLayout.MenuBar.Height + OffsetTop);
	gLayout.StructurePanel.Padding					= ImVec2(PaddingX, PaddingY);
	gLayout.StructurePanel.TitleTextPadding			= ImVec2(TitleTextPaddingX, (gLayout.StructurePanel.TitleBarHeight - TextLineHeight) * TitleTextPaddingY);
	gLayout.StructurePanel.TitleBarColor			= TitleBarColor;
	gLayout.StructurePanel.TitleBarLineColor		= TitleBarLineColor;
	gLayout.StructurePanel.ScrollbarColor			= ScrollbarColor;
	gLayout.StructurePanel.ScrollbarGrabColor		= ScrollbarGrabColor;
	gLayout.StructurePanel.ScrollbarGrabHoverColor	= ScrollbarGrabHoverColor;
	gLayout.StructurePanel.ScrollbarGrabActiveColor	= ScrollbarGrabActiveColor;
	gLayout.StructurePanel.Color					= Color;
	gLayout.StructurePanel.HeaderColor				= HeaderColor;
	gLayout.StructurePanel.HeaderHoverColor			= HeaderHoverColor;
	gLayout.StructurePanel.HeaderActiveColor		= HeaderActiveColor;
	gLayout.StructurePanel.TextPriorityColor		= TextPriorityColor;
	gLayout.StructurePanel.SmbiosNamespaceColor		= SmbiosNamespaceColor;
	gLayout.StructurePanel.OtherNamespaceColor	= OtherNamespaceColor;
}

static auto BuildStatePanel(
	float	RelativeWidth,
	float	Height,
	float	OffsetRight,
	float	OffsetTop,
	float	PaddingX,
	float	PaddingY,
	float	TitleTextPaddingX,
	float	TitleTextPaddingY,
	float	TitleBarHeightOffset,
	ImU32	TitleBarColor,
	ImU32	TitleBarLineColor,
	ImU32	Color,
	ImU32	CheckboxColor,
	ImU32	CheckboxHoverColor,
	ImU32	CheckboxActiveColor,
	ImU32	CheckboxCheckmarkColor
) -> void
{
	const auto TextLineHeight					= ImGui::GetTextLineHeight();
	const auto WindowPos						= ImGui::GetWindowPos();
	const auto WindowSize						= ImGui::GetWindowSize();

	const auto Left								= gLayout.StructurePanel.Position.x + gLayout.StructurePanel.Width + OffsetRight * 0.75f;
	const auto Right							= WindowPos.x + WindowSize.x - OffsetRight;

	gLayout.StatePanel.Position					= ImVec2(Left, WindowPos.y + gLayout.TitleBar.Height + gLayout.MenuBar.Height + OffsetTop);
	gLayout.StatePanel.Width					= Right - Left;
	gLayout.StatePanel.Height					= Height;
	gLayout.StatePanel.TitleBarHeight			= TextLineHeight + TitleBarHeightOffset;
	gLayout.StatePanel.Padding					= ImVec2(PaddingX, PaddingY);
	gLayout.StatePanel.TitleTextPadding			= ImVec2(TitleTextPaddingX, (gLayout.StatePanel.TitleBarHeight - TextLineHeight) * TitleTextPaddingY);
	gLayout.StatePanel.TitleBarColor			= TitleBarColor;
	gLayout.StatePanel.TitleBarLineColor		= TitleBarLineColor;
	gLayout.StatePanel.Color					= Color;
	gLayout.StatePanel.CheckboxColor			= CheckboxColor;
	gLayout.StatePanel.CheckboxHoverColor		= CheckboxHoverColor;
	gLayout.StatePanel.CheckboxActiveColor		= CheckboxActiveColor;
	gLayout.StatePanel.CheckboxCheckmarkColor	= CheckboxCheckmarkColor;
}

static auto DrawTitleBar(HWND hWnd, APP_STATE& State) -> void
{
	const auto WindowPos = ImGui::GetWindowPos();
	const auto WindowSize = ImGui::GetWindowSize();
	const auto DrawList = ImGui::GetWindowDrawList();

	// Draw a filled rectangle for the title bar.
	DrawList->AddRectFilled(
		WindowPos,
		ImVec2(WindowPos.x + WindowSize.x, WindowPos.y + gLayout.TitleBar.Height),
		gLayout.TitleBar.Color
	);

	// Add an invisible button to the title bar area to handle dragging the window.
	ImGui::SetCursorPos(
		ImVec2(0.0f, 0.0f)
	);

	ImGui::InvisibleButton(
		"TitleBarDragZone",
		ImVec2(WindowSize.x - gLayout.TitleBar.ButtonWidth * 2.0f, gLayout.TitleBar.Height)
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
		gLayout.TitleBar.TextPadding
	);

	// Print the text on the title bar.
	ImGui::TextUnformatted(
		TitleText.c_str()
	);

	// Set the color for the title bar buttons.
	ImGui::PushStyleColor(ImGuiCol_Button, gLayout.TitleBar.ButtonColor);
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, gLayout.TitleBar.ButtonHoverColor);
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, gLayout.TitleBar.ButtonActiveColor);

	// Set the cursor position for the minimize button.
	ImGui::SetCursorPos(
		ImVec2(WindowSize.x - gLayout.TitleBar.ButtonWidth * 2.0f, 0.0f)
	);

	// Handle minimize button.
	if (ImGui::Button(
		"-",
		ImVec2(gLayout.TitleBar.ButtonWidth, gLayout.TitleBar.Height))
	)
	{
		ShowWindow(hWnd, SW_MINIMIZE);
	}

	// Set the cursor position for the close button.
	ImGui::SetCursorPos(
		ImVec2(WindowSize.x - gLayout.TitleBar.ButtonWidth, 0.0f)
	);

	// Handle close button.
	if (ImGui::Button(
		"X",
		ImVec2(gLayout.TitleBar.ButtonWidth, gLayout.TitleBar.Height))
	)
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

static auto SaveImageFile(HWND hWnd, APP_STATE& State) -> bool
{
	if (!File::SaveFile(State.FileInfo))
	{
		MessageBox(hWnd, _T("Failed to save file."), _T("Error"), MB_ICONERROR);
		return false;
	}

	return true;
}

static auto DrawLinkText(const char* Label, const float LabelOffset, const char* LinkLabel, const char* Url, ImU32 OpenedColor, ImU32 DefaultColor) -> void
{
	ImGui::TextUnformatted(Label);
	ImGui::SameLine(LabelOffset);

	const bool IsVisited = gOpenedUrls.contains(Url);

	ImU32 Color = IsVisited ? OpenedColor : DefaultColor;

	ImGui::PushStyleColor(ImGuiCol_Text, Color);
	ImGui::TextUnformatted(LinkLabel);
	ImGui::PopStyleColor();

	if (ImGui::IsItemHovered())
	{
		ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);

		const auto Min = ImGui::GetItemRectMin();
		const auto Max = ImGui::GetItemRectMax();

		ImGui::GetForegroundDrawList()->AddLine(
			ImVec2(Min.x, Max.y),
			ImVec2(Max.x, Max.y),
			Color
		);

		ImGui::BeginTooltip();
		ImGui::TextUnformatted(Url);
		ImGui::EndTooltip();
	}

	if (ImGui::IsItemClicked())
	{
		ShellExecuteA(nullptr, "open", Url, nullptr, nullptr, SW_SHOWNORMAL);
		gOpenedUrls.insert(Url);
	}
}

static auto DrawDiscordUser(const char* Label, const float LabelOffset, const char* DisplayText, const char* CopyText, ImU32 OpenedColor, ImU32 DefaultColor) -> void
{
	ImGui::TextUnformatted(Label);
	ImGui::SameLine(LabelOffset);

	const auto IsSelected = gCopiedDiscordUsers.contains(CopyText);

	auto Color = IsSelected ? OpenedColor : DefaultColor;

	ImGui::PushStyleColor(ImGuiCol_Text, Color);
	ImGui::TextUnformatted(DisplayText);
	ImGui::PopStyleColor();

	if (ImGui::IsItemHovered())
	{
		ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);

		const auto Min = ImGui::GetItemRectMin();
		const auto Max = ImGui::GetItemRectMax();

		ImGui::GetForegroundDrawList()->AddLine(
			ImVec2(Min.x, Max.y),
			ImVec2(Max.x, Max.y),
			Color
		);

		ImGui::BeginTooltip();

		const auto Time = ImGui::GetTime();
		const auto RecentlyCopied = (gLastCopiedDiscord == CopyText) && (Time - gDiscordCopiedTime < 1.5);

		ImGui::TextUnformatted(RecentlyCopied ? "Copied!" : "Click to copy");

		ImGui::EndTooltip();
	}

	if (ImGui::IsItemClicked())
	{
		ImGui::SetClipboardText(CopyText);
		gCopiedDiscordUsers.insert(CopyText);
		gLastCopiedDiscord = CopyText;
		gDiscordCopiedTime = ImGui::GetTime();
	}
}

static auto StripExtension(const std::string& FileName) -> std::string
{
	auto Dot = FileName.find_last_of('.');

	if (Dot == std::string::npos)
	{
		return FileName;
	}

	return FileName.substr(0, Dot);
}

static auto SelectExportFilePath(std::string& FilePath, const char* DefaultName) -> bool
{
	OPENFILENAMEA Ofn = {};
	char Buffer[MAX_PATH] = {};

	if (DefaultName)
	{
		strcpy_s(Buffer, DefaultName);
	}

	Ofn.lStructSize = sizeof(Ofn);
	Ofn.hwndOwner = nullptr;
	Ofn.lpstrFilter = "Binary Files\0*.bin\0All Files\0*.*\0";
	Ofn.lpstrFile = Buffer;
	Ofn.nMaxFile = MAX_PATH;
	Ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR;
	Ofn.lpstrDefExt = "bin";

	const auto Result = GetSaveFileNameA(&Ofn);

	if (Result)
	{
		FilePath = Buffer;
	}

	return Result != 0;
}

static auto ExportBufferToFile(const std::string& FilePath, const void* Buffer, uint32_t Size) -> bool
{
	if (FilePath.empty() || !Buffer || !Size)
	{
		return false;
	}

	auto FileHandle = CreateFileA(
		FilePath.c_str(),
		GENERIC_WRITE,
		0,
		nullptr,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		nullptr
	);

	if (FileHandle == INVALID_HANDLE_VALUE)
	{
		return false;
	}

	DWORD BytesWritten = 0;
	const auto Result = WriteFile(
		FileHandle,
		Buffer,
		Size,
		&BytesWritten,
		nullptr
	);

	CloseHandle(FileHandle);

	return Result && (BytesWritten == Size);
}

static auto ExportBlock(HWND hWnd, const char* DefaultName, const void* Buffer, uint32_t Size) -> bool
{
	if (!Buffer || !Size)
	{
		MessageBox(hWnd, _T("Invalid block buffer or size."), _T("Error"), MB_ICONERROR);
		return false;
	}

	std::string FilePath;

	if (!SelectExportFilePath(FilePath, DefaultName))
	{
		return false;
	}

	if (!ExportBufferToFile(FilePath, Buffer, Size))
	{
		MessageBox(hWnd, _T("Failed to export block."), _T("Error"), MB_ICONERROR);
		return false;
	}

	return true;
}

static auto ExportLdbgBlock(HWND hWnd, APP_STATE& State) -> bool
{
	if (!State.FileInfo.Loaded || !State.FileInfo.Buffer || !State.LdbgBlockFound || !State.LdbgBlock || !State.LdbgBlockSize)
	{
		MessageBox(hWnd, _T("LDBG block is not available."), _T("Error"), MB_ICONERROR);
		return false;
	}

	auto FileSize = State.FileInfo.Size;
	auto Offset = Cast::To<uint64_t>(State.LdbgBlockOffset);
	auto Size = Cast::To<uint64_t>(State.LdbgBlockSize);

	if (Offset >= FileSize || Size > (FileSize - Offset))
	{
		MessageBox(hWnd, _T("LDBG block range is invalid."), _T("Error"), MB_ICONERROR);
		return false;
	}

	auto Buffer = State.FileInfo.Buffer + State.LdbgBlockOffset;
	std::string DefaultName = StripExtension(State.FileInfo.FileName) + "_LDBG.bin";

	return ExportBlock(hWnd, DefaultName.c_str(), Buffer, static_cast<uint32_t>(State.LdbgBlockSize));
}

static auto ExportLenvBlock1(HWND hWnd, APP_STATE& State) -> bool
{
	if (!State.FileInfo.Loaded || !State.FileInfo.Buffer || !State.LenvBlock1Found || !State.LenvBlock1 || !State.LenvBlock1Size)
	{
		MessageBox(hWnd, _T("LENV Block 1 is not available."), _T("Error"), MB_ICONERROR);
		return false;
	}

	auto FileSize = State.FileInfo.Size;
	auto Offset = Cast::To<uint64_t>(State.LenvBlock1Offset);
	auto Size = Cast::To<uint64_t>(State.LenvBlock1Size);

	if (Offset >= FileSize || Size > (FileSize - Offset))
	{
		MessageBox(hWnd, _T("LENV Block 1 range is invalid."), _T("Error"), MB_ICONERROR);
		return false;
	}

	auto Buffer = State.FileInfo.Buffer + State.LenvBlock1Offset;
	std::string DefaultName = StripExtension(State.FileInfo.FileName) + "_LENV_1.bin";

	return ExportBlock(hWnd, DefaultName.c_str(), Buffer, static_cast<uint32_t>(State.LenvBlock1Size));
}

static auto ExportLenvBlock2(HWND hWnd, APP_STATE& State) -> bool
{
	if (!State.FileInfo.Loaded || !State.FileInfo.Buffer || !State.LenvBlock2Found || !State.LenvBlock2 || !State.LenvBlock2Size)
	{
		MessageBox(hWnd, _T("LENV Block 2 is not available."), _T("Error"), MB_ICONERROR);
		return false;
	}

	auto FileSize = State.FileInfo.Size;
	auto Offset = Cast::To<uint64_t>(State.LenvBlock2Offset);
	auto Size = Cast::To<uint64_t>(State.LenvBlock2Size);

	if (Offset >= FileSize || Size > (FileSize - Offset))
	{
		MessageBox(hWnd, _T("LENV Block 2 range is invalid."), _T("Error"), MB_ICONERROR);
		return false;
	}

	auto Buffer = State.FileInfo.Buffer + State.LenvBlock2Offset;
	std::string DefaultName = StripExtension(State.FileInfo.FileName) + "_LENV_2.bin";

	return ExportBlock(hWnd, DefaultName.c_str(), Buffer, static_cast<uint32_t>(State.LenvBlock2Size));
}

static auto DrawMenuBar(HWND hWnd, APP_STATE& State) -> void
{
	const auto WindowPos = ImGui::GetWindowPos();
	const auto WindowSize = ImGui::GetWindowSize();
	const auto DrawList = ImGui::GetWindowDrawList();

	// Draw a filled rectangle for the menu bar.
	DrawList->AddRectFilled(
		ImVec2(WindowPos.x, WindowPos.y + gLayout.TitleBar.Height),
		ImVec2(WindowPos.x + WindowSize.x, WindowPos.y + gLayout.TitleBar.Height + gLayout.MenuBar.Height),
		gLayout.MenuBar.Color
	);

	// Draw a line at the bottom of the menu bar.
	DrawList->AddLine(
		ImVec2(WindowPos.x, WindowPos.y + gLayout.TitleBar.Height + gLayout.MenuBar.Height),
		ImVec2(WindowPos.x + WindowSize.x, WindowPos.y + gLayout.TitleBar.Height + gLayout.MenuBar.Height),
		gLayout.MenuBar.LineColor
	);

	// Set the color for the menu bar buttons.
	ImGui::PushStyleColor(ImGuiCol_Button, gLayout.MenuBar.ButtonColor);
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, gLayout.MenuBar.ButtonHoverColor);
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, gLayout.MenuBar.ButtonActiveColor);

	// Set the padding for the menu bar buttons.
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, gLayout.MenuBar.ButtonPadding);
	// Set the cursor position for the "File" menu button.
	ImGui::SetCursorPos(ImVec2(0.0f, gLayout.TitleBar.Height));

	// Compute the width of the "File" button based on the text size and additional width.
	auto ButtonWidth = ImGui::CalcTextSize("File").x + gLayout.MenuBar.ButtonAdditionalWidth;

	// Handle "File" button.
	if (ImGui::Button("File", ImVec2(ButtonWidth, gLayout.MenuBar.Height)))
	{
		ImGui::OpenPopup("FileMenu");
	}

	// Compute the position for the "File" dropdown menu.
	const auto FileButtonMin = ImGui::GetItemRectMin();
	const auto FileButtonMax = ImGui::GetItemRectMax();
	const auto FileMenuPos = ImVec2(FileButtonMin.x, FileButtonMax.y);

	// Set the cursor position for the "Help" menu button.
	ImGui::SetCursorPos(ImVec2(ButtonWidth, gLayout.TitleBar.Height));

	// Compute the width of the "Help" button based on the text size and additional width.
	ButtonWidth = ImGui::CalcTextSize("Help").x + gLayout.MenuBar.ButtonAdditionalWidth;

	// Handle "Help" button.
	if (ImGui::Button("Help", ImVec2(ButtonWidth, gLayout.MenuBar.Height)))
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
	ImGui::PushStyleColor(ImGuiCol_PopupBg, gLayout.MenuBar.DropdownColor);
	ImGui::PushStyleColor(ImGuiCol_HeaderHovered, gLayout.MenuBar.DropdownHoverColor);
	ImGui::PushStyleColor(ImGuiCol_HeaderActive, gLayout.MenuBar.DropdownActiveColor);

	// Set the position for the "File" dropdown menu to appear below the "File" button.
	ImGui::SetNextWindowPos(FileMenuPos, ImGuiCond_Appearing);

	// Exportable checks.
	const bool CanExportLdbg	= State.FileInfo.Loaded && State.LdbgBlockFound && State.LdbgBlock;
	const bool CanExportLenv1	= State.FileInfo.Loaded && State.LenvBlock1Found && State.LenvBlock1;
	const bool CanExportLenv2	= State.FileInfo.Loaded && State.LenvBlock2Found && State.LenvBlock2;

	if (ImGui::BeginPopup("FileMenu", ImGuiWindowFlags_NoMove))
	{
		if (ImGui::MenuItem("Open image file"))
		{
			OpenImageFile(hWnd, State);
		}

		if (ImGui::MenuItem("Save image file", nullptr, false, State.FileInfo.Loaded))
		{
			SaveImageFile(hWnd, State);
		}

		ImGui::Separator();

		if (ImGui::MenuItem("Export LDBG Block", nullptr, false, CanExportLdbg))
		{
			ExportLdbgBlock(hWnd, State);
		}

		if (ImGui::MenuItem("Export LENV Block 1", nullptr, false, CanExportLenv1))
		{
			ExportLenvBlock1(hWnd, State);
		}

		if (ImGui::MenuItem("Export LENV Block 2", nullptr, false, CanExportLenv2))
		{
			ExportLenvBlock2(hWnd, State);
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

static auto DrawAboutWindow() -> void
{
	if (gOpenAboutPopup)
	{
		ImGui::OpenPopup("AboutPopup");
		gOpenAboutPopup = false;
	}

	ImGui::SetNextWindowSize(
		ImVec2(gLayout.AboutWindow.Width, gLayout.AboutWindow.Height),
		ImGuiCond_Appearing
	);

	ImGui::SetNextWindowPos(
		ImGui::GetMainViewport()->GetCenter(),
		ImGuiCond_Appearing,
		ImVec2(0.5f, 0.5f)
	);

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::PushStyleColor(ImGuiCol_PopupBg, ImGui::GetStyle().Colors[ImGuiCol_WindowBg]);
	ImGui::PushStyleColor(ImGuiCol_Border, gLayout.AboutWindow.BorderColor);

	bool PopupOpen = false;

	if (ImGui::BeginPopupModal(
		"AboutPopup",
		nullptr,
		ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoSavedSettings
	))
	{
		PopupOpen = true;

		const auto WindowPos = ImGui::GetWindowPos();
		const auto WindowSize = ImGui::GetWindowSize();
		const auto DrawList = ImGui::GetWindowDrawList();

		const auto TitleBarMin = WindowPos;
		const auto TitleBarMax = ImVec2(
			WindowPos.x + WindowSize.x,
			WindowPos.y + gLayout.AboutWindow.TitleBarHeight
		);

		// Draw title bar background.
		DrawList->AddRectFilled(
			TitleBarMin,
			TitleBarMax,
			gLayout.AboutWindow.TitleBarColor
		);

		// Draw title bar bottom line.
		DrawList->AddLine(
			ImVec2(TitleBarMin.x, TitleBarMax.y),
			ImVec2(TitleBarMax.x, TitleBarMax.y),
			gLayout.AboutWindow.TitleBarLineColor
		);

		// Drag zone.
		ImGui::SetCursorPos(ImVec2(0.0f, 0.0f));
		ImGui::InvisibleButton(
			"AboutPopupDragZone",
			ImVec2(
				WindowSize.x - gLayout.AboutWindow.CloseButtonWidth,
				gLayout.AboutWindow.TitleBarHeight
			)
		);

		if (ImGui::IsItemActive() && ImGui::IsMouseDragging(ImGuiMouseButton_Left))
		{
			ImGui::SetWindowPos(
				ImVec2(
					ImGui::GetWindowPos().x + ImGui::GetIO().MouseDelta.x,
					ImGui::GetWindowPos().y + ImGui::GetIO().MouseDelta.y
				)
			);
		}

		// Title text.
		ImGui::SetCursorPos(gLayout.AboutWindow.TitleTextPadding);
		ImGui::TextUnformatted("About");

		// Close button in title bar.
		ImGui::PushStyleColor(ImGuiCol_Button, gLayout.AboutWindow.CloseButtonColor);
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, gLayout.AboutWindow.CloseButtonHoverColor);
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, gLayout.AboutWindow.CloseButtonActiveColor);

		ImGui::SetCursorPos(
			ImVec2(
				WindowSize.x - gLayout.AboutWindow.CloseButtonWidth,
				0.0f
			)
		);

		if (ImGui::Button(
			"X",
			ImVec2(
				gLayout.AboutWindow.CloseButtonWidth,
				gLayout.AboutWindow.TitleBarHeight
			)
		))
		{
			ImGui::CloseCurrentPopup();
		}

		ImGui::PopStyleColor(3);

		// Content area.
		ImGui::SetCursorPos(
			ImVec2(
				gLayout.AboutWindow.Padding.x,
				gLayout.AboutWindow.TitleBarHeight + gLayout.AboutWindow.Padding.y
			)
		);

		ImGui::BeginGroup();

		ImGui::TextUnformatted("Program Information");
		ImGui::Separator();

		ImGui::Text("Lenovo DMI Decryptor %s", gVersionString.c_str());
		ImGui::TextUnformatted("Copyright © 2026, Shmurkio");

		ImGui::Dummy(ImVec2(0.0f, gLayout.AboutWindow.Spacing));

		DrawDiscordUser("Discord:", gLayout.AboutWindow.LabelOffset, "@shmurkio", "shmurkio", gLayout.AboutWindow.TextLinkOpenedColor, gLayout.AboutWindow.TextLinkDefaultColor);
		DrawLinkText("License:", gLayout.AboutWindow.LabelOffset, "MIT License", "https://github.com/Shmurkio/LenovoDMIDecryptor/blob/master/LICENSE.txt", gLayout.AboutWindow.TextLinkOpenedColor, gLayout.AboutWindow.TextLinkDefaultColor);

		ImGui::Dummy(ImVec2(0.0f, gLayout.AboutWindow.Spacing));

		ImGui::TextUnformatted("Source Code & Documentation");
		ImGui::Separator();

		DrawLinkText("GitHub repository:", gLayout.AboutWindow.LabelOffset, "LenovoDMIDecryptor", "https://github.com/Shmurkio/LenovoDMIDecryptor", gLayout.AboutWindow.TextLinkOpenedColor, gLayout.AboutWindow.TextLinkDefaultColor);
		DrawLinkText("Win-Raid thread:", gLayout.AboutWindow.LabelOffset, "Lenovo DMI Decryption Tool", "https://winraid.level1techs.com/t/lenovo-dmi-decryption-tool", gLayout.AboutWindow.TextLinkOpenedColor, gLayout.AboutWindow.TextLinkDefaultColor);

		ImGui::Dummy(ImVec2(0.0f, gLayout.AboutWindow.Spacing));

		ImGui::TextUnformatted("Disclaimer");
		ImGui::Separator();

		ImGui::TextWrapped(
			"This program is based entirely on reverse-engineered Lenovo InsydeH2O firmware. "
			"There is no guarantee that it will work correctly for all Lenovo InsydeH2O firmware images. "
			"If you encounter any issues or notice any inaccuracies, feel free to reach out on Discord."
		);

		ImGui::Dummy(ImVec2(0.0f, gLayout.AboutWindow.Spacing));

		ImGui::TextWrapped(
			"That said, I am not responsible for any damage, data loss, or other issues caused "
			"by the use of this software."
		);

		ImGui::EndGroup();
		ImGui::EndPopup();
	}

	if (gAboutPopupWasOpen && !PopupOpen)
	{
		gCopiedDiscordUsers.clear();
		gOpenedUrls.clear();
		gLastCopiedDiscord = nullptr;
		gDiscordCopiedTime = 0.0;
	}

	gAboutPopupWasOpen = PopupOpen;

	ImGui::PopStyleColor(2);
	ImGui::PopStyleVar();
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

			HexPos += sprintf_s(HexLine + HexPos, sizeof(HexLine) - HexPos, "%02X ", Byte);

			AsciiLine[Col] = (Byte >= 32 && Byte <= 126) ? static_cast<char>(Byte) : '.';
		}

		for (; Col < BytesPerRow; ++Col)
		{
			if (Col == 8)
			{
				HexPos += sprintf_s(HexLine + HexPos, sizeof(HexLine) - HexPos, " ");
			}

			HexPos += sprintf_s(HexLine + HexPos, sizeof(HexLine) - HexPos, "   ");
		}

		ImGui::Text("%04zX | %s| %s", Row, HexLine, AsciiLine);
	}
}

static auto DrawLdbgBlockHeader(Lenovo::PLDBG_HEADER Header) -> void
{
	if (!ImGui::TreeNode("Header"))
	{
		return;
	}

	char Signature[5] = { 0 };
	memcpy(Signature, &Header->Signature, 4);

	ImGui::Text("Signature:");		ImGui::SameLine(300.0f);	ImGui::Text("%s", Signature);
	ImGui::Text("WriteOffset:");	ImGui::SameLine(300.0f);	ImGui::Text("0x%08X", Header->WriteOffset);

	if (ImGui::TreeNode("Unknown1"))
	{
		DrawHexRow(Header->Unknown1, sizeof(Header->Unknown1));
		ImGui::TreePop();
	}

	ImGui::TreePop();
}

constexpr auto BcdToUint8(uint8_t Value) -> uint8_t
{
	return ((Value >> 4) * 10) + (Value & 0x0F);
}

static auto LdbgOperationToString(Lenovo::LDBG_OPERATION Op) -> const char*
{
	switch (Op)
	{
	case Lenovo::LDBG_OPERATION::SetData:   return "SetData";
	case Lenovo::LDBG_OPERATION::Protect:   return "Protect";
	case Lenovo::LDBG_OPERATION::Unprotect: return "Unprotect";
	default:                                return "Unknown";
	}
}

static auto NamespaceIdColor(Lenovo::PCNAMESPACE_ID NamespaceId) -> ImU32
{
	if (Lenovo::IsSmbiosNamespace(NamespaceId))
	{
		return gLayout.StructurePanel.SmbiosNamespaceColor;
	}
	else
	{
		return gLayout.StructurePanel.OtherNamespaceColor;
	}
}

static auto EntryKeyToString(Lenovo::PCENTRY_KEY Key) -> std::string
{
	if (Lenovo::CompareEntryKeys(Key, &Lenovo::SMBIOS_ENTRY_KEY_WINDOWS_KEY)) return "Windows Key";
	if (Lenovo::CompareEntryKeys(Key, &Lenovo::SMBIOS_ENTRY_KEY_OA3_KEY_ID)) return "OA3 Key ID";
	if (Lenovo::CompareEntryKeys(Key, &Lenovo::SMBIOS_ENTRY_KEY_MOTHERBOARD_NAME)) return "Motherboard Name";
	if (Lenovo::CompareEntryKeys(Key, &Lenovo::SMBIOS_ENTRY_KEY_MACHINE_TYPE_MODEL)) return "Machine Type/Model";
	if (Lenovo::CompareEntryKeys(Key, &Lenovo::SMBIOS_ENTRY_KEY_BASEBOARD_SERIAL_NUMBER)) return "Baseboard Serial Number";
	if (Lenovo::CompareEntryKeys(Key, &Lenovo::SMBIOS_ENTRY_KEY_SYSTEM_UUID)) return "System UUID";
	if (Lenovo::CompareEntryKeys(Key, &Lenovo::SMBIOS_ENTRY_KEY_BASEBOARD_PLATFORM_ID)) return "Baseboard Platform ID";
	if (Lenovo::CompareEntryKeys(Key, &Lenovo::SMBIOS_ENTRY_KEY_OS_PRELOAD_SUFFIX)) return "OS Preload Suffix";
	return "Unknown";
}

static auto DrawLdbgEntry(Lenovo::PLDBG_ENTRY Entry, uint32_t Index) -> Lenovo::PLDBG_ENTRY
{
	char NodeLabel[64] = { 0 };
	sprintf_s(NodeLabel, sizeof(NodeLabel), "Entry[0x%08X] (%s)", Index, EntryKeyToString(&Entry->Key).c_str());

	auto NextEntry = Entry + 1;

	ImU32 NamespaceColor = NamespaceIdColor(&Entry->Key.NamespaceId);
	ImGui::PushStyleColor(ImGuiCol_Text, NamespaceColor);
	auto Ok = ImGui::TreeNode(NodeLabel);
	ImGui::PopStyleColor();

	if (!Ok)
	{
		return NextEntry;
	}

	/*
	* The timestamp is read from CMOS RTC registers via ports 0x70/0x71.
	*
	* Month, day, hour, minute, and second are stored exactly as returned by the RTC,
	* which is BCD encoded on this firmware.
	*
	* The year field is stored as:
	* Record->Year = 0x2000 + RTC_year_byte
	*
	* so only the low byte of Entry->Timestamp.Year contains the RTC BCD year value.
	* For display, we therefore decode the low byte from BCD and add 2000.
	*/
	const auto YearLow	= Cast::To<uint8_t>(Entry->Timestamp.Year & 0xFF);
	const auto Year		= 2000 + BcdToUint8(YearLow);
	const auto Month	= BcdToUint8(Entry->Timestamp.Month);
	const auto Day		= BcdToUint8(Entry->Timestamp.Day);
	const auto Hour		= BcdToUint8(Entry->Timestamp.Hour);
	const auto Minute	= BcdToUint8(Entry->Timestamp.Minute);
	const auto Second	= BcdToUint8(Entry->Timestamp.Second);

	ImGui::Text("Timestamp:");	ImGui::SameLine(300.0f);	ImGui::Text("%04u-%02u-%02u %02u:%02u:%02u", Year, Month, Day, Hour, Minute, Second);
	ImGui::Text("Operation:");	ImGui::SameLine(300.0f);	ImGui::Text("0x%02X (%s)", Cast::To<uint8_t>(Entry->Operation), LdbgOperationToString(Entry->Operation));

	if (ImGui::TreeNode("Key"))
	{
		if (ImGui::TreeNode("Namespace ID"))
		{
			DrawHexRow(Cast::To<uint8_t*>(&Entry->Key.NamespaceId), sizeof(Entry->Key.NamespaceId));
			ImGui::TreePop();
		}

		ImGui::Text("Type:"); ImGui::SameLine(300.0f); ImGui::Text("0x%04X", Entry->Key.Type);
		ImGui::TreePop();
	}

	ImGui::Text("Size:"); ImGui::SameLine(300.0f); ImGui::Text("0x%08X", Entry->Size);

	if (ImGui::TreeNode("Unknown1"))
	{
		DrawHexRow(Entry->Unknown1, sizeof(Entry->Unknown1));
		ImGui::TreePop();
	}

	ImGui::TreePop();

	return NextEntry;
}

static auto DrawLdbgBlockBody(Lenovo::PLDBG_ENTRY Entries, uint32_t BlockSize, uint32_t WriteOffset) -> void
{
	if (!ImGui::TreeNode("Entries"))
	{
		return;
	}

	const uint32_t HeaderSize = sizeof(Lenovo::LDBG_HEADER);
	const uint32_t EntrySize = sizeof(Lenovo::LDBG_ENTRY);

	if (WriteOffset < HeaderSize || WriteOffset > BlockSize)
	{
		ImGui::TextUnformatted("Invalid write offset.");
		ImGui::TreePop();
		return;
	}

	const uint32_t EntryCount = (WriteOffset - HeaderSize) / EntrySize;

	for (uint32_t i = 0; i < EntryCount; ++i)
	{
		auto CurrentEntry = Cast::To<Lenovo::PLDBG_ENTRY>(
			Cast::To<uint8_t*>(Entries) + i * EntrySize
		);

		DrawLdbgEntry(CurrentEntry, i);
	}

	ImGui::TreePop();
}

static auto DrawLdbgBlock(APP_STATE& State) -> void
{
	if (!ImGui::TreeNode("LDBG Block"))
	{
		return;
	}

	ImGui::Text("Offset:");	ImGui::SameLine(300.0f);	ImGui::Text("0x%08X", State.LdbgBlockOffset);
	ImGui::Text("Size:");	ImGui::SameLine(300.0f);	ImGui::Text("0x%08X", State.LdbgBlockSize);

	DrawLdbgBlockHeader(&State.LdbgBlock->Header);
	if (!State.LdbgBlockEncrypted) DrawLdbgBlockBody(State.LdbgBlock->Entries, State.LdbgBlockSize, State.LdbgBlock->Header.WriteOffset);

	ImGui::TreePop();
}

static auto DrawLenvBlockHeader(Lenovo::PLENV_HEADER Header) -> void
{
	if (!ImGui::TreeNode("Header"))
	{
		return;
	}

	char Signature[5] = { 0 };
	memcpy(Signature, &Header->Signature, 4);

	ImGui::Text("Signature:");	ImGui::SameLine(300.0f);	ImGui::Text("%s", Signature);
	ImGui::Text("Generation:");	ImGui::SameLine(300.0f);	ImGui::Text("0x%08X", Header->Generation);
	ImGui::Text("Entries:");	ImGui::SameLine(300.0f);	ImGui::Text("0x%08X", Header->Entries);
	ImGui::Text("AccessFlag:");	ImGui::SameLine(300.0f);	ImGui::Text("0x%02X", Header->AccessFlag);
	ImGui::Text("XorKey:");		ImGui::SameLine(300.0f);	ImGui::Text("0x%02X", Header->XorKey);
	ImGui::Text("Checksum:");	ImGui::SameLine(300.0f);	ImGui::Text("0x%04X", Header->Checksum);

	ImGui::TreePop();
}

static auto DrawLenvBlockEntry(Lenovo::PLENV_ENTRY Entry, uint32_t Index) -> Lenovo::PLENV_ENTRY
{
	char NodeLabel[64] = { 0 };
	sprintf_s(NodeLabel, sizeof(NodeLabel), "Entry[0x%08X] (%s)", Index, EntryKeyToString(&Entry->Key).c_str());

	auto NextEntry = Cast::To<Lenovo::PLENV_ENTRY>(Cast::To<uint8_t*>(Entry) + offsetof(Lenovo::LENV_ENTRY, Data) + Entry->DataSize);

	ImU32 NamespaceColor = NamespaceIdColor(&Entry->Key.NamespaceId);
	ImGui::PushStyleColor(ImGuiCol_Text, NamespaceColor);
	auto Ok = ImGui::TreeNode(NodeLabel);
	ImGui::PopStyleColor();

	if (!Ok)
	{
		return NextEntry;
	}

	if (ImGui::TreeNode("Key"))
	{
		if (ImGui::TreeNode("Namespace ID"))
		{
			DrawHexRow(Cast::To<uint8_t*>(&Entry->Key.NamespaceId), sizeof(Entry->Key.NamespaceId));
			ImGui::TreePop();
		}

		ImGui::Text("Type:");	ImGui::SameLine(300.0f);	ImGui::Text("0x%04X", Entry->Key.Type);
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

	return NextEntry;
}

static auto DrawLenvBlockBody(Lenovo::PLENV_ENTRY Entries, uint32_t EntryCount) -> void
{
	if (!ImGui::TreeNode("Entries"))
	{
		return;
	}

	auto CurrentEntry = Entries;

	for (uint32_t i = 0; i < EntryCount; ++i)
	{
		CurrentEntry = DrawLenvBlockEntry(CurrentEntry, i);

		if (!CurrentEntry)
		{
			break;
		}
	}

	ImGui::TreePop();
}

static auto DrawLenvBlock(APP_STATE& State, int BlockNumber, bool IsPriority) -> void
{
	if (IsPriority) ImGui::PushStyleColor(ImGuiCol_Text, gLayout.StructurePanel.TextPriorityColor);

	if (!((BlockNumber == 1) ? ImGui::TreeNode("LENV Block 1") : ImGui::TreeNode("LENV Block 2")))
	{
		if (IsPriority) ImGui::PopStyleColor();
		return;
	}

	if (IsPriority) ImGui::PopStyleColor();

	auto Block = (BlockNumber == 1) ? State.LenvBlock1 : State.LenvBlock2;
	auto Offset = (BlockNumber == 1) ? State.LenvBlock1Offset : State.LenvBlock2Offset;
	auto Size = (BlockNumber == 1) ? State.LenvBlock1Size : State.LenvBlock2Size;
	auto IsEncrypted = (BlockNumber == 1) ? State.LenvBlock1Encrypted : State.LenvBlock2Encrypted;

	ImGui::Text("Offset:");	ImGui::SameLine(300.0f);	ImGui::Text("0x%08X", Offset);
	ImGui::Text("Size:");	ImGui::SameLine(300.0f);	ImGui::Text("0x%08X", Size);

	DrawLenvBlockHeader(&Block->Header);
	if (!IsEncrypted) DrawLenvBlockBody(Block->Entries, Block->Header.Entries);

	ImGui::TreePop();
}

static auto DrawStructurePanel(APP_STATE& State) -> void
{
	const auto PanelPos = gLayout.StructurePanel.Position;
	const auto PanelSize = ImVec2(gLayout.StructurePanel.Width, gLayout.StructurePanel.Height);

	const auto TitleBarMin = PanelPos;
	const auto TitleBarMax = ImVec2(PanelPos.x + PanelSize.x, PanelPos.y + gLayout.StructurePanel.TitleBarHeight);

	const auto PanelMax = ImVec2(PanelPos.x + PanelSize.x, PanelPos.y + PanelSize.y);

	const auto DrawList = ImGui::GetWindowDrawList();

	// Draw the panel background.
	DrawList->AddRectFilled(
		PanelPos,
		PanelMax,
		gLayout.StructurePanel.Color
	);

	// Draw the title bar.
	DrawList->AddRectFilled(
		TitleBarMin,
		TitleBarMax,
		gLayout.StructurePanel.TitleBarColor
	);

	DrawList->AddLine(
		ImVec2(TitleBarMin.x, TitleBarMax.y),
		ImVec2(TitleBarMax.x, TitleBarMax.y),
		gLayout.StructurePanel.TitleBarLineColor,
		1.0f
	);

	// Draw the title text.
	ImGui::SetCursorScreenPos(ImVec2(
		PanelPos.x + gLayout.StructurePanel.TitleTextPadding.x,
		PanelPos.y + gLayout.StructurePanel.TitleTextPadding.y
	));

	ImGui::TextUnformatted("Structure");

	const float ChildTopOffset = gLayout.StructurePanel.TitleTextPadding.y;

	// Add a scrollable content region.
	const ImVec2 ChildPos = ImVec2(
		PanelPos.x,
		PanelPos.y
		+ gLayout.StructurePanel.TitleBarHeight
		+ gLayout.StructurePanel.Padding.y
	);

	const ImVec2 ChildSize = ImVec2(
		PanelSize.x,
		PanelSize.y
		- gLayout.StructurePanel.TitleBarHeight
		- gLayout.StructurePanel.Padding.y * 2
	);

	ImGui::SetCursorScreenPos(ChildPos);

	ImGui::PushStyleColor(ImGuiCol_ChildBg, gLayout.StructurePanel.Color);
	ImGui::PushStyleColor(ImGuiCol_ScrollbarBg, gLayout.StructurePanel.ScrollbarColor);
	ImGui::PushStyleColor(ImGuiCol_ScrollbarGrab, gLayout.StructurePanel.ScrollbarGrabColor);
	ImGui::PushStyleColor(ImGuiCol_ScrollbarGrabHovered, gLayout.StructurePanel.ScrollbarGrabHoverColor);
	ImGui::PushStyleColor(ImGuiCol_ScrollbarGrabActive, gLayout.StructurePanel.ScrollbarGrabActiveColor);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, gLayout.StructurePanel.Padding);
	ImGui::PushStyleColor(ImGuiCol_Header, gLayout.StructurePanel.HeaderColor);
	ImGui::PushStyleColor(ImGuiCol_HeaderHovered, gLayout.StructurePanel.HeaderHoverColor);
	ImGui::PushStyleColor(ImGuiCol_HeaderActive, gLayout.StructurePanel.HeaderActiveColor);

	if (ImGui::BeginChild(
		"StructurePanelChild",
		ChildSize,
		false,
		ImGuiWindowFlags_AlwaysVerticalScrollbar
	))
	{
		if (State.FileInfo.Loaded && State.LdbgBlockFound && State.LenvBlock1Found && State.LenvBlock2Found)
		{
			DrawLdbgBlock(State);
			auto IsLenv1Priority = (State.LenvBlock1->Header.Generation >= State.LenvBlock2->Header.Generation);
			DrawLenvBlock(State, 1, IsLenv1Priority);
			DrawLenvBlock(State, 2, !IsLenv1Priority);
		}
	}

	ImGui::EndChild();

	// Draw the border last so nothing covers it.
	DrawList->AddRect(
		ImVec2(PanelPos.x + 0.5f, PanelPos.y + 0.5f),
		ImVec2(PanelMax.x - 0.5f, PanelMax.y - 0.5f),
		gLayout.StructurePanel.TitleBarLineColor,
		0.0f,
		0,
		1.0f
	);

	ImGui::PopStyleVar();
	ImGui::PopStyleColor(8);
}

auto DrawStatePanelContent(HWND hWnd, APP_STATE& State) -> void
{
	ImGui::TextUnformatted("Encryption:");

	auto LdbgBlockEncrypted = State.LdbgBlockEncrypted;

	if (ImGui::Checkbox("LDBG Block encrypted", &LdbgBlockEncrypted))
	{
		Dmi::ToggleLdbgBlockEncryption(hWnd, State);
	}

	auto LenvBlock1Encrypted = State.LenvBlock1Encrypted;

	if (ImGui::Checkbox("LENV Block 1 encrypted", &LenvBlock1Encrypted))
	{
		Dmi::ToggleLenvBlock1Encryption(hWnd, State);
	}

	auto LenvBlock2Encrypted = State.LenvBlock2Encrypted;

	if (ImGui::Checkbox("LENV Block 2 encrypted", &LenvBlock2Encrypted))
	{
		Dmi::ToggleLenvBlock2Encryption(hWnd, State);
	}

	if (State.LenvBlock1Found && State.LenvBlock2Found && State.LenvBlock1 && State.LenvBlock2)
	{
		ImGui::TextUnformatted("Prioritized LENV Block:");

		auto PrioritizedBlock = (State.LenvBlock1->Header.Generation >= State.LenvBlock2->Header.Generation) ? 1 : 2;

		auto SelectedBlock = PrioritizedBlock;

		if (ImGui::RadioButton("LENV Block 1", SelectedBlock == 1))
		{
			Dmi::SetPrioritizedLenvBlock(hWnd, State, 1);
		}

		if (ImGui::RadioButton("LENV Block 2", SelectedBlock == 2))
		{
			Dmi::SetPrioritizedLenvBlock(hWnd, State, 2);
		}
	}
}

auto DrawStatePanel(HWND hWnd, APP_STATE& State) -> void
{
	const auto PanelPos = gLayout.StatePanel.Position;
	const auto PanelSize = ImVec2(gLayout.StatePanel.Width, gLayout.StatePanel.Height);

	const auto TitleBarMin = PanelPos;
	const auto TitleBarMax = ImVec2(
		PanelPos.x + PanelSize.x,
		PanelPos.y + gLayout.StatePanel.TitleBarHeight
	);

	const auto PanelMax = ImVec2(
		PanelPos.x + PanelSize.x,
		PanelPos.y + PanelSize.y
	);

	const auto DrawList = ImGui::GetWindowDrawList();

	// Background.
	DrawList->AddRectFilled(
		PanelPos,
		PanelMax,
		gLayout.StatePanel.Color
	);

	// Title bar.
	DrawList->AddRectFilled(
		TitleBarMin,
		TitleBarMax,
		gLayout.StatePanel.TitleBarColor
	);

	DrawList->AddLine(
		ImVec2(TitleBarMin.x, TitleBarMax.y),
		ImVec2(TitleBarMax.x, TitleBarMax.y),
		gLayout.StatePanel.TitleBarLineColor,
		1.0f
	);

	// Title text.
	ImGui::SetCursorScreenPos(ImVec2(
		PanelPos.x + gLayout.StatePanel.TitleTextPadding.x,
		PanelPos.y + gLayout.StatePanel.TitleTextPadding.y
	));

	ImGui::TextUnformatted("State");

	// Content area.
	const auto ContentTop = PanelPos.y + gLayout.StatePanel.TitleBarHeight + gLayout.StatePanel.Padding.y;
	const auto ContentLeft = PanelPos.x + gLayout.StatePanel.Padding.x;

	ImGui::SetCursorScreenPos(ImVec2(ContentLeft, ContentTop));
	ImGui::BeginGroup();

	ImGui::PushStyleColor(ImGuiCol_FrameBg, gLayout.StatePanel.CheckboxColor);
	ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, gLayout.StatePanel.CheckboxHoverColor);
	ImGui::PushStyleColor(ImGuiCol_FrameBgActive, gLayout.StatePanel.CheckboxActiveColor);
	ImGui::PushStyleColor(ImGuiCol_CheckMark, gLayout.StatePanel.CheckboxCheckmarkColor);

	if (State.FileInfo.Loaded && State.LdbgBlockFound && State.LenvBlock1Found && State.LenvBlock2Found)
	{
		DrawStatePanelContent(hWnd, State);
	}

	ImGui::PopStyleColor(4);

	ImGui::EndGroup();

	const auto X1 = PanelPos.x + 0.5f;
	const auto Y1 = PanelPos.y + 0.5f;
	const auto X2 = PanelMax.x - 0.5f;
	const auto Y2 = PanelMax.y - 0.5f;

	DrawList->AddLine(ImVec2(X1, Y1), ImVec2(X2, Y1), gLayout.StatePanel.TitleBarLineColor, 1.0f);
	DrawList->AddLine(ImVec2(X1, Y2), ImVec2(X2, Y2), gLayout.StatePanel.TitleBarLineColor, 1.0f);
	DrawList->AddLine(ImVec2(X1, Y1), ImVec2(X1, Y2), gLayout.StatePanel.TitleBarLineColor, 1.0f);
	DrawList->AddLine(ImVec2(X2, Y1), ImVec2(X2, Y2), gLayout.StatePanel.TitleBarLineColor, 1.0f);
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
	ImGui::Begin(
		"MainWindow",
		nullptr,
		Flags
	);

	// Build the title bar layout.
	BuildTitleBar(
		TITLE_BAR_HEIGHT_OFFSET,
		TITLE_BAR_COLOR,
		TITLE_BAR_LINE_COLOR,
		TITLE_BAR_BUTTON_WIDTH,
		TITLE_BAR_BUTTON_COLOR,
		TITLE_BAR_BUTTON_HOVER_COLOR,
		TITLE_BAR_BUTTON_ACTIVE_COLOR,
		TITLE_BAR_TEXT_PADDING_X,
		TITLE_BAR_TEXT_PADDING_Y
	);

	// Build the menu bar layout.
	BuildMenuBar(
		MENU_BAR_HEIGHT_OFFSET,
		MENU_BAR_COLOR,
		MENU_BAR_LINE_COLOR,
		MENU_BAR_BUTTON_COLOR,
		MENU_BAR_BUTTON_HOVER_COLOR,
		MENU_BAR_BUTTON_ACTIVE_COLOR,
		MENU_BAR_BUTTON_PADDING_X,
		MENU_BAR_BUTTON_PADDING_Y,
		MENU_BAR_BUTTON_ADDITIONAL_WIDTH,
		MENU_BAR_DROPDOWN_COLOR,
		MENU_BAR_DROPDOWN_HOVER_COLOR,
		MENU_BAR_DROPDOWN_ACTIVE_COLOR
	);

	// Build the about window layout.
	BuildAboutWindow(
		ABOUT_WINDOW_WIDTH,
		ABOUT_WINDOW_HEIGHT,
		ABOUT_WINDOW_BORDER_COLOR,
		ABOUT_WINDOW_PADDING_X,
		ABOUT_WINDOW_PADDING_Y,
		ABOUT_WINDOW_SPACING,
		ABOUT_WINDOW_TITLE_BAR_HEIGHT_OFFSET,
		ABOUT_WINDOW_TITLE_BAR_COLOR,
		ABOUT_WINDOW_TITLE_BAR_LINE_COLOR,
		ABOUT_WINDOW_CLOSE_BUTTON_WIDTH,
		ABOUT_WINDOW_CLOSE_BUTTON_COLOR,
		ABOUT_WINDOW_CLOSE_BUTTON_HOVER_COLOR,
		ABOUT_WINDOW_CLOSE_BUTTON_ACTIVE_COLOR,
		ABOUT_WINDOW_TITLE_TEXT_PADDING_X,
		ABOUT_WINDOW_TITLE_TEXT_PADDING_Y,
		ABOUT_WINDOW_LABEL_OFFSET,
		ABOUT_WINDOW_TEXT_LINK_DEFAULT_COLOR,
		ABOUT_WINDOW_TEXT_LINK_OPENED_COLOR
	);

	// Build the structure panel layout.
	BuildStructurePanel(
		STRUCTURE_PANEL_RELATIVE_WIDTH,
		STRUCTURE_PANEL_OFFSET_LEFT,
		STRUCTURE_PANEL_OFFSET_TOP,
		STRUCTURE_PANEL_OFFSET_BOTTOM,
		STRUCTURE_PANEL_PADDING_X,
		STRUCTURE_PANEL_PADDING_Y,
		STRUCTURE_PANEL_TITLE_TEXT_PADDING_X,
		STRUCTURE_PANEL_TITLE_TEXT_PADDING_Y,
		STRUCTURE_PANEL_TITLE_BAR_HEIGHT_OFFSET,
		STRUCTURE_PANEL_TITLE_BAR_COLOR,
		STRUCTURE_PANEL_TITLE_BAR_LINE_COLOR,
		STRUCTURE_PANEL_SCROLLBAR_COLOR,
		STRUCTURE_PANEL_SCROLLBAR_GRAB_COLOR,
		STRUCTURE_PANEL_SCROLLBAR_GRAB_HOVER_COLOR,
		STRUCTURE_PANEL_SCROLLBAR_GRAB_ACTIVE_COLOR,
		STRUCTURE_PANEL_COLOR,
		STRUCTURE_PANEL_HEADER_COLOR,
		STRUCTURE_PANEL_HEADER_HOVER_COLOR,
		STRUCTURE_PANEL_HEADER_ACTIVE_COLOR,
		STRUCTURE_PANEL_TEXT_PRIORITY_COLOR,
		STRUCTURE_PANEL_SMBIOS_NAMESPACE_COLOR,
		STRUCTURE_PANEL_OTHER_NAMESPACE_COLOR
	);

	// Build the state panel layout.
	BuildStatePanel(
		STATE_PANEL_RELATIVE_WIDTH,
		STATE_PANEL_HEIGHT,
		STATE_PANEL_OFFSET_RIGHT,
		STATE_PANEL_OFFSET_TOP,
		STATE_PANEL_PADDING_X,
		STATE_PANEL_PADDING_Y,
		STATE_PANEL_TITLE_TEXT_PADDING_X,
		STATE_PANEL_TITLE_TEXT_PADDING_Y,
		STATE_PANEL_TITLE_BAR_HEIGHT_OFFSET,
		STATE_PANEL_TITLE_BAR_COLOR,
		STATE_PANEL_TITLE_BAR_LINE_COLOR,
		STATE_PANEL_COLOR,
		STATE_PANEL_CHECKBOX_COLOR,
		STATE_PANEL_CHECKBOX_HOVER_COLOR,
		STATE_PANEL_CHECKBOX_ACTIVE_COLOR,
		STATE_PANEL_CHECKBOX_CHECKMARK_COLOR
	);

	// Draw the title bar.
	DrawTitleBar(hWnd, State);

	// Draw the menu bar.
	DrawMenuBar(hWnd, State);

	// Draw the about popup window if it's open.
	DrawAboutWindow();

	// Draw the structure panel.
	DrawStructurePanel(State);

	// Draw the state panel.
	DrawStatePanel(hWnd, State);

	// End the main window.
	ImGui::End();
}