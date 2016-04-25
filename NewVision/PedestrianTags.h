#if !defined( PEDESTRIANTAGS_H )
#define PEDESTRIANTAGS_H
#pragma once

using namespace System;
using namespace System::Configuration;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Collections::Specialized;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

#using <system.dll>
#using <system.configuration.dll>
#include "TimeCell.h"
#include "TimeStampPairTags.h"

namespace NewVision {

	/// <summary>
	/// Summary for PedestrianTags
	/// </summary>
	public ref class PedestrianTags : public System::Windows::Forms::Form
	{
	public:
		PedestrianTags(NameValueCollection ^coll)
		{
			InitializeComponent();
			
			dataGridView1->CellValidating += gcnew DataGridViewCellValidatingEventHandler(this, &PedestrianTags::dataGridView1_CellValidating);
			dataGridView1->CellClick += gcnew DataGridViewCellEventHandler(this, &PedestrianTags::dataGridView1_CellClick);

			// initialize tags and value categories
			System::Configuration::KeyValueConfigurationCollection ^settings
				= ConfigurationManager::OpenExeConfiguration(ConfigurationUserLevel::None)->AppSettings->Settings;
			KeyValueConfigurationElement ^myElem = settings["tags"];
			if (!myElem)
				return;
			CommaDelimitedStringCollectionConverter ^cv = gcnew CommaDelimitedStringCollectionConverter();
			CommaDelimitedStringCollection ^rowColl = (CommaDelimitedStringCollection ^)cv->ConvertFromString(myElem->Value);
			// Read "tags"
			for (int i=0;i<rowColl->Count;i++) {
				DataGridViewRow ^dataGridRow = gcnew DataGridViewRow();
				dataGridRow->HeaderCell->Value = rowColl[i];
				// Read "values"
				myElem = settings[rowColl[i]+" values"];
				// 1 --- For categorical variables
				if (myElem) {
					// Timestamp pair
					if (myElem->Value == "#time#time") {
						//TimeCell^ time1 = gcnew TimeCell();
						// Add a clickable button
						//dataGridRow->Cells->Add(gcnew DataGridViewButtonCell());
						DataGridViewTextBoxCell ^txt1 = gcnew DataGridViewTextBoxCell();
						
						dataGridRow->Cells->Add(txt1);
						txt1->Style->BackColor = Color::Wheat;
						txt1->Style->ForeColor = Color::Red;
						txt1->Style->Alignment = DataGridViewContentAlignment::MiddleCenter;
						txt1->Style->Font = gcnew System::Drawing::Font( dataGridView1->Font,FontStyle::Bold );
						txt1->Value = "Edit...";
						txt1->ReadOnly = true;
					}
					else {
						CommaDelimitedStringCollection ^valueColl = (CommaDelimitedStringCollection ^)cv->ConvertFromString(myElem->Value);
						DataGridViewComboBoxCell ^cbo1 = gcnew DataGridViewComboBoxCell();
						// Add default "Select ..." value
						cbo1->Items->Add((Object^)default_string);
						// Add "values"
						for (int j=0;j<valueColl->Count;j++)
							cbo1->Items->Add((Object^)valueColl[j]);
						cbo1->Value = (Object^)default_string;
						dataGridRow->Cells->Add(cbo1);
					}
				}
				// 2 --- For numerical variables
				else {
					DataGridViewTextBoxCell ^txt1 = gcnew DataGridViewTextBoxCell();
					dataGridRow->Cells->Add(txt1);
				}
				this->dataGridView1->Rows->Add(dataGridRow);
			}

			// load tag values from collection
			//this->namesValues = coll;
			ImportTags(coll);
		}

		void ExportTags(NameValueCollection ^coll) {
			coll->Clear();
			DataGridViewRowCollection ^row = this->dataGridView1->Rows;
			for (int i=0;i<row->Count;i++) {
				if (row[i]->Cells[0]->ReadOnly)
					coll->Add(row[i]->HeaderCell->Value->ToString(), row[i]->Cells[0]->ToolTipText);
				else
					coll->Add(row[i]->HeaderCell->Value->ToString(), row[i]->Cells[0]->Value->ToString());
			}
		}
		
		void ImportTags(NameValueCollection ^coll) {
 			DataGridViewRowCollection ^row = this->dataGridView1->Rows;
			for (int i=0;i<row->Count;i++) {
				String ^rowName = row[i]->HeaderCell->Value->ToString();
				// Fill with tag values from BodyPath
				if (coll != nullptr && coll[rowName]) {
					if (row[i]->Cells[0]->ReadOnly)
						// if read-only cell: store a list of comma separated strings in the tooltip
						row[i]->Cells[0]->ToolTipText = coll->Get(rowName); 
					else {
						String^ record = coll->Get(rowName);
						if (record == "")
							record = (String^)default_string;
						row[i]->Cells[0]->Value = (System::Object^)record;
					}
				}
				// Fill with default tag values
				else {
					if (row[i]->Cells[0]->GetType() == TimeCell::typeid) {
						row[i]->Cells[0]->Value = DateTime::Now;
					}
					else if (row[i]->Cells[0]->GetType() == DataGridViewComboBoxCell::typeid) {
						row[i]->Cells[0]->Value = (System::Object^)default_string;
					}
					else if (row[i]->Cells[0]->GetType() == DataGridViewTextBoxCell::typeid)
						if (row[i]->Cells[0]->ReadOnly)
							row[i]->Cells[0]->ToolTipText = ""; 
						else
							row[i]->Cells[0]->Value = "0";
				}
			}
		}

	private: System::Void dataGridView1_CellClick(System::Object^  sender, DataGridViewCellEventArgs^ e)
		{
			// Ignore clicks that are not on button cells. 
			if ( dataGridView1->Rows[e->RowIndex]->Cells[e->ColumnIndex]->GetType() != DataGridViewTextBoxCell::typeid)
				return;
			
			// Find matching #time tag data
			String ^rowName = this->dataGridView1->Rows[e->RowIndex]->HeaderCell->Value->ToString();
			DataGridViewTextBoxCell ^cell = (DataGridViewTextBoxCell^)(dataGridView1->Rows[e->RowIndex]->Cells[e->ColumnIndex]);
			if (!cell->ReadOnly)
				return;
			CommaDelimitedStringCollectionConverter ^cv = gcnew CommaDelimitedStringCollectionConverter();
			CommaDelimitedStringCollection ^valueColl = (CommaDelimitedStringCollection ^)cv->ConvertFromString(cell->ToolTipText);
			// Spawn a new window, passing a collection string of timestamp pairs
			NewVision::TimeStampPairTags ^infoDlg = gcnew NewVision::TimeStampPairTags(valueColl);
			infoDlg->Text = String::Format("Timestamp Pairs For {0}", rowName);
			if (infoDlg->ShowDialog() == ::DialogResult::OK) {
				infoDlg->ExportTags(valueColl);
				cell->ToolTipText = cv->ConvertToString(valueColl);
			}
			delete infoDlg;
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~PedestrianTags()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::DataGridView^  dataGridView1;
	private: System::Windows::Forms::Button^  buttonOk;
	private: System::Windows::Forms::Button^  buttonCancel;
	//private: System::Collections::Specialized::NameValueCollection ^namesValues;
	private: static const System::String^ default_string = "Select...";
	private: System::Windows::Forms::DataGridViewComboBoxColumn^  Value1;







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
			this->dataGridView1 = (gcnew System::Windows::Forms::DataGridView());
			this->Value1 = (gcnew System::Windows::Forms::DataGridViewComboBoxColumn());
			this->buttonOk = (gcnew System::Windows::Forms::Button());
			this->buttonCancel = (gcnew System::Windows::Forms::Button());
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->dataGridView1))->BeginInit();
			this->SuspendLayout();
			// 
			// dataGridView1
			// 
			this->dataGridView1->AllowUserToAddRows = false;
			this->dataGridView1->AllowUserToDeleteRows = false;
			this->dataGridView1->ColumnHeadersHeightSizeMode = System::Windows::Forms::DataGridViewColumnHeadersHeightSizeMode::AutoSize;
			this->dataGridView1->Columns->AddRange(gcnew cli::array< System::Windows::Forms::DataGridViewColumn^  >(1) {this->Value1});
			this->dataGridView1->Dock = System::Windows::Forms::DockStyle::Fill;
			this->dataGridView1->Location = System::Drawing::Point(0, 0);
			this->dataGridView1->Name = L"dataGridView1";
			this->dataGridView1->RowHeadersWidth = 200;
			this->dataGridView1->Size = System::Drawing::Size(583, 508);
			this->dataGridView1->TabIndex = 0;
			this->dataGridView1->DataError += gcnew System::Windows::Forms::DataGridViewDataErrorEventHandler(this, &PedestrianTags::_DoNothingOnDataError);
			// 
			// Value1
			// 
			this->Value1->HeaderText = L"Value1";
			this->Value1->Name = L"Value1";
			this->Value1->Width = 150;
			// 
			// buttonOk
			// 
			this->buttonOk->DialogResult = System::Windows::Forms::DialogResult::OK;
			this->buttonOk->Location = System::Drawing::Point(50, 473);
			this->buttonOk->Name = L"buttonOk";
			this->buttonOk->Size = System::Drawing::Size(75, 23);
			this->buttonOk->TabIndex = 1;
			this->buttonOk->Text = L"Ok";
			this->buttonOk->UseVisualStyleBackColor = true;
			// 
			// buttonCancel
			// 
			this->buttonCancel->DialogResult = System::Windows::Forms::DialogResult::Cancel;
			this->buttonCancel->Location = System::Drawing::Point(131, 473);
			this->buttonCancel->Name = L"buttonCancel";
			this->buttonCancel->Size = System::Drawing::Size(75, 23);
			this->buttonCancel->TabIndex = 2;
			this->buttonCancel->Text = L"Cancel";
			this->buttonCancel->UseVisualStyleBackColor = true;
			// 
			// PedestrianTags
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(583, 508);
			this->Controls->Add(this->buttonCancel);
			this->Controls->Add(this->buttonOk);
			this->Controls->Add(this->dataGridView1);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedToolWindow;
			this->Name = L"PedestrianTags";
			this->StartPosition = System::Windows::Forms::FormStartPosition::CenterScreen;
			this->Text = L"PedestrianTags";
			this->Load += gcnew System::EventHandler(this, &PedestrianTags::PedestrianTags_Load);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->dataGridView1))->EndInit();
			this->ResumeLayout(false);

		}
#pragma endregion

	private: System::Void PedestrianTags_Load(System::Object^  sender, System::EventArgs^  e) {
	}
	 
	private: System::Void dataGridView1_CellValidating(System::Object^  sender, DataGridViewCellValidatingEventArgs ^ e) {
		if (dataGridView1->CurrentCell->GetType()->FullName != "System.Windows.Forms.DataGridViewTextBoxCell")
			return;
		DataGridViewTextBoxCell^ cell = (DataGridViewTextBoxCell^)dataGridView1->CurrentCell;
		// validate doubles in the first column only
		 if (cell && cell->OwningColumn->Index == 0) {
			 Double newDouble;
			 if (dataGridView1->Rows[e->RowIndex]->IsNewRow) 
				 return; 

			 if (dataGridView1->Rows[e->RowIndex]->Cells[e->ColumnIndex]->ReadOnly) 
				 return; 

			 if (!Double::TryParse(e->FormattedValue->ToString(), newDouble))
				 e->Cancel = true;
		 }
	 }
private: System::Void _DoNothingOnDataError(System::Object^  sender, System::Windows::Forms::DataGridViewDataErrorEventArgs^  e) {
		 }
};
}
#endif // !defined( PEDESTRIANTAGS_H )