// tool.cpp : main project file.

#include "stdafx.h"
#include "tool.h"

using namespace tool;

[STAThreadAttribute]
int main(array<System::String ^> ^args)
{
	// Enabling Windows XP visual effects before any controls are created
	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false); 

	// Create the main window and run it
	Application::Run(gcnew tool::tool_form());
	return 0;
}
