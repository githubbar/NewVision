#pragma once

using namespace System;
using namespace System::Configuration;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;
using namespace System::Collections::Specialized;

#using <system.dll>
#using <system.configuration.dll>

namespace NewVision {

	/// <summary>
	/// Summary for TimeStampPairTags
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class TimeStampPairTags : public System::Windows::Forms::Form
	{
	public:
		TimeStampPairTags(CommaDelimitedStringCollection ^valueColl)
		{
			InitializeComponent();
			dataGridView1->CellValidating += gcnew DataGridViewCellValidatingEventHandler(this, &TimeStampPairTags::dataGridView1_CellValidating);
			ImportTags(valueColl);
		
		}

		void ExportTags(CommaDelimitedStringCollection ^valueColl) {
			valueColl->Clear();
			DataGridViewRowCollection ^row = this->dataGridView1->Rows;
			for (int i=0;i<row->Count;i++) {
				if (row[i]->IsNewRow)
					break;
				valueColl->Add(row[i]->Cells[0]->Value->ToString());
				// handle incomplete pairs
				if (row[i]->Cells[1]->Value == nullptr)
					valueColl->Add(row[i]->Cells[0]->Value->ToString());
				else
					valueColl->Add(row[i]->Cells[1]->Value->ToString());
			}
		}

		void ImportTags(CommaDelimitedStringCollection ^valueColl) {
			for (int j=0;j<valueColl->Count;j+=2) {
				DataGridViewRow ^dataGridRow = gcnew DataGridViewRow();
				dataGridRow->HeaderCell->Value = (j/2)+1;
				DataGridViewTextBoxCell ^txt1 = gcnew DataGridViewTextBoxCell();
				txt1->Value = valueColl[j];
				dataGridRow->Cells->Add(txt1);
				DataGridViewTextBoxCell ^txt2 = gcnew DataGridViewTextBoxCell();
				// handle incomplete pairs
				if (j+1 < valueColl->Count)
					txt2->Value = valueColl[j+1];			
				else
					txt2->Value = "";
				dataGridRow->Cells->Add(txt2);
				this->dataGridView1->Rows->Add(dataGridRow);
			}
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~TimeStampPairTags()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::Button^  buttonCancel;
	protected: 
	private: System::Windows::Forms::Button^  buttonOk;
	private: System::Windows::Forms::DataGridView^  dataGridView1;
	private: System::Windows::Forms::DataGridViewTextBoxColumn^  Begin;
	private: System::Windows::Forms::DataGridViewTextBoxColumn^  End;

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
			this->buttonCancel = (gcnew System::Windows::Forms::Button());
			this->buttonOk = (gcnew System::Windows::Forms::Button());
			this->dataGridView1 = (gcnew System::Windows::Forms::DataGridView());
			this->Begin = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->End = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->dataGridView1))->BeginInit();
			this->SuspendLayout();
			// 
			// buttonCancel
			// 
			this->buttonCancel->DialogResult = System::Windows::Forms::DialogResult::Cancel;
			this->buttonCancel->Location = System::Drawing::Point(139, 400);
			this->buttonCancel->Name = L"buttonCancel";
			this->buttonCancel->Size = System::Drawing::Size(75, 23);
			this->buttonCancel->TabIndex = 4;
			this->buttonCancel->Text = L"Cancel";
			this->buttonCancel->UseVisualStyleBackColor = true;
			// 
			// buttonOk
			// 
			this->buttonOk->DialogResult = System::Windows::Forms::DialogResult::OK;
			this->buttonOk->Location = System::Drawing::Point(58, 400);
			this->buttonOk->Name = L"buttonOk";
			this->buttonOk->Size = System::Drawing::Size(75, 23);
			this->buttonOk->TabIndex = 3;
			this->buttonOk->Text = L"Ok";
			this->buttonOk->UseVisualStyleBackColor = true;
			// 
			// dataGridView1
			// 
			this->dataGridView1->ColumnHeadersHeightSizeMode = System::Windows::Forms::DataGridViewColumnHeadersHeightSizeMode::AutoSize;
			this->dataGridView1->Columns->AddRange(gcnew cli::array< System::Windows::Forms::DataGridViewColumn^  >(2) {this->Begin, 
				this->End});
			this->dataGridView1->Location = System::Drawing::Point(12, 12);
			this->dataGridView1->Name = L"dataGridView1";
			this->dataGridView1->Size = System::Drawing::Size(243, 382);
			this->dataGridView1->TabIndex = 5;
			// 
			// Begin
			// 
			this->Begin->HeaderText = L"Begin";
			this->Begin->Name = L"Begin";
			// 
			// End
			// 
			this->End->HeaderText = L"End";
			this->End->Name = L"End";
			// 
			// TimeStampPairTags
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(267, 435);
			this->Controls->Add(this->dataGridView1);
			this->Controls->Add(this->buttonCancel);
			this->Controls->Add(this->buttonOk);
			this->Name = L"TimeStampPairTags";
			this->Text = L"TimeStampPairTags";
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->dataGridView1))->EndInit();
			this->ResumeLayout(false);

		}
#pragma endregion
		private: System::Void dataGridView1_CellValidating(System::Object^  sender, DataGridViewCellValidatingEventArgs ^ e) {
			if (dataGridView1->CurrentCell->GetType() != DataGridViewTextBoxCell::typeid)
				 return;
			DataGridViewTextBoxCell^ cell = (DataGridViewTextBoxCell^)dataGridView1->CurrentCell;
			 // validate doubles in the first column only
			 if (cell && cell->OwningColumn->Index == 0) {
				 Double newDouble;
				 if (dataGridView1->Rows[e->RowIndex]->IsNewRow) 
					 return; 
		
				 if (!Double::TryParse(e->FormattedValue->ToString(), newDouble))
					 e->Cancel = true;
			 }
		 }
	};
}
