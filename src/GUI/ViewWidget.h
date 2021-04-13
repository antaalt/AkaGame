#pragma	once
#include "../EditorApp.h"

namespace aka {

class ViewWidget : 
	public EditorWidget, 
	EventListener<ViewChangedEvent>
{
public:
	void draw(World& world) override;
	void onReceive(const ViewChangedEvent& event);
	template <typename T>
	void selectable(const char* name);
private:
	View::Ptr m_currentView;
};


template <typename T>
void ViewWidget::selectable(const char* name)
{
	if (ImGui::Selectable(name, dynamic_cast<T*>(m_currentView.get()) != nullptr))
		EventDispatcher<ViewChangedEvent>::emit(ViewChangedEvent{ View::create<T>() });
}

};