#include "Modal.h"

#include <IconsFontAwesome5.h>
#include "EditorUI.h"

#include <algorithm>

namespace aka {

bool Modal::LoadButton(const char* label, Path* resultPath) {
	char buffer[256];
	snprintf(buffer, 256, "%s##popupmodal", label);
	static char currentPathBuffer[256];
	static Path currentPath;
	static Path* selectedPath;
	static std::vector<Path> paths;
	if (ImGui::Button(label))
	{
		ImGui::OpenPopup(buffer);
		currentPath = Asset::path("");
		String::copy(currentPathBuffer, 256, currentPath.cstr());
		selectedPath = nullptr;
		paths = Path::enumerate(currentPath);
	}
	bool loading = false;
	bool openFlag = true;
	if (ImGui::BeginPopupModal(buffer, &openFlag, ImGuiWindowFlags_AlwaysAutoResize))
	{
		bool updatedList = false;
		// Go to parent folder
		if (ImGui::Button(ICON_FA_ARROW_UP))
		{
			currentPath = currentPath.up();
			updatedList = true;
		}
		// Refresh directory
		ImGui::SameLine();
		if (ImGui::Button(ICON_FA_SYNC))
		{
			updatedList = true;
		}
		ImGui::SameLine();
		// Path 
		if (ImGui::InputText("##Path", currentPathBuffer, 256))
		{
			currentPath = currentPathBuffer;
			updatedList = true;
		}
		if (ImGui::BeginChild("##list", ImVec2(0, 200), true))
		{
			for (Path& path : paths)
			{
				bool selected = (&path == selectedPath);
				bool isFolder = (path.str().last() == '/');
				if (isFolder)
				{
					int err = snprintf(buffer, 256, "%s %s", ICON_FA_FOLDER, path.cstr());
					if (ImGui::Selectable(buffer, &selected))
					{
						selectedPath = &path;
					}
					if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
					{
						currentPath += path;
						updatedList = true;
					}
				}
				else
				{
					int err = snprintf(buffer, 256, "%s %s", ICON_FA_FILE, path.cstr());
					if (ImGui::Selectable(buffer, &selected))
					{
						selectedPath = &path;
					}
					if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
					{
						loading = true;
						*resultPath = currentPath + path;
						ImGui::CloseCurrentPopup();
					}
				}
			}
		}
		ImGui::EndChild();
		if (updatedList)
		{
			String::copy(currentPathBuffer, 256, currentPath.cstr());
			paths = Path::enumerate(currentPath);
			selectedPath = nullptr;
		}
		if (ImGui::Button("Load"))
		{
			if (selectedPath != nullptr)
			{
				loading = true;
				*resultPath = currentPath + *selectedPath;
				ImGui::CloseCurrentPopup();
			}
		}
		ImGui::SameLine();
		if (ImGui::Button("Close"))
		{
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}
	return (loading && resultPath->str().length() > 0 && file::exist(Path(*resultPath)));
}

bool Modal::Error(const char* label, std::string& error)
{
	static bool errorShown = false;
	if (error.size() > 0 && !errorShown)
	{
		ImGui::OpenPopup("Error");
		ImGui::SetNextWindowSize(ImVec2(200, -1));
		errorShown = true;
	}
	if (ImGui::BeginPopupModal("Error", &errorShown, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text("Error while loading resources : ");
		ImGui::Separator();
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(ImColor(255, 0, 50)));
		ImGui::TextWrapped("%s", error.c_str());
		ImGui::PopStyleColor();
		ImGui::Separator();
		if (ImGui::Button("OK"))
		{
			ImGui::CloseCurrentPopup();
			errorShown = false;
			error = "";
		}
		ImGui::EndPopup();
	}
	if (!errorShown)
		error = "";
	return false;
}
;

};