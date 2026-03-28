#include "UI.hpp"

#include <string>
#include <cstring>

#include "imgui.h"
#include <tchar.h>

#include "File.hpp"
#include "Cast.hpp"
#include "Util.hpp"
#include "Lenovo.hpp"
#include "Window.hpp"
#include "DMI.hpp"

static auto OpenHexViewer(APP_STATE& State, const void* Data, uint32_t Size) -> void
{
    if (!Data || !Size)
    {
        return;
    }

    State.HexViewer.Open = true;
    State.HexViewer.PendingOpen = true;
    State.HexViewer.Data = static_cast<const uint8_t*>(Data);
    State.HexViewer.Size = Size;
}

namespace UI
{
    namespace
    {
        constexpr float gTitleBarHeightOffset = 8.0f;
        constexpr float gButtonWidth = 32.0f;
        constexpr float gContentPadding = 10.0f;
        constexpr float gGap = 10.0f;
        constexpr float gInnerPadding = 8.0f;
        constexpr float gFileMenuWidth = 45.0f;
        constexpr float gHelpMenuWidth = 50.0f;

        static bool gAboutPopupOpen = false;

        struct UI_LAYOUT
        {
            float TitleBarHeight = 0.0f;
            float MenuBarHeight = 0.0f;
            float HeaderHeight = 0.0f;

            float ContentX = 0.0f;
            float ContentY = 0.0f;
            float ContentWidth = 0.0f;
            float ContentHeight = 0.0f;

            float LeftWidth = 0.0f;
            float RightWidth = 0.0f;

            float FilePanelHeight = 0.0f;
            float EncryptionPanelHeight = 0.0f;
            float DmiPanelHeight = 0.0f;
        };

        static UI_LAYOUT gLayout{};

        auto BuildLayout() -> void
        {
            const auto WindowSize = ImGui::GetWindowSize();

            gLayout.TitleBarHeight = ImGui::GetTextLineHeight() + gTitleBarHeightOffset;
            gLayout.MenuBarHeight = gLayout.TitleBarHeight;
            gLayout.HeaderHeight = ImGui::GetTextLineHeight() + 8.0f;

            gLayout.ContentX = gContentPadding;
            gLayout.ContentY = gLayout.TitleBarHeight + gLayout.MenuBarHeight + gContentPadding;
            gLayout.ContentWidth = WindowSize.x - gContentPadding * 2.0f;
            gLayout.ContentHeight = WindowSize.y - gLayout.ContentY - gContentPadding;

            gLayout.LeftWidth = (gLayout.ContentWidth - gGap) * 0.65f;
            gLayout.RightWidth = (gLayout.ContentWidth - gGap) * 0.35f;

            const float TextHeight = ImGui::GetTextLineHeight();
            const float ItemSpacingY = ImGui::GetStyle().ItemSpacing.y;
            const int FileInfoLineCount = 5;

            const float FileBodyHeight = TextHeight * FileInfoLineCount + ItemSpacingY * (FileInfoLineCount - 1);

            gLayout.FilePanelHeight = gLayout.HeaderHeight + gInnerPadding + FileBodyHeight + gInnerPadding;

            const float EncryptionBodyHeight = ImGui::GetFrameHeight() * 2.0f + ImGui::GetStyle().ItemSpacing.y;
            gLayout.EncryptionPanelHeight = gLayout.HeaderHeight + gInnerPadding + EncryptionBodyHeight + gInnerPadding;

            gLayout.DmiPanelHeight = gLayout.ContentHeight;
        }

        auto DrawPanelHeader(const char* Title, ImVec2 PanelPos, ImVec2 PanelSize) -> void
        {
            auto DrawList = ImGui::GetWindowDrawList();

            DrawList->AddRectFilled(
                PanelPos,
                ImVec2(PanelPos.x + PanelSize.x, PanelPos.y + gLayout.HeaderHeight),
                IM_COL32(45, 75, 110, 255)
            );

            DrawList->AddLine(
                ImVec2(PanelPos.x, PanelPos.y + gLayout.HeaderHeight),
                ImVec2(PanelPos.x + PanelSize.x, PanelPos.y + gLayout.HeaderHeight),
                IM_COL32(70, 70, 70, 255)
            );

            ImGui::SetCursorPos(ImVec2(10.0f, (gLayout.HeaderHeight - ImGui::GetTextLineHeight()) * 0.5f));
            ImGui::TextUnformatted(Title);
        }

        auto DrawAboutPopup() -> void
        {
            constexpr float PopupWidth = 420.0f;

            ImGui::PushStyleColor(ImGuiCol_TitleBg, IM_COL32(45, 75, 110, 255));
            ImGui::PushStyleColor(ImGuiCol_TitleBgActive, IM_COL32(45, 75, 110, 255));
            ImGui::PushStyleColor(ImGuiCol_TitleBgCollapsed, IM_COL32(45, 75, 110, 255));

            if (ImGui::BeginPopupModal("About", &gAboutPopupOpen, ImGuiWindowFlags_AlwaysAutoResize))
            {
                ImGui::SetWindowSize(ImVec2(PopupWidth, 0.0f), ImGuiCond_Appearing);

                ImGui::TextUnformatted("Lenovo DMI Decryptor 1.4.0");
                ImGui::Spacing();

                ImGui::Separator();

                ImGui::Spacing();
                ImGui::TextUnformatted("Copyright © 2026, Shmurkio");
                ImGui::TextUnformatted("License: MIT License");
                ImGui::TextUnformatted("GitHub:");
                ImGui::SameLine();

                const char* Url = "https://github.com/Shmurkio/LenovoDMIDecryptor";
                const char* Label = "Shmurkio/LenovoDMIDecryptor";

                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.2f, 0.6f, 1.0f, 1.0f));
                ImGui::TextUnformatted(Label);
                ImGui::PopStyleColor();

                if (ImGui::IsItemHovered())
                {
                    ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);

                    auto min = ImGui::GetItemRectMin();
                    auto max = ImGui::GetItemRectMax();

                    ImGui::GetWindowDrawList()->AddLine(
                        ImVec2(min.x, max.y),
                        ImVec2(max.x, max.y),
                        IM_COL32(100, 150, 255, 255)
                    );

                    ImGui::SetTooltip("%s", Url);
                }

                if (ImGui::IsItemClicked())
                {
                    ShellExecuteA(nullptr, "open", Url, nullptr, nullptr, SW_SHOWNORMAL);
                }
                ImGui::TextUnformatted("Discord: shmurkio");
                ImGui::Spacing();

                ImGui::Separator();

                ImGui::Spacing();
                ImGui::TextWrapped("Disclaimer: I am not responsible for any damage, data loss, or other issues caused by the use of this software.");
                ImGui::Spacing();
                ImGui::EndPopup();
            }

            ImGui::PopStyleColor(3);
        }

        static auto DrawHexViewerPopup(APP_STATE& State) -> void
        {
            if (State.HexViewer.PendingOpen)
            {
                ImGui::OpenPopup("Hex Viewer");
                State.HexViewer.PendingOpen = false;
            }

            ImGui::SetNextWindowSize(ImVec2(608.0f, 500.0f), ImGuiCond_Appearing);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(6.0f, 6.0f));
            ImGui::PushStyleColor(ImGuiCol_TitleBg, IM_COL32(45, 75, 110, 255));
            ImGui::PushStyleColor(ImGuiCol_TitleBgActive, IM_COL32(45, 75, 110, 255));
            ImGui::PushStyleColor(ImGuiCol_TitleBgCollapsed, IM_COL32(45, 75, 110, 255));

            if (ImGui::BeginPopupModal("Hex Viewer", &State.HexViewer.Open, ImGuiWindowFlags_NoResize))
            {
                ImGui::BeginChild("HexViewerScroll", ImVec2(0.0f, 0.0f), true);

                constexpr uint32_t BytesPerRow = 16;

                for (uint32_t Offset = 0; Offset < State.HexViewer.Size; Offset += BytesPerRow)
                {
                    ImGui::Text("%08X", Offset);
                    ImGui::SameLine();

                    char HexBuffer[BytesPerRow * 3 + 1] = {};
                    char AsciiBuffer[BytesPerRow + 1] = {};

                    int HexIndex = 0;
                    int AsciiIndex = 0;

                    for (uint32_t i = 0; i < BytesPerRow; ++i)
                    {
                        const uint32_t Index = Offset + i;

                        if (Index < State.HexViewer.Size)
                        {
                            const uint8_t Byte = State.HexViewer.Data[Index];
                            HexIndex += sprintf_s(&HexBuffer[HexIndex], sizeof(HexBuffer) - HexIndex, "%02X ", Byte);
                            AsciiBuffer[AsciiIndex++] = (Byte >= 32 && Byte <= 126) ? static_cast<char>(Byte) : '.';
                        }
                        else
                        {
                            HexIndex += sprintf_s(&HexBuffer[HexIndex], sizeof(HexBuffer) - HexIndex, "   ");
                            AsciiBuffer[AsciiIndex++] = ' ';
                        }
                    }

                    AsciiBuffer[AsciiIndex] = '\0';

                    constexpr float Gap = 20.0f;

                    ImGui::SameLine(0.0f, Gap);
                    ImGui::TextUnformatted(HexBuffer);

                    ImGui::SameLine(0.0f, Gap);
                    ImGui::Text("| %s |", AsciiBuffer);
                }

                ImGui::EndChild();
                
                ImGui::EndPopup();
            }

            ImGui::PopStyleVar();
            ImGui::PopStyleColor(3);
        }

    }

    auto DrawMainWindow(HWND hWnd, APP_STATE& State) -> void
    {
        auto Viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(Viewport->Pos);
        ImGui::SetNextWindowSize(Viewport->Size);

        const auto Flags =
            ImGuiWindowFlags_NoTitleBar |
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoCollapse;

        ImGui::Begin("MainWindow", nullptr, Flags);

        BuildLayout();

        DrawTitleBar(hWnd, State);
        DrawMenuBar(hWnd, State);
        DrawDmiPanel(State);
        DrawFileInfoPanel(State);
        DrawEncryptionPanel(hWnd, State);
        DrawHexViewerPopup(State);

        ImGui::End();
    }

    auto DrawTitleBar(HWND hWnd, APP_STATE& State) -> void
    {
        auto WindowPos = ImGui::GetWindowPos();
        auto WindowSize = ImGui::GetWindowSize();
        auto DrawList = ImGui::GetWindowDrawList();

        DrawList->AddRectFilled(
            WindowPos,
            ImVec2(WindowPos.x + WindowSize.x, WindowPos.y + gLayout.TitleBarHeight),
            IM_COL32(35, 35, 35, 255)
        );

        DrawList->AddLine(
            ImVec2(WindowPos.x, WindowPos.y + gLayout.TitleBarHeight),
            ImVec2(WindowPos.x + WindowSize.x, WindowPos.y + gLayout.TitleBarHeight),
            IM_COL32(70, 70, 70, 255)
        );

        ImGui::SetCursorPos(ImVec2(0.0f, 0.0f));
        ImGui::InvisibleButton("TitleBarDragZone", ImVec2(WindowSize.x - gButtonWidth * 2.0f, gLayout.TitleBarHeight));

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

        std::string TitleText = "Lenovo DMI Decryptor 1.4.0";

        if (State.FileInfo.Loaded && !State.FileInfo.FileName.empty())
        {
            TitleText += " - ";
            TitleText += State.FileInfo.FileName;

            if (State.FileInfo.Dirty)
            {
                TitleText += "*";
            }
        }

        ImGui::SetCursorPos(ImVec2(10.0f, (gLayout.TitleBarHeight - ImGui::GetTextLineHeight()) * 0.5f));
        ImGui::TextUnformatted(TitleText.c_str());

        ImGui::SetCursorPos(ImVec2(WindowSize.x - gButtonWidth * 2.0f, 0.0f));
        if (ImGui::Button("-", ImVec2(gButtonWidth, gLayout.TitleBarHeight)))
        {
            ShowWindow(hWnd, SW_MINIMIZE);
        }

        ImGui::SetCursorPos(ImVec2(WindowSize.x - gButtonWidth, 0.0f));
        if (ImGui::Button("X", ImVec2(gButtonWidth, gLayout.TitleBarHeight)))
        {
            if (Window::HandleUnsavedChanges(hWnd, State.Window))
            {
                DestroyWindow(hWnd);
            }
        }
    }

    auto DrawMenuBar(HWND hWnd, APP_STATE& State) -> void
    {
        auto WindowPos = ImGui::GetWindowPos();
        auto WindowSize = ImGui::GetWindowSize();
        auto DrawList = ImGui::GetWindowDrawList();

        DrawList->AddRectFilled(
            ImVec2(WindowPos.x, WindowPos.y + gLayout.TitleBarHeight),
            ImVec2(WindowPos.x + WindowSize.x, WindowPos.y + gLayout.TitleBarHeight + gLayout.MenuBarHeight),
            IM_COL32(45, 45, 45, 255)
        );

        DrawList->AddLine(
            ImVec2(WindowPos.x, WindowPos.y + gLayout.TitleBarHeight + gLayout.MenuBarHeight),
            ImVec2(WindowPos.x + WindowSize.x, WindowPos.y + gLayout.TitleBarHeight + gLayout.MenuBarHeight),
            IM_COL32(70, 70, 70, 255)
        );

        // File menu.
        const auto FilePos = ImVec2(0.0f, gLayout.TitleBarHeight);

        ImGui::SetCursorPos(FilePos);
        ImGui::InvisibleButton("FileMenuButton", ImVec2(gFileMenuWidth, gLayout.MenuBarHeight));

        const auto FileHovered = ImGui::IsItemHovered();
        const auto FileClicked = ImGui::IsItemClicked();

        if (FileClicked)
        {
            ImGui::OpenPopup("FileMenu");
        }

        if (FileHovered)
        {
            DrawList->AddRectFilled(
                ImVec2(WindowPos.x + FilePos.x, WindowPos.y + FilePos.y),
                ImVec2(WindowPos.x + FilePos.x + gFileMenuWidth, WindowPos.y + FilePos.y + gLayout.MenuBarHeight),
                IM_COL32(70, 70, 70, 255)
            );
        }

        {
            const char* FileLabel = "File";
            const auto TextSize = ImGui::CalcTextSize(FileLabel);
            const auto TextX = WindowPos.x + FilePos.x + 10.0f;
            const auto TextY = WindowPos.y + FilePos.y + (gLayout.MenuBarHeight - TextSize.y) * 0.5f;

            DrawList->AddText(ImVec2(TextX, TextY), IM_COL32(255, 255, 255, 255), FileLabel);
        }

        ImGui::SetNextWindowPos(ImVec2(WindowPos.x + FilePos.x, WindowPos.y + FilePos.y + gLayout.MenuBarHeight));

        if (ImGui::BeginPopup("FileMenu", ImGuiWindowFlags_NoMove))
        {
            if (ImGui::MenuItem("Open image file..."))
            {
                OpenImageFile(hWnd, State);
            }

            if (ImGui::MenuItem("Save image file...", nullptr, false, State.FileInfo.Loaded && State.FileInfo.Dirty))
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

        // Help menu.
        const auto HelpPos = ImVec2(gFileMenuWidth, gLayout.TitleBarHeight);

        ImGui::SetCursorPos(HelpPos);
        ImGui::InvisibleButton("HelpMenuButton", ImVec2(gHelpMenuWidth, gLayout.MenuBarHeight));

        const auto HelpHovered = ImGui::IsItemHovered();
        const auto HelpClicked = ImGui::IsItemClicked();

        if (HelpClicked)
        {
            ImGui::OpenPopup("HelpMenu");
        }

        if (HelpHovered)
        {
            DrawList->AddRectFilled(
                ImVec2(WindowPos.x + HelpPos.x, WindowPos.y + HelpPos.y),
                ImVec2(WindowPos.x + HelpPos.x + gHelpMenuWidth, WindowPos.y + HelpPos.y + gLayout.MenuBarHeight),
                IM_COL32(70, 70, 70, 255)
            );
        }

        {
            const char* HelpLabel = "Help";
            const auto TextSize = ImGui::CalcTextSize(HelpLabel);
            const auto TextX = WindowPos.x + HelpPos.x + 10.0f;
            const auto TextY = WindowPos.y + HelpPos.y + (gLayout.MenuBarHeight - TextSize.y) * 0.5f;

            DrawList->AddText(ImVec2(TextX, TextY), IM_COL32(255, 255, 255, 255), HelpLabel);
        }

        ImGui::SetNextWindowPos(ImVec2(WindowPos.x + HelpPos.x, WindowPos.y + HelpPos.y + gLayout.MenuBarHeight));

        bool OpenAboutPopup = false;

        if (ImGui::BeginPopup("HelpMenu", ImGuiWindowFlags_NoMove))
        {
            if (ImGui::MenuItem("About"))
            {
                OpenAboutPopup = true;
            }

            ImGui::EndPopup();
        }

        if (OpenAboutPopup)
        {
            gAboutPopupOpen = true;
            ImGui::OpenPopup("About");
        }

        DrawAboutPopup();
    }

    auto DrawDmiPanel(APP_STATE& State) -> void
    {
        ImGui::SetCursorPos(ImVec2(gLayout.ContentX, gLayout.ContentY));

        ImGui::BeginChild(
            "DmiInfoPanelOuter",
            ImVec2(gLayout.LeftWidth, gLayout.DmiPanelHeight),
            true,
            ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse
        );

        {
            const auto PanelPos = ImGui::GetWindowPos();
            const auto PanelSize = ImGui::GetWindowSize();

            DrawPanelHeader("DMI Information", PanelPos, PanelSize);

            ImGui::SetCursorPos(ImVec2(gInnerPadding, gLayout.HeaderHeight + gInnerPadding));

            ImGui::BeginChild(
                "DmiInfoPanelBody",
                ImVec2(PanelSize.x - gInnerPadding * 2.0f, PanelSize.y - gLayout.HeaderHeight - gInnerPadding * 2.0f),
                false,
                ImGuiWindowFlags_None
            );

            if (State.FileInfo.Loaded && State.DmiBlock1Found && State.DmiBlock1 && State.DmiBlock2Found && State.DmiBlock2)
            {
                auto DrawHeader = [&](const char* Name, Lenovo::PDMI_DATA Block)
                {
                    const bool BlockOpen = ImGui::TreeNode(Name);

                    if (ImGui::BeginPopupContextItem())
                    {
                        if (ImGui::MenuItem("Open in Hex Viewer"))
                        {
                            OpenHexViewer(State, Block, State.DmiBlockSize);
                        }

                        ImGui::EndPopup();
                    }

                    if (BlockOpen)
                    {
                        const auto Offset = Cast::To<uint32_t>(Cast::To<uint8_t*>(Block) - State.FileInfo.Buffer);

                        char SignatureText[5] = {};
                        memcpy(SignatureText, &Block->Header.Signature, sizeof(Block->Header.Signature));

                        const bool HeaderOpen = ImGui::TreeNode("Header");

                        if (ImGui::BeginPopupContextItem())
                        {
                            if (ImGui::MenuItem("Open in Hex Viewer"))
                            {
                                OpenHexViewer(State, &Block->Header, sizeof(Lenovo::DMI_HEADER));
                            }

                            ImGui::EndPopup();
                        }

                        if (HeaderOpen)
                        {
                            ImGui::Text("Offset:");    ImGui::SameLine(120.0f); ImGui::Text("0x%08X", Offset);
                            ImGui::Text("Size:");      ImGui::SameLine(120.0f); ImGui::Text("0x%08X (%u)", State.DmiBlockSize, State.DmiBlockSize);
                            ImGui::Text("Signature:"); ImGui::SameLine(120.0f); ImGui::Text("%s (0x%08X)", SignatureText, Block->Header.Signature);
                            ImGui::Text("Unknown1:");  ImGui::SameLine(120.0f); ImGui::Text("0x%08X (%u)", Block->Header.Unknown1, Block->Header.Unknown1);
                            ImGui::Text("Entries:");   ImGui::SameLine(120.0f); ImGui::Text("0x%08X (%u)", Block->Header.Entries, Block->Header.Entries);
                            ImGui::Text("Unknown2:");  ImGui::SameLine(120.0f); ImGui::Text("0x%02X (%u)", Block->Header.Unknown2, Block->Header.Unknown2);
                            ImGui::Text("Key:");       ImGui::SameLine(120.0f); ImGui::Text("0x%02X (%u)", Block->Header.Key, Block->Header.Key);
                            ImGui::Text("Checksum:");  ImGui::SameLine(120.0f); ImGui::Text("0x%04X (%u)", Block->Header.Checksum, Block->Header.Checksum);
                            ImGui::TreePop();
                        }

                        const bool BodyOpen = ImGui::TreeNode("Body");

                        if (ImGui::BeginPopupContextItem())
                        {
                            if (ImGui::MenuItem("Open in Hex Viewer"))
                            {
                                OpenHexViewer(
                                    State,
                                    reinterpret_cast<const uint8_t*>(Block) + sizeof(Lenovo::DMI_HEADER),
                                    State.DmiBlockSize - sizeof(Lenovo::DMI_HEADER)
                                );
                            }

                            ImGui::EndPopup();
                        }

                        if (BodyOpen)
                        {
                            if (ImGui::TreeNode("Unknown[32]"))
                            {
                                for (int i = 0; i < 32; ++i)
                                {
                                    ImGui::Text("[%02d]:", i);
                                    ImGui::SameLine(120.0f);
                                    ImGui::Text("0x%02X (%u)", Block->Unknown[i], Block->Unknown[i]);
                                }

                                ImGui::TreePop();
                            }

                            ImGui::TreePop();
                        }

                        ImGui::TreePop();
                    }
                };

                DrawHeader("DMI Block 1", State.DmiBlock1);
                DrawHeader("DMI Block 2", State.DmiBlock2);
            }

            ImGui::EndChild();
        }

        ImGui::EndChild();
    }

    auto DrawFileInfoPanel(APP_STATE& State) -> void
    {
        ImGui::SetCursorPos(ImVec2(gLayout.ContentX + gLayout.LeftWidth + gGap, gLayout.ContentY));

        ImGui::BeginChild(
            "FileInfoPanelOuter",
            ImVec2(gLayout.RightWidth, gLayout.FilePanelHeight),
            true,
            ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse
        );

        {
            const auto PanelPos = ImGui::GetWindowPos();
            const auto PanelSize = ImGui::GetWindowSize();

            DrawPanelHeader("File Information", PanelPos, PanelSize);

            ImGui::SetCursorPos(ImVec2(gInnerPadding, gLayout.HeaderHeight + gInnerPadding));

            ImGui::BeginChild(
                "FileInfoPanelBody",
                ImVec2(PanelSize.x - gInnerPadding * 2.0f, 0.0f),
                false,
                ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse
            );

            std::string SizeText = State.FileInfo.Loaded ? std::to_string(State.FileInfo.Size) + " bytes" : "";

            ImGui::Text("File name:"); ImGui::SameLine(120); ImGui::Text("%s", State.FileInfo.Loaded ? State.FileInfo.FileName.c_str() : "");
            ImGui::Text("Size:");      ImGui::SameLine(120); ImGui::Text("%s", SizeText.c_str());
            ImGui::Text("Created:");   ImGui::SameLine(120); ImGui::Text("%s", State.FileInfo.Loaded ? State.FileInfo.Created.c_str() : "");
            ImGui::Text("Modified:");  ImGui::SameLine(120); ImGui::Text("%s", State.FileInfo.Loaded ? State.FileInfo.Modified.c_str() : "");
            ImGui::Text("Accessed:");  ImGui::SameLine(120); ImGui::Text("%s", State.FileInfo.Loaded ? State.FileInfo.Accessed.c_str() : "");

            ImGui::EndChild();
        }

        ImGui::EndChild();
    }

    auto DrawEncryptionPanel(HWND hWnd, APP_STATE& State) -> void
    {
        ImGui::SetCursorPos(ImVec2(
            gLayout.ContentX + gLayout.LeftWidth + gGap,
            gLayout.ContentY + gLayout.FilePanelHeight + gGap
        ));

        ImGui::BeginChild(
            "EncryptionPanelOuter",
            ImVec2(gLayout.RightWidth, gLayout.EncryptionPanelHeight),
            true,
            ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse
        );

        {
            const auto PanelPos = ImGui::GetWindowPos();
            const auto PanelSize = ImGui::GetWindowSize();

            DrawPanelHeader("Encryption", PanelPos, PanelSize);

            ImGui::SetCursorPos(ImVec2(gInnerPadding, gLayout.HeaderHeight + gInnerPadding));

            bool Block1Encrypted = false;
            bool Block1Invalid = false;
            bool Block2Encrypted = false;
            bool Block2Invalid = false;

            if (State.FileInfo.Loaded && State.DmiBlock1Found && State.DmiBlock1)
            {
                Dmi::GetBlockEncrypted(State.DmiBlock1, Block1Encrypted, Block1Invalid);
            }

            if (State.FileInfo.Loaded && State.DmiBlock2Found && State.DmiBlock2)
            {
                Dmi::GetBlockEncrypted(State.DmiBlock2, Block2Encrypted, Block2Invalid);
            }

            const bool CanClickBlock1 =
                State.FileInfo.Loaded &&
                State.DmiBlock1Found &&
                State.DmiBlock1 &&
                !Block1Invalid &&
                State.DmiBlockSize > sizeof(Lenovo::DMI_HEADER);

            const bool CanClickBlock2 =
                State.FileInfo.Loaded &&
                State.DmiBlock2Found &&
                State.DmiBlock2 &&
                !Block2Invalid &&
                State.DmiBlockSize > sizeof(Lenovo::DMI_HEADER);

            ImGui::BeginDisabled(!CanClickBlock1);

            bool NewBlock1Encrypted = Block1Encrypted;
            if (ImGui::Checkbox("Block 1", &NewBlock1Encrypted))
            {
                auto Body = Cast::To<uint8_t*>(State.DmiBlock1) + sizeof(Lenovo::DMI_HEADER);
                auto BodySize = static_cast<size_t>(State.DmiBlockSize - sizeof(Lenovo::DMI_HEADER));

                if (!Util::XorBuffer(Body, BodySize, State.DmiBlock1->Header.Key))
                {
                    MessageBox(hWnd, _T("Failed to XOR DMI Block 1."), _T("Error"), MB_ICONERROR);
                }
                else
                {
                    State.DmiBlock1->Header.Checksum =
                        Util::GetChecksum16(Cast::To<uint8_t*>(&State.DmiBlock1->Unknown), State.DmiBlockSize - sizeof(Lenovo::DMI_HEADER));
                    State.FileInfo.Dirty = true;
                }
            }

            ImGui::EndDisabled();

            if (Block1Invalid)
            {
                ImGui::SameLine();
                ImGui::TextColored(ImVec4(1.0f, 0.3f, 0.3f, 1.0f), "Invalid");
            }

            ImGui::BeginDisabled(!CanClickBlock2);

            bool NewBlock2Encrypted = Block2Encrypted;
            if (ImGui::Checkbox("Block 2", &NewBlock2Encrypted))
            {
                auto Body = Cast::To<uint8_t*>(State.DmiBlock2) + sizeof(Lenovo::DMI_HEADER);
                auto BodySize = static_cast<size_t>(State.DmiBlockSize - sizeof(Lenovo::DMI_HEADER));

                if (!Util::XorBuffer(Body, BodySize, State.DmiBlock2->Header.Key))
                {
                    MessageBox(hWnd, _T("Failed to XOR DMI Block 2."), _T("Error"), MB_ICONERROR);
                }
                else
                {
                    State.DmiBlock2->Header.Checksum =
                        Util::GetChecksum16(Cast::To<uint8_t*>(&State.DmiBlock2->Unknown), State.DmiBlockSize - sizeof(Lenovo::DMI_HEADER));
                    State.FileInfo.Dirty = true;
                }
            }

            ImGui::EndDisabled();

            if (Block2Invalid)
            {
                ImGui::SameLine();
                ImGui::TextColored(ImVec4(1.0f, 0.3f, 0.3f, 1.0f), "Invalid");
            }
        }

        ImGui::EndChild();
    }

    auto OpenImageFile(HWND hWnd, APP_STATE& State) -> bool
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

    auto SaveImageFile(HWND hWnd, APP_STATE& State) -> bool
    {
        if (!File::SaveFile(State.FileInfo))
        {
            MessageBox(hWnd, _T("Failed to save file."), _T("Error"), MB_ICONERROR);
            return false;
        }

        return true;
    }
}