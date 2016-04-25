#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


namespace NewVision {

	/// <summary>
	/// Summary for TrackExportSettings
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class TrackExportSettings : public System::Windows::Forms::Form
	{
	public:
		TrackExportSettings(void)
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~TrackExportSettings()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::Button^  buttonOk;
	private: System::Windows::Forms::Button^  buttonCancel;

	public: 
	System::Windows::Forms::GroupBox^  groupBox1;
	System::Windows::Forms::Label^  label2;
	System::Windows::Forms::Label^  label1;
	System::Windows::Forms::GroupBox^  box1;
	System::Windows::Forms::CheckBox^  crowdingData;
	System::Windows::Forms::CheckBox^  groupData;
	System::Windows::Forms::TextBox^  allowOutsideFor;
	System::Windows::Forms::CheckBox^  zoneStatistics;
	public: System::Windows::Forms::CheckBox^  crowdingBefore;



	private: 

	private: 
	protected: 

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
			this->buttonOk = (gcnew System::Windows::Forms::Button());
			this->buttonCancel = (gcnew System::Windows::Forms::Button());
			this->groupData = (gcnew System::Windows::Forms::CheckBox());
			this->crowdingData = (gcnew System::Windows::Forms::CheckBox());
			this->box1 = (gcnew System::Windows::Forms::GroupBox());
			this->groupBox1 = (gcnew System::Windows::Forms::GroupBox());
			this->crowdingBefore = (gcnew System::Windows::Forms::CheckBox());
			this->zoneStatistics = (gcnew System::Windows::Forms::CheckBox());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->allowOutsideFor = (gcnew System::Windows::Forms::TextBox());
			this->box1->SuspendLayout();
			this->groupBox1->SuspendLayout();
			this->SuspendLayout();
			// 
			// buttonOk
			// 
			this->buttonOk->DialogResult = System::Windows::Forms::DialogResult::OK;
			this->buttonOk->Location = System::Drawing::Point(128, 235);
			this->buttonOk->Name = L"buttonOk";
			this->buttonOk->Size = System::Drawing::Size(75, 23);
			this->buttonOk->TabIndex = 6;
			this->buttonOk->Text = L"Ok";
			this->buttonOk->UseVisualStyleBackColor = true;
			// 
			// buttonCancel
			// 
			this->buttonCancel->DialogResult = System::Windows::Forms::DialogResult::Cancel;
			this->buttonCancel->Location = System::Drawing::Point(209, 235);
			this->buttonCancel->Name = L"buttonCancel";
			this->buttonCancel->Size = System::Drawing::Size(75, 23);
			this->buttonCancel->TabIndex = 7;
			this->buttonCancel->Text = L"Cancel";
			this->buttonCancel->UseVisualStyleBackColor = true;
			// 
			// groupData
			// 
			this->groupData->AutoSize = true;
			this->groupData->Checked = true;
			this->groupData->CheckState = System::Windows::Forms::CheckState::Checked;
			this->groupData->Location = System::Drawing::Point(17, 25);
			this->groupData->Name = L"groupData";
			this->groupData->Size = System::Drawing::Size(60, 17);
			this->groupData->TabIndex = 1;
			this->groupData->Text = L"Groups";
			this->groupData->UseVisualStyleBackColor = true;
			// 
			// crowdingData
			// 
			this->crowdingData->AutoSize = true;
			this->crowdingData->Checked = true;
			this->crowdingData->CheckState = System::Windows::Forms::CheckState::Checked;
			this->crowdingData->Location = System::Drawing::Point(17, 48);
			this->crowdingData->Name = L"crowdingData";
			this->crowdingData->Size = System::Drawing::Size(70, 17);
			this->crowdingData->TabIndex = 2;
			this->crowdingData->Text = L"Crowding";
			this->crowdingData->UseVisualStyleBackColor = true;
			// 
			// box1
			// 
			this->box1->Controls->Add(this->crowdingData);
			this->box1->Controls->Add(this->groupData);
			this->box1->Location = System::Drawing::Point(12, 12);
			this->box1->Name = L"box1";
			this->box1->Size = System::Drawing::Size(132, 183);
			this->box1->TabIndex = 2;
			this->box1->TabStop = false;
			this->box1->Text = L"Choose data to export";
			// 
			// groupBox1
			// 
			this->groupBox1->Controls->Add(this->crowdingBefore);
			this->groupBox1->Controls->Add(this->zoneStatistics);
			this->groupBox1->Controls->Add(this->label2);
			this->groupBox1->Controls->Add(this->label1);
			this->groupBox1->Controls->Add(this->allowOutsideFor);
			this->groupBox1->Location = System::Drawing::Point(152, 12);
			this->groupBox1->Name = L"groupBox1";
			this->groupBox1->Size = System::Drawing::Size(249, 183);
			this->groupBox1->TabIndex = 4;
			this->groupBox1->TabStop = false;
			this->groupBox1->Text = L"Interest zones";
			// 
			// crowdingBefore
			// 
			this->crowdingBefore->AutoSize = true;
			this->crowdingBefore->Location = System::Drawing::Point(17, 71);
			this->crowdingBefore->Name = L"crowdingBefore";
			this->crowdingBefore->Size = System::Drawing::Size(216, 17);
			this->crowdingBefore->TabIndex = 5;
			this->crowdingBefore->Text = L"Crowding 30, 60, 90 and 120 sec before";
			this->crowdingBefore->UseVisualStyleBackColor = true;
			// 
			// zoneStatistics
			// 
			this->zoneStatistics->AutoSize = true;
			this->zoneStatistics->Checked = true;
			this->zoneStatistics->CheckState = System::Windows::Forms::CheckState::Checked;
			this->zoneStatistics->Location = System::Drawing::Point(17, 25);
			this->zoneStatistics->Name = L"zoneStatistics";
			this->zoneStatistics->Size = System::Drawing::Size(125, 17);
			this->zoneStatistics->TabIndex = 3;
			this->zoneStatistics->Text = L"Export zone statistics";
			this->zoneStatistics->UseVisualStyleBackColor = true;
			// 
			// label2
			// 
			this->label2->AutoSize = true;
			this->label2->Location = System::Drawing::Point(147, 48);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(38, 13);
			this->label2->TabIndex = 2;
			this->label2->Text = L"frames";
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(14, 48);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(81, 13);
			this->label1->TabIndex = 1;
			this->label1->Text = L"Allow ouside for";
			// 
			// allowOutsideFor
			// 
			this->allowOutsideFor->Location = System::Drawing::Point(98, 45);
			this->allowOutsideFor->Name = L"allowOutsideFor";
			this->allowOutsideFor->Size = System::Drawing::Size(44, 20);
			this->allowOutsideFor->TabIndex = 4;
			this->allowOutsideFor->Text = L"15";
			// 
			// TrackExportSettings
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(413, 283);
			this->Controls->Add(this->groupBox1);
			this->Controls->Add(this->buttonCancel);
			this->Controls->Add(this->box1);
			this->Controls->Add(this->buttonOk);
			this->Name = L"TrackExportSettings";
			this->Text = L"TrackExportSettings";
			this->Load += gcnew System::EventHandler(this, &TrackExportSettings::TrackExportSettings_Load);
			this->box1->ResumeLayout(false);
			this->box1->PerformLayout();
			this->groupBox1->ResumeLayout(false);
			this->groupBox1->PerformLayout();
			this->ResumeLayout(false);

		}
#pragma endregion
	private: System::Void TrackExportSettings_Load(System::Object^  sender, System::EventArgs^  e) {
			 }
};
}
