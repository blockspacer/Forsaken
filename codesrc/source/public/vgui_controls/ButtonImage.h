#ifndef __BUTTONIMAGE_H_
#define __BUTTONIMAGE_H_

#include <vgui_controls/Button.h>
#include <vgui_controls/Image.h>

namespace vgui
{
	class ButtonImage : public Button
	{
	public:
		DECLARE_CLASS_SIMPLE(ButtonImage, Button);

		// Constructor & Deconstructor
		ButtonImage(Panel *pParent, const char *czPanelName, const char *czText, Panel *pActionSignalTarget = NULL, 
			const char *czCommand = NULL);
		ButtonImage(Panel *pParent, const char *czPanelName, const wchar_t *czText, Panel *pActionSignalTarget = NULL, 
			const char *czCommand = NULL);
		~ButtonImage();

		// Public Accessor Functions
		bool IsActive() { return m_bIsSelected; }
		void SetActive(bool state) { m_bIsSelected = state; }

		// Public Functions

		// Public Variables

	private:
		// Private Functions

		// Private Variables

	protected:
		// Protected Functions
		virtual void ApplySchemeSettings(IScheme *pScheme);
		virtual void ApplySettings(KeyValues *pResources);
		virtual void DrawFocusBorder(int tx0, int ty0, int tx1, int ty1);
		virtual void GetSettings(KeyValues *pResources);
		virtual void PaintBackground();
		void Init();

		// Protected Variables
		bool m_bIsSelected;
		char *m_czActiveImageName;
		char *m_czArmedImageName;
		char *m_czDepressedImageName;
		char *m_czDisabledImageName;
		char *m_czEnabledImageName;
		Color m_colorNonSelectedFocus;
		Color m_colorSelectedFocus;
		IImage *m_pActiveImage;
		IImage *m_pArmedImage;
		IImage *m_pDepressedImage;
		IImage *m_pDisabledImage;
		IImage *m_pEnabledImage;
	};
};

#endif