#if !defined( TRACKS_H )
#define TRACKS_H
#pragma once
#include "PersistantFormats.h"
#include "PedestrianTags.h"
#include "NewVisionDoc.h"
#include "NewVisionView.h"
#include "BodyPath.h"
#include "TextEntryDlg.h"
#include "NewVision.h"
#include "MainFrm.h"

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Collections::Specialized;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


namespace NewVision {

	/// <summary>
	/// Summary for Tracks
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class Tracks : public System::Windows::Forms::Form
	{
	public:
		Tracks(TrackingData *tracks)
		{
			this->m_tracks = tracks;
		   InitializeComponent();
			//this->dataGridView1->RowHeadersWidth = 60;
			//this->dataGridView1->UserDeletedRow += gcnew DataGridViewRowEventHandler(this, &Tracks::DeleteRow);
		}
		
		void ExportTracks(TrackingData *tracks) {
		}
		
		void ImportTracks(TrackingData *tracks) {
			POSITION pos = tracks->data.GetStartPosition();
			DataGridViewRowCollection ^rows = this->dataGridView1->Rows;
			rows->Clear();
			
			while (pos) {
				int i; BodyPath *b;
				tracks->data.GetNextAssoc(pos, i, b);
				rows->Add();
				DataGridViewRow ^row = rows[rows->Count-1];
				row->Cells["ID"]->Value = String::Format("{0}", b->id);
				row->Cells["Start"]->Value = String::Format("{0}", b->startFrame);
				row->Cells["Stop"]->Value = String::Format("{0}", b->startFrame+b->GetCount()-1);
				row->Cells["Duration"]->Value = String::Format("{0}", b->GetCount());
				if (m_tracks->doc->m_DefaultTrackID == b->id) {
					row->HeaderCell->Value = "Merge";
					mergingWith->Text =  Convert::ToString(b->id);
					mergingWith->Cursor = Cursors::No;
				}

			}
			dataGridView1->SortCompare += gcnew DataGridViewSortCompareEventHandler(this, &Tracks::dataGridView1_SortCompare);
			DataGridViewColumn ^sCol = dataGridView1->SortedColumn;
			ListSortDirection sOrder = dataGridView1->SortOrder == SortOrder::Ascending ? ListSortDirection::Ascending : ListSortDirection::Descending;
			if (sCol)
				dataGridView1->Sort(sCol, sOrder);
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~Tracks()
		{
			if (components)
			{
				delete components;
			}
		}

	private: System::Windows::Forms::DataGridView^  dataGridView1;
	private: TrackingData *m_tracks;
	private: System::Windows::Forms::Button^  buttonInvisible;
	private: System::Windows::Forms::Button^  buttonVisible;
	private: System::Windows::Forms::Button^  buttonDelete;
	private: System::Windows::Forms::Button^  buttonTerminate;
	private: System::Windows::Forms::DataGridViewTextBoxColumn^  id;
	private: System::Windows::Forms::DataGridViewTextBoxColumn^  start;
	private: System::Windows::Forms::DataGridViewTextBoxColumn^  stop;
	private: System::Windows::Forms::DataGridViewTextBoxColumn^  Duration;
	private: System::Windows::Forms::Button^  buttonDeleteShort;
	private: System::Windows::Forms::Button^  removeSegment;
	private: System::Windows::Forms::Button^  buttonDisplaySegments;
	private: System::Windows::Forms::Button^  buttonBridgeGaps;
	private: System::Windows::Forms::Button^  buttonEditInfo;
	private: System::Windows::Forms::Button^  buttonMergeWith;


	private: System::Windows::Forms::Button^  buttonMerge;
	private: System::Windows::Forms::Button^  buttonAutoMerge;
	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::LinkLabel^  mergingWith;
	private: System::Windows::Forms::Button^  shiftTrack;

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
			this->id = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->start = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->stop = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->Duration = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->buttonInvisible = (gcnew System::Windows::Forms::Button());
			this->buttonVisible = (gcnew System::Windows::Forms::Button());
			this->buttonDelete = (gcnew System::Windows::Forms::Button());
			this->buttonTerminate = (gcnew System::Windows::Forms::Button());
			this->buttonDeleteShort = (gcnew System::Windows::Forms::Button());
			this->removeSegment = (gcnew System::Windows::Forms::Button());
			this->buttonDisplaySegments = (gcnew System::Windows::Forms::Button());
			this->buttonBridgeGaps = (gcnew System::Windows::Forms::Button());
			this->buttonEditInfo = (gcnew System::Windows::Forms::Button());
			this->buttonMergeWith = (gcnew System::Windows::Forms::Button());
			this->buttonMerge = (gcnew System::Windows::Forms::Button());
			this->buttonAutoMerge = (gcnew System::Windows::Forms::Button());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->mergingWith = (gcnew System::Windows::Forms::LinkLabel());
			this->shiftTrack = (gcnew System::Windows::Forms::Button());
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->dataGridView1))->BeginInit();
			this->SuspendLayout();
			// 
			// dataGridView1
			// 
			this->dataGridView1->AllowUserToAddRows = false;
			this->dataGridView1->ColumnHeadersHeightSizeMode = System::Windows::Forms::DataGridViewColumnHeadersHeightSizeMode::AutoSize;
			this->dataGridView1->Columns->AddRange(gcnew cli::array< System::Windows::Forms::DataGridViewColumn^  >(4) {this->id, this->start, 
				this->stop, this->Duration});
			this->dataGridView1->Dock = System::Windows::Forms::DockStyle::Fill;
			this->dataGridView1->Location = System::Drawing::Point(0, 0);
			this->dataGridView1->Name = L"dataGridView1";
			this->dataGridView1->ReadOnly = true;
			this->dataGridView1->Size = System::Drawing::Size(592, 531);
			this->dataGridView1->TabIndex = 0;
			this->dataGridView1->DoubleClick += gcnew System::EventHandler(this, &Tracks::DoubleClick);
			this->dataGridView1->Click += gcnew System::EventHandler(this, &Tracks::Click);
			// 
			// id
			// 
			this->id->HeaderText = L"ID";
			this->id->Name = L"id";
			this->id->ReadOnly = true;
			this->id->Width = 50;
			// 
			// start
			// 
			this->start->HeaderText = L"Start";
			this->start->Name = L"start";
			this->start->ReadOnly = true;
			// 
			// stop
			// 
			this->stop->HeaderText = L"Stop";
			this->stop->Name = L"stop";
			this->stop->ReadOnly = true;
			// 
			// Duration
			// 
			this->Duration->HeaderText = L"Duration";
			this->Duration->Name = L"Duration";
			this->Duration->ReadOnly = true;
			// 
			// buttonInvisible
			// 
			this->buttonInvisible->Location = System::Drawing::Point(413, 256);
			this->buttonInvisible->Name = L"buttonInvisible";
			this->buttonInvisible->Size = System::Drawing::Size(142, 23);
			this->buttonInvisible->TabIndex = 1;
			this->buttonInvisible->Text = L"Set Invisible";
			this->buttonInvisible->UseVisualStyleBackColor = true;
			this->buttonInvisible->Click += gcnew System::EventHandler(this, &Tracks::buttonInvisible_Click);
			// 
			// buttonVisible
			// 
			this->buttonVisible->Location = System::Drawing::Point(413, 285);
			this->buttonVisible->Name = L"buttonVisible";
			this->buttonVisible->Size = System::Drawing::Size(142, 23);
			this->buttonVisible->TabIndex = 2;
			this->buttonVisible->Text = L"Set Visible";
			this->buttonVisible->UseVisualStyleBackColor = true;
			this->buttonVisible->Click += gcnew System::EventHandler(this, &Tracks::buttonVisible_Click);
			// 
			// buttonDelete
			// 
			this->buttonDelete->Location = System::Drawing::Point(414, 415);
			this->buttonDelete->Name = L"buttonDelete";
			this->buttonDelete->Size = System::Drawing::Size(142, 23);
			this->buttonDelete->TabIndex = 3;
			this->buttonDelete->Text = L"Delete";
			this->buttonDelete->UseVisualStyleBackColor = true;
			this->buttonDelete->Click += gcnew System::EventHandler(this, &Tracks::buttonDelete_Click);
			// 
			// buttonTerminate
			// 
			this->buttonTerminate->Location = System::Drawing::Point(413, 330);
			this->buttonTerminate->Name = L"buttonTerminate";
			this->buttonTerminate->Size = System::Drawing::Size(142, 23);
			this->buttonTerminate->TabIndex = 4;
			this->buttonTerminate->Text = L"Cut track tail";
			this->buttonTerminate->UseVisualStyleBackColor = true;
			this->buttonTerminate->Click += gcnew System::EventHandler(this, &Tracks::buttonTerminate_Click);
			// 
			// buttonDeleteShort
			// 
			this->buttonDeleteShort->Location = System::Drawing::Point(413, 41);
			this->buttonDeleteShort->Name = L"buttonDeleteShort";
			this->buttonDeleteShort->Size = System::Drawing::Size(142, 23);
			this->buttonDeleteShort->TabIndex = 5;
			this->buttonDeleteShort->Text = L"Delete Short Tracks";
			this->buttonDeleteShort->UseVisualStyleBackColor = true;
			this->buttonDeleteShort->Click += gcnew System::EventHandler(this, &Tracks::buttonDeleteShort_Click);
			// 
			// removeSegment
			// 
			this->removeSegment->Location = System::Drawing::Point(413, 359);
			this->removeSegment->Name = L"removeSegment";
			this->removeSegment->Size = System::Drawing::Size(142, 23);
			this->removeSegment->TabIndex = 6;
			this->removeSegment->Text = L"Cut track head";
			this->removeSegment->UseVisualStyleBackColor = true;
			this->removeSegment->Click += gcnew System::EventHandler(this, &Tracks::buttonRemoveSegment_Click);
			// 
			// buttonDisplaySegments
			// 
			this->buttonDisplaySegments->Location = System::Drawing::Point(413, 227);
			this->buttonDisplaySegments->Name = L"buttonDisplaySegments";
			this->buttonDisplaySegments->Size = System::Drawing::Size(142, 23);
			this->buttonDisplaySegments->TabIndex = 7;
			this->buttonDisplaySegments->Text = L"Display segments";
			this->buttonDisplaySegments->UseVisualStyleBackColor = true;
			this->buttonDisplaySegments->Click += gcnew System::EventHandler(this, &Tracks::buttonDisplaySegments_Click);
			// 
			// buttonBridgeGaps
			// 
			this->buttonBridgeGaps->Location = System::Drawing::Point(413, 12);
			this->buttonBridgeGaps->Name = L"buttonBridgeGaps";
			this->buttonBridgeGaps->Size = System::Drawing::Size(142, 23);
			this->buttonBridgeGaps->TabIndex = 8;
			this->buttonBridgeGaps->Text = L"Bridge gaps";
			this->buttonBridgeGaps->UseVisualStyleBackColor = true;
			this->buttonBridgeGaps->Click += gcnew System::EventHandler(this, &Tracks::buttonBridgeGaps_Click);
			// 
			// buttonEditInfo
			// 
			this->buttonEditInfo->Location = System::Drawing::Point(414, 444);
			this->buttonEditInfo->Name = L"buttonEditInfo";
			this->buttonEditInfo->Size = System::Drawing::Size(142, 23);
			this->buttonEditInfo->TabIndex = 9;
			this->buttonEditInfo->Text = L"Edit Info";
			this->buttonEditInfo->UseVisualStyleBackColor = true;
			this->buttonEditInfo->Click += gcnew System::EventHandler(this, &Tracks::buttonEditInfo_Click);
			// 
			// buttonMergeWith
			// 
			this->buttonMergeWith->Location = System::Drawing::Point(413, 162);
			this->buttonMergeWith->Name = L"buttonMergeWith";
			this->buttonMergeWith->Size = System::Drawing::Size(142, 23);
			this->buttonMergeWith->TabIndex = 10;
			this->buttonMergeWith->Text = L"Merge With ...";
			this->buttonMergeWith->UseVisualStyleBackColor = true;
			this->buttonMergeWith->Click += gcnew System::EventHandler(this, &Tracks::buttonMergeWith_Click);
			// 
			// buttonMerge
			// 
			this->buttonMerge->Location = System::Drawing::Point(413, 133);
			this->buttonMerge->Name = L"buttonMerge";
			this->buttonMerge->Size = System::Drawing::Size(142, 23);
			this->buttonMerge->TabIndex = 11;
			this->buttonMerge->Text = L"Merge Selected";
			this->buttonMerge->UseVisualStyleBackColor = true;
			this->buttonMerge->Click += gcnew System::EventHandler(this, &Tracks::buttonMerge_Click);
			// 
			// buttonAutoMerge
			// 
			this->buttonAutoMerge->Location = System::Drawing::Point(413, 70);
			this->buttonAutoMerge->Name = L"buttonAutoMerge";
			this->buttonAutoMerge->Size = System::Drawing::Size(142, 23);
			this->buttonAutoMerge->TabIndex = 14;
			this->buttonAutoMerge->Text = L"Auto Merge";
			this->buttonAutoMerge->UseVisualStyleBackColor = true;
			this->buttonAutoMerge->Click += gcnew System::EventHandler(this, &Tracks::buttonAutoMerge_Click);
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->BackColor = System::Drawing::SystemColors::AppWorkspace;
			this->label1->ForeColor = System::Drawing::SystemColors::Info;
			this->label1->Location = System::Drawing::Point(411, 198);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(93, 13);
			this->label1->TabIndex = 16;
			this->label1->Text = L"Default merge ID: ";
			// 
			// mergingWith
			// 
			this->mergingWith->AutoSize = true;
			this->mergingWith->BackColor = System::Drawing::SystemColors::AppWorkspace;
			this->mergingWith->Cursor = System::Windows::Forms::Cursors::No;
			this->mergingWith->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->mergingWith->Location = System::Drawing::Point(522, 198);
			this->mergingWith->Name = L"mergingWith";
			this->mergingWith->Size = System::Drawing::Size(33, 13);
			this->mergingWith->TabIndex = 17;
			this->mergingWith->TabStop = true;
			this->mergingWith->Text = L"None";
			this->mergingWith->VisitedLinkColor = System::Drawing::Color::Blue;
			this->mergingWith->LinkClicked += gcnew System::Windows::Forms::LinkLabelLinkClickedEventHandler(this, &Tracks::mergingWith_LinkClicked);
			// 
			// shiftTrack
			// 
			this->shiftTrack->Location = System::Drawing::Point(413, 388);
			this->shiftTrack->Name = L"shiftTrack";
			this->shiftTrack->Size = System::Drawing::Size(142, 23);
			this->shiftTrack->TabIndex = 18;
			this->shiftTrack->Text = L"Shift track";
			this->shiftTrack->UseVisualStyleBackColor = true;
			this->shiftTrack->Click += gcnew System::EventHandler(this, &Tracks::shiftTrack_Click);
			// 
			// Tracks
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(592, 531);
			this->Controls->Add(this->shiftTrack);
			this->Controls->Add(this->mergingWith);
			this->Controls->Add(this->label1);
			this->Controls->Add(this->buttonAutoMerge);
			this->Controls->Add(this->buttonMerge);
			this->Controls->Add(this->buttonMergeWith);
			this->Controls->Add(this->buttonEditInfo);
			this->Controls->Add(this->buttonBridgeGaps);
			this->Controls->Add(this->buttonDisplaySegments);
			this->Controls->Add(this->removeSegment);
			this->Controls->Add(this->buttonDeleteShort);
			this->Controls->Add(this->buttonTerminate);
			this->Controls->Add(this->buttonDelete);
			this->Controls->Add(this->buttonVisible);
			this->Controls->Add(this->buttonInvisible);
			this->Controls->Add(this->dataGridView1);
			this->Name = L"Tracks";
			this->Text = L"Tracks";
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->dataGridView1))->EndInit();
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
		private: System::Void buttonRemoveSegment_Click(System::Object^  sender, System::EventArgs^  e) {
			int startFrame = 0;
			// prompt for stopframe number
			CTextEntryDlg dlgTextEntry;
			if (dlgTextEntry.Show(NULL, "Cut track head", "Cut trail before frame:") == IDOK) {
				CString strResult = dlgTextEntry.GetText();
				startFrame = _atoi64(strResult);
			}	
			else 
				return;
			// terminate track at stopFrame
			BodyPath *bp;
			if (m_tracks->data.Lookup(Convert::ToInt32(dataGridView1->CurrentRow->Cells["ID"]->Value), bp)) {
				bp->CutHead(startFrame);
				dataGridView1->CurrentRow->Cells["Start"]->Value = startFrame;
			}
		}

		private: System::Void buttonVisible_Click(System::Object^  sender, System::EventArgs^  e) {
			int startFrame = 0, stopFrame = 0;
			// prompt for stopframe number
			CTextEntryDlg dlgTextEntry;
			if (dlgTextEntry.Show(NULL, "Visible", "Make visible from:") == IDOK) {
				CString strResult = dlgTextEntry.GetText();
				startFrame = _atoi64(strResult);
				if (dlgTextEntry.Show(NULL, "Visible", "Make visible to:") == IDOK) {
					CString strResult = dlgTextEntry.GetText();
					stopFrame = _atoi64(strResult);
				}	
				else 
					return;
			}
			else
				return;
			
			BodyPath *bp;
			if (m_tracks->data.Lookup(Convert::ToInt32(dataGridView1->CurrentRow->Cells["ID"]->Value), bp)) {
				for (int i=max(startFrame-bp->startFrame, 0); i < min(stopFrame-bp->startFrame, bp->GetUpperBound());i++)
					bp->GetAt(i).visible = true;
			}
		}
		private: System::Void buttonInvisible_Click(System::Object^  sender, System::EventArgs^  e) {
			int startFrame = 0, stopFrame = 0;
			// prompt for stop frame number
			CTextEntryDlg dlgTextEntry;
			if (dlgTextEntry.Show(NULL, "InVisible", "Make invisible from:") == IDOK) {
				CString strResult = dlgTextEntry.GetText();
				startFrame = _atoi64(strResult);
				if (dlgTextEntry.Show(NULL, "InVisible", "Make invisible to:") == IDOK) {
					CString strResult = dlgTextEntry.GetText();
					stopFrame = _atoi64(strResult);
				}	
				else 
					return;
			}
			else
				return;
			// terminate track at stopFrame
			BodyPath *bp;
			if (m_tracks->data.Lookup(Convert::ToInt32(dataGridView1->CurrentRow->Cells["ID"]->Value), bp)) {
				for (int i=max(startFrame-bp->startFrame, 0); i < min(stopFrame-bp->startFrame, bp->GetUpperBound());i++)
					bp->GetAt(i).visible = false;
			}
		}

		private: System::Void dataGridView1_SortCompare(Object^ sender, DataGridViewSortCompareEventArgs^ e) {
			e->SortResult = Convert::ToInt32(e->CellValue1).CompareTo(Convert::ToInt32(e->CellValue2));
			e->Handled = true;
		}

		// Removes short junk tracks
		private: System::Void buttonDeleteShort_Click(System::Object^  sender, System::EventArgs^  e) {	
			
			// prompt for minFrames
			CTextEntryDlg dlgTextEntry;
			int minFrames = 0;
			if (dlgTextEntry.Show(NULL, "Remove Short Tracks (Note: Only tracks ending before the current frame will be deleted!", "Remove all tracks shorter than (frames):") == IDOK) {
				CString strResult = dlgTextEntry.GetText();
				minFrames = _atoi64(strResult);
			}
			else
				return;

			// Remove from data
			POSITION pos = m_tracks->data.GetStartPosition();
			while (pos) {
				int id; BodyPath *p;
				m_tracks->data.GetNextAssoc(pos, id, p);
				if (p->startFrame + p->GetCount() < (int)m_tracks->doc->trackermodel.m_frameNumber - 1 
					&& p->GetCount() < minFrames)
						m_tracks->data.RemoveKey(id);
			}
			// Reload the tracks window
			ImportTracks(m_tracks);
		 }

		private: System::Void buttonDisplaySegments_Click(System::Object^  sender, System::EventArgs^  e) {
			BodyPath *bp;
			if (m_tracks->data.Lookup(Convert::ToInt32(dataGridView1->CurrentRow->Cells["ID"]->Value), bp)) {
				String^ msg = String::Format("Visible segments for ID={0}:\n\n", bp->id);
				msg += bp->ListVisibleSegments();
				MessageBox::Show(msg);
			}

		 }
		private: System::Void buttonBridgeGaps_Click(System::Object^  sender, System::EventArgs^  e) {
				// prompt for minLength
				CTextEntryDlg dlgTextEntry;
				int maxLength = 0;
				if (dlgTextEntry.Show(NULL, "Bridge Invisibility Gaps", "Bridge all gaps shorter than (frames):") == IDOK) {
					CString strResult = dlgTextEntry.GetText();
					maxLength = _atoi64(strResult);
					
					// for all paths bridge gaps less than minLength
					POSITION pos = m_tracks->data.GetStartPosition();
					while (pos) {
						int id; BodyPath *p;
						m_tracks->data.GetNextAssoc(pos, id, p);
						p->BridgeInvisibleGaps(maxLength);
					}
				}
			 }
		private: System::Void buttonEditInfo_Click(System::Object^  sender, System::EventArgs^  e) {
				m_tracks->EditInfoID(Convert::ToInt32(dataGridView1->CurrentRow->Cells["ID"]->Value));
				m_tracks->doc->UpdateAll();
		}
		
		
		private: System::Void buttonMerge_Click(System::Object^  sender, System::EventArgs^  e) {
				int id_master = Convert::ToInt32(dataGridView1->CurrentRow->Cells["ID"]->Value);
				for each (DataGridViewRow^ row in dataGridView1->SelectedRows) {
					int id_slave = Convert::ToInt32(row->Cells["ID"]->Value);
					m_tracks->MergeID(id_master, id_slave);
				}
				m_tracks->doc->UpdateAll();
				ImportTracks(m_tracks);
		}

		private: System::Void buttonMergeWith_Click(System::Object^  sender, System::EventArgs^  e) {
				m_tracks->MergeID(Convert::ToInt32(dataGridView1->CurrentRow->Cells["ID"]->Value));
				m_tracks->doc->UpdateAll();
				ImportTracks(m_tracks);
		}

		private: void OnUserDeletedRow (System::Object^  sender, System::EventArgs^  e) {
			buttonDelete_Click(sender, e);
		}

		private: System::Void buttonDelete_Click(System::Object^  sender, System::EventArgs^  e) {
			// if  no rows selected then delete current row
			if (dataGridView1->SelectedRows->Count == 0)
				m_tracks->data.RemoveKey(Convert::ToInt32(dataGridView1->CurrentRow->Cells["ID"]->Value));
			else
				for each (DataGridViewRow^ row in dataGridView1->SelectedRows) {
					m_tracks->data.RemoveKey(Convert::ToInt32(row->Cells["ID"]->Value));
					dataGridView1->Rows->Remove(row);
				}
			 m_tracks->doc->UpdateAll();
		}

		private: System::Void shiftTrack_Click(System::Object^  sender, System::EventArgs^  e) {
				 CTextEntryDlg dlgTextEntry;
				 int nFrames = 0;
				 if (dlgTextEntry.Show(NULL, "Shift selected tracks by how many frames?", "Frames: (e.g. -5)") == IDOK) {
					 CString strResult = dlgTextEntry.GetText();
					 nFrames = _atoi64(strResult);
					 // if  no rows selected then shift current row
					 if (dataGridView1->SelectedRows->Count == 0)
						 m_tracks->data[Convert::ToInt32(dataGridView1->CurrentRow->Cells["ID"]->Value)]->ShiftTime(nFrames);
					 else {
						 for each (DataGridViewRow^ row in dataGridView1->SelectedRows) {
							 BodyPath *bp;
							 if (m_tracks->data.Lookup(Convert::ToInt32(row->Cells["ID"]->Value), bp))
								 bp->ShiftTime(nFrames);
						 }
					}
					m_tracks->doc->UpdateAll();
				 }
		}

		private: System::Void buttonTerminate_Click(System::Object^  sender, System::EventArgs^  e) {
			if (dataGridView1->CurrentRow) {
				 int stopFrame = 0;
				 // prompt for stopframe number
				 CTextEntryDlg dlgTextEntry;
				 if (dlgTextEntry.Show(NULL, "Cut track tail", "Cut track after frame:") == IDOK) {
					 CString strResult = dlgTextEntry.GetText();
					 stopFrame = _atoi64(strResult);
				 }	
				 else 
					 return;
				 // terminate track at stopFrame
				 BodyPath *bp;
				 if (m_tracks->data.Lookup(Convert::ToInt32(dataGridView1->CurrentRow->Cells["ID"]->Value), bp)) {
					 bp->CutTail(stopFrame);
					 dataGridView1->CurrentRow->Cells["Stop"]->Value = stopFrame;
				 }
			 }
		}

		protected: virtual void OnClosed(System::EventArgs^  e) override {
			 //m_tracks->doc->displayTrackInfo = false;
			 m_tracks->doc->OnViewTracks();
			 System::Windows::Forms::Form::OnClosed(e);
		}

		protected: virtual void OnActivated(System::EventArgs^  e) override {
		   mergingWith->Text =  "None";
		   mergingWith->Cursor = Cursors::Default;
		   ImportTracks(m_tracks);
		   System::Windows::Forms::Form::OnActivated(e);
		}

		private: System::Void DoubleClick(System::Object^ sender, System::EventArgs^  e) {	
				if (dataGridView1->CurrentCell->OwningColumn->Name->Equals("id")) {
					m_tracks->EditInfoID(Convert::ToInt32(dataGridView1->CurrentRow->Cells["ID"]->Value));
					m_tracks->doc->UpdateAll();
					return;
				}
				if (dataGridView1->CurrentCell->OwningColumn->Name->Equals("start")) 
					m_tracks->doc->trackermodel.SetPosition(Convert::ToInt32(dataGridView1->CurrentRow->Cells["Start"]->Value));
				if (dataGridView1->CurrentCell->OwningColumn->Name->Equals("stop")) 
					m_tracks->doc->trackermodel.SetPosition(Convert::ToInt32(dataGridView1->CurrentRow->Cells["Stop"]->Value));
				m_tracks->doc->UpdateOverlayInfo();
				if (::IsWindow(theApp.m_pMainWnd->m_hWnd)) {
					theApp.m_pMainWnd->PostMessage(WM_UPDATENAVBAR);
					theApp.m_pMainWnd->PostMessage(WM_UPDATESTATUSBAR);
					theApp.m_pMainWnd->PostMessage(WM_UPDATEACTIVEVIEW);
				}
		}
		
		private: System::Void Click(System::Object^ sender, System::EventArgs^  e) {	
			if ( Control::ModifierKeys == Keys::Alt) {
				int newID = Convert::ToInt32(dataGridView1->CurrentRow->Cells["ID"]->Value);
				// if this ID is not aleady selected
				if (m_tracks->doc->m_DefaultTrackID != newID) {
					//find row
					   //dataGridView1->CurrentRow->HeaderCell->Value = "";
					dataGridView1->CurrentRow->HeaderCell->Value = "M";
					m_tracks->doc->m_DefaultTrackID = newID;
					mergingWith->Text =  Convert::ToString(newID);
					mergingWith->Cursor = Cursors::No;
				}
				else {
					dataGridView1->CurrentRow->HeaderCell->Value = "";
					m_tracks->doc->m_DefaultTrackID = -1;
					mergingWith->Text =  "None";
					mergingWith->Cursor = Cursors::Default;
				}
			}
		}

		private: System::Void mergingWith_LinkClicked(System::Object^  sender, System::Windows::Forms::LinkLabelLinkClickedEventArgs^  e) {
			 if (m_tracks->doc->m_DefaultTrackID != -1) {
				 dataGridView1->CurrentRow->HeaderCell->Value = "";
				 m_tracks->doc->m_DefaultTrackID = -1;
				 mergingWith->Text =  "None";
				 mergingWith->Cursor = Cursors::Default;
			 }
		}
		private: System::Void buttonAutoMerge_Click(System::Object^  sender, System::EventArgs^  e) {
			CTextEntryDlg dlgTextEntry;
			CString strResult;
			if (dlgTextEntry.Show(NULL, "Auto Merge tracks", "Maximum frame distance to merge: ") != IDOK)
				return;
			strResult = dlgTextEntry.GetText();
			int maxFrames = _atoi64(strResult);
			if (dlgTextEntry.Show(NULL, "Auto Merge tracks", "Maximum floor distance to merge (cm): ") != IDOK)
				return;
			strResult = dlgTextEntry.GetText();
			double maxDist = _atoi64(strResult);
			if (dlgTextEntry.Show(NULL, "Auto Merge tracks", "No other bodies within distance (cm): ") != IDOK)
			 return;
			strResult = dlgTextEntry.GetText();
			double maxOtherDist = _atoi64(strResult);
			ShowProgressDialog("Auto-merging tracks", m_tracks->doc->m_TrackingData.data.GetCount());
			m_tracks->AutoMerge(maxFrames, maxDist, maxOtherDist);
			HideProgressDialog();
			 // Reload the tracks window
			ImportTracks(m_tracks);
			m_tracks->doc->UpdateAll();
		}
};

}
#endif // !defined( TRACKS_H )