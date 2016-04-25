#pragma once

#include "PersistantFormats.h"
#include "NewVisionDoc.h"
#include "TextEntryDlg.h"
#include "SwarmEvent.h"
#include "SwarmActivity.h"


using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


namespace NewVision {

	/// <summary>
	/// Summary for GroupTagWindow
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class GroupTagWindow : public System::Windows::Forms::Form
	{
	public:
		GroupTagWindow(ActivityData* data, int actorID) {
			this->m_actorID = actorID;
			this->m_data = data;
			InitializeComponent();
			this->dataGridView1->UserDeletedRow += gcnew DataGridViewRowEventHandler(this, &GroupTagWindow::OnGroupDeleted);
			this->dataGridView1->SortCompare += gcnew DataGridViewSortCompareEventHandler(this, &GroupTagWindow::SortCompare);
			ImportData(data);
		}

		void ImportData(ActivityData* data) {
			POSITION pos = data->sActivity.GetStartPosition();
			DataGridViewRowCollection ^rows = this->dataGridView1->Rows;
			rows->Clear();
			while (pos) {
				int id; SwarmActivity *ac;
				m_data->sActivity.GetNextAssoc(pos, id, ac);
				if (ac->type != "GROUPING")
					continue;
				rows->Add();
				DataGridViewRow ^row = rows[rows->Count-1];
				row->Cells["GroupID"]->Value = String::Format("{0}", id);
				POSITION posA = ac->actors.GetStartPosition();
				int nColumn = 1;
				while (posA) {
					int aid; double c;
					ac->actors.GetNextAssoc(posA, aid, c);
					if (this->dataGridView1->ColumnCount < nColumn+1)
						this->dataGridView1->Columns->Add(String::Format("Actor{0}", nColumn), String::Format("Actor {0}", nColumn));
					row->Cells[nColumn++]->Value = String::Format("{0}", aid);
				}
			}
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~GroupTagWindow()
		{
			if (components)
			{
				delete components;
			}
		}
	private: ActivityData *m_data;
	private: int m_actorID;
	private: System::Windows::Forms::DataGridView^  dataGridView1;

	private: System::Windows::Forms::Button^  buttonDeleteGroup;
	private: System::Windows::Forms::Button^  buttonAddGroup;
	private: System::Windows::Forms::Button^  buttonAddToSelected;
	private: System::Windows::Forms::Button^  buttonClose;
	private: System::Windows::Forms::DataGridViewTextBoxColumn^  GroupID;
	private: System::Windows::Forms::Button^  button1;
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
			this->GroupID = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->buttonDeleteGroup = (gcnew System::Windows::Forms::Button());
			this->buttonAddGroup = (gcnew System::Windows::Forms::Button());
			this->buttonAddToSelected = (gcnew System::Windows::Forms::Button());
			this->buttonClose = (gcnew System::Windows::Forms::Button());
			this->button1 = (gcnew System::Windows::Forms::Button());
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->dataGridView1))->BeginInit();
			this->SuspendLayout();
			// 
			// dataGridView1
			// 
			this->dataGridView1->AllowUserToAddRows = false;
			this->dataGridView1->AllowUserToResizeRows = false;
			this->dataGridView1->ColumnHeadersHeightSizeMode = System::Windows::Forms::DataGridViewColumnHeadersHeightSizeMode::AutoSize;
			this->dataGridView1->Columns->AddRange(gcnew cli::array< System::Windows::Forms::DataGridViewColumn^  >(1) {this->GroupID});
			this->dataGridView1->Location = System::Drawing::Point(0, 0);
			this->dataGridView1->Name = L"dataGridView1";
			this->dataGridView1->Size = System::Drawing::Size(367, 454);
			this->dataGridView1->TabIndex = 0;
			// 
			// GroupID
			// 
			this->GroupID->HeaderText = L"Group ID";
			this->GroupID->Name = L"GroupID";
			this->GroupID->ReadOnly = true;
			this->GroupID->Width = 50;
			// 
			// buttonDeleteGroup
			// 
			this->buttonDeleteGroup->Location = System::Drawing::Point(373, 63);
			this->buttonDeleteGroup->Name = L"buttonDeleteGroup";
			this->buttonDeleteGroup->Size = System::Drawing::Size(97, 23);
			this->buttonDeleteGroup->TabIndex = 1;
			this->buttonDeleteGroup->Text = L"Delete Group";
			this->buttonDeleteGroup->UseVisualStyleBackColor = true;
			this->buttonDeleteGroup->Click += gcnew System::EventHandler(this, &GroupTagWindow::buttonDeleteGroup_Click);
			// 
			// buttonAddGroup
			// 
			this->buttonAddGroup->Location = System::Drawing::Point(373, 92);
			this->buttonAddGroup->Name = L"buttonAddGroup";
			this->buttonAddGroup->Size = System::Drawing::Size(97, 23);
			this->buttonAddGroup->TabIndex = 2;
			this->buttonAddGroup->Text = L"New Group";
			this->buttonAddGroup->UseVisualStyleBackColor = true;
			this->buttonAddGroup->Click += gcnew System::EventHandler(this, &GroupTagWindow::buttonAddGroup_Click);
			// 
			// buttonAddToSelected
			// 
			this->buttonAddToSelected->Location = System::Drawing::Point(373, 14);
			this->buttonAddToSelected->Name = L"buttonAddToSelected";
			this->buttonAddToSelected->Size = System::Drawing::Size(97, 43);
			this->buttonAddToSelected->TabIndex = 3;
			this->buttonAddToSelected->Text = L"Add To Selected Group";
			this->buttonAddToSelected->UseVisualStyleBackColor = true;
			this->buttonAddToSelected->Click += gcnew System::EventHandler(this, &GroupTagWindow::buttonAddToSelected_Click);
			// 
			// buttonClose
			// 
			this->buttonClose->DialogResult = System::Windows::Forms::DialogResult::OK;
			this->buttonClose->Location = System::Drawing::Point(373, 121);
			this->buttonClose->Name = L"buttonClose";
			this->buttonClose->Size = System::Drawing::Size(97, 23);
			this->buttonClose->TabIndex = 4;
			this->buttonClose->Text = L"Close";
			this->buttonClose->UseVisualStyleBackColor = true;
			// 
			// GroupTagWindow
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(474, 454);
			this->Controls->Add(this->button1);
			this->Controls->Add(this->buttonClose);
			this->Controls->Add(this->buttonAddToSelected);
			this->Controls->Add(this->buttonAddGroup);
			this->Controls->Add(this->buttonDeleteGroup);
			this->Controls->Add(this->dataGridView1);
			this->Name = L"GroupTagWindow";
			this->Text = L"Shopper Groups";
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->dataGridView1))->EndInit();
			this->ResumeLayout(false);

		}
#pragma endregion
		private: System::Void OnGroupDeleted(System::Object^ sender, DataGridViewRowEventArgs^  e) {
			m_data->sActivity.RemoveKey(Convert::ToInt32(e->Row->Cells["GroupID"]->Value));
		}
		private: System::Void buttonDeleteGroup_Click(System::Object^  sender, System::EventArgs^  e) {
			OnGroupDeleted(sender, gcnew DataGridViewRowEventArgs(dataGridView1->CurrentRow));
			dataGridView1->Rows->Remove(dataGridView1->CurrentRow);
		}
		private: System::Void buttonAddGroup_Click(System::Object^  sender, System::EventArgs^ e) {
			SwarmActivity* ac = new SwarmActivity(); ac->type = "GROUPING";
			m_data->sActivity.SetAt(ac->id, ac);
			dataGridView1->Rows->Add();
			dataGridView1->Rows[dataGridView1->Rows->Count-1]->Cells["GroupID"]->Value = String::Format("{0}", ac->id);
			dataGridView1->CurrentCell  = dataGridView1->Rows[dataGridView1->Rows->Count-1]->Cells["GroupID"];
		}
		private: System::Void buttonAddToSelected_Click(System::Object^  sender, System::EventArgs^  e) {
			if (!dataGridView1->Rows->Count)
				buttonAddGroup_Click(sender, e);

			if (this->m_actorID != -1) {
				DataGridViewRow ^row = dataGridView1->CurrentRow;

				// == update the data source
				SwarmActivity* ac;
				if (m_data->sActivity.Lookup(Int32::Parse(row->Cells["GroupID"]->Value->ToString()), ac))
					ac->actors.SetAt(this->m_actorID, 1);

				// == update the table
				if (this->dataGridView1->ColumnCount < ac->actors.GetCount()+1) 	// if enough columns, add a new one
					this->dataGridView1->Columns->Add(String::Format("Actor{0}", ac->actors.GetCount()), String::Format("Actor {0}", ac->actors.GetCount()));
				row->Cells[ac->actors.GetCount()]->Value = String::Format("{0}", this->m_actorID);
			
			}
		}

		private: System::Void SortCompare(Object^ sender, DataGridViewSortCompareEventArgs^ e) {
			e->SortResult = Convert::ToInt32(e->CellValue1).CompareTo(Convert::ToInt32(e->CellValue2));
			e->Handled = true;
		}
};
}
