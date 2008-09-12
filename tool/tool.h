#pragma once


namespace tool {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;
	using namespace VisualHint::SmartPropertyGrid;

	/// <summary>
	/// Summary for Form1
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class tool_form : public System::Windows::Forms::Form
	{
	public:
		tool_form(void)
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//
//			propertyGrid1->SelectedObject=this;

			PropertyEnumerator^ catEnum = propertyGrid1->AppendRootCategory(1, "Application");
			PropertyEnumerator^ propEnum =propertyGrid1->AppendManagedProperty(catEnum, 2, "AllowQuit",
				bool::typeid, Application::AllowQuit, "hulyeseg", gcnew ReadOnlyAttribute(true));
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~tool_form()
		{
			if (components)
			{
				delete components;
			}
		}
	private: VisualHint::SmartPropertyGrid::PropertyGrid^  propertyGrid1;
	private: System::Windows::Forms::SplitContainer^  splitContainer1;
	private: System::Windows::Forms::SplitContainer^  splitContainer2;
	protected: 

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->propertyGrid1 = (gcnew VisualHint::SmartPropertyGrid::PropertyGrid());
			this->splitContainer1 = (gcnew System::Windows::Forms::SplitContainer());
			this->splitContainer2 = (gcnew System::Windows::Forms::SplitContainer());
			this->splitContainer1->Panel2->SuspendLayout();
			this->splitContainer1->SuspendLayout();
			this->splitContainer2->Panel2->SuspendLayout();
			this->splitContainer2->SuspendLayout();
			this->SuspendLayout();
			// 
			// propertyGrid1
			// 
			this->propertyGrid1->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
			this->propertyGrid1->CommentsBackColor = System::Drawing::SystemColors::Control;
			this->propertyGrid1->CommentsForeColor = System::Drawing::SystemColors::ControlText;
			this->propertyGrid1->CommentsHeight = 40;
			this->propertyGrid1->CommentsVisibility = false;
			this->propertyGrid1->DisplayMode = VisualHint::SmartPropertyGrid::PropertyGrid::DisplayModes::Categorized;
			this->propertyGrid1->Dock = System::Windows::Forms::DockStyle::Fill;
			this->propertyGrid1->EllipsisMode = VisualHint::SmartPropertyGrid::PropertyGrid::EllipsisModes::EllipsisOnValues;
			this->propertyGrid1->Font = (gcnew System::Drawing::Font(L"Tahoma", 11, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::World));
			this->propertyGrid1->GridBackColor = System::Drawing::SystemColors::Window;
			this->propertyGrid1->GridDisabledForeColor = System::Drawing::SystemColors::GrayText;
			this->propertyGrid1->GridForeColor = System::Drawing::SystemColors::ControlText;
			this->propertyGrid1->HighlightedTextColor = System::Drawing::SystemColors::HighlightText;
			this->propertyGrid1->LabelColumnWidth = 150;
			this->propertyGrid1->LeftColumnWidth = 16;
			this->propertyGrid1->Location = System::Drawing::Point(0, 0);
			this->propertyGrid1->MultiSelectMode = VisualHint::SmartPropertyGrid::PropertyGrid::MultiSelectModes::None;
			this->propertyGrid1->Name = L"propertyGrid1";
			this->propertyGrid1->PropertyVerticalMargin = 4;
			this->propertyGrid1->ReadOnlyVisual = VisualHint::SmartPropertyGrid::PropertyGrid::ReadOnlyVisuals::Disabled;
			this->propertyGrid1->SelectedBkgColor = System::Drawing::SystemColors::Highlight;
			this->propertyGrid1->SelectedNotFocusedBkgColor = System::Drawing::SystemColors::ActiveBorder;
			this->propertyGrid1->Size = System::Drawing::Size(265, 363);
			this->propertyGrid1->TabIndex = 0;
			this->propertyGrid1->ToolbarVisibility = false;
			this->propertyGrid1->ToolTipMode = VisualHint::SmartPropertyGrid::PropertyGrid::ToolTipModes::None;
			// 
			// splitContainer1
			// 
			this->splitContainer1->Dock = System::Windows::Forms::DockStyle::Fill;
			this->splitContainer1->Location = System::Drawing::Point(0, 0);
			this->splitContainer1->Name = L"splitContainer1";
			// 
			// splitContainer1.Panel2
			// 
			this->splitContainer1->Panel2->Controls->Add(this->splitContainer2);
			this->splitContainer1->Size = System::Drawing::Size(962, 605);
			this->splitContainer1->SplitterDistance = 693;
			this->splitContainer1->TabIndex = 1;
			// 
			// splitContainer2
			// 
			this->splitContainer2->Dock = System::Windows::Forms::DockStyle::Fill;
			this->splitContainer2->Location = System::Drawing::Point(0, 0);
			this->splitContainer2->Name = L"splitContainer2";
			this->splitContainer2->Orientation = System::Windows::Forms::Orientation::Horizontal;
			// 
			// splitContainer2.Panel2
			// 
			this->splitContainer2->Panel2->Controls->Add(this->propertyGrid1);
			this->splitContainer2->Size = System::Drawing::Size(265, 605);
			this->splitContainer2->SplitterDistance = 238;
			this->splitContainer2->TabIndex = 0;
			// 
			// tool_form
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(962, 605);
			this->Controls->Add(this->splitContainer1);
			this->Name = L"tool_form";
			this->Text = L"tool";
			this->splitContainer1->Panel2->ResumeLayout(false);
			this->splitContainer1->ResumeLayout(false);
			this->splitContainer2->Panel2->ResumeLayout(false);
			this->splitContainer2->ResumeLayout(false);
			this->ResumeLayout(false);

		}
#pragma endregion
	};
}

